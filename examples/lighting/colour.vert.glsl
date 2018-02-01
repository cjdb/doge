#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 vert_normal;
layout (location = 2) in vec2 texture_coordinates;

out vec3 frag_position;
out vec3 frag_normal;
out vec2 frag_texture_coordinates;

uniform vec3 light_position;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normal_model;

void main()
{
   gl_Position = projection * view * model * vec4(position, 1.0);
   frag_position = vec3(model * vec4(position, 1.0));
   frag_normal = mat3(normal_model) * vert_normal;
   frag_texture_coordinates = texture_coordinates;
}