#version 330 core
in vec3 normal_worldspace;
in vec3 fragment_pos_worldspace;
in vec2 uv;

out vec3 color;

struct PointLight {
    vec3 position;
    vec3 color;
    float diffuse_strength;

    float const_coeff, linear_coeff, quadratic_coeff;
};

const int POINT_LIGHTS_AMOUNT = 2;

uniform PointLight POINT_LIGHTS[POINT_LIGHTS_AMOUNT];
uniform sampler2D TEXTURE_SAMPLER;

const float AMBIENT_STRENGTH = 0.25;
const vec3 AMBIENT_COLOR = vec3(1.0, 1.0, 1.0);

vec3 point_light(PointLight light) {
    vec3 light_fragment_vec = fragment_pos_worldspace - light.position;
    float d = length(light_fragment_vec);
    vec3 light_direction = (fragment_pos_worldspace - light.position) / d;
    float diffuse = max(dot(normal_worldspace, -light_direction), 0.0);
    vec3 diffuse_color = light.diffuse_strength * diffuse * light.color;

    float attenuation = 1.0 / (light.const_coeff + light.linear_coeff * d + light.quadratic_coeff * (d * d));

    return diffuse_color * attenuation;
}

void main() {
    vec3 obj_color = texture(TEXTURE_SAMPLER, uv).xyz;

    vec3 ambient_light = AMBIENT_COLOR * AMBIENT_STRENGTH * obj_color;

    vec3 light_coeff = ambient_light;
    for (int i = 0; i < POINT_LIGHTS_AMOUNT; i++) {
        light_coeff += point_light(POINT_LIGHTS[i]);
    }

    color = obj_color * light_coeff;
}
