R""(
#version 330

uniform sampler2D base_color_texture;
uniform vec4 color_factor;

in vec2 frag_uv;

out vec4 color_out;

void main() {
    color_out = texture(base_color_texture, frag_uv) * color_factor;
}
)""