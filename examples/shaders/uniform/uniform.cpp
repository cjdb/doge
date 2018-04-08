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
#include "../../static_objects.hpp"
#include <gl/gl_core.hpp>
#include <doge/engine.hpp>
#include <doge/gl/vertex_array.hpp>
#include <doge/gl/shader_binary.hpp>
#include <doge/gl/shader_source.hpp>
#include <doge/gl/uniform.hpp>
#include <doge/utility/screen_data.hpp>
#include <doge/hid.hpp>
#include <experimental/ranges/concepts>
#include <glm/vec3.hpp>
#include <vector>

int main()
{
   auto engine = doge::engine{};
   auto program = doge::shader_binary{{
      std::make_pair(doge::shader_source::vertex, "uniform.vert.glsl"),
      std::make_pair(doge::shader_source::fragment, "uniform.frag.glsl")}};
   auto vbo = doge::make_vertex_array_buffer(triangle[0]);

   engine.play([&]{
      doge::hid::on_key_press<doge::hid::keyboard>(GLFW_KEY_ESCAPE, [&engine]{ engine.close(); });

      gl::ClearColor(0.2f, 0.3f, 0.4f, 1.0f);
      gl::Clear(gl::COLOR_BUFFER_BIT);

      program.use([&program, &vbo]{
         using std::experimental::ranges::Regular;
         Regular time = glfwGetTime();
         Regular green = (std::sin(time) / 2.0f) + 0.5f;
         doge::uniform(program, "out_colour", glm::vec4{0.0f, green, 0.0f, 1.0f});

         vbo.draw([]{});
      });
   });
}
