R""(
#version 330

layout (location = 0) in vec3 vertex_pos;

out vec3 frag_local_pos;

uniform mat4 mvp;

void main() {
    frag_local_pos = vertex_pos;
    gl_Position = mvp * vec4(vertex_pos, 1.0);
}
)""