#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

#define FORWARD glm::normalize(dir)
#define BACKWARD glm::normalize(-dir)
#define LEFT glm::normalize(glm::cross(up, dir))
#define RIGHT glm::normalize(glm::cross(dir, up))
#define WALK_FORWARD glm::normalize(glm::vec3(dir.x, 0.0f, dir.z));
#define WALK_BACKWARD glm::normalize(glm::vec3(-dir.x, 0.0f, -dir.z));

#define UAXIS glm::cross(dir, up)
#define VAXIS up
#define NAXIS dir

enum Direction {
	Forward,
	Backward,
	WalkForward,
	WalkBackward,
	Left,
	Right
};

enum Axis {
	U, V, N
};

class Camera {
public:
	glm::mat4 projection;
	glm::mat4 view;

	glm::vec3 pos;
	glm::vec3 dir;
	glm::vec3 up;

	Camera(float width, float height) {
		Camera(width, height, glm::vec3(), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	}
	Camera(float width, float height, glm::vec3 pos, glm::vec3 dir, glm::vec3 up) : pos(pos), dir(dir), up(up) {
		update();
		resize(width, height);
	};
	~Camera() {}

	void update() {
		view = glm::lookAt(pos, pos + dir, up);
	}

	void resize(float width, float height) {
		projection = glm::perspective(glm::radians(50.0f), width / height, 0.1f, 256.0f);
		//projection = glm::ortho(-5.f, 5.f, 5.f, -5.f);
	}

	void translate(Direction direction, float magnitude) {
		switch (direction) {
		case Forward:
			pos += magnitude * FORWARD;
			break;
		case Backward:
			pos += magnitude * BACKWARD;
			break;
		case Left:
			pos += magnitude * LEFT;
			break;
		case Right:
			pos += magnitude * RIGHT;
			break;
		case WalkForward:
			pos += magnitude * WALK_FORWARD;
			break;
		case WalkBackward:
			pos += magnitude * WALK_BACKWARD;
			break;
		}
		update();
	}

	void rotate(Axis axis, float angle) {
		// rotate dir about axis by angle
		switch (axis) {
		case U:
			dir = glm::vec3(glm::rotate(glm::mat4(), angle, UAXIS) * glm::vec4(dir, 1.0f));
			break;
		case V:
			dir = glm::vec3(glm::rotate(glm::mat4(), angle, VAXIS) * glm::vec4(dir, 1.0f));
			break;
		case N:
			dir = glm::vec3(glm::rotate(glm::mat4(), angle, NAXIS) * glm::vec4(dir, 1.0f));
			break;
		}
		update();
	}

	float uAxisAngle() {
		return glm::acos(glm::dot(glm::normalize(up), glm::normalize(dir)) / (glm::l1Norm(glm::normalize(up)) * glm::l1Norm(glm::normalize(dir))));
	}
};