#version 330
layout (location = 0) in vec3 vertex_pos_modelspace;

out vec3 tex_coords;

uniform mat4 MVP;

void main() {
    tex_coords = vertex_pos_modelspace;

    vec4 pos = MVP * vec4(vertex_pos_modelspace, 1.0);

    // Perspective division will divide all components of
    // position by w. So, if we set the depth (z) to w,
    // it will end up with value 1.0 (the greatest depth value).
    // I need this to draw the skybox after all other objects
    // (except overlay ones).
    gl_Position = pos.xyww;
}
