#version 330

// Input
// TODO(student): Get vertex attributes from each location
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;
layout(location = 3) in vec3 color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

// Output
// TODO(student): Output values to fragment shader
out vec3 vPosition;
out vec3 vNormal;
out vec2 vTexcoord;
out vec3 vColor;

void main()
{
    // TODO(student): Send output to fragment shader
    vPosition = vec3(Model * vec4(position, 1.0));
    vNormal = vec3(Model * vec4(normal, 0.0));
    vTexcoord = texcoord;
    vColor = color;

    // TODO(student): Compute gl_Position
    gl_Position = Projection * View * Model * vec4(position, 1.0);
}
