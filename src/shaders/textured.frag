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

out vec4 COLOR_OUT;

#ifdef USING_BASE_UV_TRANSFORM
    in vec2 FRAG_BASE_UV;
#endif
#ifdef USING_NORMAL_UV_TRANSFORM
    in vec2 FRAG_NORMAL_UV;
#endif
#ifdef USING_UV
    in vec2 FRAG_UV;
#endif
#ifdef USING_VERTEX_NORMALS
    in vec3 FRAG_NORMAL;
#endif
#ifdef USING_FRAGMENT_POSITION
    in vec3 FRAG_POS;
#endif
#ifdef USING_NORMAL_TEXTURE
    #if POINT_LIGHTS_COUNT > 0
        in vec3 POINT_LIGHT_POSITIONS_TANGENT_SPACE[POINT_LIGHTS_COUNT];
    #endif
    #if SPOT_LIGHTS_COUNT > 0
        in vec3 SPOT_LIGHT_POSITIONS_TANGENT_SPACE[SPOT_LIGHTS_COUNT];
        in vec3 SPOT_LIGHT_DIRECTIONS_TANGENT_SPACE[SPOT_LIGHTS_COUNT];
    #endif
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
uniform sampler2D BASE_COLOR_TEXTURE;
uniform sampler2D NORMAL_TEXTURE;
uniform vec4 BASE_COLOR_FACTOR;
uniform float NORMAL_MAP_SCALE;
uniform vec3 AMBIENT;
#if POINT_LIGHTS_COUNT > 0
    uniform PointLight POINT_LIGHTS[POINT_LIGHTS_COUNT];
#endif
#if SPOT_LIGHTS_COUNT > 0
    uniform SpotLight SPOT_LIGHTS[SPOT_LIGHTS_COUNT];
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
            vec3 result = texture(NORMAL_TEXTURE, get_normal_uv()).rgb;
            result = result * 2.0 - 1.0;
            #ifdef USING_NORMAL_MAP_SCALE
                return NORMAL_MAP_SCALE * result;
            #else
                return result;
            #endif
        #else
            return FRAG_NORMAL;
        #endif
    #else
        return vec3(0.0, 0.0, 0.0);
    #endif
}

vec3 calc_point_light(int light_index) {
    #ifdef USING_NORMAL_TEXTURE
        vec3 light_position = POINT_LIGHT_POSITIONS_TANGENT_SPACE[light_index];
    #else
        vec3 light_position = POINT_LIGHTS[light_index].position;
    #endif

    float distance = length(FRAG_POS - light_position);
    vec3 direction = (FRAG_POS - light_position) / distance;

    float diffuse_factor = max(dot(get_normal(), -direction), 0.0) *
            POINT_LIGHTS[light_index].diffuse_strength;
    vec3 diffuse_color = diffuse_factor * POINT_LIGHTS[light_index].color;

    float attenuation = 1.0 / (
        POINT_LIGHTS[light_index].const_coeff +
        POINT_LIGHTS[light_index].linear_coeff * distance +
        POINT_LIGHTS[light_index].quadratic_coeff * (distance * distance)
    );

    return attenuation * diffuse_color;
}

void main() {
    vec3 lightning_result = AMBIENT;
    
    #if POINT_LIGHTS_COUNT > 0
        for (int i = 0; i < POINT_LIGHTS_COUNT; i++) {
            lightning_result += calc_point_light(i);
        }
    #endif

    COLOR_OUT = vec4(lightning_result, 1.0) * get_base_color();
}
