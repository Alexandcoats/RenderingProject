#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <iostream>
#include "Map.hpp"
#include "Pipeline.hpp"
#include "FrameBuffer.hpp"
#include "Texture.hpp"

class Application {
	const int INIT_WIDTH = 800, INIT_HEIGHT = 600;
	int width, height;
	GLFWwindow* window;
	Map map;
	Pipeline* pipeline;
	FrameBuffer* frameBuffer;

public:
	void run() {
		width = INIT_WIDTH;
		height = INIT_HEIGHT;
		//map.initMap("./textures/pieces2.png");
		//map.saveMap("./textures/output.png");
		
		initWindow();
		frameBuffer = new FrameBuffer(glm::ivec2(INIT_WIDTH, INIT_HEIGHT));
		initPipeline();
		mainLoop();
	}

	void mainLoop() {
		float speed_modifier = 1.0f;
		while (!glfwWindowShouldClose(window)) {
			auto startTime = std::chrono::high_resolution_clock::now();
			glfwPollEvents();

			frameBuffer->startWrite();

			pipeline->draw();

			frameBuffer->endWrite();

			frameBuffer->copyToSystemFramebuffer(glm::ivec2(width, height), GL_COLOR_BUFFER_BIT, GL_NEAREST);

			frameBuffer->bindDepthTexture(GL_TEXTURE5);

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
		//glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		window = glfwCreateWindow(INIT_WIDTH, INIT_HEIGHT, "Rendering Lunch and Learn", nullptr, nullptr);

		glfwMakeContextCurrent(window);

		if (!window) {
			glfwTerminate();
			throw std::runtime_error("Failed to create GLFW window");
		}

		glfwSetWindowUserPointer(window, this);
		glfwSetWindowSizeCallback(window, onWindowResized);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			throw std::runtime_error("Failed initialize OpenGL extensions");
		}

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
	}

	void initPipeline() {
		Shader VertShader{ "./shaders/basic_shader.vert", GL_VERTEX_SHADER };
		Shader FragShader{ "./shaders/basic_shader.frag", GL_FRAGMENT_SHADER };

		Geometry geometry{};

		pipeline = new Pipeline{ &VertShader, &FragShader };
		pipeline->use();
		pipeline->bindIndices(geometry.indices);

		VertexArrayMacro macro{};
		macro.bind();
		macro.createBuffer("vertices", sizeof(Vertex) * geometry.vertices.size(), &geometry.vertices[0]);
		macro.bindVertexAttribute("vertices", pipeline->map["pos"][0], 3, 2, (void*)0);
		macro.bindVertexAttribute("vertices", pipeline->map["texCoord"][0], 2, 3, (void*)3);
		macro.unbind();

		pipeline->setMacro(&macro);

		Texture test(pipeline->map["texSampler"][0], 0);
		int height, width, channels;
		unsigned char * pixels = stbi_load("./textures/texture.jpg", &width, &height, &channels, 4);
		test.push(pixels, width, height);
	}

	void initInput() {
		glfwSetKeyCallback(window, handleKeyInput);
	}

	static void onWindowResized(GLFWwindow * window, int width, int height) {
		if (width == 0 || height == 0) return;

		Application * app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));

		app->width = width;
		app->height = height;

		glViewport(0, 0, width, height);

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
		std::cout << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}