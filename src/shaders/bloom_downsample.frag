R""(
#version 330 core

// This shader performs downsampling on a texture,
// as taken from Call Of Duty method, presented at ACM Siggraph 2014.
// This particular method was customly designed to eliminate
// "pulsating artifacts and temporal stability issues".

uniform sampler2D original_texture;
uniform vec2 orig_tex_resolution;

in vec2 frag_uv;
out vec3 color_out;

void main() {
    vec2 pixel_size = 1.0 / orig_tex_resolution;
    float x = pixel_size.x; // Aliases to shorten the code below.
    float y = pixel_size.y;

    // Take 13 samples around current texel:
    // a - b - c
    // - j - k -
    // d - e - f
    // - l - m -
    // g - h - i
    // 'e' is the current texel.
    vec3 a = texture(original_texture, vec2(frag_uv.x - 2.0 * x, frag_uv.y + 2.0 * y)).rgb;
    vec3 b = texture(original_texture, vec2(frag_uv.x, frag_uv.y + 2*y)).rgb;
    vec3 c = texture(original_texture, vec2(frag_uv.x + 2.0 * x, frag_uv.y + 2.0 * y)).rgb;

    vec3 d = texture(original_texture, vec2(frag_uv.x - 2.0 * x, frag_uv.y)).rgb;
    vec3 e = texture(original_texture, vec2(frag_uv.x, frag_uv.y)).rgb;
    vec3 f = texture(original_texture, vec2(frag_uv.x + 2.0 * x, frag_uv.y)).rgb;

    vec3 g = texture(original_texture, vec2(frag_uv.x - 2.0 * x, frag_uv.y - 2.0 * y)).rgb;
    vec3 h = texture(original_texture, vec2(frag_uv.x, frag_uv.y - 2.0 * y)).rgb;
    vec3 i = texture(original_texture, vec2(frag_uv.x + 2.0 * x, frag_uv.y - 2.0 * y)).rgb;

    vec3 j = texture(original_texture, vec2(frag_uv.x - x, frag_uv.y + y)).rgb;
    vec3 k = texture(original_texture, vec2(frag_uv.x + x, frag_uv.y + y)).rgb;
    vec3 l = texture(original_texture, vec2(frag_uv.x - x, frag_uv.y - y)).rgb;
    vec3 m = texture(original_texture, vec2(frag_uv.x + x, frag_uv.y - y)).rgb;

    // Apply weighted distribution:
    // 0.5 + 0.125 + 0.125 + 0.125 + 0.125 = 1
    // a,b,d,e * 0.125
    // b,c,e,f * 0.125
    // d,e,g,h * 0.125
    // e,f,h,i * 0.125
    // j,k,l,m * 0.5
    // This shows 5 square areas that are being sampled. But some of them overlap,
    // so to have an energy preserving downsample we need to make some adjustments.
    // The weights are the distributed, so that the sum of j,k,l,m (e.g.)
    // contribute 0.5 to the final color output. The code below is written
    // to effectively yield this sum. We get:
    // 0.125*5 + 0.03125*4 + 0.0625*4 = 1
    color_out = e * 0.125;
    color_out += (a + c + g + i) * 0.03125;
    color_out += (b + d + f + h) * 0.0625;
    color_out += (j + k + l + m) * 0.125;
    color_out = max(color_out, 0.00001);
}
)""