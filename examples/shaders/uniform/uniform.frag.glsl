#version 430 core
out vec4 frag_colour;

uniform vec4 out_colour;

void main()
{
   frag_colour = out_colour;
}
