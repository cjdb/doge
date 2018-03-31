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

   template <typename T>
   struct underlying_type {
      using type = T;
   };

   template <typename T>
   requires requires { typename ranges::value_type_t<T>; }
   struct underlying_type<T> {
      using type = ranges::value_type_t<T>;
   };


   namespace detail {
      template <GLenum Kind>
      constexpr bool is_texture_type_v = Kind == gl::TEXTURE_1D || Kind == gl::TEXTURE_2D ||
         Kind == gl::TEXTURE_3D;

      template <typename T>
      constexpr bool is_glm_vec = false;

      template <glm::length_t I, typename T, glm::qualifier Q>
      constexpr bool is_glm_vec<glm::vec<I, T, Q>> = true;

      template <typename T>
      struct vec_length {
         static_assert(std::is_same_v<T, T*>, "T is not a glm::vec");
      };

      template <glm::length_t I, typename T, glm::qualifier Q>
      struct vec_length<glm::vec<I, T, Q>> : std::integral_constant<glm::length_t, I> {};

      template <typename T, glm::length_t N>
      constexpr bool is_vecn = false;

      template <typename T, glm::length_t N>
      requires
         is_glm_vec<T>
      constexpr bool is_vecn<T, N> = vec_length<T>() == N;

      template <typename Expected, typename T>
      constexpr bool type_is_glm_vec = false;

      template <typename T> constexpr bool is_vec2 = is_vecn<T, 2>;
      template <typename T> constexpr bool is_vec3 = is_vecn<T, 3>;
      template <typename T> constexpr bool is_vec4 = is_vecn<T, 4>;

      template <typename Expected, glm::length_t I, typename T, glm::qualifier Q>
      requires
         ranges::Same<T, Expected>
      constexpr bool type_is_glm_vec<Expected, glm::vec<I, T, Q>> = true;

      template <typename T>
      constexpr auto is_glm_matrix = is_one_of_v<T, glm::mat2, glm::mat2x3, glm::mat2x4,
         glm::mat3, glm::mat3x2, glm::mat3x4, glm::mat4, glm::mat4x2, glm::mat4x3>;

      template <typename T, typename U = std::decay_t<T>>
      constexpr auto is_glm_GLfloat = std::is_same_v<U, GLfloat> || type_is_glm_vec<GLfloat, U>
         || is_glm_matrix<U>;

      template <typename T, typename U = std::decay_t<T>>
      constexpr auto is_glm_GLint = std::is_same_v<U, GLint> || type_is_glm_vec<GLint, U>;

      template <typename T, typename U = std::decay_t<T>>
      constexpr auto is_glm_GLuint = std::is_same_v<U, GLuint> || type_is_glm_vec<GLuint, U>;
   } // namespace detail
} // namespace doge

#endif // DOGE_UTILITY_TYPE_TRAITS
