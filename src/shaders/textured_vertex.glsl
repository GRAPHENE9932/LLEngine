#version 330 core
layout(location = 0) in vec3 vertex_pos_modelspace;
layout(location = 1) in vec2 vertex_uv;
layout(location = 2) in vec3 vertex_normal_modelspace;

out vec3 vertex_normal_worldspace;
out vec3 fragment_pos_worldspace;
out vec2 uv;

uniform mat4 MVP;
uniform mat4 MODEL_MATRIX;

void main() {
    gl_Position = MVP * MODEL_MATRIX * vec4(vertex_pos_modelspace, 1.0);

    vertex_normal_worldspace = (MODEL_MATRIX * vec4(vertex_normal_modelspace, 0.0)).xyz;
    fragment_pos_worldspace = (MODEL_MATRIX * vec4(vertex_pos_modelspace, 1.0)).xyz;
    uv = vertex_uv;
}
