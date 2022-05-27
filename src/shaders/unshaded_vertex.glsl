#version 330 core
layout(location = 0) in vec3 vertex_pos_modelspace;

uniform mat4 MVP;
uniform mat4 MODEL_MATRIX;

void main() {
    gl_Position = MVP * MODEL_MATRIX * vec4(vertex_pos_modelspace, 1.0);
}
