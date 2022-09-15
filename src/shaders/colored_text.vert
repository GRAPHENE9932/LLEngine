#version 330 core
layout(location = 0) in vec3 vertex_pos_modelspace_in;
layout(location = 1) in vec2 vertex_uv_in;

out vec2 passed_uv;

uniform mat4 mvp_unif;

void main() {
    gl_Position = mvp_unif * vec4(vertex_pos_modelspace_in, 1.0);
    passed_uv = vertex_uv_in;
}