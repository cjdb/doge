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
#ifndef DOGE_GL_SHADER_BINARY_HPP
#define DOGE_GL_SHADER_BINARY_HPP

#include <doge/gl/shader_source.hpp>
#include <experimental/ranges/concepts>
#include <gl/gl_core.hpp>
#include <stdexcept>
#include <string>
#include <vector>

namespace doge {
   class shader_binary {
   public:
      shader_binary(const std::vector<std::pair<shader_source::type, std::string>>& paths)
         : shader_binary{compile_shaders(paths)}
      {}

      shader_binary(const std::vector<shader_source>& shaders)
         : index_{gl::CreateProgram()}
      {
         for (const auto& i : shaders)
            gl::AttachShader(index_, i);
         gl::LinkProgram(index_);

         std::experimental::ranges::SignedIntegral successful = 0;
         if (gl::GetProgramiv(index_, gl::LINK_STATUS, &successful); not successful) {
            std::experimental::ranges::Regular log = std::string(512, '\0');
            gl::GetProgramInfoLog(index_, log.size(), nullptr, log.data());
            throw std::runtime_error{log};
         }
      }

      constexpr operator GLuint() const noexcept
      {
         return index_;
      }
   private:
      GLuint index_;

      std::vector<shader_source>
      compile_shaders(const std::vector<std::pair<shader_source::type, std::string>>& paths)
      {
         std::experimental::ranges::Regular shaders = std::vector<shader_source>{};
         shaders.reserve(paths.size());
         for (const auto& i : paths)
            shaders.emplace_back(i.first, i.second);
         return shaders;
      }
   };
} // namespace doge

#endif // DOGE_GL_SHADER_BINARY_HPP
