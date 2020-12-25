#pragma once
#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Model.h"
#include "Camera.h"
#include "Light.h"
#include "Texture.h"
#include "Shader.h"


/* Производит примитивы Vertex для объекта из переданных данных */
class ObjectCreater {
public:
    ObjectCreater(const std::vector<glm::vec3>&, const std::vector<glm::vec3>&, const std::vector<glm::vec2>&, 
                  const std::vector<GLuint>&, const std::vector<GLuint>&, const std::vector<GLuint>&, size_t);
    std::vector<Vertex> CreateObject() const;

private:
    static std::pair<glm::vec3, glm::vec3> GetBiTangent(const std::vector<glm::vec3>&, const glm::vec3, const std::vector<glm::vec2>&);

private:
    std::vector<glm::vec3> vertexes_coordinates;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texture_coordinates;

    std::vector<GLuint> includes_vertexes_coords;
    std::vector<GLuint> includes_normals;
    std::vector<GLuint> includes_texture_coords;

    size_t cycle;
};


class Scene {
public:
    enum class SwitchRender {
        SCENE,
        SHADOW_MAP,
        SHADOW_CUBE
    };

public:
	Scene(std::vector<Mesh>& objects, Texture2D& shadow_texture, GLuint shadow_FBO, 
		std::vector<TextureCube> &shadow_cube, GLuint shadow_cube_FBO, std::vector<LightPoint>& lights_point, 
		LightDirected& light_directed, std::vector<Transform>& transforms)
		: objects(objects), shadow_texture(shadow_texture), shadow_FBO(shadow_FBO),
		  shadow_cube(shadow_cube), shadow_cube_FBO(shadow_cube_FBO), lights_point(lights_point), 
		  light_directed(light_directed), transforms(transforms) {}

