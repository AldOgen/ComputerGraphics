#pragma once
#include <iostream>
#include <optional>
#include <vector>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"


class Texture2D {
public:
    Texture2D();
    void LoadTexture(const std::string&);
    std::optional<GLuint> GetTextureID() const;
    void UseTexture(const GLuint) const;

private:
    std::optional<GLuint> texture_id;
};


void CreateTexture2DUnion(const std::vector<Texture2D>&);
