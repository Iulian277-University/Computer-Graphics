#version 330

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform vec3 object_color;
uniform vec3 car_position;

// Output value to fragment shader
out vec3 color;

void main()
{
	// Compute world space vectors
	vec3 world_position = (Model * vec4(v_position, 1)).xyz;
	vec3 world_normal	= normalize(mat3(Model) * normalize(v_normal));

	// Pos_{v_y} = Pos_{v_y} - \|{Pos_{car}-Pos_v}\|^2 \cdot scaleFactor
	float scaleFactor = 0.02;
	// float scaleFactor = 0;
	float distance = length(car_position - world_position);
	float height = world_position.y - pow(distance, 2) * scaleFactor;

	// Compute the final position
	vec4 position = vec4(world_position.x, height, world_position.z, 1);
	gl_Position = Projection * View * position;

	// gl_Position = Projection * View * Model * vec4(v_position, 1.0);
	// Compute the color
	color = object_color;
}