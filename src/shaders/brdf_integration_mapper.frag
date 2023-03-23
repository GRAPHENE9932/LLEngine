#version 330

in vec2 frag_uv;

out vec2 data_out;

const float PI = 3.1415926;
const uint AMOUNT_OF_SAMPLES = 128u;
const vec3 NORMAL = vec3(0.0, 0.0, 1.0);

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

float geometric_shadowing_schlick_ggx(float n_dot_v, float roughness) {
    float k = (roughness * roughness) / 2.0;
    float denominator = n_dot_v * (1.0 - k) + k;
	
    return n_dot_v / denominator;
}

float geometric_shadowing_smith(float n_dot_v, float n_dot_l, float roughness) {
    float ggx_1  = geometric_shadowing_schlick_ggx(n_dot_l, roughness);
    float ggx_2  = geometric_shadowing_schlick_ggx(n_dot_v, roughness);
	
    return ggx_1 * ggx_2;
}

// This function is reformatted version of IntegrateBRDF function from
// https://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
vec2 integrate_brdf(float roughness, float n_dot_v) {
    vec2 coeff_and_bias = vec2(0.0, 0.0);

    vec3 view = vec3(
        sqrt(1.0 - n_dot_v * n_dot_v), // Sine.
        0.0,
        n_dot_v // Cosine.
    );

    for (uint i = 0u; i < AMOUNT_OF_SAMPLES; i++) {
        vec3 halfway = get_halfway_sample_vector(i, roughness);
        vec3 light = compute_light_vector(view, halfway);

        float n_dot_l = clamp(light.z, 0.0, 1.0);
        if (n_dot_l > 0.0) {
            float n_dot_h = clamp(halfway.z, 0.0, 1.0);
            float v_dot_h = clamp(dot(view, halfway), 0.0, 1.0);

            // Here we compute the coefficient and bias to F0 (surface reflectance at zero incidence)
            // by formula that's emerged from split sum approximation and substituting in
            // Schlick's Fresnel approximation as written in the paper linked above.

            float gsf = geometric_shadowing_smith(n_dot_v, n_dot_l, roughness);

            // The full BRDF formula is normal_distribution * fresnel * gsf / (4 * n_dot_v * n_dot_l).
            // We are dividing it by PDF (probability density function).
            // According to the paper above, PDF = normal_distribution * n_dot_h / (4 * v_dot_h).
            // We are also multiplying this with n_dot_l. I don't know why we are multiplying it with n_dot_l,
            // there is nothing about it in the paper.
            // So, the formula below means BRDF * n_dot_l / PDF.
            float brdf_mul_n_dot_l_div_pdf = gsf * v_dot_h / (n_dot_h * n_dot_v);

            float fresnel_part = pow(1.0 - v_dot_h, 5);
            coeff_and_bias.x += (1.0 - fresnel_part) * brdf_mul_n_dot_l_div_pdf;
            coeff_and_bias.y += fresnel_part * brdf_mul_n_dot_l_div_pdf;
        }
    }

    coeff_and_bias /= float(AMOUNT_OF_SAMPLES);
    return coeff_and_bias;
}

void main() {
    vec2 integrated_brdf = integrate_brdf(frag_uv.x, frag_uv.y);
    data_out = integrated_brdf;
}