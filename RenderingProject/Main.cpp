#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <iostream>
#include "Map.hpp"
#include "Camera.hpp"
#include "TilesetReader.hpp"
#include "FrameBuffers.hpp"

static const float WALK_SPEED = 0.1f, SPRINT_SPEED = 1.0f, FLY_SPEED = 1.0f, CAMERA_SPEED = 0.002f;

class Application {
	const int INIT_WIDTH = 800, INIT_HEIGHT = 600;
	int width, height;
	GLFWwindow* window;
	Map map;
	GeometryPipeline* pipelineGbuffer;
	DeferredPipeline* pipelineBRDF;
	DeferredPipeline* pipelineDebugGBuff;
	GeometryPipeline* pipelineShadows;
	GeometryPipeline* pipelineNormals;
	Camera* camera;
	GBuffer* gBuffer;
	ShadowBuffer* shadowBuffer;
	std::vector<Light> lights;
	std::vector<Tile> tiles;

	bool keyboardState[GLFW_KEY_LAST] = { false };
	float speedModifier = 1.0f;
	double cursorPos[2];

	bool walkMode = false;
	bool showNormals = false;
	bool debugGBuffer = false;

public:
	void run() {
		width = INIT_WIDTH;
		height = INIT_HEIGHT;
		map.initMap("./textures/pieces2.png");
		map.saveMap("./textures/output.png");
		
		initWindow();
		initPipelines();
		initTiles();
		initShadowMap();
		initInput();
		mainLoop();
	}

