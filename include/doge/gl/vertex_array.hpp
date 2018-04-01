#ifndef DOGE_GL_VERTEX_HPP
#define DOGE_GL_VERTEX_HPP

#include "doge/meta/partial_sum.hpp"
#include "doge/utility/std_layout_tuple.hpp"
#include "doge/utility/type_traits.hpp"

#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Wsign-compare"
#include "unique_resource.h"
#pragma GCC diagnostic pop

#include <iostream>
#include <iomanip>
#include "doge/gl/gl_error.hpp"

namespace doge {
   auto allocate_gl_resource(void(* allocator)(GLsizei, GLuint*), 
                             void(* deleter)  (GLsizei, GLuint const*)) noexcept
   {
      ranges::UnsignedIntegral result = GLuint{};
      ranges::invoke(allocator, 1, &result);
      return std::experimental::unique_resource<GLuint, std::function<void(GLuint)>>(result,
         [deleter]([[maybe_unused]] auto const i) mutable noexcept { 
            //ranges::invoke(deleter, GLsizei{1}, &i);
         });
   }

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

      basic_buffer(basic_buffer const&) = delete;
      basic_buffer& operator=(basic_buffer const&) = delete;

      template <typename... Ts>
      void write(gsl::span<std_layout_tuple<Ts...>> const data) noexcept
      {
         write_impl<(sizeof(Ts) + ...)>(data);
      }

      // void write(gsl::span<GLint> const elements) noexcept
      // requires
      //    BufferType == basic_buffer_type::element_array
      // {
      //    write_impl<0>(elements);
      // }

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
      void write_impl(gsl::span<T> const data) noexcept
      {
         gl::BindBuffer(static_cast<GLuint>(BufferType), buffer_.get());
         gl::BufferData(static_cast<GLuint>(BufferType), ranges::size(data) * Size,
            ranges::data(data), static_cast<GLuint>(BufferUsage));
      }
   };

   using array_buffer = basic_buffer<basic_buffer_type::array, basic_buffer_usage::static_draw>;
   using element_array_buffer = basic_buffer<basic_buffer_type::element_array,
      basic_buffer_usage::static_draw>;
   using uniform_buffer = basic_buffer<basic_buffer_type::uniform, basic_buffer_usage::static_draw>;

   array_buffer make_array_buffer() noexcept
   {
      return array_buffer{gl::GenBuffers, gl::DeleteBuffers};
   }

   element_array_buffer make_element_array_buffer() noexcept
   {
      return element_array_buffer{gl::GenBuffers, gl::DeleteBuffers};
   }

   template <basic_buffer_type>
   struct vertex_array_data;

   template <>
   struct vertex_array_data<basic_buffer_type::array> {
      GLsizei count_ = 0;
      array_buffer vbo_ = array_buffer{gl::GenBuffers, gl::DeleteBuffers};
      std::experimental::unique_resource<GLuint, std::function<void(GLuint)>> vao_ =
         ::doge::allocate_gl_resource(gl::GenVertexArrays, gl::DeleteVertexArrays);
   };

   // template <>
   // struct vertex_array_data<basic_buffer_type::element_array>
   //    : vertex_array_data<basic_buffer_type::array> {
   //    element_array_buffer ebo_ = ::doge::make_element_array_buffer();
   // };

   template <basic_buffer_type BufferType, typename... Ts>
   requires
      (StandardLayout<Ts> && ...) &&
      ((sizeof(Ts) / sizeof(underlying_type_t<Ts>) <= 4) && ...)
   class vertex_array {
   public:
      vertex_array(gsl::span<std_layout_tuple<Ts...>> const data) noexcept
      requires
         BufferType == basic_buffer_type::array
      {
         write(data);
      }

      // vertex_array(gsl::span<std_layout_tuple<Ts...>> const data,
      //    gsl::span<GLint> const elements) noexcept
      // requires
      //    BufferType == basic_buffer_type::element_array
      // {
      //    Expects(ranges::size(data) == ranges::size(elements));
      //    write(data, elements);
      // }

      void write(gsl::span<std_layout_tuple<Ts...>> const data) noexcept
      requires
         BufferType == basic_buffer_type::array
      {
         {
            array_data_.vbo_.write(data);
            vertex_attrib_pointer(std::index_sequence_for<Ts...>{},
               meta::partial_sum_t<sizeof(Ts)...>{},
               meta::partial_sum_t<(sizeof(Ts) / sizeof(underlying_type_t<Ts>))...>{});
         }
         gl::BindVertexArray(0);
      }

      // void write(gsl::span<std_layout_tuple<Ts...>> const data,
      //    gsl::span<GLint> const elements) noexcept
      // requires
      //    BufferType == basic_buffer_type::element_array
      // {
      //    {
      //       auto buffer_binding = array_data_.vbo_.write(data);
      //       vertex_attrib_pointer(std::index_sequence_for<Ts...>{},
      //          meta::partial_sum_t<sizeof(Ts)...>{},
      //          meta::partial_sum_t<(sizeof(Ts) / sizeof(underlying_type_t<Ts>))...>{});
      //       array_data_.count = ranges::size(elements);
      //       array_data_.ebo_.write(elements);
      //    }
      //    gl::BindVertexArray(0);
      // }

      template <ranges::Invocable F>
      void bind(F const& f) const noexcept
      {
         gl::BindVertexArray(array_data_.vao_.get());
         ranges::invoke(f);
         gl::DrawArrays(gl::TRIANGLES, 0, array_data_.count_);
         gl::BindVertexArray(0);
      }
   private:
      vertex_array_data<BufferType> array_data_;

      template <typename...> struct dne;

      template <std::size_t... Index, std::size_t... Stride, std::size_t... Offset>
      void vertex_attrib_pointer(std::index_sequence<Index...>, std::index_sequence<Stride...>,
         std::index_sequence<Offset...>)
      {
         // (gl::DisableVertexAttribArray(Index), ...);
         auto clos = [](auto&& index, auto&& size, auto&& type, auto&& normalised, auto&& stride, auto&& offset) {
            std::cout << "index = " << index << '\n'
                      << "size = " << std::boolalpha << (size == 3 && size != gl::BGRA) << '\n'
                      << "type = " << (type == gl::FLOAT) << '\n'
                      << "normalised = " << normalised << std::noboolalpha << '\n'
                      << "stride = " << stride << '\n'
                      << "offset = " << reinterpret_cast<GLsizeiptr>(offset) << '\n'
                      << '\n';
            // gl::VertexAttribPointer(index, size, type, normalised, stride, offset);
            
         };

         (clos(Index,                  // index
            sizeof(Ts) / sizeof(underlying_type_t<Ts>),     // size
            glsl_type_v<Ts>,                              // type
            false,                                        // normalised
            Stride - 12,                                       // stride
            reinterpret_cast<void const*>(Offset)), ...); // offset

         check_error("pre");
         gl::EnableVertexAttribArray(0);
         check_error("middle");
         gl::VertexAttribPointer(0, 3, gl::FLOAT, false, 6 * sizeof(float), reinterpret_cast<GLvoid*>(0 * sizeof(float)));
         // gl::VertexAttribPointer(1, 3, gl::FLOAT, false, 6 * sizeof(float), reinterpret_cast<GLvoid*>(3 * sizeof(float)));
         check_error("post");
         (gl::EnableVertexAttribArray(Index), ...);
      }
   };
} // namespace doge

#endif // DOGE_GL_VERTEX_HPP
