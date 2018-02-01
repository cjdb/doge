#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texture;

out vec2 out_texture;

uniform mat4 transform;

void main()
{
   gl_Position = transform * vec4(position, 1.0f);
   out_texture = vec2(texture.x, texture.y);
}
