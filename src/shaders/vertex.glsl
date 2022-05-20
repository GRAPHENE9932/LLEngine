#version 330 core
layout(location = 0) in vec3 vertex_pos_modelspace;
layout(location = 1) in vec3 normals_input;

out vec3 normals_output;

uniform mat4 MVP;

void main() {
    gl_Position = MVP * vec4(vertex_pos_modelspace, 1.0);
    normals_output = normals_input;
}
