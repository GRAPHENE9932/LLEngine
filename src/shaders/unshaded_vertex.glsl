#version 330 core
layout(location = 0) in vec3 vertex_pos_modelspace;

uniform mat4 MVP;

void main() {
    gl_Position = MVP * vec4(vertex_pos_modelspace, 1.0);
}