	void mainLoop() {
		float speed_modifier = 1.0f;
		while (!glfwWindowShouldClose(window)) {
			auto startTime = std::chrono::high_resolution_clock::now();

			int width, height;
			glfwGetFramebufferSize(window, &width, &height);

			gBuffer->startWrite();

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			pipelineGbuffer->use();

			glUniformMatrix4fv(pipelineGbuffer->getAttributeLocation("p"), 1, GL_FALSE, &camera->projection[0][0]);
			glUniformMatrix4fv(pipelineGbuffer->getAttributeLocation("v"), 1, GL_FALSE, &camera->view[0][0]);

			for (const auto & tile : tiles) {
				tile.draw(pipelineGbuffer);
			}

			gBuffer->endWrite();

			if (debugGBuffer) {

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				pipelineDebugGBuff->use();

				gBuffer->bindTexture((uint32_t)GBuffer::Texture::Normal, GL_TEXTURE0);
				gBuffer->bindTexture((uint32_t)GBuffer::Texture::Color, GL_TEXTURE1);
				gBuffer->bindTexture((uint32_t)GBuffer::Texture::MSSR, GL_TEXTURE2);
				gBuffer->bindTexture((uint32_t)GBuffer::Texture::SASS, GL_TEXTURE3);
				gBuffer->bindTexture((uint32_t)GBuffer::Texture::CC, GL_TEXTURE4);
				gBuffer->bindTexture((uint32_t)GBuffer::Texture::Depth, GL_TEXTURE5);

				glUniformMatrix4fv(pipelineDebugGBuff->getAttributeLocation("p"), 1, GL_FALSE, &camera->projection[0][0]);
				glUniformMatrix4fv(pipelineDebugGBuff->getAttributeLocation("v"), 1, GL_FALSE, &camera->view[0][0]);

				pipelineDebugGBuff->draw();
			}
			else {

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				pipelineBRDF->use();

				gBuffer->bindTexture((uint32_t)GBuffer::Texture::Normal, GL_TEXTURE0);
				gBuffer->bindTexture((uint32_t)GBuffer::Texture::Color, GL_TEXTURE1);
				gBuffer->bindTexture((uint32_t)GBuffer::Texture::MSSR, GL_TEXTURE2);
				gBuffer->bindTexture((uint32_t)GBuffer::Texture::SASS, GL_TEXTURE3);
				gBuffer->bindTexture((uint32_t)GBuffer::Texture::CC, GL_TEXTURE4);
				gBuffer->bindTexture((uint32_t)GBuffer::Texture::Depth, GL_TEXTURE5);

				shadowBuffer->bindTexture(GL_TEXTURE6);

				glUniformMatrix4fv(pipelineBRDF->getAttributeLocation("p"), 1, GL_FALSE, &camera->projection[0][0]);
				glUniformMatrix4fv(pipelineBRDF->getAttributeLocation("v"), 1, GL_FALSE, &camera->view[0][0]);

				glUniform1i(pipelineBRDF->getAttributeLocation("numLights"), lights.size());

				for (int i = 0; i < lights.size(); ++i) {
					glUniform3fv(pipelineBRDF->getAttributeLocation("lights[" + std::to_string(i) + "].pos"), 1, &lights[i].pos[0]);
					glUniform3fv(pipelineBRDF->getAttributeLocation("lights[" + std::to_string(i) + "].color"), 1, &lights[i].color[0]);
					glUniform1f(pipelineBRDF->getAttributeLocation("lights[" + std::to_string(i) + "].brightness"), lights[i].brightness);
				}

				glUniform3fv(pipelineBRDF->getAttributeLocation("camPos"), 1, &camera->pos[0]);

				pipelineBRDF->draw();
			}

			gBuffer->copyToSystemFramebuffer();

			if (showNormals) {
				pipelineNormals->use();
				glUniformMatrix4fv(pipelineNormals->getAttributeLocation("p"), 1, GL_FALSE, &camera->projection[0][0]);
				glUniformMatrix4fv(pipelineNormals->getAttributeLocation("v"), 1, GL_FALSE, &camera->view[0][0]);
				for (const auto & tile : tiles) {
					tile.draw(pipelineNormals);
				}
			}

			glfwSwapBuffers(window);
			glfwPollEvents();

			auto endTime = std::chrono::high_resolution_clock::now();
			float delta = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() / 1000.0f;
			speedModifier = 1.0f / delta / 60.0f;

			// Handle camera stuff
			// We should move this somewhere better maybe
			float speed;
			if (walkMode) {
				if (keyboardState[GLFW_KEY_LEFT_SHIFT])
					speed = SPRINT_SPEED;
				else
					speed = WALK_SPEED;
			}
			else speed = FLY_SPEED;
			if (keyboardState[GLFW_KEY_W]) {
				if (walkMode) camera->translate(Direction::WalkForward, speed*speedModifier);
				else camera->translate(Direction::Forward, speed*speedModifier);
			}
			if (keyboardState[GLFW_KEY_S]) {
				if (walkMode) camera->translate(Direction::WalkBackward, speed*speedModifier);
				else camera->translate(Direction::Backward, speed*speedModifier);
			}
			if (keyboardState[GLFW_KEY_A]) {
				camera->translate(Direction::Left, speed*speedModifier);
			}
			if (keyboardState[GLFW_KEY_D]) {
				camera->translate(Direction::Right, speed*speedModifier);
			}

			camera->update();

			//glClearColor(camera->pos.x/1000.0f, camera->pos.y / 1000.0f, camera->pos.z / 1000.0f, 1.0f);
			//std::cout << "(" << camera->pos.x << ", " << camera->pos.y << ", " << camera->pos.z << ") (" << camera->dir.x << ", " << camera->dir.y << ", " << camera->dir.z << ")" << std::endl;
		}

		// Cleanup

		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void initWindow() {

		glfwSetErrorCallback(onWindowError);

		if(!glfwInit()) throw std::runtime_error("Failed to initialize GLFW");
		//glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
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
		glfwSwapInterval(1);

		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_CULL_FACE);

		camera = new Camera(width, height, glm::vec3(-5.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	void initPipelines() {
		Shader GVertShader{ "./shaders/G_shader.vert", GL_VERTEX_SHADER };
		Shader GFragShader{ "./shaders/G_shader.frag", GL_FRAGMENT_SHADER };

		pipelineGbuffer = new GeometryPipeline{ &GVertShader, &GFragShader };

		gBuffer = new GBuffer{glm::ivec2(width, height)};

		Shader DeferredVertShader{ "./shaders/deferred_shader.vert", GL_VERTEX_SHADER };

		if (!debugGBuffer) {

			Shader BRDFFragShader{ "./shaders/BRDF_shader.frag", GL_FRAGMENT_SHADER };

			pipelineBRDF = new DeferredPipeline{ &DeferredVertShader, &BRDFFragShader };
			pipelineBRDF->use();

			glUniform1i(pipelineBRDF->getAttributeLocation("gNormal"), 0);
			glUniform1i(pipelineBRDF->getAttributeLocation("gColor"), 1);
			glUniform1i(pipelineBRDF->getAttributeLocation("gMSSR"), 2);
			glUniform1i(pipelineBRDF->getAttributeLocation("gSASS"), 3);
			glUniform1i(pipelineBRDF->getAttributeLocation("gCC"), 4);
			glUniform1i(pipelineBRDF->getAttributeLocation("gDepth"), 5);

			glUniform1i(pipelineBRDF->getAttributeLocation("shadowMap"), 6);
		}
		else {

			Shader GBuffFragShader{ "./shaders/GBuff_shader.frag", GL_FRAGMENT_SHADER };

			pipelineDebugGBuff = new DeferredPipeline{ &DeferredVertShader, &GBuffFragShader };
			pipelineDebugGBuff->use();

			glUniform1i(pipelineDebugGBuff->getAttributeLocation("gNormal"), 0);
			glUniform1i(pipelineDebugGBuff->getAttributeLocation("gColor"), 1);
			glUniform1i(pipelineDebugGBuff->getAttributeLocation("gMSSR"), 2);
			glUniform1i(pipelineDebugGBuff->getAttributeLocation("gSASS"), 3);
			glUniform1i(pipelineDebugGBuff->getAttributeLocation("gCC"), 4);
			glUniform1i(pipelineDebugGBuff->getAttributeLocation("gDepth"), 5);
		}

		Shader ShadowVertShader{ "./shaders/shadow_shader.vert", GL_VERTEX_SHADER };
		Shader ShadowFragShader{ "./shaders/shadow_shader.frag", GL_FRAGMENT_SHADER };

		pipelineShadows = new GeometryPipeline{ &ShadowVertShader, &ShadowFragShader };
		pipelineShadows->use();

		glUniform1i(pipelineShadows->getAttributeLocation("shadowMap"), 0);

		Shader NormVertShader{ "./shaders/normals_shader.vert", GL_VERTEX_SHADER };
		Shader NormGeomShader{ "./shaders/normals_shader.geom", GL_GEOMETRY_SHADER };
		Shader NormFragShader{ "./shaders/normals_shader.frag", GL_FRAGMENT_SHADER };

		pipelineNormals = new GeometryPipeline{ &NormVertShader, &NormGeomShader, &NormFragShader };
	}

	void initTiles() {
		readOBJ("./models/tileset.obj", "./models", "./models/metadata.json", tiles);

		for (int i = 0; i < map.map.size(); ++i) {
			for (int j = 0; j < map.map[0].size(); ++j) {
				if (map.map[i][j]->pieceInd) {
					glm::mat4 flip = glm::scale(glm::mat4(), glm::vec3(map.map[i][j]->flp ? -1.0f : 1.0f, 1.0f, 1.0f));
					glm::mat4 rotate = glm::rotate(glm::mat4(), (glm::pi<float>() / 2.0f) * map.map[i][j]->rot, glm::vec3(0.0f, 1.0f, 0.0f));
					glm::mat4 translate = glm::translate(glm::mat4(), glm::vec3((float)j * 20.0f, 0.0f, (float)i * 20.0f));
					glm::mat4 m = translate * rotate * flip;
					tiles[map.map[i][j]->pieceInd - 1].instances.push_back(m);
					for (auto light : tiles[map.map[i][j]->pieceInd - 1].lights) {
						light.pos = glm::vec3(m * glm::vec4(light.pos, 1.0f));
						lights.push_back(light);
					}
				}
			}
		}

		for (auto & tile : tiles) {
			tile.setInstances();
		}
	}

	void initShadowMap() {
		glCullFace(GL_FRONT);
		shadowBuffer = new ShadowBuffer(lights.size());

		pipelineShadows->use();

		shadowBuffer->startWrite();

		glUniform1i(pipelineShadows->getAttributeLocation("octSize"), shadowBuffer->octSize);

		float frustumY = 2.0f * glm::distance(glm::vec3(0.33f), glm::vec3(0.0f, 1.0f, 0.0f));
		float frustumX = sqrt(2.0f);
		float fovy = 2.0f * glm::asin(frustumY/2.0f);
		float aspect = frustumX / frustumY;
		//float d = glm::degrees(fovy);
		//float h = 0.5f * frustumX * tan(glm::radians(60.0f));

		auto p = glm::perspective(fovy, aspect, 0.01f, 256.0f);
		glUniformMatrix4fv(pipelineShadows->getAttributeLocation("p"), 1, GL_FALSE, &p[0][0]);

		auto up = glm::vec3(0.0f, 1.0f, 0.0f);

		glm::vec3 dirVectors[8] = { {-1.0f, 1.0f, 1.0f}, // quad 1, forward oct
									{-1.0f, 1.0f, -1.0f}, // quad 1, rear oct
									{1.0f, 1.0f, 1.0f}, // quad 2, forward oct
									{1.0f, 1.0f, -1.0f}, // quad 2, rear oct
									{1.0f, -1.0f, 1.0f}, // quad 3 forward oct
									{1.0f, -1.0f, -1.0f}, // quad 3, rear oct
									{-1.0f, -1.0f, 1.0f}, // quad 4, forward oct
									{-1.0f, -1.0f, -1.0f} }; // quad 4, rear oct
		
		for (int l = 0; l < lights.size(); ++l) {

			shadowBuffer->bindLayer(0, l);

			auto pos = lights[l].pos;
			//auto pos = glm::vec3(10.0f, 3.0f, 10.0f);

			glUniform3fv(pipelineShadows->getAttributeLocation("lightPos"), 1, &pos[0]);

			for (int i = 0; i < 8; ++i) {
			
				glUniform1i(pipelineShadows->getAttributeLocation("octant"), i);

				glClear(GL_DEPTH_BUFFER_BIT);

				auto dir = glm::normalize(dirVectors[i]);

				auto v = glm::lookAt(pos, pos + dir, up);

				glUniformMatrix4fv(pipelineShadows->getAttributeLocation("v"), 1, GL_FALSE, &v[0][0]);

				for (const auto & tile : tiles) {
					tile.draw(pipelineShadows);
				}
			}

			//auto pixels = new float[shadowBuffer->octSize * shadowBuffer->octSize];
			//glGetTextureSubImage(shadowBuffer->octmapTex, 0, 0, 0, l, shadowBuffer->octSize, shadowBuffer->octSize, 1, GL_RGBA, GL_UNSIGNED_BYTE, sizeof(float) * shadowBuffer->octSize * shadowBuffer->octSize, pixels);

			//stbi_write_png(("shadowMap" + std::to_string(l) + ".png").c_str(), shadowBuffer->octSize, shadowBuffer->octSize, 4, pixels, sizeof(float) * shadowBuffer->octSize);
		}
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
		glCullFace(GL_BACK);
	}

	void initInput() {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		if (glfwRawMouseMotionSupported())
			glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

		glfwGetCursorPos(window, &cursorPos[0], &cursorPos[1]);

		glfwSetKeyCallback(window, handleKeyInput);
		glfwSetCursorPosCallback(window, handleCursorInput);
		glfwSetMouseButtonCallback(window, handleMouseInput);
	}

	static void APIENTRY glDebugOutput(GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar *message,
		const void *userParam)
	{
		// ignore non-significant error/warning codes
		if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

		std::cout << "---------------" << std::endl;
		std::cout << "Debug message (" << id << "): " << message << std::endl;

		switch (source)
		{
		case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
		case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
		case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
		} std::cout << std::endl;

		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
		case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
		case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
		case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
		case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
		case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
		case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
		} std::cout << std::endl;

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
		case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
		case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
		} std::cout << std::endl;
		std::cout << std::endl;
	}

	static void onWindowError(int error, const char* description)
	{
		std::cout << "Error: " << description << std::endl;
	}

	static void onWindowResized(GLFWwindow * window, int width, int height) {
		if (width == 0 || height == 0) return;

		Application * app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));

