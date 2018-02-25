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
#ifndef DOGE_TYPES_HPP
#define DOGE_TYPES_HPP

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat2x2.hpp>
#include <glm/mat2x3.hpp>
#include <glm/mat2x4.hpp>
#include <glm/mat3x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat3x4.hpp>
#include <glm/mat4x2.hpp>
#include <glm/mat4x3.hpp>
#include <glm/mat4x4.hpp>

namespace doge {
   using vec2 = glm::vec2;
   using vec3 = glm::vec3;
   using vec4 = glm::vec4;
   using ivec2 = glm::ivec2;
   using ivec3 = glm::ivec3;
   using ivec4 = glm::ivec4;
   using uvec2 = glm::uvec2;
   using uvec3 = glm::uvec3;
   using uvec4 = glm::uvec4;
   using mat2x2 = glm::mat2x2;
   using mat2x3 = glm::mat2x3;
   using mat2x4 = glm::mat2x4;
   using mat3x2 = glm::mat3x2;
   using mat3x3 = glm::mat3x3;
   using mat3x4 = glm::mat3x4;
   using mat4x2 = glm::mat4x2;
   using mat4x3 = glm::mat4x3;
   using mat4x4 = glm::mat4x4;

   using mat2 = mat2x2;
   using mat3 = mat3x3;
   using mat4 = mat4x4;

   template <class T>
   constexpr auto unit = T{1.0f};
} // namespace doge

#endif // DOGE_TYPES_HPP
