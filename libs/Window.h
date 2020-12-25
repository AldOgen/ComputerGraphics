#pragma once
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Scene.h"
#include "Camera.h"

/* Контекс окна, хранящий состояние предыдущего кадра и обработчики прерываний пользователя */
static struct WindowContext {
public:
	// Синглтон опции
	WindowContext() = delete;
	~WindowContext() = delete;
	WindowContext(const WindowContext&) = delete;
	WindowContext& operator=(const WindowContext&) = delete;

	static void FramebufferSizeCallback(GLFWwindow *window, GLint width, GLint height);
	static void CursorPosCallback(GLFWwindow *window, GLdouble x_pos, GLdouble y_pos);
	static void ScrollCallback(GLFWwindow *window, GLdouble x_offset, GLdouble y_offset);
	static void InitializeWindowContext(CameraFly& camera);

public:
	static CameraFly *camera;
	static std::optional<GLfloat> last_x;
	static std::optional<GLfloat> last_y;
};


/* Окно отрисовки сцены */
class Window {
private:
	GLFWwindow *window = nullptr;
	CameraFly camera{ glm::vec3(0.0f, 0.0f, 3.0f) };

	GLfloat delta_time = 0.0f;
	GLfloat last_frame = 0.0f;

public:
	static constexpr GLuint SCR_WIDTH = 1200;
	static constexpr GLuint SCR_HEIGHT = 800;
	static constexpr GLuint SHADOW_MAP_WIDTH = 4096;
	static constexpr GLuint SHADOW_MAP_HEIGHT = 4069;
	static constexpr GLuint SHADOW_CUBE_MAP_WIDTH = 2048;
	static constexpr GLuint SHADOW_CUBE_MAP_HEIGHT = 2048;

public:
	Window() = default;

	void Initialize(const std::string& title);
	void Rendering(Scene &scene, std::vector<ShaderPipe> shaders_shadow, std::vector<ShaderPipe> shaders_scene);
	void KeyboardInput();
};




