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
#include "libs/Model.h"


void InitGLFW() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
}


void GLADLoader() {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "ERROR::GLAD::FAILED_TO_INITIALIZE_GLAD" << std::endl;
		exit(-1);
	}
}


struct WindowContext {
public:
	WindowContext() = delete;

	~WindowContext() = delete;

	WindowContext(const WindowContext&) = delete;

	WindowContext& operator=(const WindowContext&) = delete;

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

public:
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
	CameraFly camera{ glm::vec3(0.0f, 0.0f, 3.0f) };

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
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwMakeContextCurrent(window);
		glfwSetFramebufferSizeCallback(window, WindowContext::FramebufferSizeCallback);
		glfwSetCursorPosCallback(window, WindowContext::CursorPosCallback);
		glfwSetScrollCallback(window, WindowContext::ScrollCallback);
	}


	void Rendering() {
		std::vector<ShaderLoadInfo> shareds_cube_info = { {"./scr/Shaders/ModelVertexShader.hlsl", GL_VERTEX_SHADER},
														   {"./scr/Shaders/ModelFragmentShader.hlsl", GL_FRAGMENT_SHADER} };
		std::vector<ShaderLoadInfo> shareds_light_info = { {"./scr/Shaders/LightVertexShader.hlsl", GL_VERTEX_SHADER},
														   {"./scr/Shaders/LightFragmentShader.hlsl", GL_FRAGMENT_SHADER} };
		std::vector<ShaderLoadInfo> shareds_floor_info = { {"./scr/Shaders/FloorVertexShader.hlsl", GL_VERTEX_SHADER},
														   {"./scr/Shaders/FloorFragmentShader.hlsl", GL_FRAGMENT_SHADER} };

		ShaderPipe shader_cube_program = CreateShaderProgram(shareds_cube_info.begin(), shareds_cube_info.end());
		ShaderPipe shader_light_program = CreateShaderProgram(shareds_light_info.begin(), shareds_light_info.end());
		ShaderPipe shareds_floor_program = CreateShaderProgram(shareds_floor_info.begin(), shareds_floor_info.end());

		std::vector<Vertex> vertexes_cube;
		vertexes_cube.push_back(CreateVertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f)));
		vertexes_cube.push_back(CreateVertex(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f)));
		vertexes_cube.push_back(CreateVertex(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f)));
		vertexes_cube.push_back(CreateVertex(glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 1.0f)));
		vertexes_cube.push_back(CreateVertex(glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 1.0f)));
		vertexes_cube.push_back(CreateVertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 0.0f)));

		vertexes_cube.push_back(CreateVertex(glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f,  0.0f, 1.0f), glm::vec2(0.0f, 0.0f)));
		vertexes_cube.push_back(CreateVertex(glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(0.0f,  0.0f, 1.0f), glm::vec2(1.0f, 0.0f)));
		vertexes_cube.push_back(CreateVertex(glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  0.0f, 1.0f), glm::vec2(1.0f, 1.0f)));
		vertexes_cube.push_back(CreateVertex(glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  0.0f, 1.0f), glm::vec2(1.0f, 1.0f)));
		vertexes_cube.push_back(CreateVertex(glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  0.0f, 1.0f), glm::vec2(0.0f, 1.0f)));
		vertexes_cube.push_back(CreateVertex(glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f,  0.0f, 1.0f), glm::vec2(0.0f, 0.0f)));

		vertexes_cube.push_back(CreateVertex(glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f)));
		vertexes_cube.push_back(CreateVertex(glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f)));
		vertexes_cube.push_back(CreateVertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f)));
		vertexes_cube.push_back(CreateVertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f)));
		vertexes_cube.push_back(CreateVertex(glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f)));
		vertexes_cube.push_back(CreateVertex(glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f)));

		vertexes_cube.push_back(CreateVertex(glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f)));
		vertexes_cube.push_back(CreateVertex(glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f)));
		vertexes_cube.push_back(CreateVertex(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f)));
		vertexes_cube.push_back(CreateVertex(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f)));
		vertexes_cube.push_back(CreateVertex(glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f)));
		vertexes_cube.push_back(CreateVertex(glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f)));

		vertexes_cube.push_back(CreateVertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 1.0f)));
		vertexes_cube.push_back(CreateVertex(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 1.0f)));
		vertexes_cube.push_back(CreateVertex(glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 0.0f)));
		vertexes_cube.push_back(CreateVertex(glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 0.0f)));
		vertexes_cube.push_back(CreateVertex(glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 0.0f)));
		vertexes_cube.push_back(CreateVertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 1.0f)));

		vertexes_cube.push_back(CreateVertex(glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 1.0f)));
		vertexes_cube.push_back(CreateVertex(glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 1.0f)));
		vertexes_cube.push_back(CreateVertex(glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 0.0f)));
		vertexes_cube.push_back(CreateVertex(glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 0.0f)));
		vertexes_cube.push_back(CreateVertex(glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 0.0f)));
		vertexes_cube.push_back(CreateVertex(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)));

		std::vector<GLuint> indexes_cube = {
			0, 1, 3,
			1, 2, 3
		};

		std::vector<Vertex> vertexes_floor;
	
		vertexes_floor.push_back(CreateVertex(glm::vec3(5.0f, -0.5f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(2.0f, 0.0f)));
		vertexes_floor.push_back(CreateVertex(glm::vec3(-5.0f, -0.5f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)));
		vertexes_floor.push_back(CreateVertex(glm::vec3(-5.0f, -0.5f, -5.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 2.0f)));
		vertexes_floor.push_back(CreateVertex(glm::vec3(5.0f, -0.5f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(2.0f, 0.0f)));
		vertexes_floor.push_back(CreateVertex(glm::vec3(-5.0f, -0.5f, -5.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 2.0f)));
		vertexes_floor.push_back(CreateVertex(glm::vec3(5.0f, -0.5f, -5.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(2.0f, 2.0f)));

		std::vector<GLuint> indexes_floor = {
			0, 1, 3,
			1, 2, 3
		};

		Texture2D texture;

		std::vector<Texture2D> textures_cube;
		texture.LoadTexture("./textures/1-5.png", std::string(Texture2D::DIFFUSE_TEXTURE), true);
		textures_cube.push_back(texture);
		texture.LoadTexture("./textures/3-4.png", std::string(Texture2D::SPECULAR_TEXTURE), true);
		textures_cube.push_back(texture);

		std::vector<Texture2D> textures_floor;
		texture.LoadTexture("./textures/floor.jpeg", std::string(Texture2D::DIFFUSE_TEXTURE));
		textures_floor.push_back(texture);


		Mesh cube{ vertexes_cube, indexes_cube, textures_cube };
		cube.InitializeMesh();

		Mesh floor{ vertexes_floor, indexes_floor, textures_floor };
		floor.InitializeMesh();

		Mesh light{ vertexes_cube, indexes_cube, std::vector<Texture2D>{} };
		light.InitializeMesh();


		glEnable(GL_DEPTH_TEST);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		

		glm::vec3 light_ambient(0.1f, 0.1f, 0.1f);
		glm::vec3 light_diffuse(0.6f, 0.6f, 0.6f);
		glm::vec3 light_specular(0.9f, 0.9f, 0.9f);

		glm::vec3 light_position(1.2f, 1.0f, 2.0f);
		glm::vec3 floor_position(0.0f, -1.0f, 0.0f);
		glm::vec3 cube1_position(2.0f, -1.0f, 5.0f);

		GLfloat flare = 128;

		while (!glfwWindowShouldClose(window)) {
			GLfloat current_frame = glfwGetTime();
			delta_time = current_frame - last_frame;

			KeyboardInput();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

			glm::mat4 view = camera.GetViewMatrix();

			shareds_floor_program.UseShaderPipe();
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, floor_position);
			shareds_floor_program.SetMat4("projection", projection);
			shareds_floor_program.SetMat4("view", view);
			shareds_floor_program.SetMat4("model", model);

			floor.DrawMesh(shareds_floor_program);


			shader_cube_program.UseShaderPipe();
			shader_cube_program.SetVec3("light_position", light_position);
			shader_cube_program.SetVec3("light_ambient", light_ambient);
			shader_cube_program.SetVec3("light_diffuse", light_diffuse);
			shader_cube_program.SetVec3("light_specular", light_specular);
			shader_cube_program.SetVec3("view_position", camera.GetPosition());
			shader_cube_program.SetFloat("flare", flare);

			model = glm::mat4(1.0f);
			model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
			shader_cube_program.SetMat4("projection", projection);
			shader_cube_program.SetMat4("view", view);
			shader_cube_program.SetMat4("model", model);

			cube.DrawMesh(shader_cube_program);

			model = glm::mat4(1.0f);
			model = glm::translate(model, cube1_position);
			shader_cube_program.SetMat4("model", model);

			cube.DrawMesh(shader_cube_program);

			shader_light_program.UseShaderPipe();
			model = glm::mat4(1.0f);
			model = glm::translate(model, light_position);
			model = glm::scale(model, glm::vec3(0.2f));
			shader_light_program.SetMat4("projection", projection);
			shader_light_program.SetMat4("view", view);
			shader_light_program.SetMat4("model", model);
			light.DrawMesh(shader_light_program);


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


int main(int argc, char **argv) {
	InitGLFW();
	Window window;
	window.Initialize("test");
	GLADLoader();
	window.Rendering();
	return 0;
}