    void Rendering(GLfloat scr_wight, GLfloat scr_height, std::vector<ShaderPipe>& shader_programs, GLfloat time, SwitchRender switch_render) {
		static std::vector<std::vector<glm::mat4>> shadow_transforms(lights_point.size());

		switch (switch_render) {
		case SwitchRender::SHADOW_MAP: {
			GLfloat near_plane = 1.0f, far_plane = 7.5f;
			glm::mat4 light_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
			glm::mat4 light_view = glm::lookAt(light_directed.GetPosition(), light_directed.GetDirection(), glm::vec3(0.0f, 1.0f, 0.0f));
			light_space = light_projection * light_view;

			shader_programs[0].UseShaderPipe();
			shader_programs[0].SetMat4("light_space", light_space);
			glActiveTexture(GL_TEXTURE0);

			for (size_t idx = 0; idx < objects.size(); ++idx) {
				if (objects[idx].IsVolume()) {
					glCullFace(GL_FRONT);
				}
				model = glm::mat4(1.0f);
				model = glm::translate(model, transforms[idx].translate);
				model = glm::rotate(model, glm::radians(transforms[idx].turn), glm::vec3(1.0f));
				model = glm::scale(model, transforms[idx].scale);

				FigurePosition figure_position{ model, view, projection };
				figure_position.UseFigurePosition(shader_programs[0]);

				objects[idx].DrawMesh(shader_programs[0]);
				glCullFace(GL_BACK);
			}
			break;
		} 
		case SwitchRender::SHADOW_CUBE: {
			GLfloat coef_resolution = static_cast<GLfloat>(scr_wight) / scr_height;
			GLfloat near_plane = 1.0f, far_plane = 25.0f;
			glm::mat4 shadow_projection = glm::perspective(glm::radians(90.0f), coef_resolution, near_plane, far_plane);
			for (size_t idx = 0; idx < lights_point.size(); ++idx) {
				shadow_transforms[idx].clear();
				shadow_transforms[idx].emplace_back(shadow_projection * glm::lookAt(lights_point[idx].GetPosition(),
					lights_point[idx].GetPosition() + glm::vec3(1.0f, 0.0f, 0.0f),
					glm::vec3(0.0f, -1.0f, 0.0f)));
				shadow_transforms[idx].emplace_back(shadow_projection * glm::lookAt(lights_point[idx].GetPosition(),
					lights_point[idx].GetPosition() + glm::vec3(-1.0f, 0.0f, 0.0f),
					glm::vec3(0.0f, -1.0f, 0.0f)));
				shadow_transforms[idx].emplace_back(shadow_projection * glm::lookAt(lights_point[idx].GetPosition(),
					lights_point[idx].GetPosition() + glm::vec3(0.0f, 1.0f, 0.0f),
					glm::vec3(0.0f, 0.0f, 1.0f)));
				shadow_transforms[idx].emplace_back(shadow_projection * glm::lookAt(lights_point[idx].GetPosition(),
					lights_point[idx].GetPosition() + glm::vec3(0.0f, -1.0f, 0.0f),
					glm::vec3(0.0f, 0.0f, -1.0f)));
				shadow_transforms[idx].emplace_back(shadow_projection * glm::lookAt(lights_point[idx].GetPosition(),
					lights_point[idx].GetPosition() + glm::vec3(0.0f, 0.0f, 1.0f),
					glm::vec3(0.0f, -1.0f, 0.0f)));
				shadow_transforms[idx].emplace_back(shadow_projection * glm::lookAt(lights_point[idx].GetPosition(),
					lights_point[idx].GetPosition() + glm::vec3(0.0f, 0.0f, -1.0f),
					glm::vec3(0.0f, -1.0f, 0.0f)));
				shader_programs[0].UseShaderPipe();
				for (size_t jdx = 0; jdx < shadow_transforms[idx].size(); ++jdx) {
					std::stringstream name;
					name << "shadow_view[" << jdx << "]";
					shader_programs[0].SetMat4(name.str(), shadow_transforms[idx][jdx]);
				}
				shader_programs[0].SetFloat("far_plane", far_plane);
				shader_programs[0].SetVec3("light_position", lights_point[idx].GetPosition());
				//shadow_cube[idx].UseTextureForShadowRendering();
				for (size_t jdx = 0; jdx < objects.size(); ++jdx) {
					model = glm::mat4(1.0f);
					model = glm::translate(model, transforms[jdx].translate);
					model = glm::rotate(model, glm::radians(transforms[jdx].turn), glm::vec3(1.0f));
					model = glm::scale(model, transforms[jdx].scale);

					FigurePosition figure_position{ model, view, projection };
					figure_position.UseFigurePosition(shader_programs[0]);

					objects[jdx].DrawMesh(shader_programs[0]);
				}
			}
			break;
		}
		case SwitchRender::SCENE: {
			projection = glm::perspective(glm::radians(camera->GetZoom()), scr_wight / scr_height, 0.1f, 100.0f);
			view = camera->GetViewMatrix();

			for (size_t idx = 0; idx < objects.size(); ++idx) {
				model = glm::mat4(1.0f);
				model = glm::translate(model, transforms[idx].translate);
				model = glm::rotate(model, glm::radians(transforms[idx].turn), glm::vec3(1.0f));
				model = glm::scale(model, transforms[idx].scale);

				shader_programs[idx].UseShaderPipe();
				shader_programs[idx].SetMat4("light_space", light_space);

				shadow_texture.UseTexture(shader_programs[idx], std::string(Texture2D::SHADOW_MAP), SHADOW_MAP_POSITION);
				shader_programs[idx].SetInt(std::string(Texture2D::SHADOW_MAP), SHADOW_MAP_POSITION);

				for (size_t jdx = 0; jdx < shadow_cube.size(); ++jdx) {
					shadow_cube[jdx].UseTexture(shader_programs[idx], std::string(TextureCube::SHADOW_CUBE_MAP), SHADOW_CUBE_MAP_POSITION + jdx);
				}

				FigurePosition figure_position{ model, view, projection };
				figure_position.UseFigurePosition(shader_programs[idx]);
				shader_programs[idx].SetVec3("view_position", camera->GetPosition());

				for (size_t jdx = 0; jdx < lights_point.size(); ++jdx) {
					lights_point[jdx].UseLight(shader_programs[idx], jdx);
					shader_programs[idx].SetFloat("far_plane", 25.0f);
				}

				light_directed.UseLight(shader_programs[idx]);

				objects[idx].DrawMesh(shader_programs[idx]);
			}
			break;
		}
		default:
			std::cerr << "ERROR::SCENE::RENDERING::UNKNOWN_RENDER_TYPE" << std::endl;
			break;
		}
    }

	GLuint GetShadowFBO() const {
		return shadow_FBO;
	}

	GLuint GetShadowCubeFBO() const {
		return shadow_cube_FBO;
	}

	void SetCamera(CameraFly *camera_window) {
		camera = camera_window;
	}

private:
	static constexpr GLuint SHADOW_MAP_POSITION = 5;
	static constexpr GLuint SHADOW_CUBE_MAP_POSITION = 6;

private:
    std::vector<Mesh>& objects;
    Texture2D& shadow_texture;
	GLuint shadow_FBO;
    std::vector<TextureCube> &shadow_cube;
	GLuint shadow_cube_FBO;
    std::vector<LightPoint>& lights_point;
    LightDirected& light_directed;
    std::vector<Transform>& transforms;

	CameraFly *camera = nullptr;

	glm::mat4 light_space{0.0f};

	glm::mat4 model{0.0f};
	glm::mat4 view{0.0f};
	glm::mat4 projection{0.0f};
};


