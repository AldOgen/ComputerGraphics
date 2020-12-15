#include "../libs/Texture.h"


std::optional<GLuint> Texture::GetTextureID() const {
    return texture_id;
}

std::optional <std::string> Texture::GetType() const {
    return type;
}


void Texture2D::LoadTexture(const std::string& texture_file_path, const std::string& type_texture, bool flg_png) {
    if (type_texture != DIFFUSE_TEXTURE && type_texture != SPECULAR_TEXTURE) {
        std::cerr << "ERROR::TEXTURE::UNKNOWN_TEXTURE_TYPE" << std::endl;
        return;
    }

    GLuint tmp_texture_id;
    glGenTextures(1, &tmp_texture_id);

    texture_id = tmp_texture_id;
    type = type_texture;

    glBindTexture(GL_TEXTURE_2D, *texture_id); 

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLint width, height, nr_channels;
    GLboolean *texture = stbi_load(texture_file_path.c_str(), &width, &height, &nr_channels, 0);

    if (texture) {
        if (flg_png) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
    } else {
        std::cerr << "ERROR::TEXTURE::TEXTURE_LOADING_FAILED" << std::endl;
    }

    stbi_image_free(texture);
}

void Texture2D::UseTexture(GLuint idx) const  {
    glActiveTexture(GL_TEXTURE0 + idx);
    glBindTexture(GL_TEXTURE_2D, *texture_id);
}


void CreateTexture2DUnion(const std::vector<Texture2D> &texture_array) {
    for (size_t idx = 0; idx < texture_array.size(); ++idx) {
        texture_array[idx].UseTexture(idx);
    }
}