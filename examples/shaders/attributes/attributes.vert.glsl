#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 in_colour;

out vec3 out_colour;

void main()
{
   gl_Position = vec4(position, 1.0);
   out_colour = in_colour;
}
