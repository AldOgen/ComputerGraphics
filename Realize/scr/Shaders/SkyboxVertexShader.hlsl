#version 330 core
layout(location = 0) in vec3 aPos;

out vec3 TexCoords;

struct FigurePosition {
    mat4 model;
    mat4 view;
    mat4 projection;
};


uniform FigurePosition figure_position;


void main() {
    TexCoords = aPos;
    gl_Position = projection * view * vec4(aPos, 1.0);
}