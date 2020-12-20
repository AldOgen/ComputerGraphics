#pragma once
#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Model.h"


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


