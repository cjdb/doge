#version 330 core

out vec4 out_colour;

in vec3 colour;
in vec2 out_texture;

uniform sampler2D texture0;
uniform sampler2D texture1;

void main()
{
   out_colour = mix(texture(texture0, out_texture), texture(texture1, out_texture), 0.2);
}
