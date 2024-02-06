R""(
#version 330 core

uniform sampler2D source_1;
uniform sampler2D source_2;

in vec2 frag_uv;
out vec4 color_out;

void main() {
    color_out = vec4(texture(source_1, frag_uv).rgb + texture(source_2, frag_uv).rgb, 1.0);
}
)""