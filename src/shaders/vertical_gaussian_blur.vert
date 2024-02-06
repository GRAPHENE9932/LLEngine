R""(
#version 330

layout(location = 0) in vec3 vert_pos;
layout(location = 1) in vec2 vert_uv;

out vec2 frag_uv;

void main() {
    gl_Position = vec4(vert_pos, 1.0);
    frag_uv = vert_uv;
}
)""