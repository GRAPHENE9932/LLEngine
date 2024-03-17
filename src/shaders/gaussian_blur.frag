R""(
#version 330

in vec2 frag_uv;

out vec4 color_out;

uniform sampler2D source_texture;
uniform sampler1D gauss_weights;
uniform float pixel_width; // 1.0 / resolution
uniform float radius;
uniform bool is_vertical;

uniform bool combine;
uniform sampler2D texture_to_combine;

void main() {
    vec3 result = vec3(0.0);
    float off = pixel_width * 0.5;
    int samples_to_take = int(radius / pixel_width);
    for (int i = 0; i < samples_to_take; i++) {
        float off_in_gauss_texture = off / radius;
        float gauss_weight = texture(gauss_weights, off_in_gauss_texture).r;

        if (is_vertical) {
            result += texture(source_texture, vec2(frag_uv.x, frag_uv.y + off)).rgb * gauss_weight;
            result += texture(source_texture, vec2(frag_uv.x, frag_uv.y - off)).rgb * gauss_weight;
        }
        else {
            result += texture(source_texture, vec2(frag_uv.x + off, frag_uv.y)).rgb * gauss_weight;
            result += texture(source_texture, vec2(frag_uv.x - off, frag_uv.y)).rgb * gauss_weight;
        }
        
        off += pixel_width * 2.0;
    }

    color_out = vec4(result / samples_to_take * 0.5, 1.0);
    if (combine) {
        color_out.rgb += texture(texture_to_combine, frag_uv).rgb;
    }
}
)""