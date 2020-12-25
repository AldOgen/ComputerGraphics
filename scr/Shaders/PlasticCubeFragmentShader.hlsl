#version 330 core
out vec4 FragColor;


in FigureParam{
    vec3 FragPos;
    mat3 TBNMatrix;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} figure_param;


struct Texture {
    sampler2D texture_data;
    float flare;
    float diff_coef;
    float height_coef;
};

struct Light {
    vec3 position;

    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float attenuation_const;
    float attenuation_lin;
    float attenuation_quad;
};


#define CNT_DIFFUSE_MAP 1
uniform Texture diffuse_map[CNT_DIFFUSE_MAP];

#define CNT_SPECULAR_MAP 1
uniform Texture specular_map[CNT_SPECULAR_MAP];

#define CNT_DEPTH_MAP 1
uniform Texture depth_map[CNT_DEPTH_MAP];

#define CNT_SHADOW_MAP 1
uniform sampler2D shadow_map[CNT_SHADOW_MAP];

#define CNT_SHADOW_CUBE_MAP 1
uniform samplerCube shadow_cube_map;
uniform float far_plane;

uniform Light light_directed;

#define CNT_LIGHT_POINT  1
uniform Light light_point[CNT_LIGHT_POINT];

uniform vec3 view_position;


float ShadowCoefficientDirected(sampler2D shadow_map, vec3 normal, vec3 position) {
    // ����������� ���������� � ���������������
    vec3 projection_coords = figure_param.FragPosLightSpace.xyz / figure_param.FragPosLightSpace.w;
    projection_coords = projection_coords * 0.5 + 0.5;

    // ��������� ������� ������� �����
    float current_depth = projection_coords.z;

    // ������� ��������
    vec3 n_normal = normalize(normal);
    vec3 n_light_direction = normalize(position - figure_param.FragPos);

    float shadow_bias = max(0.005 * (1.0 - dot(n_normal, n_light_direction)), 0.0005);

    // PCF-������ � ����� ���������� 7�7
    float shadow = 0.0;
    vec2 size_texture = 1.0 / textureSize(shadow_map, 0);
    for (int x = -3; x <= 3; ++x) {
        for (int y = -3; y <= 3; ++y) {
            float current_closest_depth = texture(shadow_map, projection_coords.xy + vec2(x, y) * size_texture).r;
            shadow += current_depth - shadow_bias > current_closest_depth ? 1.0 : 0.0;
        }
    }
    shadow /= 49.0;

    if (projection_coords.z > 1.0) {
        shadow = 0.0;
    }

    return shadow;
}


float gamma = 2.2; // ��������� �����-���������


vec3 PhongLuminousFluxDirected(Texture diffuse_map, Texture specular_map, vec2 TexCoords, vec3 normal, Light light, float shadow) {
    // �����-���������
    vec3 diffuse_map_gamma = pow(texture(diffuse_map.texture_data, TexCoords).rgb, vec3(1.0 / gamma));
    vec3 specular_map_gamma = pow(texture(specular_map.texture_data, TexCoords).rgb, vec3(1.0 / gamma));

    // ���������� ��������� ������������ �����
    vec3 n_normal = normalize(normal);
    vec3 n_light_direction = normalize(-light.direction);
    float diff = max(dot(n_normal, n_light_direction), 0.0);

    // ���������� �������� ������������ �����
    vec3 n_view_direction = normalize(view_position - figure_param.FragPos);
    vec3 n_middle_normal = normalize(n_view_direction + n_light_direction);
    float spec = pow(max(dot(n_view_direction, n_middle_normal), 0.0), specular_map.flare);

    vec3 ambient = light.ambient * diffuse_map_gamma;
    vec3 diffuse = light.diffuse * diff * diffuse_map_gamma;
    vec3 specular = light.specular * spec * specular_map_gamma;

    return ambient + (1.0f - shadow) * (diffuse + specular);
}


vec3 PhongLuminousFluxPoint(Texture diffuse_map, Texture specular_map, vec2 TexCoords, vec3 normal, Light light, float shadow) {
    // �����-���������
    vec3 diffuse_map_gamma = pow(texture(diffuse_map.texture_data, TexCoords).rgb, vec3(1.0 / gamma));
    vec3 specular_map_gamma = pow(texture(specular_map.texture_data, TexCoords).rgb, vec3(1.0 / gamma));

    // ������ ����� ���������� � ��������
    vec3 direction = light.position - figure_param.FragPos;

    // ���������� ��������� ������������ �����
    vec3 n_normal = normalize(normal);
    vec3 n_light_direction = normalize(direction);
    float diff = max(dot(n_normal, n_light_direction), 0.0);

    // ���������� �������� ������������ �����
    vec3 n_view_direction = normalize(view_position - figure_param.FragPos);
    vec3 n_middle_normal = normalize(n_view_direction + n_light_direction);
    float spec = pow(max(dot(n_view_direction, n_middle_normal), 0.0), specular_map.flare);

    // ���������
    float distance = length(direction);
    float attenuation = 1.0 / (light.attenuation_const +
                               light.attenuation_lin * distance +
                               light.attenuation_quad * distance * distance);

    vec3 ambient = light.ambient * diffuse_map_gamma;
    vec3 diffuse = light.diffuse * diff * diffuse_map_gamma;
    vec3 specular = light.specular * spec * diffuse_map_gamma;

    return attenuation * (ambient + (1.0f - shadow) * (diffuse + specular));
}



void main() {
    float shadow_directed = ShadowCoefficientDirected(shadow_map[0], figure_param.Normal, -light_directed.position);
    FragColor = vec4(PhongLuminousFluxDirected(diffuse_map[0], diffuse_map[0], figure_param.TexCoords, figure_param.Normal, light_directed, shadow_directed), 1.0);
}
