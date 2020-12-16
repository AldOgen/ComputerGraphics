#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec2 aTexCoord;


out VertexParam {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
    vec4 FragPosLightSpace;
} vertex_param;


struct FigurePosition {
    mat4 model;
    mat4 view;
    mat4 projection;
};

uniform FigurePosition figure_position;
uniform mat4 light_space;


void main() {
    gl_Position = figure_position.projection * figure_position.view * figure_position.model * vec4(aPos, 1.0);

    vertex_param.FragPos = vec3(figure_position.model * vec4(aPos, 1.0));
    vertex_param.Normal = transpose(inverse(mat3(figure_position.model))) * aNorm;
    vertex_param.TexCoord = aTexCoord;
    vertex_param.FragPosLightSpace = light_space * vec4(vertex_param.FragPos, 1.0);
}