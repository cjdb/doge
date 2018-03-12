#version 430 core

out vec4 fragment_colour;
in vec3 fragment_position;
in vec3 fragment_normal;
in vec2 fragment_texture_coordinates;

struct material_t {
   sampler2D diffuse;
   sampler2D specular;
   float shininess;
};

struct phong {
   vec3 ambient;
   vec3 diffuse;
   vec3 specular;
};

struct directional_lighting_t {
   vec3 direction;
   phong lighting;
};

uniform vec3 view_position;
uniform material_t material;
uniform directional_lighting_t directional_lighting;

vec3 make_texture(in sampler2D lighting)
{
   return texture(lighting, fragment_texture_coordinates).rgb;
}

float diff(in const vec3 normal, in const vec3 light_direction)
{
   return max(dot(normal, light_direction), 0.0);
}

float spec(in const vec3 viewer_direction, in const vec3 light_direction, in const vec3 normal,
   const float shininess)
{
   const vec3 reflect_direction = reflect(-light_direction, normal);
   return pow(max(dot(viewer_direction, reflect_direction), 0.0), shininess);
}

vec3 calculate_directional_lighting(in const directional_lighting_t light, in const vec3 normal,
   in const vec3 viewer_direction)
{
   const vec3 light_direction = normalize(-light.direction);
   const vec3 diffuse_texture = make_texture(material.diffuse);
   const vec3 ambient = light.lighting.ambient * diffuse_texture;
   const vec3 diffuse = light.lighting.diffuse * diff(normal, light_direction) * diffuse_texture;
   const vec3 specular = light.lighting.specular
      * spec(viewer_direction, light_direction, normal, material.shininess)
      * make_texture(material.specular);
   return ambient + diffuse + specular;
}

struct attenuation_t {
   float constant;
   float linear;
   float quadratic;
};

struct positional_lighting_t {
   vec3 position;
   attenuation_t attenuation;
   phong lighting;
};

const int number_of_lights = 4;
uniform positional_lighting_t positional_lighting[number_of_lights];

vec3 calculate_positional_lighting(in const positional_lighting_t light, in const vec3 normal,
   in const vec3 fragment_position, in const vec3 viewer_direction)
{
   const vec3 light_direction = normalize(light.position - fragment_position);

   // result
   const vec3 ambient = light.lighting.ambient * make_texture(material.diffuse);
   const vec3 diffuse = light.lighting.diffuse * diff(normal, light_direction)
      * make_texture(material.diffuse);
   const vec3 specular = light.lighting.specular * spec(viewer_direction, light_direction, normal, material.shininess)
      * make_texture(material.specular);

   // scalar multipliers
   const float distance = length(light.position - fragment_position);
   const float attenuation = 1.0 / (light.attenuation.constant + light.attenuation.linear * distance
      + light.attenuation.quadratic * (distance * distance));
   return (ambient + diffuse + specular) * attenuation;
}

void main()
{
   const vec3 norm = normalize(fragment_normal);
   const vec3 viewer_direction = normalize(view_position - fragment_position);
   vec3 result = calculate_directional_lighting(directional_lighting, norm, viewer_direction);

   for (int i = 0; i < number_of_lights; ++i) {
      result += calculate_positional_lighting(positional_lighting[i], norm, fragment_position, viewer_direction);
   }

   fragment_colour = vec4(result, 1.0);
}
