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


class LightPoint {
public:
    static constexpr std::string_view LIGHT = "light_point";

public:
    LightPoint(glm::vec3, glm::vec3, glm::vec3, glm::vec3, GLfloat, GLfloat, GLfloat);
    void UseLight(const ShaderPipe&, int) const;

private:
    static constexpr std::string_view POSITION = "position";
    static constexpr std::string_view AMBIDENT = "ambient";
    static constexpr std::string_view DIFFUSE = "diffuse";
    static constexpr std::string_view SPECULAR = "specular";
    static constexpr std::string_view ATTENUATION_CONST = "attenuation_const";
    static constexpr std::string_view ATTENUATION_LIN = "attenuation_lin";
    static constexpr std::string_view ATTENUATION_QUAD = "attenuation_quad";

private:
    glm::vec3 position;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    GLfloat attenuation_const;
    GLfloat attenuation_lin;
    GLfloat attenuation_quad;
};


class LightDirected {
public:
    static constexpr std::string_view LIGHT = "light_directed";

public:
    LightDirected(glm::vec3, glm::vec3, glm::vec3, glm::vec3);
    void UseLight(const ShaderPipe&, int) const;

private:
    static constexpr std::string_view DIRECTION = "direction";
    static constexpr std::string_view AMBIDENT = "ambient";
    static constexpr std::string_view DIFFUSE = "diffuse";
    static constexpr std::string_view SPECULAR = "specular";

private:
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};


struct Transform {
public:
    Transform(glm::vec3 = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 = glm::vec3(1.0f, 1.0f, 1.0f), GLfloat = 0.0f);

public:
    glm::vec3 translate;
    glm::vec3 scale;
    GLfloat turn;
};
