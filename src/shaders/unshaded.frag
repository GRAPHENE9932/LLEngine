#version 330 core
out vec3 color_out;

uniform vec3 color_unif;

void main() {
    color_out = color_unif;
}
