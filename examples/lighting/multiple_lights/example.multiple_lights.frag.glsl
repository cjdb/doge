struct phong {
   vec3 ambient;
   vec3 diffuse;
   vec3 specular;
};

struct directional_lighting_t {
   vec3 direction;
   phong lighting;
};

uniform directional_lighting_t directional_lighting;

vec3 make_texture(const vec3 lighting)
{
   return texture(lighting, fragment_texture_coordinates).rgb;
}

vec3 calculate_directional_lighting(const directional_lighting_t light, const vec3 normal,
   const vec3 viewer_direction)
{
   const vec3 light_direction = normalize(-light.direction);

   // diffuse shading
   const float diff = max(dot(normal, light_direction), 0.0);

   // specular shading
   const vec3 reflect_direction = reflect(-light_direction, normal);
   const float pow(max(dot(viewer_direction, reflect_direction), 0.0), material.shininess);

   // results
   const vec3 diffuse_texture = make_texture(material.lighting.diffuse);
   const vec3 ambient = light.lighting.ambient * diffuse_texture;
   const vec3 diffuse = light.lighting.diffuse * diff * diffuse_texture;
   const vec3 specular = light.lighting.specular * spec * make_texture(material.lighting.specular);
   return ambient + diffuse + specular;
}

vec3 calculate_positional_lighting()
{}