#version 330 core

// #define USING_BASE_COLOR_TEXTURE
// #define USING_BASE_COLOR_FACTOR
// #define USING_NORMALS
// #define USING_NORMAL_MAP
// #define USING_NORMAL_MAP_SCALE
// #define USING_GENERAL_UV_OFFSET_SCALE
// #define USING_SEPARATE_UV_OFFSET_SCALE
// #define POINT_LIGHTS_COUNT <int>
// #define SPOT_LIGHTS_COUNT <int>

#if defined(USING_BASE_COLOR_TEXTURE) ||\
    defined(USING_NORMAL_MAP)
    #define USING_UV
#endif

// BEGIN out section.
#ifdef USING_UV
    #ifdef USING_SEPARATE_UV_OFFSET_SCALE
        out vec2 FRAG_BASE_UV;
    #else
        out vec2 FRAG_UV;
#endif

#ifdef USING_NORMALS
    out vec3 FRAG_NORMAL;
#endif

#ifdef USING_NORMAL_MAP
    out mat3 FRAG_TBN;
    #ifdef USING_SEPARATE_UV_OFFSET_SCALE
        out vec2 FRAG_NORMAL_UV;
    #endif
#endif

#if POINT_LIGHTS_COUNT > 0 || SPOT_LIGHTS_COUNT > 0
    out vec3 FRAG_POS;
#endif
// END out section.

// BEGIN in section.
layout(location = 0) in vec3 VERTEX_POS;
#ifdef USING_UV
    layout(location = 1) in vec2 VERTEX_UV;
#endif
#ifdef USING_NORMALS
    layout(location = 2) in vec3 VERTEX_NORMAL;
#endif
// END in section.

// BEGIN uniform section.
uniform mat4 MVP;
#if defined(USING_NORMALS) || POINT_LIGHTS_COUNT > 0 || SPOT_LIGHTS_COUNT > 0
    uniform mat4 MODEL_MATRIX;
#endif
#ifdef USING_NORMALS
    uniform mat4 NORMAL_MATRIX;
#endif
#ifdef USING_GENERAL_UV_OFFSET_SCALE
    uniform vec2 UV_OFFSET;
    uniform vec2 UV_SCALE;
#elif defined(USING_SEPARATE_UV_OFFSET_SCALE)
    uniform vec2 BASE_UV_OFFSET;
    uniform vec2 BASE_UV_SCALE;
    uniform vec2 NORMAL_UV_OFFSET;
    uniform vec2 NORMAL_UV_SCALE;
#endif
// END uniform section.

void main() {
    gl_Position = MVP * vec4(VERTEX_POS, 1.0);

    #ifdef USING_NORMALS
        vec3 unfixed_normal = (MODEL_MATRIX * vec4(VERTEX_NORMAL, 0.0)).xyz;
        FRAG_NORMAL = (NORMAL_MATRIX * vec4(unfixed_normal, 0.0)).xyz;
    #endif

    #if POINT_LIGHTS_COUNT > 0 || SPOT_LIGHTS_COUNT > 0
        FRAG_POS = (MODEL_MATRIX * vec4(VERTEX_POS, 1.0)).xyz;
    #endif

    #ifdef USING_UV
        #ifdef USING_GENERAL_UV_OFFSET_SCALE
            FRAG_UV = VERTEX_UV * UV_SCALE + UV_OFFSET;
        #elif defined(USING_SEPARATE_UV_OFFSET_SCALE)

        #else
            FRAG_UV = VERTEX_UV;
        #endif
    #endif
}
 