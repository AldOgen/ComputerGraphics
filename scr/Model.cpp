#include "../libs/Model.h"


//template<typename TextureType>
Mesh::Mesh(const std::vector<Vertex>& vertexes,
           const std::vector<GLuint>& indexes,
           const std::vector<Texture2D>& textures,
           bool volume)
    : vertexes(vertexes.begin(), vertexes.end()),
      indexes(indexes.begin(), indexes.end()),
      textures(textures.begin(), textures.end()),
      volume(volume) {}

//template<typename TextureType>
void Mesh::InitializeMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &IBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, SizeofContainer(vertexes), std::data(vertexes), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, SizeofContainer(indexes), std::data(indexes), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(0));

    glEnableVertexAttribArray(1); 
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
    
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texture_position)));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, tangent)));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, bitangent)));

    glBindVertexArray(0);
}

//template<typename TextureType>
void Mesh::DrawMesh(const ShaderPipe &shader_program) {
    int cnt_diffuse = 0;
    int cnt_specular = 0;
    int cnt_normal = 0;
    int cnt_depth = 0;

    for (size_t idx = 0; idx < textures.size(); ++idx) {
        std::stringstream name_texture;

        if (textures[idx].GetType() == Texture2D::DIFFUSE_MAP) {
            name_texture << Texture2D::DIFFUSE_MAP << "[" << cnt_diffuse++ << "]";
        } else if (textures[idx].GetType() == Texture2D::SPECULAR_MAP) {
            name_texture << Texture2D::SPECULAR_MAP << "[" << cnt_specular++ << "]";
        } else if (textures[idx].GetType() == Texture2D::NORMAL_MAP) {
            name_texture << Texture2D::NORMAL_MAP << "[" << cnt_normal++ << "]";
        } else {
            name_texture << Texture2D::DEPTH_MAP << "[" << cnt_depth++ << "]";
        }

        textures[idx].UseTexture(shader_program, name_texture.str(), idx);
        name_texture << "." << Texture::TEXTURE;
        shader_program.SetInt(name_texture.str(), idx);
    }

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexes.size());
    //glDrawElements(GL_TRIANGLES, indexes.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
}

bool Mesh::IsVolume() const {
    return volume;
}


template <typename Type>
size_t SizeofContainer(const std::vector<Type>& container) {
    return container.size() * sizeof(Type);
}


FigurePosition::FigurePosition(glm::mat4 model, glm::mat4 view, glm::mat4 projection) 
    : model(model), view(view), projection(projection) {}

void FigurePosition::UseFigurePosition(const ShaderPipe& shader_program) const {
    std::stringstream name;
    name << FIGURE_POSITION << ".";

    shader_program.SetMat4(name.str() + std::string(MODEL), model);
    shader_program.SetMat4(name.str() + std::string(VIEW), view);
    shader_program.SetMat4(name.str() + std::string(PROJECTION), projection);
}


Vertex CreateVertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texture_position) {
    Vertex vertex;
    vertex.position = position;
    vertex.normal = normal;
    vertex.texture_position = texture_position;
    //vertex.tangent = tangent;
    //vertex.bitangent = bitangent;

    return vertex;
}


Transform::Transform(glm::vec3 translate, glm::vec3 scale, GLfloat turn)
    : translate(translate), scale(scale), turn(turn) {}
