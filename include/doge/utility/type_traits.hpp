//
// Copyright 2017 Christopher Di Bella
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
#ifndef DOGE_UTILITY_TYPE_TRAITS
#define DOGE_UTILITY_TYPE_TRAITS

#include <experimental/ranges/concepts>
#include <gl/gl_core.hpp>
#include <glm/fwd.hpp>

namespace doge {
   namespace ranges = std::experimental::ranges;

   template <typename T, typename... Args>
   constexpr auto is_one_of_v = (std::is_same_v<T, Args> || ...);

   template <typename T>
   using remove_cv_ref_t = std::remove_cv_t<std::remove_reference_t<T>>;

   template <typename T>
   using remove_cv_pointer_t = std::remove_cv_t<std::remove_pointer_t<T>>;

   namespace detail {
      template <GLenum Kind>
      constexpr bool is_texture_type_v = Kind == gl::TEXTURE_1D || Kind == gl::TEXTURE_2D ||
         Kind == gl::TEXTURE_3D;

      template <glm::length_t I, typename T, glm::qualifier Q>
      constexpr bool is_glm_vec(const glm::vec<I, T, Q>&) noexcept
      {
         return true;
      }

      template <typename T>
      constexpr bool is_glm_vec(const T&) noexcept
      {
         return false;
      }

      template <typename Expected, glm::length_t I, typename T, glm::qualifier Q>
      requires
         ranges::Same<T, Expected>
      constexpr bool is_glm_vec_type(const glm::vec<I, T, Q>&) noexcept
      {
         return true;
      }

      template <typename T>
      constexpr bool is_glm_vec_type(const T&) noexcept
      {
         return false;
      }

      template <ranges::DefaultConstructible T>
      constexpr bool is_glm_vec_v = doge::detail::is_glm_vec(T{});

      template <ranges::DefaultConstructible T, typename U>
      constexpr bool is_glm_vec_type_v = doge::detail::is_glm_vec_type<U>(T{});

      template <typename T>
      constexpr auto is_glm_matrix_v = is_one_of_v<T, glm::mat2, glm::mat2x3, glm::mat2x4,
         glm::mat3, glm::mat3x2, glm::mat3x4, glm::mat4, glm::mat4x2, glm::mat4x3>;

      template <typename T, typename U = std::decay_t<T>>
      constexpr auto is_glm_GLfloat_v = std::is_same_v<U, GLfloat> || is_glm_vec_type_v<U, GLfloat>
         || is_glm_matrix_v<U>;

      template <typename T, typename U = std::decay_t<T>>
      constexpr auto is_glm_GLint_v = std::is_same_v<U, GLint> || is_glm_vec_type_v<U, GLint>;

      template <typename T, typename U = std::decay_t<T>>
      constexpr auto is_glm_GLuint_v = std::is_same_v<U, GLuint> || is_glm_vec_type_v<U, GLuint>;
   } // namespace detail
} // namespace doge

#endif // DOGE_UTILITY_TYPE_TRAITS
