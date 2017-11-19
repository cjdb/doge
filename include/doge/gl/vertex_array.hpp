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
#ifndef DOGE_GL_VERTEX_ARRAY_HPP
#define DOGE_GL_VERTEX_ARRAY_HPP

#include <doge/gl/binder.hpp>
#include <experimental/ranges/concepts>
#include <gl/gl_core.hpp>
#include <optional>

namespace doge {
   template <typename T>
   requires
      std::experimental::ranges::Same<T, float> ||
      std::experimental::ranges::Same<T, double>
   class vertex_array {
   public:
      vertex_array(const GLuint location, const GLint size,
         const std::optional<GLsizei> stride = {}, const GLvoid* const pointer = nullptr) noexcept
         : location_{location},
           size_{size},
           stride_{stride.value_or(sizeof(T) * size)},
           pointer_{pointer}
      {
         gl::GenVertexArrays(1, &index_);
      }

      vertex_array(vertex_array&& v) noexcept
         : location_{v.location},
           size_{v.size_},
           stride_{v.stride_},
           pointer_{v.pointer_}
      {
         v.moved_ = true;
      }

      vertex_array(const vertex_array& v) noexcept
         : vertex_array{v.location_, v.size_, v.stride_, v.pointer_}
      {}

      vertex_array& operator=(vertex_array&& v) noexcept
      {
         std::experimental::ranges::swap(v, *this);
         moved_ = false;
         return *this;
      }

      vertex_array& operator=(const vertex_array& v) noexcept
      {
         auto t = v;
         std::experimental::ranges::swap(t, *this);
         moved_ = false;
         return *this;
      }

      ~vertex_array()
      {
         if (not moved_)
            gl::DeleteVertexArrays(1, &index_);
      }

      void enable() const noexcept
      {
         gl::EnableVertexAttribArray(location());
      }

      constexpr GLuint location() const noexcept
      {
         return location_;
      }

      constexpr GLint size() const noexcept
      {
         return size_;
      }

      constexpr GLenum type() const noexcept
      {
         return type_;
      }

      constexpr GLsizei stride() const noexcept
      {
         return stride_;
      }

      constexpr const GLvoid* pointer() const noexcept
      {
         return pointer_;
      }

      void bind() const
      {
         gl::BindVertexArray(index_);
         //return {[]{ gl::BindVertexArray(0); }};
      }

      void interpret() const noexcept
      {
         gl::VertexAttribPointer(location(), size(), type(), false, stride(), pointer());
      }

      operator GLuint() const noexcept
      {
         return index_;
      }
   private:
      GLuint index_;
      GLuint location_;
      GLint size_;
      GLenum type_ = std::is_same_v<T, float> ? gl::FLOAT : gl::DOUBLE;
      GLsizei stride_;
      const GLvoid* pointer_;
      bool moved_ = false;
   };
} // namespace doge

#endif // DOGE_GL_VERTEX_ARRAY_HPP
