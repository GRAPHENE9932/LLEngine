R""(#version 330

in vec3 frag_local_pos;

out vec4 color_out;

uniform float roughness;
uniform samplerCube cubemap;

const float PI = 3.1415926;
const uint AMOUNT_OF_SAMPLES = 128u;

// Efficient Van Der Corput sequence implementation taken from
// https://learnopengl.com/PBR/IBL/Specular-IBL.
float van_der_corput_sequence(uint index) {
    index = (index << 16u) | (index >> 16u);
    index = ((index & 0x55555555u) << 1u) | ((index & 0xAAAAAAAAu) >> 1u);
    index = ((index & 0x33333333u) << 2u) | ((index & 0xCCCCCCCCu) >> 2u);
    index = ((index & 0x0F0F0F0Fu) << 4u) | ((index & 0xF0F0F0F0u) >> 4u);
    index = ((index & 0x00FF00FFu) << 8u) | ((index & 0xFF00FF00u) >> 8u);
    return float(index) * 2.3283064365386963e-10; // 0x100000000
}

vec2 hammersley_point_set(uint index) {
    return vec2(
        float(index) / float(AMOUNT_OF_SAMPLES),
        float(van_der_corput_sequence(index))
    );
}

// Cosine of the GGX distribution function is easier to compute than
// just GGX distribution function. So, implement this as such.
float cosine_of_ggx_inverse(float random_uniform, float roughness) {
    float alpha = roughness * roughness;

    float numerator = 1.0 - random_uniform;
    float denominator = random_uniform * (alpha * alpha - 1.0) + 1.0;
    return sqrt(numerator / denominator);
}

vec3 compute_light_vector(vec3 view_vector, vec3 halfway_vector) {
    return normalize(2.0 * dot(view_vector, halfway_vector) * halfway_vector - view_vector);
}

// Returns halfway vector in tangent space.
vec3 get_halfway_sample_vector(uint index, float roughness) {
    vec2 hammersley_point = hammersley_point_set(index);

    // Compute angles.
    float hor_angle = 2.0 * PI * hammersley_point.x;
    float cos_vert_angle = cosine_of_ggx_inverse(hammersley_point.y, roughness);
    float sin_vert_angle = sqrt(1.0 - cos_vert_angle * cos_vert_angle);

    // Convert to cartesian coordinates.
    vec3 result = vec3(
        cos(hor_angle) * sin_vert_angle,
        sin(hor_angle) * sin_vert_angle,
        cos_vert_angle
    );
    return result;
}

vec3 get_sample_vector(uint index, float roughness, vec3 normal, mat3 tangent_to_world_space_matrix) {
    vec3 halfway_tangent_space = get_halfway_sample_vector(index, roughness);
    vec3 halfway = tangent_to_world_space_matrix * halfway_tangent_space;
    vec3 result = compute_light_vector(normal, halfway);
    return result;
}

void main() {
    vec3 normal = normalize(frag_local_pos);
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, normal));
    up = normalize(cross(normal, right));
    mat3 tangent_to_world_space_matrix = mat3(right, up, normal);

    vec3 prefiltered_color = vec3(0.0);
    float total_weight = 0.0;

    for (uint i = 0u; i < AMOUNT_OF_SAMPLES; i++) {
        vec3 sample_vector = get_sample_vector(i, roughness, normal, tangent_to_world_space_matrix);
        float normal_dot_sample = max(dot(normal, sample_vector), 0.0);
        prefiltered_color += texture(cubemap, sample_vector).rgb * normal_dot_sample;
        total_weight += normal_dot_sample;
    }

    prefiltered_color /= total_weight;

    color_out = vec4(prefiltered_color, 1.0);
}
)""