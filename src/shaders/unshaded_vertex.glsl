#version 330 core
layout(location = 0) in vec3 vertex_pos_modelspace_in;

uniform mat4 mvp_unif;

void main() {
    gl_Position = mvp_unif * vec4(vertex_pos_modelspace_in, 1.0);
}
