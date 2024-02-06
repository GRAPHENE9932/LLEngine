R""(
#version 330

in vec2 frag_uv;

out vec4 color_out;

uniform sampler2D source_texture;
uniform sampler1D gauss_weights;
uniform float vert_step; // 1.0 / vertical resolution
uniform float radius;
uniform float coefficient;

void main() {
    vec3 result = vec3(0.0);

    result += texture(source_texture, vec2(frag_uv.x, frag_uv.y)).rgb * texture(gauss_weights, 0.0).r;
    for (float off = vert_step; off <= radius; off += vert_step) {
        float off_in_gauss_texture = off / radius;
        result += texture(source_texture, vec2(frag_uv.x, frag_uv.y + off)).rgb * texture(gauss_weights, off_in_gauss_texture).r;
        result += texture(source_texture, vec2(frag_uv.x, frag_uv.y - off)).rgb * texture(gauss_weights, off_in_gauss_texture).r;
    }

    color_out = vec4(result * coefficient, 1.0);
}
)""