#version 430 core
layout (location = 0) in vec3 pos;
out vec4 vertex_colour;

void main()
{
   gl_Position = vec4(pos, 1.0);
   vertex_colour = vec4(0.5, 0.0, 0.0, 1.0);
}
