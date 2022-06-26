#version 330 core
in vec2 uv;

out vec4 color;

uniform sampler2D TEXTURE_SAMPLER;
uniform vec3 COLOR;

void main() {
    color.rgb = COLOR;
    color.a = texture(TEXTURE_SAMPLER, uv).r;
}
