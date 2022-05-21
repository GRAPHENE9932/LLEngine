#version 330 core
layout(location = 0) in vec3 vertex_pos_modelspace;
layout(location = 1) in vec3 vertex_normal_modelspace;

out vec3 vertex_normal_worldspace;

uniform mat4 MVP;
uniform mat4 CAMERA_MATRIX;
uniform mat4 OBJECT_MATRIX;

void main() {
    gl_Position = MVP * OBJECT_MATRIX * vec4(vertex_pos_modelspace, 1.0);
    vertex_normal_worldspace = (OBJECT_MATRIX * vec4(vertex_normal_modelspace, 0.0)).xyz;
}
