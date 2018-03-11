#version 430 core

out vec4 fragment_result;
in vec3 frag_position;
in vec3 frag_normal;
in vec2 frag_texture_coordinates;

struct material_t {
   sampler2D diffuse;
   sampler2D specular;
   float shininess;
};

struct light_t {
   vec3 position;
   vec3 ambient;
   vec3 diffuse;
   vec3 specular;
};

uniform vec3 view_position;
uniform material_t material;
uniform light_t light;

void main()
{
   // ambience
   const vec3 ambient = light.ambient * texture(material.diffuse, frag_texture_coordinates).rgb;

   // diffuse
   const vec3 norm = normalize(frag_normal);
   const vec3 light_direction = normalize(-light.position);
   const float diff = max(dot(norm, light_direction), 0.0);
   const vec3 diffuse = light.diffuse * diff * texture(material.diffuse, frag_texture_coordinates).rgb;

   // specular
   const vec3 view_direction = normalize(view_position - frag_position);
   const vec3 reflect_direction = reflect(-light_direction, norm);
   const float spec = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);
   const vec3 specular = light.specular * spec * texture(material.specular, frag_texture_coordinates).rgb;

   fragment_result = vec4(ambient + diffuse + specular, 1.0);
}