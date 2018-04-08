#version 430 core
out vec4 frag_colour;

struct material_properties {
   sampler2D diffuse;
   sampler2D specular;
   float shininess;
};

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
   vec3 direction;
   float inner_cutoff;
   float outer_cutoff;

   attenuation_t attenuation;
};

in vec3 frag_position;
in vec3 frag_normal;
in vec2 frag_texture_coordinates;

uniform material_properties material;
uniform light_properties light;
uniform vec3 view_position;

void main()
{
   const vec3 light_dir = normalize(light.position - frag_position);

   // radius
   const float theta = dot(light_dir, normalize(-light.direction));
   const float epsilon = light.inner_cutoff - light.outer_cutoff;
   const float intensity = clamp((theta - light.outer_cutoff) / epsilon, 0.0, 1.0);
   // ambient
   const vec3 ambient = light.ambience * texture(material.diffuse, frag_texture_coordinates).rgb;

   // diffuse
   const vec3 norm = normalize(frag_normal);
   const float diff = max(dot(norm, light_dir), 0.0);
   vec3 diffuse = light.diffuse * diff * texture(material.diffuse, frag_texture_coordinates).rgb;

   // specular
   const vec3 view_dir = normalize(view_position - frag_position);
   const vec3 reflect_dir = reflect(-light_dir, norm);
   const float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
   vec3 specular = light.specular * spec * texture(material.specular, frag_texture_coordinates).rgb;

   // attenuation
   const float distance = length(light.position - frag_position);
   const float attenuation = 1.0 / (light.attenuation.constant + light.attenuation.linear * distance
      + light.attenuation.quadratic * (distance * distance));

   diffuse *= attenuation * intensity;
   specular *= attenuation * intensity;
   frag_colour = vec4(ambient + diffuse + specular, 1.0);
}
