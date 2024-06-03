R""(
#version 330 core

layout(location = 0) in vec3 vertex_pos;
#ifdef USING_UV
    layout(location = 1) in vec2 vertex_uv;
#endif
#ifdef USING_VERTEX_NORMALS
    layout(location = 2) in vec3 vertex_normal;
#endif
#ifdef USING_NORMAL_TEXTURE
    layout(location = 3) in vec4 vertex_tangent;
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
uniform mat4 mvp;
uniform mat4 model_matrix;
uniform mat4 normal_matrix;
uniform vec2 uv_offset;
uniform vec2 uv_scale;
uniform vec2 base_uv_offset;
uniform vec2 base_uv_scale;
uniform vec2 normal_uv_offset;
uniform vec2 normal_uv_scale;
uniform vec2 metallic_uv_offset;
uniform vec2 metallic_uv_scale;
uniform vec2 roughness_uv_offset;
uniform vec2 roughness_uv_scale;
uniform vec2 ao_uv_offset;
uniform vec2 ao_uv_scale;
uniform mat4 shadow_view_proj_matrix;
uniform float shadow_map_bias_at_45_deg;
uniform vec3 shadow_light_direction;
#if POINT_LIGHTS_COUNT > 0
    uniform PointLight point_lights[POINT_LIGHTS_COUNT];
#endif
#if SPOT_LIGHTS_COUNT > 0
    uniform SpotLight spot_lights[SPOT_LIGHTS_COUNT];
#endif

#ifdef USING_BASE_UV_TRANSFORM
    out vec2 frag_base_uv;
#endif
#ifdef USING_NORMAL_UV_TRANSFORM
    out vec2 frag_normal_uv;
#endif
#ifdef USING_METALLIC_UV_TRANSFORM
    out vec2 frag_metallic_uv;
#endif
#ifdef USING_ROUGHNESS_UV_TRANSFORM
    out vec2 frag_roughness_uv;
#endif
#ifdef USING_AO_UV_TRANSFORM
    out vec2 frag_ao_uv;
#endif
#ifdef USING_UV
    out vec2 frag_uv;
#endif
#ifdef USING_VERTEX_NORMALS
    out vec3 frag_normal;
#endif
#ifdef USING_FRAGMENT_POSITION
    out vec3 frag_pos;
#endif
#ifdef USING_NORMAL_TEXTURE
    out mat3 tbn;
#endif
#ifdef USING_SHADOW_MAP
    out vec4 shadow_map_space_frag_pos;
    out float shadow_map_bias;
#endif

const float COS_45_DEG = 0.7071067812;

void main() {
    gl_Position = mvp * vec4(vertex_pos, 1.0);

    #ifdef USING_VERTEX_NORMALS
        vec3 normal = normalize((normal_matrix * vec4(vertex_normal, 0.0)).xyz);
        frag_normal = normal;

        #ifdef USING_NORMAL_TEXTURE
            vec3 tangent = (normal_matrix * vec4(vertex_tangent.xyz, 0.0)).xyz;
            vec3 bitangent = cross(normal, tangent) * vertex_tangent.w;
            tbn = mat3(tangent, bitangent, normal);
        #endif
    #endif

    #ifdef USING_FRAGMENT_POSITION
        frag_pos = (model_matrix * vec4(vertex_pos, 1.0)).xyz;
    #endif

    #ifdef USING_UV
        #ifdef USING_GENERAL_UV_TRANSFORM
            frag_uv = vertex_uv * uv_scale + uv_offset;
        #else
            frag_uv = vertex_uv;
        #endif

        #ifdef USING_BASE_UV_TRANSFORM
            frag_base_uv = vertex_uv * base_uv_scale + base_uv_offset;
        #endif

        #ifdef USING_NORMAL_UV_TRANSFORM
            frag_normal_uv = vertex_uv * normal_uv_scale + normal_uv_offset;
        #endif

        #ifdef USING_METALLIC_UV_TRANSFORM
            frag_metallic_uv = vertex_uv * metallic_uv_scale + metallic_uv_offset;
        #endif

        #ifdef USING_ROUGHNESS_UV_TRANSFORM
            frag_roughness_uv = vertex_uv * roughness_uv_scale + roughness_uv_offset;
        #endif

        #ifdef USING_AO_UV_TRANSFORM
            frag_ao_uv = vertex_uv * ao_uv_scale + ao_uv_offset;
        #endif
    #endif

    #ifdef USING_SHADOW_MAP
        shadow_map_space_frag_pos = shadow_view_proj_matrix * vec4(frag_pos, 1.0);
        shadow_map_bias = COS_45_DEG / dot(normal, -shadow_light_direction) * shadow_map_bias_at_45_deg;
    #endif
}
)""