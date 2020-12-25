#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

#define CNT_SKYBOX_MAP 1

uniform samplerCube skybox_map[CNT_SKYBOX_MAP;

void main() {
    FragColor = texture(skybox[0], TexCoords);
}