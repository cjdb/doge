//
//  Copyright 2018 Christopher Di Bella
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
#include "../../static_objects.hpp"
#include "doge/engine.hpp"
#include "doge/entity/camera.hpp"
#include "doge/entity/light_source.hpp"
#include "doge/gl/shader_binary.hpp"
#include "doge/gl/shader_source.hpp"
#include "doge/gl/uniform.hpp"
#include "doge/utility/utility.hpp"
#include "doge/gl/vertex_array.hpp"
#include "doge/glm/matrix.hpp"
#include "doge/utility/utility.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

int main()
{
   auto engine = doge::engine{};
   auto program = doge::shader_binary{{
      std::make_pair(doge::shader_source::vertex, "example.directional_light.vert.glsl"),
      std::make_pair(doge::shader_source::fragment, "example.directional_light.frag.glsl")}};
   auto cube = doge::vertex{gl::ARRAY_BUFFER, gl::STATIC_DRAW, cube_with_normal, 8, {3, 3, 2}};
   auto diffuse_map = doge::texture2d{"resources/container2.png",
      {doge::texture_wrap_t::repeat, doge::texture_wrap_t::repeat}, doge::minmag_t::linear,
      doge::minmag_t::linear};
   auto specular_map = doge::texture2d{"resources/container2_specular.png",
      {doge::texture_wrap_t::repeat, doge::texture_wrap_t::repeat}, doge::minmag_t::linear,
      doge::minmag_t::linear, 1};
   auto camera = doge::camera{};

   program.use([&program, &camera, &engine]{
      doge::uniform(program, "material.diffuse", 0);
      doge::uniform(program, "material.specular", 1);
      doge::uniform(program, "material.shininess", 32.0f);
      doge::uniform(program, "projection", false, camera.project(engine.screen().aspect_ratio(), 0.1f, 100.0f));
   });

   constexpr auto unit3 = doge::vec3{1.0f, 1.0f, 1.0f};
   auto light = doge::directional_light(program, {"light.direction", -doge::vec3{0.2f, 1.0f, 0.3f}},
      {"light.ambient", unit3 * 0.2f}, {"light.diffuse", unit3 * 0.5f},
      {"light.specular", unit3 * 0.5f});

   auto view = doge::uniform(program, "view", false, camera.view());
   auto position = doge::uniform(program, "view_position", camera.position());
   auto model = doge::uniform(program, "model", false, doge::mat4{1.0f});

   gl::Enable(gl::DEPTH_TEST);
   engine.play([&]{
      namespace hid = doge::hid;
      hid::on_key_press<hid::keyboard>(GLFW_KEY_ESCAPE, [&engine]{ engine.close(); });

      camera.mouselook(hid::mouse::cursor_delta());

      hid::on_key_down<hid::keyboard>('W', [&camera]{
         camera.move(doge::cardinality::north, doge::camera::free); });
      hid::on_key_down<hid::keyboard>('A', [&camera]{
         camera.move(doge::cardinality::west, doge::camera::free); });
      hid::on_key_down<hid::keyboard>('S', [&camera]{
         camera.move(doge::cardinality::south, doge::camera::free); });
      hid::on_key_down<hid::keyboard>('D', [&camera]{
         camera.move(doge::cardinality::east, doge::camera::free); });

      hid::on_key_down<hid::keyboard>(GLFW_KEY_UP, [&light]{
         light = -doge::vec3{0.2f, 1.0f, 0.3f}; });
      hid::on_key_down<hid::keyboard>(GLFW_KEY_DOWN, [&light]{
         light = doge::vec3{0.2f, 1.0f, 0.3f}; });
      hid::on_key_down<hid::keyboard>(GLFW_KEY_LEFT, [&light]{
         light = -doge::vec3{1.0f, 0.3f, 0.2f}; });
      hid::on_key_down<hid::keyboard>(GLFW_KEY_RIGHT, [&light]{
         light = doge::vec3{0.3f, 0.2f, 1.0f}; });

      gl::ClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);

      program.use([&]{
         view = camera.view();
         position = camera.position();
         model = doge::mat4{1.0f};

         diffuse_map.bind(gl::TEXTURE0);
         specular_map.bind(gl::TEXTURE1);

         cube.bind([&]{
            for (auto i = doge::zero(cube_positions); i < ranges::size(cube_positions); ++i) {
               model = doge::mat4{1.0f}
                     | doge::translate(cube_positions[i])
                     | doge::rotate(doge::as_radians<float>(20.0f * i), {0.5f, 1.0f, 0.5f});
               cube.draw(doge::vertex::triangles, 0, 36);
            }
         });
      });
   });
}
