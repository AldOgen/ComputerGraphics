#pragma once
#include <fstream>
#include <iostream>
#include <optional>
#include <vector>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Shader {
public:
    Shader(const GLenum);
    void DeleteShader();
    std::optional<GLuint> GetShaderID() const;
    void LoadRealizationShader(const std::string &, int);
private:
    std::optional<GLuint> shader_id;
    GLenum type_shader;
};


class ShaderPipe {
public:
    ShaderPipe();
    std::optional <GLuint> GetShaderPipeID() const;
    void CreateShaderPipe(const std::vector<Shader>&, int);
    GLint GetLocation(const std::string&);
    void UseShaderPipe() const;
    void SetInt(const std::string&, GLint) const;
    void SetFloat(const std::string&, GLfloat) const;
    void SetVec2(const std::string&, const glm::vec2&) const;
    void SetVec3(const std::string&, const glm::vec3&) const;
    void SetVec4(const std::string&, const glm::vec4&) const;
    void SetMat2(const std::string&, const glm::mat2&) const;
    void SetMat3(const std::string&, const glm::mat3&) const;
    void SetMat4(const std::string&, const glm::mat4&) const;
private:
    std::optional<GLuint> shader_pipe_id = 0;
};


struct ShaderLoadInfo {
    ShaderLoadInfo(const std::string &, const GLenum);

    std::string file_shader_path;
    GLenum type_shader;
};


ShaderPipe CreateShaderProgram(const std::vector<ShaderLoadInfo> &);
