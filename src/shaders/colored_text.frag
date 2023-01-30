#version 330 core

in vec2 frag_uv;

out vec4 color_out;

uniform sampler2D font_texture;
uniform vec3 text_color;

void main() {
    color_out.rgb = text_color;
    color_out.a = texture(font_texture, frag_uv).r;
}
