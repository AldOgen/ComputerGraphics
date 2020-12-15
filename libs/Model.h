#pragma once
#include <cstddef>
#include <iostream>
#include <vector>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "./Texture.h"
#include "./Shader.h"


//C compatible POD structure
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texture_position;
};

Vertex CreateVertex(glm::vec3, glm::vec3, glm::vec2);


//template<typename TextureType> 
class Mesh {
public:
    static constexpr std::string_view MATERIAL = "material";

public:
    Mesh(const std::vector<Vertex>&, 
         const std::vector<GLuint>&,
         const std::vector<Texture2D>&);
    void InitializeMesh();
    void DrawMesh(const ShaderPipe &);

public:
    std::vector<Vertex> vertexes;
    std::vector<GLuint> indexes;
    std::vector<Texture2D> textures;

    GLuint VAO, VBO, IBO;
};


template <typename Type>
static size_t SizeofContainer(const std::vector<Type>&);
