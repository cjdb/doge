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
#include "doge/engine.hpp"
#include "doge/gl/vertex_array.hpp"
#include "doge/gl/shader_binary.hpp"
#include "doge/gl/shader_source.hpp"
#include "doge/gl/texture.hpp"
#include "doge/gl/uniform.hpp"
#include "doge/glm/matrix.hpp"
#include "doge/hid.hpp"
#include "doge/utility/utility.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace ranges = std::experimental::ranges;
using ranges::Regular;

int main()
{
   auto engine = doge::engine{};

   auto program = doge::shader_binary{{
      std::make_pair(doge::shader_source::vertex, "transform.vert.glsl"),
      std::make_pair(doge::shader_source::fragment, "transform.frag.glsl")}};

   auto tex = make_awesomeface(program);

   auto elements = std::vector<GLuint>{0, 1, 3, 1, 2, 3};
   auto vbo = doge::make_vertex_element_buffer(rectangle, elements);

   auto transform = doge::uniform(program, "transform", false, glm::mat4{1.0f});

   engine.play([&]{
      doge::hid::on_key_press<doge::hid::keyboard>(GLFW_KEY_ESCAPE, [&engine]{ engine.close(); });

      gl::ClearColor(0.2f, 0.3f, 0.4f, 1.0f);
      gl::Clear(gl::COLOR_BUFFER_BIT);

      program.use([&]{
         for (auto i = decltype(tex.size()){}; i != tex.size(); ++i) {
            tex[i].bind(gl::TEXTURE0 + i);
         }

         transform = glm::mat4(1.0f)
                   | doge::rotate(doge::as_radians(doge::glfw_time()), {0.0f, 0.0f, 1.0f})
                   | doge::scale({0.5f, 0.5f, 0.5f});
         vbo.draw([]{});
      });
   });
}
