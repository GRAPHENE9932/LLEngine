R""(
#version 330 core

uniform vec3 particle_color;

out vec3 color_out;

void main() {
    color_out = particle_color;
}
)""