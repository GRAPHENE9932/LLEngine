#version 330 core
layout(location = 0) in vec3 vertex_pos_modelspace;
layout(location = 1) in vec2 vertex_uv;

out vec3 fragment_pos_worldspace;
out vec2 uv;

uniform mat4 MVP;
uniform mat4 MODEL_MATRIX;

void main() {
    gl_Position = MVP * vec4(vertex_pos_modelspace, 1.0);

    fragment_pos_worldspace = (MODEL_MATRIX * vec4(vertex_pos_modelspace, 1.0)).xyz;
    uv = vertex_uv;
}
