#version 330 core
in vec3 tex_coords;

out vec4 color;

uniform samplerCube CUBEMAP_SAMPLER;

void main() {
    color = texture(CUBEMAP_SAMPLER, tex_coords);
}
