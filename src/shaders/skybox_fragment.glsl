#version 330 core
in vec3 passed_tex_coords;

out vec4 color_out;

uniform samplerCube cubemap_unif;

void main() {
    color_out = texture(cubemap_unif, passed_tex_coords);
}
