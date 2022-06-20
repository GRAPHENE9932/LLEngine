#version 330 core
in vec2 uv;

out vec4 color;

uniform sampler2D TEXTURE_SAMPLER;

void main() {
    color = texture(TEXTURE_SAMPLER, uv);
}
