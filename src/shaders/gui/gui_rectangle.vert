R""(
#version 330
layout(location = 0) in vec3 vertex_pos_modelspace;
layout(location = 1) in vec2 vertex_uv;

uniform mat4 mvp;
uniform vec2 uv_scale;
uniform vec2 uv_offset;

out vec2 frag_uv;

void main() {
    gl_Position = mvp * vec4(vertex_pos_modelspace, 1.0);
    frag_uv = vertex_uv * uv_scale + uv_offset;
}
)""