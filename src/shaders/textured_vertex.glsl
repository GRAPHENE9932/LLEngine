#version 330 core
// SPOT_LIGHTS_COUNT and POINT_LIGHTS_COUNT macros
// must be defined here.
layout(location = 0) in vec3 vertex_pos_modelspace;
layout(location = 1) in vec2 vertex_uv;
#if POINT_LIGHTS_COUNT != 0 || SPOT_LIGHTS_COUNT != 0
layout(location = 2) in vec3 vertex_normal_modelspace;
#endif

#if POINT_LIGHTS_COUNT != 0 || SPOT_LIGHTS_COUNT != 0
out vec3 normal_worldspace;
#endif
#if POINT_LIGHTS_COUNT != 0 || SPOT_LIGHTS_COUNT != 0
out vec3 fragment_pos_worldspace;
#endif
out vec2 uv;

uniform mat4 MVP;
uniform mat4 MODEL_MATRIX;
#if POINT_LIGHTS_COUNT != 0 || SPOT_LIGHTS_COUNT != 0
uniform mat4 NORMAL_MATRIX;
#endif

void main() {
    gl_Position = MVP * vec4(vertex_pos_modelspace, 1.0);

#if POINT_LIGHTS_COUNT != 0 || SPOT_LIGHTS_COUNT != 0
    vec3 orig_normal_worldspace = (MODEL_MATRIX * vec4(vertex_normal_modelspace, 0.0)).xyz;
    normal_worldspace = (NORMAL_MATRIX * vec4(orig_normal_worldspace, 0.0)).xyz;
    fragment_pos_worldspace = (MODEL_MATRIX * vec4(vertex_pos_modelspace, 1.0)).xyz;
#endif

    uv = vertex_uv;
}
