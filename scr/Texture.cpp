#include "../libs/Texture.h"


Texture2D::Texture2D() = default;

void Texture2D::LoadTexture(const std::string& texture_file_path) {
    GLuint tmp_texture_id;
    glGenTextures(1, &tmp_texture_id);
    texture_id = tmp_texture_id;
    glBindTexture(GL_TEXTURE_2D, *texture_id); 

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLint width, height, nr_channels;
    GLboolean *texture = stbi_load(texture_file_path.c_str(), &width, &height, &nr_channels, 0);

    if (texture) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "Failed to load texture" << std::endl;
    }

    stbi_image_free(texture);
}

std::optional<GLuint> Texture2D::GetTextureID() const {
    return texture_id;
}

void Texture2D::UseTexture(GLuint idx) const {
    glActiveTexture(GL_TEXTURE0 + idx);
    glBindTexture(GL_TEXTURE_2D, *texture_id);
}


void CreateTexture2DUnion(const std::vector<Texture2D> &texture_array) {
    for (size_t idx = 0; idx < texture_array.size(); ++idx) {
        texture_array[idx].UseTexture(idx);
    }
}