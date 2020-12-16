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



void Render(const CameraFly& camera, GLfloat scr_wight, GLfloat scr_height,
	std::vector<Mesh>& objects, Texture2D& shadow_texture, std::vector<ShaderPipe>& shader_programs,
	std::vector<LightPoint>& lights_point, std::vector<LightDirected>& light_directed,
	std::vector<Transform>& transforms, bool shadow = false) {
	static glm::mat4 light_space;

	static glm::mat4 model;
	static glm::mat4 view;
	static glm::mat4 projection;

	if (shadow) {
		float near_plane = 1.0f, far_plane = 7.5f;
		glm::mat4 light_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		glm::mat4 light_view = glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		light_space = light_projection * light_view;

		shader_programs[0].UseShaderPipe();
		shader_programs[0].SetMat4("light_space", light_space);
		shadow_texture.UseTexture(shader_programs[0], "", 0);

		for (auto &object : objects) {
			object.DrawMesh(shader_programs[0]);
		}
		int a = 0;
	} else {
		projection = glm::perspective(glm::radians(camera.GetZoom()), scr_wight / scr_height, 0.1f, 100.0f);
		view = camera.GetViewMatrix();

		for (size_t idx = 0; idx < shader_programs.size(); ++idx) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, transforms[idx].translate);
			model = glm::rotate(model, glm::radians(transforms[idx].turn), glm::vec3(1.0f));
			model = glm::scale(model, transforms[idx].scale);

			shader_programs[idx].UseShaderPipe();
			shader_programs[idx].SetMat4("light_space", light_space);

			FigurePosition figure_position{ model, view, projection };

			figure_position.UseFigurePosition(shader_programs[idx]);

			for (size_t jdx = 0; jdx < lights_point.size(); ++jdx) {
				lights_point[jdx].UseLight(shader_programs[idx], jdx);
			}

			for (size_t jdx = 0; jdx < light_directed.size(); ++jdx) {
				light_directed[jdx].UseLight(shader_programs[idx], jdx);
			}

			objects[idx].DrawMesh(shader_programs[idx]);
		}
	}
}


class Window {
private:
	static constexpr GLuint SCR_WIDTH = 1200;
	static constexpr GLuint SCR_HEIGHT = 800;
	static constexpr GLuint SHADOW_WIDTH = 1400;
	static constexpr GLuint SHADOW_HEIGHT = 1400;

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

		std::vector<ShaderPipe> shaders_scene;
		shaders_scene.push_back(shader_cube_program);
		shaders_scene.push_back(shareds_floor_program);
		shaders_scene.push_back(shader_light_program);


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
		texture.SetTextureParametrs({1, 0.5});
		textures_cube.push_back(texture);
		texture.LoadTexture("./textures/3-4.png", std::string(Texture2D::SPECULAR_TEXTURE), true);
		texture.SetTextureParametrs({ 128, 0.5 });
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
	
		std::vector<Mesh> meshs_scene;
		meshs_scene.push_back(cube);
		meshs_scene.push_back(floor);
		meshs_scene.push_back(light);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glm::vec3 light_position(0.0f, 2.0f, 0.0f);

		LightPoint light_point_param{
			light_position,
			glm::vec3(0.05f, 0.05f, 0.05f),
			glm::vec3(0.4f, 0.4f, 0.4f),
			glm::vec3(0.5f, 0.5f, 0.5f),
			0.1, 0.09, 0.032 };

		LightDirected light_directed_param{
			glm::vec3(-0.2f, -1.0f, -0.3f),
			glm::vec3(0.05f, 0.05f, 0.05f),
			glm::vec3(0.4f, 0.4f, 0.4f),
			glm::vec3(0.5f, 0.5f, 0.5f)};

		std::vector<LightPoint> lights_point;
		lights_point.push_back(light_point_param);

		std::vector<LightDirected> lights_directed;
		lights_directed.push_back(light_directed_param);


		std::vector<Transform> transforms;
		transforms.emplace_back();
		transforms.emplace_back(glm::vec3(2.0f, -0.995f, 5.0f));
		transforms.emplace_back(glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.2f));


		glm::vec3 floor_position(0.0f, -1.0f, 0.0f);
		glm::vec3 cube1_position(2.0f, -0.995f, 5.0f);

		GLfloat flare_cube = 32;
		GLfloat flare_floor = 8;

		GLuint shadow_FBO;
		glGenFramebuffers(1, &shadow_FBO);

		std::vector<ShaderLoadInfo> shareds_shadow_info = { {"./scr/Shaders/ModelVertexShader.hlsl", GL_VERTEX_SHADER} };
		ShaderPipe shader_shadow_program = CreateShaderProgram(shareds_shadow_info.begin(), shareds_shadow_info.end());

		std::vector<ShaderPipe> shaders_shadow{ shader_shadow_program };

		Texture2D shadow_map;
		shadow_map.GenShadowTexture(SHADOW_WIDTH, SCR_HEIGHT);
		
		BindShadowTexture(shadow_map, shadow_FBO);

		while (!glfwWindowShouldClose(window)) {
			GLfloat current_frame = glfwGetTime();
			delta_time = current_frame - last_frame;

			//glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
			//glBindFramebuffer(GL_FRAMEBUFFER, shadow_FBO);
			//glClear(GL_DEPTH_BUFFER_BIT);

			//Render(camera, SCR_WIDTH, SCR_HEIGHT, meshs_scene, shadow_map, shaders_shadow, lights_point, lights_directed, transforms, true);


			//glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
			KeyboardInput();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			Render(camera, SCR_WIDTH, SCR_HEIGHT, meshs_scene, shadow_map, shaders_scene, lights_point, lights_directed, transforms);


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
