#version 330 core
in vec2 uv_in;

out vec4 color_out;

uniform sampler2D texture_unif;

void main() {
    color_out = texture(texture_unif, uv_in);
}
