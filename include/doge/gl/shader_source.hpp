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
#ifndef DOGE_GL_SHADER_SOURCE_HPP
#define DOGE_GL_SHADER_SOURCE_HPP

#include <doge/utility/file.hpp>
#include <experimental/ranges/concepts>
#include <gl/gl_core.hpp>
#include <stdexcept>
#include <string>

namespace doge {
   class shader_source {
   public:
      enum type {
         vertex = gl::VERTEX_SHADER,
         fragment = gl::FRAGMENT_SHADER,
         geometry = gl::GEOMETRY_SHADER,
         compute = gl::COMPUTE_SHADER
      };

      shader_source(const type t, const std::string& path);

      shader_source(shader_source&&) = default;
      shader_source(const shader_source&) = default;

      shader_source& operator=(shader_source&&) = default;
      shader_source& operator=(const shader_source&) = default;

      ~shader_source() noexcept;

      constexpr operator GLuint() const noexcept
      {
         return index_;
      }
   private:
      GLuint index_;
   };
} // namespace doge

#endif // DOGE_GL_SHADER_SOURCE_HPP
