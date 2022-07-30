#version 330 core

// SPOT_LIGHTS_COUNT and POINT_LIGHTS_COUNT macros
// must be defined here.

layout(location = 0) in vec3 vertex_pos_modelspace_in;
layout(location = 1) in vec2 vertex_uv_in;
#if POINT_LIGHTS_COUNT != 0 || SPOT_LIGHTS_COUNT != 0
layout(location = 2) in vec3 vertex_normal_modelspace_in;
#endif

#if POINT_LIGHTS_COUNT != 0 || SPOT_LIGHTS_COUNT != 0
out vec3 passed_normal_worldspace;
#endif
#if POINT_LIGHTS_COUNT != 0 || SPOT_LIGHTS_COUNT != 0
out vec3 passed_fragment_pos_worldspace;
#endif
out vec2 passed_uv;

uniform mat4 mvp_unif;
uniform mat4 model_matrix_unif;
#if POINT_LIGHTS_COUNT != 0 || SPOT_LIGHTS_COUNT != 0
uniform mat4 normal_matrix_unif;
#endif

void main() {
    gl_Position = mvp_unif * vec4(vertex_pos_modelspace_in, 1.0);

#if POINT_LIGHTS_COUNT != 0 || SPOT_LIGHTS_COUNT != 0
    vec3 orig_normal_worldspace = (model_matrix_unif * vec4(vertex_normal_modelspace_in, 0.0)).xyz;
    passed_normal_worldspace = (normal_matrix_unif * vec4(orig_normal_worldspace, 0.0)).xyz;
    passed_fragment_pos_worldspace = (model_matrix_unif * vec4(vertex_pos_modelspace_in, 1.0)).xyz;
#endif

    passed_uv = vertex_uv_in;
}
