R""(
#version 330 core

layout(location = 0) in vec2 vertex_pos;
layout(location = 1) in vec2 vertex_uv;

out vec2 frag_uv;

void main() {
    gl_Position = vec4(vertex_pos, 0.0, 1.0);
    frag_uv = vertex_uv;
}
)""