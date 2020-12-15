#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoord;
in vec3 FragPos;


uniform sampler2D diffuse_texture0;
uniform sampler2D specular_texture0;
uniform float flare;


uniform vec3 light_position;

uniform vec3 light_ambient;
uniform vec3 light_diffuse;
uniform vec3 light_specular;


uniform vec3 view_position;

void main() {
    vec3 normal = normalize(Normal);
    vec3 light_direction = normalize(light_position - FragPos);
    float diff = max(dot(normal, light_direction), 0.0);

    vec3 view_direction = normalize(view_position - FragPos);
    vec3 reflect_direction = reflect(-light_direction, normal);
    float spec = pow(max(dot(view_direction, reflect_direction), 0.0), flare);

    vec3 ambient = light_ambient * texture(diffuse_texture0, TexCoord).rgb;
    vec3 diffuse = light_diffuse * diff * texture(diffuse_texture0, TexCoord).rgb;
    vec3 specular = light_specular * spec * texture(specular_texture0, TexCoord).rgb;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
