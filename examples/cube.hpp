#ifndef DOGE_EXAMPLES_CUBE_HPP
#define DOGE_EXAMPLES_CUBE_HPP

#include "doge/doge.hpp"
#include "static_objects.hpp"
#include <string>

namespace demo {
   class cube {
   public:
      template <class Light>
      cube(std::shared_ptr<std::vector<GLfloat>> const& vertices, std::string const& basic_shader_path,
         std::string const& basic_map_path, Light const& light,
         std::string_view const light_position, std::string_view const ambient,
         std::string_view const diffuse, std::string_view const specular)
         : vertices_{gl::ARRAY_BUFFER, gl::STATIC_DRAW, vertices, 8, {3, 3, 2}},
           program_{doge::make_shader(basic_shader_path)},
           diffuse_map_{doge::make_texture_map<doge::texture_t::texture_2d>(basic_map_path + "_diffuse.png")},
           specular_map_{doge::make_texture_map<doge::texture_t::texture_2d>(basic_map_path + "_specular.png")},
           light_position_{program(), light_position, doge::position(light)},
           ambient_{program(), ambient, light.ambient()},
           diffuse_{program(), diffuse, light.diffuse()},
           specular_{program(), specular, light.specular()}
      {
         program_.use([&]{
            doge::uniform(program(), "material.diffuse", 0);
            doge::uniform(program(), "material.specular", 1);
            doge::uniform(program(), "material.shininess", 32.0f);

            if constexpr (not std::is_same_v<Light, doge::directional_lighting>) {
               doge::uniform(program(), "light.attenuation.constant", light.attenuation_constant());
               doge::uniform(program(), "light.attenuation.linear", light.attenuation_linear());
               doge::uniform(program(), "light.attenuation.quadratic", light.attenuation_quadratic());
            }
            if constexpr (std::is_same_v<Light, doge::spot_lighting>) {
               doge::uniform(program(), "light.direction", light.direction());
               doge::uniform(program(), "light.inner_cutoff", static_cast<GLfloat>(light.inner_cutoff()));
               doge::uniform(program(), "light.outer_cutoff", static_cast<GLfloat>(light.outer_cutoff()));
            }
         });
      }

      template <ranges::Invocable<doge::uniform<doge::mat4>&, doge::uniform<doge::vec3>&,
         doge::uniform<doge::mat4>&, doge::uniform<doge::mat4>&> F, class Light>
      void draw(F const& f, Light const& light)
      {
         program_.use([&, this]{
            doge::draw(light, [this](auto const& position, auto&&...) {
               light_position_ = position; });
            ranges::invoke(f, view_, light_position_, model_, projection_);

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
      doge::uniform<doge::mat4> model_{program(), "model", false, {}};
      doge::uniform<doge::mat4> projection_{program(), "projection", false, {}};
      doge::uniform<doge::vec3> light_position_;
      doge::uniform<doge::vec3> ambient_;
      doge::uniform<doge::vec3> diffuse_;
      doge::uniform<doge::vec3> specular_;
   };
} // namespace demo

#endif // DOGE_EXAMPLES_CUBE_HPP
