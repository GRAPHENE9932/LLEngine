#version 330 core

// Possible defines:
// USING_BASE_COLOR_TEXTURE
// USING_BASE_COLOR_FACTOR
// USING_VERTEX_NORMALS
// USING_NORMAL_TEXTURE
// USING_NORMAL_MAP_SCALE
// USING_FRAGMENT_POSITION
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
#ifdef USING_NORMAL_TEXTURE
    layout(location = 3) in vec4 VERTEX_TANGENT;
#endif

struct PointLight {
    vec3 position;
    vec3 color;
    float diffuse_strength;
    float const_coeff, linear_coeff, quadratic_coeff;
};
struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 color_and_strength;
    float inner_cutoff_angle_cos, outer_cutoff_angle_cos;
};

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
#if POINT_LIGHTS_COUNT > 0
    uniform PointLight POINT_LIGHTS[POINT_LIGHTS_COUNT];
#endif
#if SPOT_LIGHTS_COUNT > 0
    uniform SpotLight SPOT_LIGHTS[SPOT_LIGHTS_COUNT];
#endif

#ifdef USING_BASE_UV_TRANSFORM
    out vec2 FRAG_BASE_UV;
#endif
#ifdef USING_NORMAL_UV_TRANSFORM
    out vec2 FRAG_NORMAL_UV;
#endif
#ifdef USING_UV
    out vec2 FRAG_UV;
#endif
#ifdef USING_VERTEX_NORMALS
    out vec3 FRAG_NORMAL;
#endif
#ifdef USING_FRAGMENT_POSITION
    // May be in tangent space if USING_NORMAL_TEXTURE is enabled.
    out vec3 FRAG_POS;
#endif
#ifdef USING_NORMAL_TEXTURE
    #if POINT_LIGHTS_COUNT > 0
        out vec3 POINT_LIGHT_POSITIONS_TANGENT_SPACE[POINT_LIGHTS_COUNT];
    #endif
    #if SPOT_LIGHTS_COUNT > 0
        out vec3 SPOT_LIGHT_POSITIONS_TANGENT_SPACE[SPOT_LIGHTS_COUNT];
        out vec3 SPOT_LIGHT_DIRECTIONS_TANGENT_SPACE[SPOT_LIGHTS_COUNT];
    #endif
#endif

void main() {
    gl_Position = MVP * vec4(VERTEX_POS, 1.0);

    #ifdef USING_VERTEX_NORMALS
        vec3 normal = (NORMAL_MATRIX * MODEL_MATRIX * vec4(VERTEX_NORMAL, 0.0)).xyz;
        FRAG_NORMAL = normal;

        #ifdef USING_NORMAL_TEXTURE
            vec3 tangent = (NORMAL_MATRIX * MODEL_MATRIX * vec4(VERTEX_TANGENT.xyz, 0.0)).xyz;
            vec3 bitangent = cross(normal, tangent) * VERTEX_TANGENT.w;
            mat3 tbn = mat3(tangent, bitangent, normal);
            // TBN matrix is orthogonal, so we can use the transpose() function
            // instead of inverse().
            mat3 tbn_inverse = transpose(tbn);

            #if POINT_LIGHTS_COUNT > 0
                for (int i = 0; i < POINT_LIGHTS_COUNT; i++) {
                    POINT_LIGHT_POSITIONS_TANGENT_SPACE[i] = tbn_inverse * POINT_LIGHTS[i].position;
                }
            #endif
            #if SPOT_LIGHTS_COUNT > 0
                for (int i = 0; i < SPOT_LIGHTS_COUNT; i++) {
                    SPOT_LIGHT_POSITIONS_TANGENT_SPACE[i] = tbn_inverse * SPOT_LIGHTS[i].position;
                    SPOT_LIGHT_DIRECTIONS_TANGENT_SPACE[i] = tbn_inverse * SPOT_LIGHTS[i].direction;
                }
            #endif
        #endif
    #endif

    #ifdef USING_FRAGMENT_POSITION
        #ifdef USING_NORMAL_TEXTURE
            FRAG_POS = tbn_inverse * (MODEL_MATRIX * vec4(VERTEX_POS, 1.0)).xyz;
        #else
            FRAG_POS = (MODEL_MATRIX * vec4(VERTEX_POS, 1.0)).xyz;
        #endif
    #endif

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
