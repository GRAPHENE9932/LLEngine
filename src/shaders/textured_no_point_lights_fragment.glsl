#version 330 core
in vec3 fragment_pos_worldspace;
in vec2 uv;

out vec3 color;

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 color_and_strength;
    float inner_cutoff_angle_cos, outer_cutoff_angle_cos;
};

const int SPOT_LIGHTS_AMOUNT = 1;

uniform SpotLight SPOT_LIGHTS[SPOT_LIGHTS_AMOUNT];
uniform sampler2D TEXTURE_SAMPLER;

const float AMBIENT_STRENGTH = 0.0;
const vec3 AMBIENT_COLOR = vec3(1.0, 1.0, 1.0);

vec3 spot_light(SpotLight light) {
    float dist = distance(fragment_pos_worldspace, light.position);
    vec3 dir_from_light = (fragment_pos_worldspace - light.position) / dist;
    float cosine = dot(dir_from_light, light.direction);

    float inner_diff = cosine - light.inner_cutoff_angle_cos;
    float outer_diff = cosine - light.outer_cutoff_angle_cos;

    float attenuation = 1.0 / (1.0 + dist * dist);

    float intensity = (cosine - light.outer_cutoff_angle_cos) /
            (light.inner_cutoff_angle_cos - light.outer_cutoff_angle_cos);

    return intensity * attenuation * light.color_and_strength;
}

void main() {
    vec3 obj_color = texture(TEXTURE_SAMPLER, uv).xyz;

    vec3 ambient_light = AMBIENT_COLOR * AMBIENT_STRENGTH * obj_color;

    vec3 light_coeff = ambient_light;
    for (int i = 0; i < SPOT_LIGHTS_AMOUNT; i++)
        light_coeff += spot_light(SPOT_LIGHTS[i]);

    color = obj_color * light_coeff;
}
