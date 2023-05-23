R""(
#version 330

in vec3 frag_local_pos;

out vec4 color_out;

uniform samplerCube cubemap;

const float PI = 3.1415926;
const float SAMPLE_STEP = 0.00625;
const float AMOUNT_OF_SAMPLES = ceil((2.0 * PI) / SAMPLE_STEP) * ceil((PI / 2.0) / SAMPLE_STEP);

void main() {
    vec3 normal = normalize(frag_local_pos);
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, normal));
    up = normalize(cross(normal, right));
    mat3 tangent_to_world_space_matrix = mat3(right, up, normal);

    vec3 irradiance = vec3(0.0);

    for (float hor_angle = 0.0; hor_angle < 2.0 * PI; hor_angle += SAMPLE_STEP) {
        for (float vert_angle = 0.0; vert_angle < PI / 2.0; vert_angle += SAMPLE_STEP) {
            vec3 tangent_space_vector = vec3(
                sin(vert_angle) * cos(hor_angle),
                sin(vert_angle) * sin(hor_angle),
                cos(vert_angle)
            );
            vec3 world_space_vector = tangent_to_world_space_matrix * tangent_space_vector;
            // We are multiplying by sin(vert_angle) here because the less the vert_angle is, the smaller areas we take with each
            // sample. So, to compensate this effect, we multiply by sin(vert_angle).
            irradiance += texture(cubemap, world_space_vector).rgb * cos(vert_angle) * sin(vert_angle);
        }
    }

    irradiance = PI * irradiance / AMOUNT_OF_SAMPLES;
    color_out = vec4(irradiance, 1.0);
}
)""