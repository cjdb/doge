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
#include <experimental/ranges/functional>
#include <gl/gl_core.hpp>
#include <stdexcept>
#include <string>
#include <vector>

namespace doge {
   namespace ranges = std::experimental::ranges;

   class shader_binary {
   public:
      shader_binary(const std::vector<std::pair<shader_source::type, std::string>>& paths)
         : shader_binary{compile_shaders(paths)}
      {}

      shader_binary(const std::vector<shader_source>& shaders);

      template <ranges::Invocable F>
      auto use(const F& f) const noexcept
      {
         gl::UseProgram(index_);
         return ranges::invoke(f);
      }

      explicit operator GLuint() const noexcept
      {
         return index_;
      }
   private:
      GLuint index_;

      std::vector<shader_source>
      compile_shaders(const std::vector<std::pair<shader_source::type, std::string>>& paths);
   };
} // namespace doge

#endif // DOGE_GL_SHADER_BINARY_HPP
