#version 330

// Input
in vec3 world_position;
in vec3 world_normal;

// Uniforms for light properties
uniform vec3 light_direction;
uniform vec3 light_position;
uniform vec3 eye_position;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

uniform int light_type;
uniform vec3 object_color;

// Output
layout(location = 0) out vec4 out_color;


void main() {
    vec3 V = normalize(eye_position - world_position);
	vec3 L = normalize(light_position - world_position);
	vec3 R = normalize(reflect(-L, world_normal));

    // Define ambient, diffuse and specular light components
    float ambient_light = 0.25;
    float diffuse_light = material_kd * max(dot(world_normal, L), 0);

    // It's important to distinguish between "reflection model" and
    // "shading method". In this shader, we are experimenting with the Phong
    // (1975) and Blinn-Phong (1977) reflection models, and we are using the
    // Phong (1975) shading method. Don't mix them up!
    float specular_light = 0;
    if (diffuse_light > 0)
        specular_light = material_ks * pow(max(dot(R, V), 0.0), material_shininess);

    // TODO(student): If (and only if) the light is a spotlight, we need to do
    // some additional things.
    float cutoff = radians(30.0);
    float spot_light = dot(-L, normalize(light_direction));
    float attenuation_factor;
    if (light_type == 1) { // spotlight
        if (spot_light > cos(cutoff)) { // inside light cone
            float spot_light_limit = cos(cutoff);
            // Quadratic attenuation
            float linear_att = (spot_light - spot_light_limit) / (1.0 - spot_light_limit); 
            attenuation_factor = pow(linear_att, 2);
        }
    }

    // TODO(student): Compute the total light. You can just add the components
    // together, but if you're feeling extra fancy, you can add individual
    // colors to the light components. To do that, pick some vec3 colors that
    // you like, and multiply them with the respective light components.
    float d = distance(light_position, world_position);
	if (light_type == 0)
        attenuation_factor = 1 / (1 + 0.14 * d + 0.07 * d * d);
	float light = ambient_light + attenuation_factor * (diffuse_light + specular_light);

    // TODO(student): Write pixel out color
    out_color = vec4(object_color * light, 1);
}
