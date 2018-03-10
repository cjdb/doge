#ifndef DOGE_EXAMPLES_CUBE_HPP
#define DOGE_EXAMPLES_CUBE_HPP

#include "doge/doge.hpp"
#include "static_objects.hpp"
#include <string>

namespace demo {
   class cube {
   public:
      cube(std::shared_ptr<std::vector<GLfloat>> const& vertices, std::string const& basic_shader_path,
         std::string const& basic_map_path)
         : vertices_{gl::ARRAY_BUFFER, gl::STATIC_DRAW, vertices, 8, {3, 3, 2}},
           program_{doge::make_shader(basic_shader_path)},
           diffuse_map_{doge::make_texture_map<doge::texture_t::texture_2d>(basic_map_path + "_diffuse.png")},
           specular_map_{doge::make_texture_map<doge::texture_t::texture_2d>(basic_map_path + "_specular.png")}
      {
         program_.use([&]{
            doge::uniform(program(), "material.diffuse", 0);
            doge::uniform(program(), "material.specular", 1);
            doge::uniform(program(), "material.shininess", 32.0f);
         });
      }

      template <ranges::Invocable<doge::uniform<doge::mat4>&, doge::uniform<doge::vec3>&,
         doge::uniform<doge::vec3>&, doge::uniform<doge::mat4>&, doge::uniform<doge::mat4>&> F>
      void draw(const F& f)
      {
         program_.use([&, this]{
            ranges::invoke(f, view_, light_position_, viewer_position_, model_, projection_);

            diffuse_map_.bind(gl::TEXTURE0);
            specular_map_.bind(gl::TEXTURE1);

            vertices_.bind([&]{
               for (auto i = doge::zero(cube_positions); i < ranges::size(cube_positions); ++i) {
                  model_ = doge::mat4{1.0f}
                         | doge::translate(cube_positions[i])
                         | doge::rotate(doge::as_radians<float>(20.0f * i), {0.5f, 1.0f, 0.5f});
                  vertices_.draw(doge::vertex::triangles, 0, 36);
               }
            });
         });
      }

      doge::shader_binary const& program() const noexcept
      {
         return program_;
      }
   private:
      doge::vertex vertices_;
      doge::shader_binary program_;
      doge::texture2d diffuse_map_;
      doge::texture2d specular_map_;

      doge::uniform<doge::mat4> view_{program(), "view", false, {}};
      doge::uniform<doge::vec3> light_position_{program(), "light.position", doge::vec3{}};
      doge::uniform<doge::vec3> viewer_position_{program(), "viewer_position", doge::vec3{}};
      doge::uniform<doge::mat4> model_{program(), "model", false, {}};
      doge::uniform<doge::mat4> projection_{program(), "projection", false, {}};
   };
} // namespace demo

#endif // DOGE_EXAMPLES_CUBE_HPP
