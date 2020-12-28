#include <iostream>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "libs/Initializer.h"
#include "libs/Light.h"
#include "libs/Model.h"
#include "libs/Texture.h"
#include "libs/Scene.h"
#include "libs/Shader.h"
#include "libs/Window.h"


int main(int argc, char **argv) {
	InitializeGLFW();

	Window window;
	window.Initialize("test");
	GLADLoader();

	// Загружаем шейдеры сцены

	std::vector<ShaderLoadInfo> shareds_cube_info = { {"./scr/Shaders/ModelVertexShader.hlsl", GL_VERTEX_SHADER},
												      {"./scr/Shaders/ModelFragmentShader.hlsl", GL_FRAGMENT_SHADER} };

	std::vector<ShaderLoadInfo> shareds_plastic_cube_info = { {"./scr/Shaders/ModelVertexShader.hlsl", GL_VERTEX_SHADER},
													          {"./scr/Shaders/PlasticCubeFragmentShader.hlsl", GL_FRAGMENT_SHADER} };

	std::vector<ShaderLoadInfo> shareds_light_info = { {"./scr/Shaders/LightVertexShader.hlsl", GL_VERTEX_SHADER},
													   {"./scr/Shaders/LightFragmentShader.hlsl", GL_FRAGMENT_SHADER} };

	std::vector<ShaderLoadInfo> shareds_floor_info = { {"./scr/Shaders/FloorVertexShader.hlsl", GL_VERTEX_SHADER},
													   {"./scr/Shaders/FloorFragmentShader.hlsl", GL_FRAGMENT_SHADER} };

	ShaderPipe shader_cube_program = CreateShaderProgram(shareds_cube_info.begin(), shareds_cube_info.end());
	ShaderPipe shader_plastic_cube_program = CreateShaderProgram(shareds_plastic_cube_info.begin(), shareds_plastic_cube_info.end());
	ShaderPipe shader_light_program = CreateShaderProgram(shareds_light_info.begin(), shareds_light_info.end());
	ShaderPipe shareds_floor_program = CreateShaderProgram(shareds_floor_info.begin(), shareds_floor_info.end());

	std::vector<ShaderPipe> shaders_scene;
	shaders_scene.push_back(shader_cube_program);
	shaders_scene.push_back(shader_plastic_cube_program);
	shaders_scene.push_back(shader_cube_program);
	shaders_scene.push_back(shader_cube_program);
	shaders_scene.push_back(shader_cube_program);
	shaders_scene.push_back(shareds_floor_program);
	shaders_scene.push_back(shader_light_program);


	// Инициализируем исходные данные для создания примитива "Куб"

	std::vector<glm::vec3> vertex_coords_cube{
		glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(-1.0f, 1.0f, -1.0f),
		glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-1.0f, 1.0f, 1.0f),
		glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(-1.0f, -1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(1.0f, -1.0f, 1.0f),
		glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(-1.0f, -1.0f, 1.0f),
		glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-1.0f, 1.0f, 1.0f)
	};

	std::vector<glm::vec3> normal_cube{
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	};

	std::vector<glm::vec2> tex_coords_cube{
		glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f)
	};

	std::vector<glm::vec2> tex_coords_column{
		glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 3.0f), glm::vec2(0.0f, 3.0f),
		glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 3.0f), glm::vec2(0.0f, 3.0f),
		glm::vec2(0.0f, 3.0f), glm::vec2(1.0f, 3.0f), glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 3.0f), glm::vec2(1.0f, 3.0f), glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f)
	};

	std::vector<GLuint> indexes_vertex_coords_cube = {
		0, 1, 2,
		2, 3, 0
	};

	std::vector<GLuint> indexes_norm_cube = {
		0, 0, 0,
		0, 0, 0
	};

	std::vector<GLuint> indexes_tex_coords_cube = {
		0, 1, 2,
		2, 3, 0
	};

	size_t cycle_cube = 6;


	// Инициализируем исходные данные для создания примитива "Плоскость"

	std::vector<glm::vec3> vertex_coords_floor{
		glm::vec3(7.0f, 0.0f, 7.0f), glm::vec3(-7.0f, 0.0f, 7.0f), glm::vec3(-7.0f, 0.0f, -7.0f), glm::vec3(7.0f, 0.0f, -7.0f)
	};

	std::vector<glm::vec3> normal_floor{
		glm::vec3(0.0f, 1.0f, 0.0f)
	};

	std::vector<glm::vec2> tex_coords_floor{
		glm::vec2(28.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 28.0f), glm::vec2(28.0f, 28.0f)
	};

	std::vector<GLuint> indexes_vertex_coords_floor{
		0, 1, 2,
		0, 2, 3
	};

	std::vector<GLuint> indexes_norm_floor{
		0, 0, 0,
		0, 0, 0
	};

	std::vector<GLuint> indexes_tex_coords_floor{
		0, 1, 2,
		0, 2, 3
	};

	size_t cycle_floor = 1;


	// Создаем объекты-фабрики для координат примитивов

	ObjectCreater creater_cube{ vertex_coords_cube, normal_cube, tex_coords_cube,
		indexes_vertex_coords_cube, indexes_norm_cube, indexes_tex_coords_cube, cycle_cube };

	ObjectCreater creater_column{ vertex_coords_cube, normal_cube, tex_coords_column,
		indexes_vertex_coords_cube, indexes_norm_cube, indexes_tex_coords_cube, cycle_cube };

	ObjectCreater creater_floor{ vertex_coords_floor, normal_floor, tex_coords_floor,
		indexes_vertex_coords_floor, indexes_norm_floor, indexes_tex_coords_floor, cycle_floor };


	// Загружаем текстуры

	Texture2D texture;

	std::vector<Texture2D> texture_cube;
	texture.LoadTexture({ "./textures/Cube/Plastic_001_COLOR.jpg" }, std::string(Texture2D::DIFFUSE_MAP));
	texture.SetTextureParametrs({ 256, 0.5 });
	texture_cube.push_back(texture);


	std::vector<Texture2D> textures_column;
	texture.LoadTexture({ "./textures/bricks2.jpg" }, std::string(Texture2D::DIFFUSE_MAP));
	texture.SetTextureParametrs({ 1, 0.5 });
	textures_column.push_back(texture);

	texture.LoadTexture({ "./textures/bricks2.jpg" }, std::string(Texture2D::SPECULAR_MAP));
	texture.SetTextureParametrs({ 16, 0.5 });
	textures_column.push_back(texture);

	texture.LoadTexture({ "./textures/bricks2_normal.jpg" }, std::string(Texture2D::NORMAL_MAP));
	textures_column.push_back(texture);

	texture.LoadTexture({ "./textures/bricks2_disp.jpg" }, std::string(Texture2D::DEPTH_MAP));
	texture.SetTextureParametrs({ 0.0, 0.0, 0.1 });
	textures_column.push_back(texture);


	std::vector<Texture2D> textures_floor;
	texture.LoadTexture({ "./textures/Floor/Stone_Wall_007_COLOR.jpg" }, std::string(Texture2D::DIFFUSE_MAP));
	textures_floor.push_back(texture);

	texture.LoadTexture({ "./textures/Floor/Stone_Wall_007_NORM.jpg" }, std::string(Texture2D::NORMAL_MAP));
	textures_floor.push_back(texture);

	texture.LoadTexture({ "./textures/Floor/Stone_Wall_007_DEPTH.png" }, std::string(Texture2D::DEPTH_MAP));
	texture.SetTextureParametrs({ 0.0, 0.0, 0.1 });
	textures_floor.push_back(texture);


	// Создаем объекты графических примитивов

	Mesh plastic_cube{ creater_cube.CreateObject(), indexes_vertex_coords_cube, texture_cube };
	plastic_cube.InitializeMesh();

	Mesh cube{ creater_cube.CreateObject(), indexes_vertex_coords_cube, textures_column };
	cube.InitializeMesh();

	Mesh column{ creater_column.CreateObject(), indexes_vertex_coords_cube, textures_column };
	column.InitializeMesh();

	Mesh floor{ creater_floor.CreateObject(), indexes_vertex_coords_floor, textures_floor, false };
	floor.InitializeMesh();

	Mesh light{ creater_cube.CreateObject(), indexes_vertex_coords_cube, std::vector<Texture2D>{} };
	light.InitializeMesh();


	// Добавляем объеты в пул отрисовки

	std::vector<Mesh> meshs_scene;
	meshs_scene.push_back(cube);
	meshs_scene.push_back(plastic_cube);
	meshs_scene.push_back(column);
	meshs_scene.push_back(column);
	meshs_scene.push_back(column);
	meshs_scene.push_back(floor);
	meshs_scene.push_back(light);

	// Создаем источник направленного света

	glm::vec3 light_point_position(2.0f, 3.0f, 1.5f);
	glm::vec3 light_directed_position(2.0f, 3.0f, 1.5f);

	LightPoint light_point_param{
		light_point_position,
		glm::vec3(0.05f, 0.05f, 0.05f),
		glm::vec3(0.4f, 0.4f, 0.4f),
		glm::vec3(0.5f, 0.5f, 0.5f),
		0.1, 0.09, 0.032 };

	LightDirected light_directed{
		light_directed_position,
		glm::vec3(-0.2f, -1.0f, -0.3f),
		glm::vec3(0.05f, 0.05f, 0.05f),
		glm::vec3(0.4f, 0.4f, 0.4f),
		glm::vec3(0.5f, 0.5f, 0.5f) };

	std::vector<LightPoint> lights_point;
	lights_point.push_back(light_point_param);

	// Задаем матрицы перевода объектов в мировое пространство

	std::vector<Transform> transforms;
	transforms.emplace_back(glm::vec3(1.0f, -1.2f, -0.0f), glm::vec3(0.5f), 45);
	transforms.emplace_back(glm::vec3(0.0f, 1.7f, 0.0f), glm::vec3(0.5f), 35);
	transforms.emplace_back(glm::vec3(-1.5f, -0.498f, -1.5f), glm::vec3(0.5f, 1.5f, 0.5f));
	transforms.emplace_back(glm::vec3(-1.5f, -0.498f, 1.5f), glm::vec3(0.5f, 1.5f, 0.5f));
	transforms.emplace_back(glm::vec3(1.5f, -0.498f, 1.5f), glm::vec3(0.5f, 1.5f, 0.5f));
	transforms.emplace_back(glm::vec3(0.0f, -2.0f, 0.0f));
	transforms.emplace_back(light_directed_position, glm::vec3(0.2f));

	// Загружаем шейдеры для создания теней

	std::vector<ShaderLoadInfo> shareds_shadow_info = { {"./scr/Shaders/ShadowVertexShader.hlsl", GL_VERTEX_SHADER},
														{"./scr/Shaders/ShadowFragmentShader.hlsl", GL_FRAGMENT_SHADER} };


	ShaderPipe shader_shadow_program = CreateShaderProgram(shareds_shadow_info.begin(), shareds_shadow_info.end());

	std::vector<ShaderPipe> shaders_shadow{ shader_shadow_program };
	std::vector<ShaderPipe> shaders_shadow_cube{ };


	// Создаем текстуру для карт глубины и связываем ее с соответсвующем фреймбуфером

	GLuint shadow_FBO;
	glGenFramebuffers(1, &shadow_FBO);

	GLuint shadow_cube_FBO;
	glGenFramebuffers(1, &shadow_cube_FBO);

	Texture2D shadow_map;
	shadow_map.GenShadowTexture(Window::SHADOW_MAP_WIDTH, Window::SHADOW_MAP_HEIGHT);

	TextureCube shadow_cube_map;
	std::vector<TextureCube> shadow_cube_maps;
	BindShadowTexture(shadow_map, shadow_FBO);

	shadow_cube_map.GenShadowTexture(Window::SHADOW_CUBE_MAP_WIDTH, Window::SHADOW_CUBE_MAP_HEIGHT);
	BindShadowCubeTexture(shadow_cube_map, shadow_cube_FBO);
	shadow_cube_maps.push_back(shadow_cube_map);

	// Инициализируем объект сцены

	Scene scene{ meshs_scene, shadow_map, shadow_FBO, shadow_cube_maps, shadow_cube_FBO, lights_point, light_directed, transforms };

	// Рендерим полученную сцену

	window.Rendering(scene, shaders_shadow, shaders_scene);

	return 0;
}
