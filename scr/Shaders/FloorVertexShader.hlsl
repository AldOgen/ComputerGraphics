#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec2 aTexCoord;


out FigureParam {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
    vec4 FragPosLightSpace;
} figure_param;

struct FigurePosition {
    mat4 model;
    mat4 view;
    mat4 projection;
}

uniform FigurePosition figure_position;
uniform mat4 light_space;


void main() {
    gl_Position = figure_position.projection * figure_position.view * figure_position.model * vec4(aPos, 1.0);

    figure_param.FragPos = vec3(model * vec4(aPos, 1.0));
    figure_param.Normal = transpose(inverse(mat3(model))) * aNorm;
    figure_param.TexCoord = aTexCoord;
    figure_param.FragPosLightSpace = light_space * vec4(vertex_param.FragPos, 1.0);
}