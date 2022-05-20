#version 330 core
in vec3 normals_output;

out vec3 color;

uniform vec3 COLOR;

float cos_alpha = dot(normals_output, vec3(0.0, 1.0, 1.0));

void main() {
    color = COLOR * cos_alpha;
}
