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

#ifdef USING_UV
    #ifdef USING_SEPARATE_UV_OFFSET_SCALE
        in vec2 FRAG_BASE_UV;
    #else
        in vec2 FRAG_UV;
    #endif
#endif

#if defined(USING_NORMAL_MAP) 

// BEGIN Base color
#ifdef USING_BASE_COLOR_FACTOR
    uniform vec4 BASE_COLOR_FACTOR;
#endif

#ifdef USING_BASE_COLOR_TEXTURE
    uniform sampler2D BASE_COLOR_TEXTURE;
#endif

vec4 get_base_color() {
    #if defined(USING_BASE_COLOR_FACTOR) && defined(USING_BASE_COLOR_TEXTURE)
        return BASE_COLOR_FACTOR * texture(BASE_COLOR_TEXTURE, FRAG_UV);
    #elif defined(USING_BASE_COLOR_FACTOR)
        return BASE_COLOR_FACTOR;
    #elif defined(USING_BASE_COLOR_TEXTURE)
        return texture(BASE_COLOR_TEXTURE, FRAG_UV);
    #else
        return vec4(1.0, 0.0, 0.0, 1.0);
    #endif
}
// END Base color

// BEGIN Normals
#ifdef USING_NORMALS
    in vec3 FRAG_NORMAL;
#endif

#ifdef USING_NORMAL_MAP
    uniform sampler2D NORMAL_TEXTURE;
    in mat3 FRAG_TBN;
#endif

#ifdef USING_NORMAL_MAP_SCALE
    uniform float NORMAL_SCALE;
#endif

vec3 get_normal() {
    #if defined(USING_NORMAL_TEXTURE_SCALE) && defined(USING_NORMAL_TEXTURE)
        vec3 result = texture(NORMAL_TEXTURE, FRAG_UV);
        result = result * 2.0 - 1.0;
        result = normalize(FRAG_TBN * result) * NORMAL_SCALE;
        return result;
    #elif defined(USING_NORMAL_TEXTURE)
        vec3 result = texture(NORMAL_TEXTURE, FRAG_UV);
        result = result * 2.0 - 1.0;
        result = normalize(FRAG_TBN * result);
        return result;
    #elif defined(USING_NORMALS)
        return FRAG_NORMAL;
    #else
        return vec3(0.0, 0.0, 0.0);
    #endif
}
// END Normals

// BEGIN Lights
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

#if SPOT_LIGHTS_COUNT > 0
    uniform SpotLight SPOT_LIGHTS[SPOT_LIGHTS_COUNT];
#endif

#if POINT_LIGHTS_COUNT > 0
    uniform PointLight POINT_LIGHTS[POINT_LIGHTS_COUNT];
#endif

#if POINT_LIGHTS_COUNT > 0 || SPOT_LIGHTS_COUNT > 0
    in vec3 FRAG_POS;

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
#endif

// TODO: calc_spot_light
// END Lights

uniform vec3 AMBIENT;
out vec4 COLOR_OUT;

void main() {
    vec3 lightning_result = AMBIENT;
    
    #if POINT_LIGHTS_COUNT > 0
        for (int i = 0; i < POINT_LIGHTS_COUNT; i++) {
            lightning_result += calc_point_light(POINT_LIGHTS[i]);
        }
    #endif

    COLOR_OUT = get_base_color() * vec4(lightning_result, 1.0);
}