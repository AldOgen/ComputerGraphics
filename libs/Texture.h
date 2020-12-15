#pragma once
#include <iostream>
#include <optional>
#include <vector>
#include <string>
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"


class Texture {
public:
    virtual ~Texture() {}
    virtual void LoadTexture(const std::string&, const std::string&, bool) = 0;
    std::optional<GLuint> GetTextureID() const;
    std::optional <std::string> GetType() const;
    virtual void UseTexture(const GLuint) const = 0;

protected:
    std::optional<GLuint> texture_id;
    std::optional <std::string> type;
};


class Texture2D : public Texture {
public:
    static constexpr std::string_view DIFFUSE_TEXTURE = "diffuse_texture";
    static constexpr std::string_view SPECULAR_TEXTURE = "specular_texture";

public:
    Texture2D() = default;
    void LoadTexture(const std::string&, const std::string&, bool = false) override;
    void UseTexture(const GLuint) const override;
};


void CreateTexture2DUnion(const std::vector<Texture2D>&);
