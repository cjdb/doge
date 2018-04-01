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
#include <gl/gl_core.hpp>
#include <doge/engine.hpp>
#include <doge/gl/vertex_array.hpp>
#include <doge/gl/shader_binary.hpp>
#include <doge/gl/shader_source.hpp>
#include <doge/utility/screen_data.hpp>
#include <doge/hid.hpp>
#include <glm/vec3.hpp>
#include "../static_objects.hpp"
#include <vector>

namespace ranges = std::experimental::ranges;

int main()
{
   auto engine = doge::engine{};

   auto shaders = std::vector<doge::shader_binary>{
         {{std::make_pair(doge::shader_source::vertex, "vertex.glsl"), 
           std::make_pair(doge::shader_source::fragment, "fragment.glsl")}},
         {{std::make_pair(doge::shader_source::vertex, "vertex.glsl"),
           std::make_pair(doge::shader_source::fragment, "yellow.glsl")}}
   };

   auto vbo = []{
      auto result = std::vector<doge::vertex_array_buffer<doge::vec3>>{};
      result.emplace_back(triangle[0]);
      result.emplace_back(triangle[1]);
      return result;
   }();

   engine.play([&]{
      doge::hid::on_key_press<doge::hid::keyboard>(GLFW_KEY_ESCAPE, [&engine]{ engine.close(); });

      gl::ClearColor(0.2f, 0.3f, 0.4f, 1.0f);
      gl::Clear(gl::COLOR_BUFFER_BIT);

      using std::experimental::ranges::Integral;
      for (Integral i = decltype(vbo.size()){}; i != vbo.size() && i != shaders.size(); ++i) {
         shaders[i].use([i, &vbo]{
            vbo[i].draw([]{});
         });
      }
   });
}