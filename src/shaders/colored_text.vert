#version 330 core
layout(location = 0) in vec3 vertex_pos_modelspace;
layout(location = 1) in vec2 vertex_uv;

out vec2 frag_uv;

uniform mat4 mvp;

void main() {
    gl_Position = mvp * vec4(vertex_pos_modelspace, 1.0);
    frag_uv = vertex_uv;
}
