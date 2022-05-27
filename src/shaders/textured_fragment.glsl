#version 330 core
in vec3 vertex_normal_worldspace;
in vec3 fragment_pos_worldspace;
in vec2 uv;

out vec3 color;

struct PointLight {
    vec3 position;
    vec3 color;
    float diffuse_strength;
    float specular_strength;

    float const_coeff, linear_coeff, quadratic_coeff;
};

const int TX_DRW_POINT_LIGHTS_AMOUNT = 4;

uniform PointLight POINT_LIGHTS[TX_DRW_POINT_LIGHTS_AMOUNT];
uniform vec3 CAMERA_DIRECTION;
uniform mat4 NORMAL_MATRIX;
uniform sampler2D TEXTURE_SAMPLER;

vec3 normal_worldspace = (NORMAL_MATRIX * vec4(vertex_normal_worldspace, 0.0)).xyz;

const float AMBIENT_STRENGTH = 0.25;
const vec3 AMBIENT_COLOR = vec3(1.0, 1.0, 1.0);
const int SPECULAR_SHININESS = 4;

vec3 point_light(PointLight light) {
    light.diffuse_strength = 1.0;
    light.specular_strength = 0.5;

    vec3 light_direction = normalize(fragment_pos_worldspace - light.position);
    float diffuse = max(dot(normal_worldspace, -light_direction), 0.0);
    vec3 diffuse_color = light.diffuse_strength * diffuse * light.color;

    vec3 reflect_dir = reflect(light_direction, normal_worldspace);
    float specular = pow(max(dot(CAMERA_DIRECTION, reflect_dir), 0.0), SPECULAR_SHININESS);
    vec3 specular_color = light.specular_strength * specular * light.color;

    float d = length(light.position - fragment_pos_worldspace);
    float attenuation = 1.0 / (light.const_coeff + light.linear_coeff * d + light.quadratic_coeff * (d * d));

    return (diffuse_color + specular_color) * attenuation;
}

void main() {
    vec3 obj_color = texture(TEXTURE_SAMPLER, uv).xyz;

    vec3 ambient_light = AMBIENT_COLOR * AMBIENT_STRENGTH * obj_color;

    vec3 light_coeff = ambient_light;
    for (int i = 0; i < TX_DRW_POINT_LIGHTS_AMOUNT; i++) {
        light_coeff += point_light(POINT_LIGHTS[i]);
    }

    color = obj_color * light_coeff;
}
