R""(
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
#ifdef USING_IBL
    in vec3 frag_camera_position;
#endif
#ifdef USING_NORMAL_TEXTURE
    in mat3 tbn;
#endif
#ifdef USING_SHADOW_MAP
    in vec4 shadow_map_space_frag_pos;
    in float shadow_map_bias;
#endif

struct PointLight {
    vec3 position;
    vec3 color;
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
uniform sampler2D emissive_texture;
uniform sampler2DShadow shadow_map;
uniform samplerCube irradiance_map;
uniform samplerCube prefiltered_specular_map;
uniform sampler2D brdf_integration_map;
uniform vec3 camera_position;
uniform vec4 base_color_factor;
uniform vec3 emissive_factor;
uniform float normal_map_scale;
uniform float metallic_factor;
uniform float roughness_factor;
uniform float ao_factor;
uniform vec3 ambient;
uniform float pcf_sparsity;
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
            return 1.0;
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

vec3 get_emissive() {
    #ifdef USING_EMISSIVE_TEXTURE
        #ifdef USING_EMISSIVE_FACTOR
            return texture(emissive_texture, get_base_uv()).rgb * emissive_factor;
        #else
            return texture(emissive_texture, get_base_uv()).rgb;
        #endif
    #else
        #ifdef USING_EMISSIVE_FACTOR
            return emissive_factor;
        #else
            return vec3(0.0, 0.0, 0.0);
        #endif
    #endif
}

#ifdef USING_SHADOW_MAP
    float sample_shadow(vec3 projection_clip_space) {
        float real_depth = projection_clip_space.z - shadow_map_bias;
        return texture(shadow_map, vec3(projection_clip_space.xy, real_depth));
    }

    float compute_shadow_coefficient() {
        vec3 projection_clip_space = shadow_map_space_frag_pos.xyz / shadow_map_space_frag_pos.w;
        projection_clip_space = projection_clip_space * 0.5 + 0.5; // Convert from [-1; 1] range to [0; 1].

        float shadow_coeff = 0.0;
        for (int x = 0; x < 3; x++) {
            for (int y = 0; y < 3; y++) {
                vec3 cur = projection_clip_space;
                cur.x += (x - 1) * pcf_sparsity;
                cur.y += (y - 1) * pcf_sparsity;
                shadow_coeff += sample_shadow(cur);
            }
        }

        return shadow_coeff * 0.111111;
    }
#endif

const float PI = 3.1415926;

vec3 fresnel_schlick(float cosine, vec3 refl_ratio_at_zero_inc) {
    return refl_ratio_at_zero_inc + (1.0 - refl_ratio_at_zero_inc) * pow(clamp(1.0 - cosine, 0.0, 1.0), 5.0);
}

vec3 fresnel_schlick_considering_roughness(float cosine, vec3 refl_ratio_at_zero_inc, float roughness) {
    return refl_ratio_at_zero_inc + (max(vec3((1.0 - roughness)), refl_ratio_at_zero_inc) - refl_ratio_at_zero_inc)
    * pow(clamp(1.0 - cosine, 0.0, 1.0), 5.0);
}

float normal_distribution_ggx(float roughness, vec3 normal, vec3 halfway, vec3 view) {
    float alpha = roughness * roughness;
    float alpha_2 = alpha * alpha;
    float v_dot_n = dot(view, normal);
    float v_dot_n_2 = v_dot_n * v_dot_n;

    float numerator = alpha_2 * clamp(dot(halfway, normal), 0.0, 1.0);
    float denominator = v_dot_n_2 * (alpha_2 + ((1.0 - v_dot_n_2) / v_dot_n_2));
    denominator = PI * denominator * denominator;

    return min(numerator / denominator, 200.0);
}

float geometric_shadowing_schlick_ggx(float n_dot_v, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    float denominator = n_dot_v * (1.0 - k) + k;

    return n_dot_v / denominator;
}

float geometric_shadowing_smith(vec3 n, vec3 v, vec3 l, float roughness) {
    float n_dot_v = max(dot(n, v), 0.0);
    float n_dot_l = max(dot(n, l), 0.0);
    float ggx_1  = geometric_shadowing_schlick_ggx(n_dot_l, roughness);
    float ggx_2  = geometric_shadowing_schlick_ggx(n_dot_v, roughness);

    return ggx_1 * ggx_2;
}

// surf_refl is result of the fresnel function.
// ndf is result of the normal distribution function.
// gsf is result of the geometric shadowing function.
// n is normal.
// v is view direction.
// l is light direction.
vec3 cook_torrance_brdf(vec3 surf_refl, float ndf, float gsf, vec3 n, vec3 v, vec3 l) {
    vec3 numerator = ndf * gsf * surf_refl;
    float denominator = 4.0 * max(dot(n, v), 0.0) * max(dot(n, l), 0.0) + 0.00001;
    return numerator / denominator;
}

const float LAST_PREFILTERED_MIPMAP_LEVEL = 8.0;
void main() {
    // Compute surface reflection ratio at zero incedence.
    vec3 refl_ratio_at_zero_inc = vec3(0.04);
    refl_ratio_at_zero_inc = mix(refl_ratio_at_zero_inc, vec3(get_base_color()), get_metallic());

    vec3 view_direction = normalize(camera_position - frag_pos);

    #ifdef USING_SHADOW_MAP
        float shadow_coeff = compute_shadow_coefficient();
    #else
        float shadow_coeff = 1.0;
    #endif

    // Compute ambient.
    vec3 lightning_result;
    #ifdef USING_IBL
    {
        // Diffuse part.
        vec3 reflection_ratio = fresnel_schlick_considering_roughness(
            max(dot(get_normal(), view_direction), 0.0),
            refl_ratio_at_zero_inc,
            get_roughness()
        );
        vec3 refraction_ratio = (vec3(1.0) - reflection_ratio) * (1.0 - get_metallic());
        vec3 ambient_irradiance = texture(irradiance_map, get_normal()).rgb;
        vec3 diffuse = ambient_irradiance * get_base_color().rgb;

        // Specular part.
        vec3 reflection = reflect(-view_direction, get_normal());
        vec3 prefiltered_color = textureLod(prefiltered_specular_map, reflection, get_roughness() * LAST_PREFILTERED_MIPMAP_LEVEL).rgb;
        vec2 env_brdf = texture(brdf_integration_map, vec2(
            get_roughness(),
            max(dot(get_normal(), view_direction), 0.0)
        )).rg;
        vec3 specular = prefiltered_color * (reflection_ratio * env_brdf.x + env_brdf.y);

        // Combine diffuse part, specular part and ambient occlusion.
        // We are not muliplying specular with reflection_ratio because we already
        // did it earlier.
        lightning_result = (refraction_ratio * diffuse * (shadow_coeff * 0.5 + 0.5) + specular * shadow_coeff) * get_ao();
    }
    #else
        lightning_result = ambient * get_ao();
    #endif

    #if POINT_LIGHTS_COUNT > 0
        for (int i = 0; i < POINT_LIGHTS_COUNT; i++) {
            // Calculate vectors that we will need later.
            float dist_to_frag = length(point_lights[i].position - frag_pos);
            vec3 light_direction = (point_lights[i].position - frag_pos) / dist_to_frag;
            vec3 halfway = normalize(view_direction + light_direction);
            float attenuation = 1.0 / (dist_to_frag * dist_to_frag);

            // Compute radiance.
            vec3 radiance = point_lights[i].color * attenuation;

            // Compute surface reflection ratio.
            vec3 reflection_ratio = fresnel_schlick(max(dot(halfway, view_direction), 0.0), refl_ratio_at_zero_inc);
            // Compute results of the normal distribution function and the geometric shadowing function.
            float ndf = normal_distribution_ggx(get_roughness(), get_normal(), halfway, view_direction);
            float gsf = geometric_shadowing_smith(get_normal(), view_direction, light_direction, get_roughness());

            // Compute specular using Cook-Torrance BRDF.
            vec3 specular = cook_torrance_brdf(reflection_ratio, ndf, gsf, get_normal(), view_direction, light_direction);

            vec3 refraction_ratio = (vec3(1.0) - reflection_ratio) * (1.0 - get_metallic());

            // Calculate the outgoing radiance (result contribution).
            float n_dot_l = dot(get_normal(), light_direction);
            lightning_result += max((refraction_ratio * vec3(get_base_color()) / PI + specular) * radiance * n_dot_l, 0.0);
        }
    #endif
    color_out = vec4(lightning_result + get_emissive() * 10.0, 1.0);
}
)""