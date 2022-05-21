#version 330 core
in vec3 vertex_normal_worldspace;
in vec2 uv;

out vec3 color;

uniform vec3 LIGHT_DIRECTION;
uniform sampler2D TEXTURE_SAMPLER;

float light_component = max(dot(vertex_normal_worldspace, LIGHT_DIRECTION), 0.0);
const float ambient_component = 0.75;

void main() {
    color = texture(TEXTURE_SAMPLER, uv).rgb * min(light_component + ambient_component, 1.0);
}
