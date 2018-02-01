#version 430 core

out vec4 frag_colour;
in vec3 out_colour;

void main()
{
   frag_colour = vec4(out_colour, 1.0);
}
