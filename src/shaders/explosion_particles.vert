R""(
#version 330 core

layout (location = 0) in vec3 vertex_pos;
layout (location = 1) in vec3 target_offset;

uniform mat4 center_mvp;
uniform vec3 particle_scale_inverse;

uniform float spreading_distance;
uniform float scale_decline_power;

uniform float phase;
uniform uint particles_count;

const float PI = 3.1415926;

mat4 build_matrix(vec3 translation, float scale) {
    mat4 result_1 = mat4(scale);
    result_1[3][3] = 1.0;

    mat4 result_2 = mat4(1.0);
    result_2[3][0] = translation.x;
    result_2[3][1] = translation.y;
    result_2[3][2] = translation.z;
    return result_2 * result_1;
}

void main() {
    vec3 offset = target_offset * phase;
    float scale = 1.0 - pow(phase, scale_decline_power);

    mat4 extra_matrix = build_matrix(offset * particle_scale_inverse, scale);
    gl_Position = (center_mvp * extra_matrix) * vec4(vertex_pos, 1.0);
}
)""