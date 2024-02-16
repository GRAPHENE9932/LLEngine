R""(
#version 330

in vec2 frag_uv;

out vec4 color_out;

uniform sampler2D source_texture;
uniform sampler1D gauss_weights;
uniform float hor_step; // 1.0 / horizontal resolution
uniform float radius;
uniform float coefficient;
uniform int samples_to_take;

void main() {
    vec3 result = vec3(0.0);
    float off = hor_step * 0.5;
    for (int i = 0; i < samples_to_take / 2; i++) {
        float off_in_gauss_texture = off / radius;
        result += texture(source_texture, vec2(frag_uv.x + off, frag_uv.y)).rgb * texture(gauss_weights, off_in_gauss_texture).r;
        result += texture(source_texture, vec2(frag_uv.x - off, frag_uv.y)).rgb * texture(gauss_weights, off_in_gauss_texture).r;
        off += hor_step * 2.0;
    }

    color_out = vec4(result * coefficient, 1.0);
}
)""