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
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

Vertex CreateVertex(glm::vec3, glm::vec3, glm::vec2);


//template<typename TextureType> 
class Mesh {
public:
    static constexpr std::string_view MATERIAL = "material";

public:
    Mesh(const std::vector<Vertex>&, 
         const std::vector<GLuint>&,
         const std::vector<Texture2D>&, 
         bool = true);
    void InitializeMesh();
    void DrawMesh(const ShaderPipe &);
    bool IsVolume() const;

public:
    std::vector<Vertex> vertexes;
    std::vector<GLuint> indexes;
    std::vector<Texture2D> textures;
    bool volume;

    GLuint VAO = 0, VBO = 0, IBO = 0;
};

template <typename Type>
static size_t SizeofContainer(const std::vector<Type>&);



class FigurePosition {
public:
    static constexpr std::string_view FIGURE_POSITION = "figure_position";
    static constexpr std::string_view MODEL = "model";
    static constexpr std::string_view VIEW = "view";
    static constexpr std::string_view PROJECTION = "projection";

public:
    FigurePosition(glm::mat4, glm::mat4, glm::mat4);
    void UseFigurePosition(const ShaderPipe&) const;

private:
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
};



struct Transform {
public:
    Transform(glm::vec3 = glm::vec3(0.0f), glm::vec3 = glm::vec3(1.0f), GLfloat = 0.0f);

public:
    glm::vec3 translate;
    glm::vec3 scale;
    GLfloat turn;
};
