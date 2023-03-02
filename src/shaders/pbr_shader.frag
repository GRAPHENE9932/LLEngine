#version 330 core

out vec4 color_out;

#ifdef USING_BASE_UV_TRANSFORM
    in vec2 frag_base_uv;
#endif
#ifdef USING_NORMAL_UV_TRANSFORM
    in vec2 frag_normal_uv;
#endif
#ifdef USING_METALLIC_UV_TRANSFORM
    in vec2 frag_metallic_uv;
#endif
#ifdef USING_ROUGHNESS_UV_TRANSFORM
    in vec2 frag_roughness_uv;
#endif
#ifdef USING_AO_UV_TRANSFORM
    in vec2 frag_ao_uv;
#endif
#ifdef USING_UV
    in vec2 frag_uv;
#endif
#ifdef USING_VERTEX_NORMALS
    in vec3 frag_normal;
#endif
#ifdef USING_FRAGMENT_POSITION
    in vec3 frag_pos;
#endif
#ifdef USING_ENVIRONMENT_CUBEMAP
    in vec3 frag_camera_position;
#endif
#ifdef USING_NORMAL_TEXTURE
    in mat3 tbn;
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
uniform sampler2D metallic_texture;
uniform sampler2D roughness_texture;
uniform sampler2D ao_texture;
uniform samplerCube environment_cubemap;
uniform vec4 base_color_factor;
uniform float normal_map_scale;
uniform float metallic_factor;
uniform float roughness_factor;
uniform float ao_factor;
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

vec2 get_metallic_uv() {
    #ifdef USING_UV
        #ifdef USING_METALLIC_UV_TRANSFORM
            return frag_metallic_uv;
        #else
            return frag_uv;
        #endif
    #else
        return vec2(0.0, 0.0);
    #endif
}

vec2 get_roughness_uv() {
    #ifdef USING_UV
        #ifdef USING_ROUGHNESS_UV_TRANSFORM
            return frag_roughness_uv;
        #else
            return frag_uv;
        #endif
    #else
        return vec2(0.0, 0.0);
    #endif
}

vec2 get_ao_uv() {
    #ifdef USING_UV
        #ifdef USING_AO_UV_TRANSFORM
            return frag_ao_uv;
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
            result = tbn * result;
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

float get_metallic() {
    #ifdef USING_METALLIC_TEXTURE
        #ifdef USING_METALLIC_FACTOR
            return texture(metallic_texture, get_metallic_uv()).METALLIC_TEXTURE_CHANNEL * metallic_factor;
        #else
            return texture(metallic_texture, get_metallic_uv()).METALLIC_TEXTURE_CHANNEL;
        #endif
    #else
        #ifdef USING_METALLIC_FACTOR
            return metallic_factor;
        #else
            return 0.0;
        #endif
    #endif
}

float get_roughness() {
    #ifdef USING_ROUGHNESS_TEXTURE
        #ifdef USING_ROUGHNESS_FACTOR
            return texture(roughness_texture, get_roughness_uv()).ROUGHNESS_TEXTURE_CHANNEL * roughness_factor;
        #else
            return texture(roughness_texture, get_roughness_uv()).ROUGHNESS_TEXTURE_CHANNEL;
        #endif
    #else
        #ifdef USING_ROUGHNESS_FACTOR
            return roughness_factor;
        #else
            return 0.0;
        #endif
    #endif
}

float get_ao() {
    #ifdef USING_AO_TEXTURE
        #ifdef USING_AO_FACTOR
            return texture(ao_texture, get_ao_uv()).AO_TEXTURE_CHANNEL * ao_factor;
        #else
            return texture(ao_texture, get_ao_uv()).AO_TEXTURE_CHANNEL;
        #endif
    #else
        #ifdef USING_AO_FACTOR
            return ao_factor;
        #else
            return 1.0;
        #endif
    #endif
}

#if POINT_LIGHTS_COUNT > 0
vec3 calc_point_light(int light_index) {
    float distance = length(frag_pos - point_lights[light_index].position);
    vec3 direction = (frag_pos - point_lights[light_index].position) / distance;

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

#ifdef USING_ENVIRONMENT_CUBEMAP
vec3 calc_reflection() {
    vec3 from_camera = normalize(frag_pos - frag_camera_position);
    vec3 reflected = reflect(from_camera, get_normal());
    return texture(environment_cubemap, reflected).rgb;
}
#endif

void main() {
    vec3 lightning_result = ambient;
    
    #if POINT_LIGHTS_COUNT > 0
        for (int i = 0; i < POINT_LIGHTS_COUNT; i++) {
            lightning_result += calc_point_light(i);
        }
    #endif

    #ifdef USING_ENVIRONMENT_CUBEMAP
        color_out = vec4(lightning_result, 1.0) * get_base_color() * vec4(calc_reflection(), 1.0);
    #else
        color_out = vec4(lightning_result, 1.0) * get_base_color();
    #endif
}
