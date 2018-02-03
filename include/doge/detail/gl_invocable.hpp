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
#ifndef DOGE_DETAIL_GL_INVOCABLE_HPP
#define DOGE_DETAIL_GL_INVOCABLE_HPP

#include "doge/utility/type_traits.hpp"
#include "gl/gl_core.hpp"

namespace doge::detail {
   template <size_t N>
   struct size_constant {
      static constexpr size_t size = N;
   };

   template <typename T>
   struct type_t {
      using type = T;
   };

   template <typename T>
   struct gl_traits;
   template<> struct gl_traits<GLfloat> : type_t<GLfloat>, size_constant<1> {};
   template<> struct gl_traits<glm::vec2> : type_t<GLfloat>, size_constant<2> {};
   template<> struct gl_traits<glm::vec3> : type_t<GLfloat>, size_constant<3> {};
   template<> struct gl_traits<glm::vec4> : type_t<GLfloat>, size_constant<4> {};
   template<> struct gl_traits<GLint> : type_t<GLint>, size_constant<1> {};
   template<> struct gl_traits<glm::ivec2> : type_t<GLint>, size_constant<2> {};
   template<> struct gl_traits<glm::ivec3> : type_t<GLint>, size_constant<3> {};
   template<> struct gl_traits<glm::ivec4> : type_t<GLint>, size_constant<4> {};
   template<> struct gl_traits<GLuint> : type_t<GLuint>, size_constant<1> {};
   template<> struct gl_traits<glm::uvec2> : type_t<GLuint>, size_constant<2> {};
   template<> struct gl_traits<glm::uvec3> : type_t<GLuint>, size_constant<3> {};
   template<> struct gl_traits<glm::uvec4> : type_t<GLuint>, size_constant<4> {};

   template <typename T>
   class gl_invocable {
      using traits = gl_traits<T>;

      template <typename U, std::size_t>
      using dependent_type = U;
      
      template <typename U, std::size_t... Idx>
      static void (* uniform_function_type_helper (std::index_sequence<Idx...>))
         (GLint, dependent_type<U, Idx>...);
   public:
      using uniform_function_type = decltype(
         uniform_function_type_helper<typename traits::type>(std::make_index_sequence<traits::size>{})
      );

      using get_uniform_function_type = void(*)(GLuint, GLint, typename traits::type*);
   };

   // contiguous range types
   namespace ranges = std::experimental::ranges;
   template <ranges::RandomAccessRange T>
   struct gl_traits<T> : gl_traits<ranges::value_type_t<T>> {};

   template <ranges::RandomAccessRange T>
   struct gl_invocable<T> {
      using uniform_function_type = void(*)(GLint, GLsizei, const typename gl_traits<T>::type*);
      using get_uniform_function_type = void(*)(GLuint, GLint, typename gl_traits<T>::type*);
   };

   // matrix types
   template <typename T>
   requires
      is_glm_matrix<T>
   struct gl_traits<T> : type_t<GLfloat> {};

   template <typename T>
   requires
      is_glm_matrix<T>
   struct gl_invocable<T> {
      using uniform_function_type = void(*)(GLint, GLsizei, GLboolean, const GLfloat*);
      using get_uniform_function_type = void(*)(GLuint, GLint, typename gl_traits<T>::type*);
   };

   template <typename T>
   using uniform_function_t = typename gl_invocable<T>::uniform_function_type;

   template <typename T>
   using get_uniform_function_t = typename gl_invocable<T>::get_uniform_function_type;
} // namespace doge::detail

#endif // DOGE_DETAIL_GL_INVOCABLE_HPP
