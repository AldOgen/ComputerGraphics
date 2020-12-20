#include "../libs/Shader.h"


Shader::Shader(const GLenum type_shader): type_shader(type_shader) {}

void Shader::DeleteShader() {
    if (shader_id) {
        glDeleteShader(*shader_id);
    }
    shader_id = std::nullopt;
}

std::optional <GLuint> Shader::GetShaderID() const {
    return shader_id;
}

void Shader::LoadRealizationShader(const std::string& path_shader_realization, int log_info_size = 512) {
    std::string shader_code_realization = "";
    std::ifstream shader_realization(path_shader_realization, std::ios::in);

    if (shader_realization.is_open()) {
        std::string line_buf = "";
        while (std::getline(shader_realization, line_buf)) {
            shader_code_realization += line_buf + "\n";
        }
        shader_realization.close();
    } else {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

    GLchar const * shader_source = shader_code_realization.c_str();
    shader_id = glCreateShader(type_shader);
    glShaderSource(*shader_id, 1, &shader_source, nullptr);
    glCompileShader(*shader_id);
    int status;
    std::string log_info;
    log_info.resize(log_info_size);
    glGetShaderiv(*shader_id, GL_COMPILE_STATUS, &status);
    if (!status) {
        glGetShaderInfoLog(*shader_id, log_info_size, nullptr, std::data(log_info));
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << log_info << std::endl;
    }
}


ShaderPipe::ShaderPipe() = default;

std::optional <GLuint> ShaderPipe::GetShaderPipeID() const {
    return shader_pipe_id;
}

void ShaderPipe::CreateShaderPipe(const std::vector<Shader> &shaders, int log_info_size = 512) {
    shader_pipe_id = glCreateProgram();
    for (const auto &shader : shaders) {
        auto shader_id = shader.GetShaderID();
        if (shader_id) {
            glAttachShader(*shader_pipe_id, *shader_id);
        }
    }
    glLinkProgram(*shader_pipe_id);

    int status;
    std::string log_info;
    log_info.resize(log_info_size);
    glGetProgramiv(*shader_pipe_id, GL_LINK_STATUS, &status);
    if (!status) {
         glGetProgramInfoLog(*shader_pipe_id, 512, nullptr, std::data(log_info));
         std::cerr << "ERROR::SHADER::LINKING_FAILED\n" << log_info << std::endl;
    }
}

GLint ShaderPipe::GetLocation(const std::string &name_uniform_var) const {
    return glGetUniformLocation(*shader_pipe_id, name_uniform_var.c_str());
}

void ShaderPipe::UseShaderPipe() const {
    glUseProgram(*shader_pipe_id);
}

void ShaderPipe::SetInt(const std::string& var_key, GLint var_val) const {
    auto a = glGetUniformLocation(*shader_pipe_id, var_key.c_str());
    glUniform1i(glGetUniformLocation(*shader_pipe_id, var_key.c_str()), var_val);
}

void ShaderPipe::SetFloat(const std::string& var_key, GLfloat var_val) const {
    glUniform1f(glGetUniformLocation(*shader_pipe_id, var_key.c_str()), var_val);
}

void ShaderPipe::SetVec2(const std::string& var_key, const glm::vec2& var_val) const {
    glUniform2fv(glGetUniformLocation(*shader_pipe_id, var_key.c_str()), 1, glm::value_ptr(var_val));
}

void ShaderPipe::SetVec3(const std::string& var_key, const glm::vec3& var_val) const {
    glUniform3fv(glGetUniformLocation(*shader_pipe_id, var_key.c_str()), 1, glm::value_ptr(var_val));
}

void ShaderPipe::SetVec4(const std::string& var_key, const glm::vec4& var_val) const {
    glUniform4fv(glGetUniformLocation(*shader_pipe_id, var_key.c_str()), 1, glm::value_ptr(var_val));
}

void ShaderPipe::SetMat2(const std::string& var_key, const glm::mat2& var_val) const {
    glUniformMatrix2fv(glGetUniformLocation(*shader_pipe_id, var_key.c_str()), 1, GL_FALSE, glm::value_ptr(var_val));
}

void ShaderPipe::SetMat3(const std::string& var_key, const glm::mat3& var_val) const {
    glUniformMatrix3fv(glGetUniformLocation(*shader_pipe_id, var_key.c_str()), 1, GL_FALSE, glm::value_ptr(var_val));
}

void ShaderPipe::SetMat4(const std::string& var_key, const glm::mat4& var_val) const {
    glUniformMatrix4fv(glGetUniformLocation(*shader_pipe_id, var_key.c_str()), 1, GL_FALSE, glm::value_ptr(var_val));
}


ShaderLoadInfo::ShaderLoadInfo(const std::string &file_shader_path, const GLenum type_shader)
    : file_shader_path(file_shader_path), type_shader(type_shader) {}



ShaderPipe CreateShaderProgram(std::vector<ShaderLoadInfo>::const_iterator info_begin,
                               std::vector<ShaderLoadInfo>::const_iterator info_end) {
    std::vector<Shader> shaders;
    std::for_each(info_begin, 
                  info_end, 
                  [&shaders](const auto& shader_info) {
                      Shader shader(shader_info.type_shader);
                      shader.LoadRealizationShader(shader_info.file_shader_path);
                      shaders.push_back(shader); 
                  });

    ShaderPipe shader_program;
    shader_program.CreateShaderPipe(shaders);

    std::for_each(shaders.begin(), shaders.end(), [](auto &shader) { shader.DeleteShader(); });

    return shader_program;
}
