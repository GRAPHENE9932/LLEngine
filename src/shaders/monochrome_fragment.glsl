#version 330 core
in vec3 vertex_normal_worldspace;

out vec3 color;

uniform vec3 COLOR;
uniform vec3 LIGHT_DIRECTION;

float cos_alpha = dot(vertex_normal_worldspace, LIGHT_DIRECTION);

void main() {
    color = COLOR * cos_alpha;
}
