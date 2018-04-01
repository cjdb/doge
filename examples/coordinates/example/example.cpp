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
#include <array>
#include "doge/engine.hpp"
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

void debug_messaging(GLenum const source, GLenum const type, GLuint const id, GLenum const severity,
   GLsizei const length, GLchar const* const message,  void const* const) {
    std::cerr << (type == gl::DEBUG_TYPE_ERROR ? "OpenGL error:" : "OpenGL message:") << '\n'
      << "\tsource: " << std::hex << source << "\n"
         "\tid: " << id << "\n"
         "\ttype: " << type << "\n"
         "\tseverity: " << severity << "\n"
         "\tlength: " << length << "\n"
         "\tmessage: \"" << message << "\"\n"
      << std::dec;
}

int main()
{
   auto engine = doge::engine{};
   gl::Enable(gl::DEBUG_OUTPUT);
   gl::DebugMessageCallback(debug_messaging, nullptr);

   auto program = doge::shader_binary{{
      std::make_pair(doge::shader_source::vertex, "coordinates.example.vert.glsl"),
      std::make_pair(doge::shader_source::fragment, "coordinates.example.frag.glsl")}};

   auto tex = make_awesomeface(program);

   auto vbo = doge::vertex_array_buffer{textured_cubes};

   doge::uniform(program, "projection", false, glm::perspective(glm::radians(45.0f),
      engine.screen().aspect_ratio(), 0.1f, 100.0f));
   doge::uniform(program, "view", false, glm::translate(glm::mat4{1.0f}, {0.0f, 0.0f, -3.0f}));
   auto model = doge::uniform(program, "model", false, glm::mat4{});

   gl::Enable(gl::DEPTH_TEST);
   engine.play([&]{
      doge::hid::on_key_press<doge::hid::keyboard>(GLFW_KEY_ESCAPE, [&engine]{ engine.close(); });

      gl::ClearColor(0.2f, 0.3f, 0.4f, 1.0f);
      gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);

      program.use([&]{
         for (auto i = decltype(tex.size()){}; i != tex.size(); ++i) {
            tex[i].bind(gl::TEXTURE0 + i);
         }

         for (const Regular& i : cube_positions) {
            model = glm::mat4{1.0f}
                  | doge::translate(i)
                  | doge::rotate(doge::as_radians<float>(glfwGetTime() * -50.0), {0.5f, 1.0f, 0.5f});

            vbo.draw([]{});
         }
      });
   });
}
