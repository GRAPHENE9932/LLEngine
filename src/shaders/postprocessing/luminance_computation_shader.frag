R""(
#version 330 core

in vec2 frag_uv;
out vec4 color_out;

uniform sampler2D source_texture;

void main() {
    color_out.r = dot(vec3(0.2126, 0.7152, 0.0722), texture(source_texture, frag_uv).rgb);
    color_out.gba = vec3(0.0, 0.0, 1.0);
}
)""