R""(
#version 330 core

in vec2 frag_uv;

out vec4 color_out;

uniform sampler2D previous_lod;

void main() {
    color_out = texture(previous_lod, frag_uv);
}
)""