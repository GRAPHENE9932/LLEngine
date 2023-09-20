R""(
#version 330 core

in vec2 frag_uv;

uniform sampler2D main_image;
uniform sampler2D bloom_image;
uniform float exposure;

out vec3 color_out;

void main() {
    color_out = texture(main_image, frag_uv).rgb + texture(bloom_image, frag_uv).rgb * 0.1;
    color_out = 1.0 - exp(-color_out * exposure);
}
)""