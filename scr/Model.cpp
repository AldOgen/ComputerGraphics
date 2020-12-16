#include "../libs/Model.h"


//template<typename TextureType>
Mesh::Mesh(const std::vector<Vertex>& vertexes,
           const std::vector<GLuint>& indexes,
           const std::vector<Texture2D>& textures)
    : vertexes(vertexes.begin(), vertexes.end()),
      indexes(indexes.begin(), indexes.end()),
      textures(textures.begin(), textures.end()) {}

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

    glBindVertexArray(0);
}

//template<typename TextureType>
void Mesh::DrawMesh(const ShaderPipe &shader_program) {
    int cnt_diffuse = 0;
    int cnt_specular = 0;

    for (size_t idx = 0; idx < textures.size(); ++idx) {
        std::stringstream name_texture;

        if (textures[idx].GetType() == Texture2D::DIFFUSE_TEXTURE) {
            name_texture << Texture2D::DIFFUSE_TEXTURE << cnt_diffuse++;
        } else {
            name_texture << Texture2D::SPECULAR_TEXTURE << cnt_specular++;
        }
        textures[idx].UseTexture(shader_program, name_texture.str(), idx);
        name_texture << Texture::TEXTURE;
        shader_program.SetInt(name_texture.str(), idx);
    }

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexes.size());
    //glDrawElements(GL_TRIANGLES, indexes.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
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
    vertex.texture_position =  texture_position;
    return vertex;
}

LightPoint::LightPoint(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
             GLfloat attenuation_const, GLfloat attenuation_lin, GLfloat attenuation_quad) 
    : position(position), ambient(ambient), diffuse(diffuse), specular(specular), 
      attenuation_const(attenuation_const), attenuation_lin(attenuation_lin), attenuation_quad(attenuation_quad) {}

void LightPoint::UseLight(const ShaderPipe &shader_program, int idx) const {
    std::stringstream name;
    name << LIGHT << idx << ".";
    shader_program.SetVec3(name.str() + std::string(POSITION), position);

    shader_program.SetVec3(name.str() + std::string(AMBIDENT), ambient);
    shader_program.SetVec3(name.str() + std::string(DIFFUSE), diffuse);
    shader_program.SetVec3(name.str() + std::string(SPECULAR), specular);

    shader_program.SetFloat(name.str() + std::string(ATTENUATION_CONST), attenuation_const);
    shader_program.SetFloat(name.str() + std::string(ATTENUATION_LIN), attenuation_lin);
    shader_program.SetFloat(name.str() + std::string(ATTENUATION_QUAD), attenuation_quad);
}


LightDirected::LightDirected(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) 
    : direction(direction), ambient(ambient), diffuse(diffuse), specular(specular) {}

void LightDirected::UseLight(const ShaderPipe& shader_program, int idx) const {
    std::stringstream name;
    name << LIGHT << idx << ".";
    shader_program.SetVec3(name.str() + std::string(DIRECTION), direction);

    shader_program.SetVec3(name.str() + std::string(AMBIDENT), ambient);
    shader_program.SetVec3(name.str() + std::string(DIFFUSE), diffuse);
    shader_program.SetVec3(name.str() + std::string(SPECULAR), specular);
}


Transform::Transform(glm::vec3 translate, glm::vec3 scale, GLfloat turn)
    : translate(translate), scale(scale), turn(turn) {}
