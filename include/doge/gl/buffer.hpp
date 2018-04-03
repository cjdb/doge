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
#ifndef DOGE_GL_VERTEX_BUFFER_HPP
#define DOGE_GL_VERTEX_BUFFER_HPP

#include "doge/gl/memory.hpp"
#include "doge/utility/std_layout_tuple.hpp"
#include <experimental/ranges/iterator>
#include <gl/gl_core.hpp>
#include <gsl/gsl>

#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Wsign-compare"
#include "unique_resource.h"
#pragma GCC diagnostic pop

namespace doge {
   enum class basic_buffer_type {
      invalid,
      array = gl::ARRAY_BUFFER,
      element_array = gl::ELEMENT_ARRAY_BUFFER,
      uniform = gl::UNIFORM_BUFFER,
   };

   enum class basic_buffer_usage {
      invalid,
      stream_draw = gl::STREAM_DRAW,
      stream_read = gl::STREAM_READ,
      stream_copy = gl::STREAM_COPY,
      static_draw = gl::STATIC_DRAW,
      static_read = gl::STATIC_READ,
      static_copy = gl::STATIC_COPY,
      dynamic_draw = gl::DYNAMIC_DRAW,
      dynamic_read = gl::DYNAMIC_READ,
      dynamic_copy = gl::DYNAMIC_COPY
   };

   template <basic_buffer_type BufferType, basic_buffer_usage BufferUsage>
   class basic_buffer {
   public:
      using create_t = void(*)(GLsizei, GLuint*);
      using deleter_t = void(*)(GLsizei, GLuint const*);

      explicit basic_buffer(create_t create, deleter_t deleter) noexcept
         : buffer_{allocate_gl_resource(create, deleter)}
      {}

      template <typename... Ts>
      void write(gsl::span<std_layout_tuple<Ts...> const> const data) noexcept
      {
         write_impl<(sizeof(Ts) + ...)>(data);
      }

      void write(gsl::span<GLuint const> const elements) noexcept
      requires
         BufferType == basic_buffer_type::element_array
      {
         write_impl<sizeof(GLuint)>(elements);
      }

      GLuint get() const noexcept
      {
         return buffer_.get();
      }

      explicit operator GLuint() const noexcept
      {
         return get();
      }
   private:
      std::experimental::unique_resource<GLuint, std::function<void(GLuint)>> buffer_;

      template <std::size_t Size, typename T>
      void write_impl(gsl::span<T const> const data) noexcept
      {
         gl::BindBuffer(static_cast<GLuint>(BufferType), buffer_.get());
         gl::BufferData(static_cast<GLuint>(BufferType), ranges::size(data) * Size,
            ranges::data(data), static_cast<GLuint>(BufferUsage));
      }
   };

   template <basic_buffer_usage Usage>
   using array_buffer = basic_buffer<basic_buffer_type::array, Usage>;

   template <basic_buffer_usage Usage>
   using element_array_buffer = basic_buffer<basic_buffer_type::element_array, Usage>;

   template <basic_buffer_usage Usage>
   using uniform_buffer = basic_buffer<basic_buffer_type::uniform, Usage>;

   // array_buffer make_array_buffer() noexcept
   // {
   //    return array_buffer{gl::GenBuffers, gl::DeleteBuffers};
   // }

   // element_array_buffer make_element_array_buffer() noexcept
   // {
   //    return element_array_buffer{gl::GenBuffers, gl::DeleteBuffers};
   // }
} // namespace doge

#endif // DOGE_GL_VERTEX_BUFFER_HPP
