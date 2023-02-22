#version 330 core

in vec3 frag_local_pos;

uniform sampler2D equirectangular_map;

out vec4 color_out;

const vec2 angles_to_uv_coeff = vec2(0.1591549, 0.3183099); // (1/2PI, 1/PI)
const vec2 angles_to_uv_offset = vec2(0.5, 0.0);
// Returns UV on the equirectangular map that corresponds
// to the specified point on the unit sphere.
vec2 direction_to_equirectangular_uv(vec3 point_on_sphere) {
    vec2 angles = vec2(atan(point_on_sphere.z, point_on_sphere.x), acos(point_on_sphere.y));
    return angles * angles_to_uv_coeff + angles_to_uv_offset;
}

void main() {
    vec2 uv = direction_to_equirectangular_uv(normalize(frag_local_pos));
    color_out = texture(equirectangular_map, uv);
}
