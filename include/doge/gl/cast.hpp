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
#ifndef DOGE_GL_CAST_HPP
#define DOGE_GL_CAST_HPP

#include <gl/gl_core.hpp>
#include <gsl/gsl>

namespace doge {
   template <std::experimental::ranges::SignedIntegral I>
   requires
      sizeof(I) == 1
   constexpr [[nodiscard]] auto gl_cast(const I i) noexcept
   {
      return gsl::narrow_cast<GLbyte>(i);
   }

   template <std::experimental::ranges::UnsignedIntegral I>
   requires
      sizeof(I) == 1
   constexpr [[nodiscard]] auto gl_cast(const I i) noexcept
   {
      return gsl::narrow_cast<GLubyte>(i);
   }

   template <std::experimental::ranges::SignedIntegral I>
   requires
      sizeof(I) == 2
   constexpr [[nodiscard]] auto gl_cast(const I i) noexcept
   {
      return gsl::narrow_cast<GLshort>(i);
   }

   template <std::experimental::ranges::UnsignedIntegral I>
   requires
      sizeof(I) == 2
   constexpr [[nodiscard]] auto gl_cast(const I i) noexcept
   {
      return gsl::narrow_cast<GLushort>(i);
   }

   template <std::experimental::ranges::SignedIntegral I>
   requires
      sizeof(I) == 4
   constexpr [[nodiscard]] auto gl_cast(const I i) noexcept
   {
      return gsl::narrow_cast<GLint>(i);
   }

   template <std::experimental::ranges::UnsignedIntegral I>
   requires
      sizeof(I) == 4
   constexpr [[nodiscard]] auto gl_cast(const I i) noexcept
   {
      return gsl::narrow_cast<GLuint>(i);
   }

   template <std::experimental::ranges::SignedIntegral I>
   requires
      sizeof(I) == 8
   constexpr [[nodiscard]] auto gl_cast(const I i) noexcept
   {
      return gsl::narrow_cast<GLint64>(i);
   }

   template <std::experimental::ranges::UnsignedIntegral I>
   requires
      sizeof(I) == 8
   constexpr [[nodiscard]] auto gl_cast(const I i) noexcept
   {
      return gsl::narrow_cast<GLuint64>(i);
   }

   constexpr [[nodiscard]] auto gl_cast(const bool b) noexcept
   {
      return gsl::narrow_cast<GLboolean>(b);
   }

   constexpr [[nodiscard]] auto gl_cast(const std::size_t i) noexcept
   {
      return gsl::narrow_cast<GLsizeiptr>(i);
   }

   constexpr [[nodiscard]] auto gl_cast(const std::ptrdiff_t i) noexcept
   {
      return gsl::narrow_cast<GLintptr>(i);
   }

   template <typename F>
   requires
      std::numeric_limits<F>::is_iec559 &&
      sizeof(F) == 2
   constexpr [[nodiscard]] auto gl_cast(const F f) noexcept
   {
      return gsl::narrow_cast<GLhalf>(f);
   }

   template <typename F>
   requires
      std::numeric_limits<F>::is_iec559 &&
      sizeof(F) == 4
   constexpr [[nodiscard]] auto gl_cast(const F f) noexcept
   {
      return gsl::narrow_cast<GLfloat>(f);
   }

   template <typename F>
   requires
      std::numeric_limits<F>::is_iec559 &&
      sizeof(F) == 8
   constexpr [[nodiscard]] auto gl_cast(const F f) noexcept
   {
      return gsl::narrow_cast<GLdouble>(f);
   }

   template <typename F>
   requires
      std::numeric_limits<F>::is_iec559 &&
      sizeof(F) > 8
   constexpr [[nodiscard]] auto gl_cast(const F f) noexcept
   {
      return gsl::narrow_cast<GLdouble>(f);
   }

   constexpr [[nodiscard]] auto gl_cast(const GLclampf f) noexcept
   {
      return f;
   }

   constexpr [[nodiscard]] auto gl_cast(const GLclampd d) noexcept
   {
      return d;
   }
}

#endif // DOGE_GL_CAST_HPP
