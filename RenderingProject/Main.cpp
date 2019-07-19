#pragma once
#include <GLFW/glfw3.h>
#include <chrono>
#include <iostream>
#include "Map.hpp"

class Application {
	const int INIT_WIDTH = 800, INIT_HEIGHT = 600;
	GLFWwindow* window;
	Map map;

public:
	void run() {
		map.initMap("./textures/pieces2.png");
		map.saveMap("./textures/output.png");
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

		glfwDestroyWindow(window);
	}

	void initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		window = glfwCreateWindow(INIT_WIDTH, INIT_HEIGHT, "Rendering Lunch and Learn", nullptr, nullptr);

		glfwSetWindowUserPointer(window, this);
		glfwSetWindowSizeCallback(window, onWindowResized);
	}

	void initInput() {
		glfwSetKeyCallback(window, handleKeyInput);
	}

	static void onWindowResized(GLFWwindow * window, int width, int height) {
		if (width == 0 || height == 0) return;

		Application * app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
	}

	static void handleKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods) {
		Application * app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
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