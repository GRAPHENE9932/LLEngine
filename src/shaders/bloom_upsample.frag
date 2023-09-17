R""(#version 330 core

// This shader performs upsampling on a texture,
// as taken from Call Of Duty method, presented at ACM Siggraph 2014.

uniform sampler2D original_texture;
uniform float filter_radius;

in vec2 frag_uv;
out vec3 color_out;

void main() {
    // The filter kernel is applied with a radius, specified in texture
    // coordinates, so that the radius will vary across mip resolutions.
    float x = filter_radius;
    float y = filter_radius;

    // Take 9 samples around current texel:
    // a - b - c
    // d - e - f
    // g - h - i
    // 'e' is the current texel.
    vec3 a = texture(original_texture, vec2(frag_uv.x - x, frag_uv.y + y)).rgb;
    vec3 b = texture(original_texture, vec2(frag_uv.x,     frag_uv.y + y)).rgb;
    vec3 c = texture(original_texture, vec2(frag_uv.x + x, frag_uv.y + y)).rgb;

    vec3 d = texture(original_texture, vec2(frag_uv.x - x, frag_uv.y)).rgb;
    vec3 e = texture(original_texture, vec2(frag_uv.x,     frag_uv.y)).rgb;
    vec3 f = texture(original_texture, vec2(frag_uv.x + x, frag_uv.y)).rgb;

    vec3 g = texture(original_texture, vec2(frag_uv.x - x, frag_uv.y - y)).rgb;
    vec3 h = texture(original_texture, vec2(frag_uv.x,     frag_uv.y - y)).rgb;
    vec3 i = texture(original_texture, vec2(frag_uv.x + x, frag_uv.y - y)).rgb;

    // Apply weighted distribution, by using a 3x3 tent filter:
    // | 1 2 1 |
    // | 2 4 2 |
    // | 1 2 1 |
    color_out = e * 4.0;
    color_out += (b + d + f + h) * 2.0;
    color_out += (a + c + g + i);
    color_out *= 1.0 / 16.0;
}
)""