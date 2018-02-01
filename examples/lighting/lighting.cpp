//
//  Copyright 2017 Christopher Di Bella
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
#include "../static_objects.hpp"
#include "doge/engine.hpp"
#include "doge/entity/camera.hpp"
#include "doge/gl/shader_binary.hpp"
#include "doge/gl/shader_source.hpp"
#include "doge/gl/uniform.hpp"
#include "doge/gl/vertex_array.hpp"
#include "doge/glm/matrix.hpp"
#include "doge/utility/utility.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include <iostream>
#include <experimental/ranges/algorithm>

int main()
{
   namespace ranges = std::experimental::ranges;
   using ranges::Regular;

   auto engine = doge::engine{};
   auto light_source_program = doge::shader_binary{{
      std::make_pair(doge::shader_source::vertex, "light_source.vert.glsl"),
      std::make_pair(doge::shader_source::fragment, "light_source.frag.glsl")}};
   auto cube_program = doge::shader_binary{{
      std::make_pair(doge::shader_source::vertex, "colour.vert.glsl"),
      std::make_pair(doge::shader_source::fragment, "colour.frag.glsl")}};

   auto cube = doge::vertex{gl::ARRAY_BUFFER, gl::STATIC_DRAW, cube_with_normal, 8, {3, 3, 2}};
   auto light_source = doge::vertex{gl::ARRAY_BUFFER, gl::STATIC_DRAW, cube_with_normal, 8, {3}};
   auto diffuse_map = doge::texture2d{"resources/container2.png",
      {doge::texture_wrap_t::repeat, doge::texture_wrap_t::repeat}, doge::minmag_t::linear,
      doge::minmag_t::linear};
   auto specular_map = doge::texture2d{"resources/container2_specular.png",
      {doge::texture_wrap_t::repeat, doge::texture_wrap_t::repeat}, doge::minmag_t::linear,
      doge::minmag_t::linear, 1};
   auto camera = doge::camera{};

   cube_program.use([&cube_program]{
      doge::uniform(cube_program, "material.diffuse", 0);
      doge::uniform(cube_program, "material.specular", 1);
      doge::uniform(cube_program, "material.shininess", 32.0f);

      const auto light_colour = glm::vec3{1.0f, 1.0f, 1.0f};
      doge::uniform(cube_program, "light.ambience", light_colour * 0.2f);
      doge::uniform(cube_program, "light.diffuse", light_colour * 0.5f);
      doge::uniform(cube_program, "light.specular", light_colour);
      doge::uniform(cube_program, "light.attenuation_constant", 1.0f);
      doge::uniform(cube_program, "light.attenuation_linear", 0.09f);
      doge::uniform(cube_program, "light.attenuation_quadratic", 0.032f);
   });

   auto light_position_xz = 0.0f;
   auto light_position_y = 0.0f;
   gl::Enable(gl::DEPTH_TEST);
   engine.play([&]{
      namespace hid = doge::hid;
      hid::on_key_press<hid::keyboard>(GLFW_KEY_ESCAPE, [&engine]{ engine.close(); });

      gl::ClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);

      camera.mouselook(hid::mouse::cursor_delta());

      hid::on_key_down<hid::keyboard>('W', [&camera]{
            camera.move(doge::cardinality::north, doge::camera::free);
         });
      hid::on_key_down<hid::keyboard>('A', [&camera]{
            camera.move(doge::cardinality::west, doge::camera::free);
         });
      hid::on_key_down<hid::keyboard>('S', [&camera]{
            camera.move(doge::cardinality::south, doge::camera::free);
         });
      hid::on_key_down<hid::keyboard>('D', [&camera]{
            camera.move(doge::cardinality::east, doge::camera::free);
         });

      constexpr auto pi = 3.141592f;
      constexpr auto light_speed = 0.01f;
      hid::on_key_down<hid::keyboard>('J', [&light_position_xz, pi]{
         light_position_xz += light_speed;
         if (light_position_xz > pi) {
            light_position_xz = -pi;
         }
      });
      hid::on_key_down<hid::keyboard>('L', [&light_position_xz, pi]{
         light_position_xz -= light_speed;
         if (light_position_xz < -pi) {
            light_position_xz = pi;
         }
      });
      hid::on_key_down<hid::keyboard>('I', [&light_position_y, pi]{
         light_position_y += light_speed;
         if (light_position_y > pi) {
            light_position_y = -pi;
         }
      });
      hid::on_key_down<hid::keyboard>('K', [&light_position_y, pi]{
         light_position_y -= light_speed;
         if (light_position_y < -pi) {
            light_position_y = pi;
         }
      });

      const auto view = camera.view();
      const auto projection = camera.project(engine.screen().aspect_ratio(), 0.1f, 100.0f);
      const auto light_position = glm::vec3{1.2f, 1.0f, 2.0f}
         * glm::vec3{std::cos(light_position_xz), 1.0f, std::sin(light_position_xz)}
         * glm::vec3{1.0f, std::sin(light_position_y), 1.0f};

      cube_program.use([&]{
         doge::uniform(cube_program, "light.position", light_position);
         doge::uniform(cube_program, "projection", false, projection);
         doge::uniform(cube_program, "view", false, view);

         const auto model = glm::mat4{1.0f};
         doge::uniform(cube_program, "model", false, model);
         doge::uniform(cube_program, "view_position", camera.position());
         doge::uniform(cube_program, "normal_model", false,
              model
            | doge::invert
            | doge::transpose);

         diffuse_map.bind(gl::TEXTURE0);
         specular_map.bind(gl::TEXTURE1);
         cube.bind([&]{
            for (const Regular& i : cube_positions) {
               doge::uniform(cube_program, "model", false, 
                 glm::mat4{1.0f}
               | doge::translate(i)
               | doge::rotate(doge::as_radians<float>(glfwGetTime() * -50.0), {0.5f, 1.0f, 0.5f}));
               cube.draw(doge::vertex::triangles, 0, 36);
            }
         });
      });

      light_source_program.use([&]{
         doge::uniform(light_source_program, "projection", false, projection);
         doge::uniform(light_source_program, "view", false, view);
         doge::uniform(light_source_program, "model", false,
              glm::mat4{1.0f}
            | doge::translate(light_position)
            | doge::scale(glm::vec3{0.2f}));

         light_source.bind([&]{
            light_source.draw(doge::vertex::triangles, 0, 36);
         });
      });
   });
}
