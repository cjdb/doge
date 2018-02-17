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
#include "../static_objects.hpp"
#include "doge/engine.hpp"
#include "doge/entity/camera.hpp"
#include "doge/entity/light_source.hpp"
#include "doge/gl/shader_binary.hpp"
#include "doge/gl/shader_source.hpp"
#include "doge/gl/uniform.hpp"
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
      std::make_pair(doge::shader_source::vertex, "colour.vert.glsl"),
      std::make_pair(doge::shader_source::fragment, "example.directional_light.frag.glsl")}};
   auto cube = doge::vertex{gl::ARRAY_BUFFER, gl::STATIC_DRAW, cube_with_normal, 8, {3, 3, 2}};
   auto diffuse_map = doge::texture2d{"resources/container2.png",
      {doge::texture_wrap_t::repeat, doge::texture_wrap_t::repeat}, doge::minmag_t::linear,
      doge::minmag_t::linear};
   auto specular_map = doge::texture2d{"resources/container2_specular.png",
      {doge::texture_wrap_t::repeat, doge::texture_wrap_t::repeat}, doge::minmag_t::linear,
      doge::minmag_t::linear, 1};
   auto camera = doge::camera{};

   {
      doge::uniform(program, "material.diffuse", 0);
      doge::uniform(program, "material.specular", 1);
      doge::uniform(program, "material.shininess", 32.0f);
      doge::uniform(program, "light.direction", -doge::vec3{0.2f, 1.0f, 0.3f});
      doge::uniform(program, "light.ambient", doge::vec3{0.2f});
      doge::uniform(program, "light.diffuse", -doge::vec3{0.5f});
      doge::uniform(program, "light.specular", -doge::vec3{1.0f});
   }

   auto view = doge::uniform(program, "view", false, camera.view());
   doge::uniform(program, "projection", false, camera.project(engine.screen().aspect_ratio(), 0.1f, 100.0f));
   auto model = doge::uniform(program, "model", false, doge::mat4{1.0f});
   auto position = doge::uniform(program, "view_position", camera.position());

   gl::Enable(gl::DEPTH_TEST);
   engine.play([&]{
      namespace hid = doge::hid;
      hid::on_key_press<hid::keyboard>(GLFW_KEY_ESCAPE, [&engine]{ engine.close(); });

      gl::ClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);

      program.use([&]{
         view = camera.view();
         position = camera.position();
         model = doge::mat4{1.0f};

         diffuse_map.bind(gl::TEXTURE0);
         specular_map.bind(gl::TEXTURE1);

         cube.bind([&]{
            for (const auto& i : cube_positions) {
               model = doge::mat4{1.0f}
                     | doge::translate(i)
                     | doge::rotate(doge::as_radians<float>(glfwGetTime() * -50.0f), {0.5f, 1.0f, 0.5f});
               cube.draw(doge::vertex::triangles, 0, 36);
            }
         });
      });
   });
}
