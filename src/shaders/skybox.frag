#version 330 core

in vec3 frag_tex_coords;

out vec4 color_out;

uniform samplerCube cubemap;

void main() {
    color_out = texture(cubemap, frag_tex_coords);
}
