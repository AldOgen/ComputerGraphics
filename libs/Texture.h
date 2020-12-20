#pragma once
#include <iostream>
#include <optional>
#include <vector>
#include <string>
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"
#include "Shader.h"


struct TextureParametrs {
public:
    static constexpr std::string_view FLARE = "flare";
    static constexpr std::string_view DIFF_COEF = "diff_coef";
    static constexpr std::string_view HEIGHT_COEF = "height_coef";

public:
    TextureParametrs(GLfloat = 0.0f, GLfloat = 0.0f, GLfloat = 0.0f);

public:
    GLfloat flare;
    GLfloat diff_coef;
    GLfloat height_coef;
};

class Texture {
public:
    static constexpr std::string_view TEXTURE = "texture_data";

public:
    virtual ~Texture() {}
    virtual void LoadTexture(const std::string&, const std::string&, bool) = 0;
    std::optional<GLuint> GetTextureID() const;
    std::optional <std::string> GetType() const;
    void SetTextureParametrs(const TextureParametrs&);
    virtual void UseTexture(const ShaderPipe&, const std::string, GLuint) const = 0;

protected:
    std::optional <TextureParametrs> texture_params;
    std::optional <std::string> type;
    std::optional<GLuint> texture_id;
};


class Texture2D : public Texture {
public:
    friend void BindShadowTexture(const Texture2D&, GLuint);

public:
    static constexpr std::string_view DIFFUSE_MAP = "diffuse_map";
    static constexpr std::string_view SPECULAR_MAP = "specular_map";
    static constexpr std::string_view SHADOW_MAP = "shadow_map";
    static constexpr std::string_view NORMAL_MAP = "normal_map";
    static constexpr std::string_view DEPTH_MAP = "depth_map";

public:
    Texture2D() = default;
    void LoadTexture(const std::string&, const std::string&, bool = false) override;
    void GenShadowTexture(GLuint, GLuint);
    void UseTexture(const ShaderPipe& shader_program, const std::string, GLuint) const override;
};


void CreateTexture2DUnion(const std::vector<Texture2D> &, const std::vector <ShaderPipe>&, const std::vector <std::string>);

void BindShadowTexture(const Texture2D&, GLuint);


class TextureCube : public Texture {
public:
    friend void BindShadowCubeTexture(const TextureCube&, GLuint);

public:
    static constexpr std::string_view CUBE_MAP_DATA = "cube_map_data";
    static constexpr std::string_view SHADOW_CUBE_MAP = "shadow_cube_map";

public:
    void LoadTexture(const std::string&, const std::string&, bool) override;
    void GenShadowTexture(GLuint, GLuint);
    void UseTextureForShadowRendering() const;
    void UseTexture(const ShaderPipe&, const std::string, GLuint) const;
};

void BindShadowCubeTexture(const TextureCube&, GLuint);