		app->width = width;
		app->height = height;

		app->gBuffer->resolution = glm::ivec2(width, height);

		app->camera->resize(width, height);

		// Do anything needed when window resizes
	}

	static void handleCursorInput(GLFWwindow* window, double xpos, double ypos) {
		Application * app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));

		app->camera->rotate(Axis::V, CAMERA_SPEED*app->speedModifier*(app->cursorPos[0] - xpos));
		float angle = app->camera->uAxisAngle();
		float yDelta = app->cursorPos[1] - ypos;
		if((angle > 0.1f && yDelta > 0) || (glm::pi<float>() - angle > 0.1f && yDelta < 0)) // This works okay unless you move the camera really fast, but whatevs
			app->camera->rotate(Axis::U, CAMERA_SPEED*app->speedModifier*(yDelta));

		app->cursorPos[0] = xpos;
		app->cursorPos[1] = ypos;

		app->camera->update();
	}

	static void handleKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods) {
		Application * app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));

		if (action == GLFW_PRESS) {
			app->keyboardState[key] = true;
			if (app->keyboardState[GLFW_KEY_ESCAPE] && glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				glfwSetCursorPosCallback(window, nullptr);
				glfwSetKeyCallback(window, nullptr);
				app->keyboardState[key] = false;
			}
			else if (app->keyboardState[GLFW_KEY_G]) {
				app->walkMode = !app->walkMode;
				if (app->walkMode) app->camera->pos = glm::vec3(10.0f, 3.0f, 10.0f);
			}
		}
		else if (action == GLFW_RELEASE) {
			app->keyboardState[key] = false;
		}
	}

	static void handleMouseInput(GLFWwindow* window, int button, int action, int mods)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwSetCursorPosCallback(window, handleCursorInput);
			glfwSetKeyCallback(window, handleKeyInput);
		}
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