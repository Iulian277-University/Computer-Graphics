#version 330

// Input
// TODO(student): Get values from vertex shader
in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;
in vec3 vColor;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    // TODO(student): Write pixel out color
    // out_color = vec4(vColor, 1);
    out_color = vec4(vNormal, 1);
}
