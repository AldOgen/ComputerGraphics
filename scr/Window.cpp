#include "../libs/Window.h"


CameraFly *WindowContext::camera = nullptr;
std::optional<GLfloat> WindowContext::last_x = std::nullopt;
std::optional<GLfloat> WindowContext::last_y = std::nullopt;

void WindowContext::FramebufferSizeCallback(GLFWwindow *window, GLint width, GLint height) {
	glViewport(0, 0, width, height);
}

void WindowContext::CursorPosCallback(GLFWwindow *window, GLdouble x_pos, GLdouble y_pos) {
	if (!last_x || !last_y) {
		last_x = x_pos;
		last_y = y_pos;
	}
	GLfloat x_offset = static_cast<GLfloat>(x_pos) - *last_x;
	GLfloat y_offset = *last_y - static_cast<GLfloat>(y_pos);

	last_x = x_pos;
	last_y = y_pos;

	camera->ProcessMouseMovement(x_offset, y_offset);
}

void WindowContext::ScrollCallback(GLFWwindow *window, GLdouble x_offset, GLdouble y_offset) {
	camera->ProcessMouseScroll(static_cast<GLfloat>(y_offset));
}

void WindowContext::InitializeWindowContext(CameraFly& camera) {
	WindowContext::camera = &camera;
}


void Window::Initialize(const std::string& title) {
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, title.c_str(), nullptr, nullptr);
	if (window == nullptr) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(-1);
	}
	WindowContext::InitializeWindowContext(camera);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, WindowContext::FramebufferSizeCallback);
	glfwSetCursorPosCallback(window, WindowContext::CursorPosCallback);
	glfwSetScrollCallback(window, WindowContext::ScrollCallback);
}


void Window::Rendering(Scene &scene, std::vector<ShaderPipe> shaders_shadow, std::vector<ShaderPipe> shaders_scene) {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	scene.SetCamera(&camera);

	while (!glfwWindowShouldClose(window)) {
		GLfloat current_frame = glfwGetTime();
		delta_time = current_frame - last_frame;
		last_frame = current_frame;
		KeyboardInput();

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, scene.GetShadowFBO());
		glClear(GL_DEPTH_BUFFER_BIT);

		scene.Rendering(SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, shaders_shadow, delta_time, Scene::SwitchRender::SHADOW_MAP);

		//glViewport(0, 0, SHADOW_CUBE_MAP_WIDTH, SHADOW_CUBE_MAP_HEIGHT);
		//glBindFramebuffer(GL_FRAMEBUFFER, shadow_cube_FBO);
		//glClear(GL_DEPTH_BUFFER_BIT);

		//scene.Rendering(SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, shaders_shadow, Scene::SwitchRender::SHADOW_MAP);

		//Render(camera, SHADOW_CUBE_MAP_WIDTH, SHADOW_CUBE_MAP_HEIGHT, meshs_scene, shadow_map, shadow_cube_maps, 
		//	shaders_shadow_cube, lights_point, light_directed, transforms, delta_time, SwitchRender::SHADOW_CUBE);


		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		scene.Rendering(SCR_WIDTH, SCR_HEIGHT, shaders_scene, delta_time, Scene::SwitchRender::SCENE);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
}

void Window::KeyboardInput() {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.ProcessKeyboard(CameraFly::Movement::FORWARD, delta_time);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.ProcessKeyboard(CameraFly::Movement::BACKWARD, delta_time);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.ProcessKeyboard(CameraFly::Movement::LEFT, delta_time);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.ProcessKeyboard(CameraFly::Movement::RIGHT, delta_time);
	}
}