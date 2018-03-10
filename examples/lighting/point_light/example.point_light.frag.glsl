#version 430 core
out vec4 frag_colour;

in vec3 frag_position;
in vec3 frag_normal;
in vec2 frag_texture_coordinates;

uniform vec3 viewer_position;

struct material_properties {
   sampler2D diffuse;
   sampler2D specular;
   float shininess;
};

uniform material_properties material;

struct attenuation_t {
   float constant;
   float linear;
   float quadratic;
};

struct light_properties {
   vec3 ambience;
   vec3 diffuse;
   vec3 specular;
   vec3 position;

   attenuation_t attenuation;
};

uniform light_properties light;

void main()
{
   // ambient
   const vec3 ambient = light.ambience * texture(material.diffuse, frag_texture_coordinates).rgb;

   // diffuse
   const vec3 norm = normalize(frag_normal);
   const vec3 light_dir = normalize(light.position - frag_position);
   const float diff = max(dot(norm, light_dir), 0.0);
   const vec3 diffuse = light.diffuse * diff * texture(material.diffuse, frag_texture_coordinates).rgb;

   // specular
   const vec3 view_dir = normalize(viewer_position - frag_position);
   const vec3 reflect_dir = reflect(-light_dir, norm);
   const float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
   const vec3 specular = light.specular * spec * texture(material.specular, frag_texture_coordinates).rgb;


   // attenuation
   const float distance = length(light.position - frag_position);
   const float attenuation = 1.0 / (light.attenuation.constant + light.attenuation.linear * distance
      + light.attenuation.quadratic * (distance * distance));

   frag_colour = vec4((ambient + diffuse + specular) * attenuation, 1.0);
}
