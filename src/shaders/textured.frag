#version 330 core

// SPOT_LIGHTS_COUNT and POINT_LIGHTS_COUNT macros
// must be defined here.

#if POINT_LIGHTS_COUNT != 0 || SPOT_LIGHTS_COUNT != 0
in vec3 passed_normal_worldspace;
in vec3 passed_fragment_pos_worldspace;
#endif
in vec2 passed_uv;

out vec3 color_out;

#if SPOT_LIGHTS_COUNT != 0
struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 color_and_strength;
    float inner_cutoff_angle_cos, outer_cutoff_angle_cos;
};
uniform SpotLight spot_lights_unif[SPOT_LIGHTS_COUNT];
#endif

#if POINT_LIGHTS_COUNT != 0
struct PointLight {
    vec3 position;
    vec3 color;
    float diffuse_strength;
    float const_coeff, linear_coeff, quadratic_coeff;
};
uniform PointLight point_lights_unif[POINT_LIGHTS_COUNT];
#endif

uniform sampler2D texture_unif;

const float AMBIENT_STRENGTH = 0.0;
const vec3 AMBIENT_COLOR = vec3(1.0, 1.0, 1.0);

#if SPOT_LIGHTS_COUNT != 0
vec3 spot_light(SpotLight light) {
    float dist = distance(passed_fragment_pos_worldspace, light.position);
    vec3 dir_from_light = (passed_fragment_pos_worldspace - light.position) / dist;
    float cosine = dot(dir_from_light, light.direction);

    float inner_diff = cosine - light.inner_cutoff_angle_cos;
    float outer_diff = cosine - light.outer_cutoff_angle_cos;

    float attenuation = 1.0 / (1.0 + dist * dist);

    float intensity = (cosine - light.outer_cutoff_angle_cos) /
            (light.inner_cutoff_angle_cos - light.outer_cutoff_angle_cos);

    float normals_contribution = max(dot(passed_normal_worldspace, -dir_from_light), 0.0);

    return intensity * attenuation * normals_contribution * light.color_and_strength;
}
#endif

#if POINT_LIGHTS_COUNT != 0
vec3 point_light(PointLight light) {
    vec3 light_fragment_vec = passed_fragment_pos_worldspace - light.position;
    float dist = length(light_fragment_vec);
    vec3 light_direction = (passed_fragment_pos_worldspace - light.position) / dist;
    float diffuse = max(dot(passed_normal_worldspace, -light_direction), 0.0);
    vec3 diffuse_color = light.diffuse_strength * diffuse * light.color;

    float attenuation =
            1.0 / (light.const_coeff + light.linear_coeff * dist + light.quadratic_coeff * (dist * dist));

    return diffuse_color * attenuation;
}
#endif

void main() {
    vec3 obj_color = texture(texture_unif, passed_uv).xyz;

    vec3 ambient_light = AMBIENT_COLOR * AMBIENT_STRENGTH * obj_color;

    vec3 light_coeff = ambient_light;

#if POINT_LIGHTS_COUNT > 0
    for (int i = 0; i < POINT_LIGHTS_COUNT; i++)
        light_coeff += point_light(point_lights_unif[i]);
#endif

#if SPOT_LIGHTS_COUNT > 0
    for (int i = 0; i < SPOT_LIGHTS_COUNT; i++)
        light_coeff += spot_light(spot_lights_unif[i]);
#endif

    color_out = obj_color * light_coeff;
}
