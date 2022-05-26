#version 330 core
in vec3 vertex_normal_worldspace;
in vec2 uv;

out vec3 color;

uniform vec3 LIGHT_DIRECTION;
uniform vec3 CAMERA_DIRECTION;
uniform sampler2D TEXTURE_SAMPLER;

const vec3 LIGHT_COLOR = vec3(0.5, 0.5, 0.5);
const float AMBIENT_WEIGHT = 0.55;

// Cosine of the angle between the normal and light direction.
float cos_theta = max(dot(vertex_normal_worldspace, LIGHT_DIRECTION), 0.0);

vec3 reflected_dir = reflect(LIGHT_DIRECTION, vertex_normal_worldspace);
float cos_alpha = clamp(dot(reflected_dir, LIGHT_DIRECTION), 0.0, 1.0);

void main() {
    vec3 diffuse_color = texture(TEXTURE_SAMPLER, uv).xyz;
    color = diffuse_color * LIGHT_COLOR * cos_theta +
        diffuse_color * AMBIENT_WEIGHT +
        diffuse_color * pow(cos_alpha, 5) * LIGHT_COLOR;
}
