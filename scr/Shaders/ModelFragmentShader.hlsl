#version 330 core
out vec4 FragColor;


in VertexParam{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
    vec4 FragPosLightSpace;
} vertex_param;


struct Texture {
    sampler2D texture_data;
    float flare;
    float diff_coef;
};

uniform Texture diffuse_texture0;
uniform Texture specular_texture0;


struct LightDirected {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


struct LightPoint {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float attenuation_const;
    float attenuation_lin;
    float attenuation_quad;
};


uniform LightDirected light_directed0;
uniform LightPoint light_point0;

uniform vec3 view_position;


vec3 PhongLuminousFluxDirected(vec3 diffuse_texture0_gamma, vec3 specular_texture0_gamma, 
                               float flare, LightDirected light) {
    vec3 normal = normalize(vertex_param.Normal);
    vec3 light_direction = normalize(-light.direction);
    float diff = max(dot(normal, light_direction), 0.0);

    vec3 view_direction = normalize(view_position - vertex_param.FragPos);
    vec3 middle_normal = normalize(view_direction + light_direction);
    float spec = pow(max(dot(view_direction, middle_normal), 0.0), flare);

    vec3 ambient = light.ambient * diffuse_texture0_gamma;
    vec3 diffuse = light.diffuse * diff * diffuse_texture0_gamma;
    vec3 specular = light.specular * (spec * specular_texture0_gamma);

    return ambient + diffuse + specular;
}


vec3 PhongLuminousFluxPoint(vec3 diffuse_texture0_gamma, vec3 specular_texture0_gamma, 
                            float flare, LightPoint light) {
    vec3 direction = light.position - vertex_param.FragPos;

    vec3 normal = normalize(vertex_param.Normal);
    vec3 light_direction = normalize(direction);
    float diff = max(dot(normal, light_direction), 0.0);

    vec3 view_direction = normalize(view_position - vertex_param.FragPos);
    vec3 middle_normal = normalize(view_direction - light_direction);
    float spec = pow(max(dot(view_direction, middle_normal), 0.0), flare);

    vec3 ambient = light.ambient * diffuse_texture0_gamma;
    vec3 diffuse = light.diffuse * diff * diffuse_texture0_gamma;
    vec3 specular = light.specular * (spec * specular_texture0_gamma);


    float distance = length(direction);
    float attenuation = 1.0 / (light.attenuation_const +
                               light.attenuation_lin * distance +
                               light.attenuation_quad * distance * distance);
    return attenuation * (ambient + diffuse + specular);
}


void main() {
    float gamma = 2.2;
    vec3 diffuse_texture0_gamma = pow(texture(diffuse_texture0.texture_data, vertex_param.TexCoord).rgb, vec3(1.0 / gamma));
    vec3 specular_texture0_gamma = pow(texture(specular_texture0.texture_data, vertex_param.TexCoord).rgb, vec3(1.0 / gamma));

    FragColor = vec4(PhongLuminousFluxPoint(diffuse_texture0_gamma, specular_texture0_gamma, diffuse_texture0.flare, light_point0) +
                     PhongLuminousFluxDirected(diffuse_texture0_gamma, specular_texture0_gamma, specular_texture0.flare, light_directed0), 1.0);
    //FragColor = texture(diffuse_texture0.texture_data, vertex_param.TexCoord);
}