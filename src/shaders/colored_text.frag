#version 330 core
in vec2 passed_uv;

out vec4 color_out;

uniform sampler2D texture_unif;
uniform vec3 color_unif;

void main() {
    color_out.rgb = color_unif;
    color_out.a = texture(texture_unif, passed_uv).r;
}
