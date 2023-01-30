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

out vec4 color_out;

#ifdef USING_BASE_UV_TRANSFORM
    in vec2 frag_base_uv;
#endif
#ifdef USING_NORMAL_UV_TRANSFORM
    in vec2 frag_normal_uv;
#endif
#ifdef USING_UV
    in vec2 frag_uv;
#endif
#ifdef USING_VERTEX_NORMALS
    in vec3 frag_normal;
#endif
#ifdef USING_FRAGMENT_POSITION
    // May be in tangent space if USING_NORMAL_TEXTURE is enabled.
    in vec3 frag_pos;
#endif
#ifdef USING_NORMAL_TEXTURE
    #if POINT_LIGHTS_COUNT > 0
        in vec3 point_light_positions_tangent_space[POINT_LIGHTS_COUNT];
    #endif
    #if SPOT_LIGHTS_COUNT > 0
        in vec3 spot_light_positions_tangent_space[SPOT_LIGHTS_COUNT];
        in vec3 spot_light_directions_tangent_space[SPOT_LIGHTS_COUNT];
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
uniform sampler2D base_color_texture;
uniform sampler2D normal_texture;
uniform vec4 base_color_factor;
uniform float normal_map_scale;
uniform vec3 ambient;
#if POINT_LIGHTS_COUNT > 0
    uniform PointLight point_lights[POINT_LIGHTS_COUNT];
#endif
#if SPOT_LIGHTS_COUNT > 0
    uniform SpotLight spot_lights[SPOT_LIGHTS_COUNT];
#endif

vec2 get_base_uv() {
    #ifdef USING_UV
        #ifdef USING_BASE_UV_TRANSFORM
            return frag_base_uv;
        #else
            return frag_uv;
        #endif
    #else
        return vec2(0.0, 0.0);
    #endif
}

vec2 get_normal_uv() {
    #ifdef USING_UV
        #ifdef USING_NORMAL_UV_TRANSFORM
            return frag_normal_uv;
        #else
            return frag_uv;
        #endif
    #else
        return vec2(0.0, 0.0);
    #endif
}

vec4 get_base_color() {
    #ifdef USING_BASE_COLOR_TEXTURE
        #ifdef USING_BASE_COLOR_FACTOR
            return texture(base_color_texture, get_base_uv()) * base_color_factor;
        #else
            return texture(base_color_texture, get_base_uv());
        #endif
    #else
        #ifdef USING_BASE_COLOR_FACTOR
            return base_color_factor;
        #else
            return vec4(0.0, 0.0, 0.0, 1.0);
        #endif
    #endif
}

vec3 get_normal() {
    #ifdef USING_VERTEX_NORMALS
        #ifdef USING_NORMAL_TEXTURE
            vec3 result = texture(normal_texture, get_normal_uv()).rgb;
            result = result * 2.0 - 1.0;
            #ifdef USING_NORMAL_MAP_SCALE
                return normal_map_scale * result;
            #else
                return result;
            #endif
        #else
            return frag_normal;
        #endif
    #else
        return vec3(0.0, 0.0, 0.0);
    #endif
}

#if POINT_LIGHTS_COUNT > 0
vec3 calc_point_light(int light_index) {
    #ifdef USING_NORMAL_TEXTURE
        vec3 light_position = point_light_positions_tangent_space[light_index];
    #else
        vec3 light_position = point_lights[light_index].position;
    #endif

    float distance = length(frag_pos - light_position);
    vec3 direction = (frag_pos - light_position) / distance;

    float diffuse_factor = max(dot(get_normal(), -direction), 0.0) *
            point_lights[light_index].diffuse_strength;
    vec3 diffuse_color = diffuse_factor * point_lights[light_index].color;

    float attenuation = 1.0 / (
        point_lights[light_index].const_coeff +
        point_lights[light_index].linear_coeff * distance +
        point_lights[light_index].quadratic_coeff * (distance * distance)
    );

    return attenuation * diffuse_color;
}
#endif

void main() {
    vec3 lightning_result = ambient;
    
    #if POINT_LIGHTS_COUNT > 0
        for (int i = 0; i < POINT_LIGHTS_COUNT; i++) {
            lightning_result += calc_point_light(i);
        }
    #endif

    color_out = vec4(lightning_result, 1.0) * get_base_color();
}
