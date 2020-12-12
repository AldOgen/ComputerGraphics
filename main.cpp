#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "libs/Texture.h"
#include "libs/Shader.h"
#include "libs/Camera.h"


void InitGLFW() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
}


void GLADLoader() {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		exit(-1);
	}
}


struct WindowContext {
public:
	static void FramebufferSizeCallback(GLFWwindow *window, GLint width, GLint height) {
		glViewport(0, 0, width, height);
	}

	static void CursorPosCallback(GLFWwindow *window, GLdouble x_pos, GLdouble y_pos) {
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

	static void ScrollCallback(GLFWwindow *window, GLdouble x_offset, GLdouble y_offset) {
		camera->ProcessMouseScroll(static_cast<GLfloat>(y_offset));
	}

	static void InitializeWindowContext(CameraFly& camera) {
		WindowContext::camera = &camera;
	}

	static CameraFly *camera;
	static std::optional<GLfloat> last_x;
	static std::optional<GLfloat> last_y;
};

CameraFly *WindowContext::camera = nullptr;
std::optional<GLfloat> WindowContext::last_x = std::nullopt;
std::optional<GLfloat> WindowContext::last_y = std::nullopt;


class Window {
private:
	static constexpr GLuint SCR_WIDTH = 1200;
	static constexpr GLuint SCR_HEIGHT = 800;

private:
	GLFWwindow *window = nullptr;
	CameraFly camera;

	GLfloat delta_time = 0.0f;
	GLfloat last_frame = 0.0f;

public:
	Window() = default;

	void Initialize(const std::string& title) {
        window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, title.c_str(), nullptr, nullptr);
		if (window == nullptr) {
			std::cerr << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			exit(-1);
		}
		WindowContext::InitializeWindowContext(camera);
		glfwMakeContextCurrent(window);
		glfwSetFramebufferSizeCallback(window, WindowContext::FramebufferSizeCallback);
		glfwSetCursorPosCallback(window, WindowContext::CursorPosCallback);
		glfwSetScrollCallback(window, WindowContext::ScrollCallback);
	}


	void Rendering() {
		std::vector<ShaderLoadInfo> shareds_info = { {"./scr/Shaders/TestVertexShader.hlsl", GL_VERTEX_SHADER},
													 {"./scr/Shaders/TestFragmentShader.hlsl", GL_FRAGMENT_SHADER} };
		ShaderPipe shaderProgram = CreateShaderProgram(shareds_info);
		Texture2D texture;
		texture.LoadTexture("./textures/wooden_container.jpg");
	
		float vertices[] = {
			// координаты          // текстурные координаты
			 -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
			  0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
			  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			 -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

			 -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			  0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			  0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			  0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			 -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
			 -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

			 -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			 -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			 -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			  0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			  0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			  0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			  0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			  0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			 -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			  0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
			  0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			  0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			 -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			 -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

			 -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			  0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			  0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			 -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
			 -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
		};
		unsigned int indices[] = {
			0, 1, 3, // первый треугольник
			1, 2, 3  // второй треугольник
		};

		glm::vec3 cubePositions[] = {
			 glm::vec3(0.0f,  0.0f,  0.0f),
			 glm::vec3(2.0f,  5.0f, -15.0f),
			 glm::vec3(-1.5f, -2.2f, -2.5f),
			 glm::vec3(-3.8f, -2.0f, -12.3f),
			 glm::vec3(2.4f, -0.4f, -3.5f),
			 glm::vec3(-1.7f,  3.0f, -7.5f),
			 glm::vec3(1.3f, -2.0f, -2.5f),
			 glm::vec3(1.5f,  2.0f, -2.5f),
			 glm::vec3(1.5f,  0.2f, -1.5f),
			 glm::vec3(-1.3f,  1.0f, -1.5f)
		};

		unsigned int VBO, VAO, EBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// координатные атрибуты
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);


		glEnable(GL_DEPTH_TEST);

		shaderProgram.UseShaderPipe();
		shaderProgram.SetInt("Texture", 0);

		while (!glfwWindowShouldClose(window)) {
			GLfloat current_frame = glfwGetTime();
			delta_time = current_frame - last_frame;

			KeyboardInput();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			texture.UseTexture(0);

			glm::mat4 model = glm::mat4(1.0f);
			glm::mat4 view = camera.GetViewMatrix();
			glm::mat4 projection = glm::mat4(1.0f);
			model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
			//view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
			projection = glm::perspective(glm::radians(camera.GetZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

			shaderProgram.SetMat4("model", model);
			shaderProgram.SetMat4("view", view);
			shaderProgram.SetMat4("projection", projection);

			glBindVertexArray(VAO);
			for (unsigned int i = 0; i < 10; i++) {
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, cubePositions[i]);
				float angle = 20.0f * i;
				model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
				shaderProgram.SetMat4("model", model);

				glDrawArrays(GL_TRIANGLES, 0, 36);
			}

			glBindVertexArray(0);

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
		glfwTerminate();
	}

	void KeyboardInput() {
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
};



int main(int argc, char **argv)
{
	InitGLFW();
	Window window;
	window.Initialize("test");
	GLADLoader();
	window.Rendering();
	return 0;
}
