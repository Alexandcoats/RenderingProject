#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <iostream>
#include "Map.hpp"
#include "Pipeline.hpp"

class Application {
	const int INIT_WIDTH = 800, INIT_HEIGHT = 600;
	GLFWwindow* window;
	Map map;
	Pipeline* pipeline;

public:
	void run() {
		map.initMap("./textures/pieces2.png");
		map.saveMap("./textures/output.png");
		initWindow();
		mainLoop();
	}

	void mainLoop() {
		float speed_modifier = 1.0f;
		while (!glfwWindowShouldClose(window)) {
			auto startTime = std::chrono::high_resolution_clock::now();
			glfwPollEvents();

			// Do stuff

			auto endTime = std::chrono::high_resolution_clock::now();
			float delta = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() / 1000.0f;
			float speed_modifier = 1.0f / delta / 60.0f;
		}

		// Cleanup

		//glfwDestroyWindow(window);
		glfwTerminate();
	}

	void initWindow() {
		if(!glfwInit()) throw std::runtime_error("Failed to initialize GLFW");
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		window = glfwCreateWindow(INIT_WIDTH, INIT_HEIGHT, "Rendering Lunch and Learn", nullptr, nullptr);

		if (!window) {
			glfwTerminate();
			throw std::runtime_error("Failed to create GLFW window");
		}

		glfwSetWindowUserPointer(window, this);
		glfwSetWindowSizeCallback(window, onWindowResized);
	}

	void initPipeline() {
		Shader VertShader{ "./shaders/basic_shader.vert", GL_VERTEX_SHADER };
		Shader FragShader{ "./shaders/basic_shader.frag", GL_FRAGMENT_SHADER };

		pipeline = new Pipeline{&VertShader, &FragShader};
		pipeline->use();
	}

	void initInput() {
		glfwSetKeyCallback(window, handleKeyInput);
	}

	static void onWindowResized(GLFWwindow * window, int width, int height) {
		if (width == 0 || height == 0) return;

		Application * app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));

		// Do anything needed when window resizes
	}

	static void handleKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods) {
		Application * app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));

		// Do anything needed when there is keyboard input
	}
};

int main() {
	Application app;
	try {
		app.run();
	}
	catch (const std::runtime_error & e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}