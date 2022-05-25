#version 330 core
layout(location = 0) in vec3 vertex_pos_modelspace;
layout(location = 1) in vec2 vertex_uv;

out vec2 uv;

uniform mat4 OBJECT_MATRIX;

void main() {
    gl_Position = OBJECT_MATRIX * vec4(vertex_pos_modelspace, 1.0);
    uv = vertex_uv;
}
