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
#include "doge/gl/vertex_array.hpp"
#include "doge/gl/shader_binary.hpp"
#include "doge/gl/shader_source.hpp"
#include "doge/gl/texture.hpp"
#include "doge/hid.hpp"
#include <vector>

namespace ranges = std::experimental::ranges;

int main()
{
   auto engine = doge::engine{};

   auto program = doge::shader_binary{{
      std::make_pair(doge::shader_source::vertex, "example.vert.glsl"),
      std::make_pair(doge::shader_source::fragment, "example.frag.glsl")}};

   auto tex = make_awesomeface(program);

   auto vbo = doge::vertex{gl::ARRAY_BUFFER, gl::STATIC_DRAW, coloured_rectangle,
      {0, 1, 3, 1, 2, 3}, 8, {3, 3, 2}};

   engine.play([&]{
      doge::hid::on_key_press<doge::hid::keyboard>(GLFW_KEY_ESCAPE, [&engine]{ engine.close(); });

      gl::ClearColor(0.2f, 0.3f, 0.4f, 1.0f);
      gl::Clear(gl::COLOR_BUFFER_BIT);

      program.use([&]{
         for (auto i = decltype(tex.size()){}; i != tex.size(); ++i)
            tex[i].bind(gl::TEXTURE0 + i);

         vbo.bind([&vbo]{
            vbo.draw(doge::vertex::triangles);
         });
      });
   });
}
