#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 in_colour;

uniform float offset;

out vec3 out_colour;
out vec3 out_position;

void main()
{
   gl_Position = vec4(position.x + offset, -position.y, position.z, 1.0);
   out_colour = gl_Position.xyz;
}
