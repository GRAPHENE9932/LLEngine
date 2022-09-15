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

out vec4 COLOR_OUT;

// Not all ins are used at the same time.
// Unused ones must be optimized out (I hope).
in vec2 FRAG_BASE_UV;
in vec2 FRAG_NORMAL_UV;
in vec2 FRAG_UV;
in vec3 FRAG_NORMAL;
in mat3 FRAG_TBN;
in vec3 FRAG_POS;

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 color_and_strength;
    float inner_cutoff_angle_cos, outer_cutoff_angle_cos;
};
struct PointLight {
    vec3 position;
    vec3 color;
    float diffuse_strength;
    float const_coeff, linear_coeff, quadratic_coeff;
};

// Not all uniforms are used at the same time.
// Unused ones will be optimized out.
uniform sampler2D BASE_COLOR_TEXTURE;
uniform sampler2D NORMAL_TEXTURE;
uniform vec4 BASE_COLOR_FACTOR;
uniform float NORMAL_MAP_SCALE;
uniform vec3 AMBIENT;
#if SPOT_LIGHTS_COUNT > 0
    uniform SpotLight SPOT_LIGHTS[SPOT_LIGHTS_COUNT];
#endif
#if POINT_LIGHTS_COUNT > 0
    uniform PointLight POINT_LIGHTS[POINT_LIGHTS_COUNT];
#endif

vec2 get_base_uv() {
    #ifdef USING_UV
        #ifdef USING_BASE_UV_TRANSFORM
            return FRAG_BASE_UV;
        #else
            return FRAG_UV;
        #endif
    #else
        return vec2(0.0, 0.0);
    #endif
}

vec2 get_normal_uv() {
    #ifdef USING_UV
        #ifdef USING_NORMAL_UV_TRANSFORM
            return FRAG_NORMAL_UV;
        #else
            return FRAG_UV;
        #endif
    #else
        return vec2(0.0, 0.0);
    #endif
}

vec4 get_base_color() {
    #ifdef USING_BASE_COLOR_TEXTURE
        #ifdef USING_BASE_COLOR_FACTOR
            return texture(BASE_COLOR_TEXTURE, get_base_uv()) * BASE_COLOR_FACTOR;
        #else
            return texture(BASE_COLOR_TEXTURE, get_base_uv());
        #endif
    #else
        #ifdef USING_BASE_COLOR_FACTOR
            return BASE_COLOR_FACTOR;
        #else
            return vec4(0.0, 0.0, 0.0, 1.0);
        #endif
    #endif
}

vec3 get_normal() {
    #ifdef USING_VERTEX_NORMALS
        #ifdef USING_NORMAL_TEXTURE
            #ifdef USING_NORMAL_MAP_SCALE
                UNIMPLEMENTED
            #else
                UNIMPLEMENTED 
            #endif
        #else
            return FRAG_NORMAL;
        #endif
    #else
        return vec3(0.0, 0.0, 0.0);
    #endif
}

vec3 calc_point_light(PointLight light) {
    float distance = length(FRAG_POS - light.position);
    vec3 direction = (FRAG_POS - light.position) / distance;

    float diffuse_factor = max(dot(get_normal(), -direction), 0.0) * light.diffuse_strength;
    vec3 diffuse_color = diffuse_factor * light.color;

    float attenuation = 1.0 / (
        light.const_coeff +
        light.linear_coeff * distance +
        light.quadratic_coeff * (distance * distance)
    );

    return attenuation * diffuse_color;
}

void main() {
    vec3 lightning_result = AMBIENT;
    
    #if POINT_LIGHTS_COUNT > 0
        for (int i = 0; i < POINT_LIGHTS_COUNT; i++) {
            lightning_result += calc_point_light(POINT_LIGHTS[i]);
        }
    #endif

    COLOR_OUT = get_base_color() * vec4(lightning_result, 1.0);
}
