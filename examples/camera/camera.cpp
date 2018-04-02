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
#include "doge/gl/vertex_array.hpp"
#include "doge/gl/shader_binary.hpp"
#include "doge/gl/shader_source.hpp"
#include "doge/gl/texture.hpp"
#include "doge/gl/uniform.hpp"
#include "doge/glm/matrix.hpp"
#include "doge/hid.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace ranges = std::experimental::ranges;
using ranges::Regular;

int main()
{
   auto engine = doge::engine{};

   auto program = doge::shader_binary{{
      std::make_pair(doge::shader_source::vertex, "coordinates.example.vert.glsl"),
      std::make_pair(doge::shader_source::fragment, "coordinates.example.frag.glsl")}};

   auto tex = make_awesomeface(program);
   auto vbo = doge::make_vertex_array_buffer(textured_cubes);

   auto camera = doge::camera{};
   auto projection = doge::uniform(program, "projection", false, glm::mat4{});
   auto view = doge::uniform(program, "view", false, glm::mat4{});
   auto model = doge::uniform(program, "model", false, glm::mat4{});

   gl::Enable(gl::DEPTH_TEST);
   doge::hid::mouse::sensitivity(0.4f);
   engine.play([&]{
      namespace hid = doge::hid;
      hid::on_key_press<hid::keyboard>(GLFW_KEY_ESCAPE, [&engine]{ engine.close(); });

      gl::ClearColor(0.2f, 0.3f, 0.4f, 1.0f);
      gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);

      hid::on_key_down<hid::keyboard>('W', [&camera]{
            camera.move(doge::cardinality::north, doge::camera::fps);
         });
      hid::on_key_down<hid::keyboard>('A', [&camera]{
            camera.move(doge::cardinality::west, doge::camera::fps);
         });
      hid::on_key_down<hid::keyboard>('S', [&camera]{
            camera.move(doge::cardinality::south, doge::camera::fps);
         });
      hid::on_key_down<hid::keyboard>('D', [&camera]{
            camera.move(doge::cardinality::east, doge::camera::fps);
         });


      hid::on_key_down<hid::keyboard>('I', [&camera]{
            camera.move(doge::cardinality::north, doge::camera::free);
         });
      hid::on_key_down<hid::keyboard>('J', [&camera]{
            camera.move(doge::cardinality::west, doge::camera::free);
         });
      hid::on_key_down<hid::keyboard>('K', [&camera]{
            camera.move(doge::cardinality::south, doge::camera::free);
         });
      hid::on_key_down<hid::keyboard>('L', [&camera]{
            camera.move(doge::cardinality::east, doge::camera::free);
         });

      using namespace doge::angle_literals;
      hid::on_key_down<hid::keyboard>('U', [&engine, &camera]{ camera.field_of_view(1.0_deg); });
      hid::on_key_down<hid::keyboard>('O', [&engine, &camera]{ camera.field_of_view(-1.0_deg); });


      camera.mouselook(hid::mouse::cursor_delta());

      program.use([&]{
         for (auto i = decltype(tex.size()){}; i != tex.size(); ++i) {
            tex[i].bind(gl::TEXTURE0 + i);
         }

         projection = camera.project(engine.screen().aspect_ratio(), 0.1f, 100.0f);
         view = camera.view();

         for (const Regular& i : cube_positions) {
            model = glm::mat4{1.0f}
                  | doge::translate(i)
                  | doge::rotate(doge::as_radians<float>(glfwGetTime() * -50.0), {0.5f, 1.0f, 0.5f});
            vbo.draw([]{});
         }
      });
   });
}
