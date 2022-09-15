#version 330 core

// Possible defines:
// USING_BASE_COLOR_TEXTURE
// USING_BASE_COLOR_FACTOR
// USING_VERTEX_NORMALS
// USING_NORMAL_TEXTURE
// USING_NORMAL_MAP_SCALE
// USING_UV
// USING_GENERAL_UV_TRANSFORM
// USING_NORMAL_UV_TRANSFORM
// USING_BASE_UV_TRANSFORM
// POINT_LIGHTS_COUNT <int>
// SPOT_LIGHTS_COUNT <int>

layout(location = 0) in vec3 VERTEX_POS;
#ifdef USING_UV
    layout(location = 1) in vec2 VERTEX_UV;
#endif
#ifdef USING_VERTEX_NORMALS
    layout(location = 2) in vec3 VERTEX_NORMAL;
#endif

// Not all uniforms are used at the same time.
// Unused ones will be optimized out.
uniform mat4 MVP;
uniform mat4 MODEL_MATRIX;
uniform mat4 NORMAL_MATRIX;
uniform vec2 UV_OFFSET;
uniform vec2 UV_SCALE;
uniform vec2 BASE_UV_OFFSET;
uniform vec2 BASE_UV_SCALE;
uniform vec2 NORMAL_UV_OFFSET;
uniform vec2 NORMAL_UV_SCALE;

// Not all outs are used at the same time.
// Unused ones must be optimized out (I hope).
out vec2 FRAG_BASE_UV;
out vec2 FRAG_NORMAL_UV;
out vec2 FRAG_UV;
out vec3 FRAG_NORMAL;
out mat3 FRAG_TBN;
out vec3 FRAG_POS;

void main() {
    gl_Position = MVP * vec4(VERTEX_POS, 1.0);

    #ifdef USING_VERTEX_NORMALS
        vec3 unfixed_normal = (MODEL_MATRIX * vec4(VERTEX_NORMAL, 0.0)).xyz;
        FRAG_NORMAL = (NORMAL_MATRIX * vec4(unfixed_normal, 0.0)).xyz;
    #endif

    FRAG_POS = (MODEL_MATRIX * vec4(VERTEX_POS, 1.0)).xyz;

    #ifdef USING_UV
        #ifdef USING_GENERAL_UV_TRANSFORM
            FRAG_UV = VERTEX_UV * UV_SCALE + UV_OFFSET;
        #else
            FRAG_UV = VERTEX_UV;
        #endif

        #ifdef USING_BASE_UV_TRANSFORM
            FRAG_BASE_UV = VERTEX_UV * BASE_UV_SCALE + BASE_UV_OFFSET;
        #endif

        #ifdef USING_NORMAL_UV_TRANSFORM
            FRAG_NORMAL_UV = VERTEX_UV * NORMAL_UV_SCALE + NORMAL_UV_OFFSET;
        #endif
    #endif
}
