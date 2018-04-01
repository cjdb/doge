#ifndef DOGE_GL_VERTEX_HPP
#define DOGE_GL_VERTEX_HPP

#include "doge/meta/clear.hpp"
#include "doge/meta/partial_sum.hpp"
#include "doge/meta/rotate.hpp"
#include "doge/utility/std_layout_tuple.hpp"
#include "doge/utility/type_traits.hpp"

#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Wsign-compare"
#include "unique_resource.h"
#pragma GCC diagnostic pop

namespace doge {
   auto allocate_gl_resource(void(* allocator)(GLsizei, GLuint*), 
                             void(* deleter)  (GLsizei, GLuint const*)) noexcept
   {
      ranges::UnsignedIntegral result = GLuint{};
      ranges::invoke(allocator, 1, &result);
      return std::experimental::unique_resource<GLuint, std::function<void(GLuint)>>(result,
         [deleter]([[maybe_unused]] auto const i) mutable noexcept { 
            ranges::invoke(deleter, GLsizei{1}, &i);
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

   template <typename... Ts>
   requires
      (StandardLayout<Ts> && ...) &&
      ((sizeof(Ts) / sizeof(underlying_type_t<Ts>) <= 4) && ...)
   class vertex_array_buffer {
   public:
      explicit vertex_array_buffer(gsl::span<std_layout_tuple<Ts...> const> const data) noexcept
      {
         write(data);
      }

      void write(gsl::span<std_layout_tuple<Ts...> const> const data) noexcept
      {
         write(data, []{});
      }

      template <ranges::Invocable F>
      void bind(F const& f) const noexcept(noexcept(std::is_nothrow_invocable_v<F>))
      {
         gl::BindVertexArray(vao_.get());
         ranges::invoke(f);
         gl::BindVertexArray(0);
      }

      template <ranges::Invocable F>
      void draw(F const& f) const noexcept(noexcept(std::is_nothrow_invocable_v<F>))
      {
         bind([this, f]{
            ranges::invoke(f);
            gl::DrawArrays(gl::TRIANGLES, 0, count_);
         });
      }
   protected:
      GLsizei count() const noexcept
      {
         return count_;
      }

      void count(GLsizei const c) noexcept
      {
         count_ = c;
      }

      template <ranges::Invocable F>
      void write(gsl::span<std_layout_tuple<Ts...> const> const data, F const& f) noexcept(noexcept(
         std::is_nothrow_invocable_v<F>))
      {
         bind([this, data, f]{
            vbo_.write(data);
            count_ = ranges::size(data);
            f();
            vertex_attrib_pointer(std::index_sequence_for<Ts...>{},
               meta::clear_t<0, meta::rotate_t<sizeof...(Ts) - 1, meta::partial_sum_t<(sizeof(Ts) / sizeof(underlying_type_t<Ts>))...>>>{});
          });
      }
   private:
      GLsizei count_ = 0;
      array_buffer vbo_ = array_buffer{gl::GenBuffers, gl::DeleteBuffers};
      std::experimental::unique_resource<GLuint, std::function<void(GLuint)>> vao_ =
         ::doge::allocate_gl_resource(gl::GenVertexArrays, gl::DeleteVertexArrays);

      template <std::size_t... Index, std::size_t... Offset>
      void vertex_attrib_pointer(std::index_sequence<Index...>, std::index_sequence<Offset...>)
      {
         (gl::DisableVertexAttribArray(Index), ...);
         (gl::VertexAttribPointer(Index,                                            // index
            sizeof(Ts) / sizeof(underlying_type_t<Ts>),                             // size
            glsl_type_v<Ts>,                                                        // type
            false,                                                                  // normalised
            (sizeof(Ts) + ...),                                                     // stride
            reinterpret_cast<void const*>(Offset * sizeof(glsl_type_t<Ts>))), ...); // offset
         (gl::EnableVertexAttribArray(Index), ...);
      }
   };

   template <template <typename...> typename Container, typename... Ts, typename A>
   vertex_array_buffer(Container<std_layout_tuple<Ts...>, A> const&) -> vertex_array_buffer<Ts...>;

   template <template <typename, std::size_t> typename Container, std::size_t N, typename... Ts>
   vertex_array_buffer(Container<std_layout_tuple<Ts...>, N> const&) -> vertex_array_buffer<Ts...>;

   template <typename... Ts>
   requires
      (StandardLayout<Ts> && ...) &&
      ((sizeof(Ts) / sizeof(underlying_type_t<Ts>) <= 4) && ...)
   class vertex_element_buffer : vertex_array_buffer<Ts...> {
   public:
      using vertex_array_buffer<Ts...>::bind;

      explicit vertex_element_buffer(gsl::span<std_layout_tuple<Ts...> const> const data,
         gsl::span<GLuint const> const elements) noexcept
         : vertex_array_buffer<Ts...>{data}
      {
         write(elements);
      }

      void write(gsl::span<std_layout_tuple<Ts...> const> const data,
         gsl::span<GLuint> const elements) noexcept
      {
         this->write(data, [this, elements]{
            write(elements);
         });
      }

      template <ranges::Invocable F>
      void draw(F const& f) noexcept(noexcept(std::is_nothrow_invocable_v<F>))
      {
         bind([this, f]{
            ranges::invoke(f);
            gl::DrawElements(gl::TRIANGLES, this->count(), gl::UNSIGNED_INT, nullptr);
         });
      }
   private:
      element_array_buffer ebo_ = element_array_buffer{gl::GenBuffers, gl::DeleteBuffers};

      void write(gsl::span<GLuint const> const elements) noexcept
      {
         bind([this, elements]{
            this->count(ranges::size(elements));
            ebo_.write(elements);
         });
      }
   };

   template <template <typename> typename Container1, typename Container2, typename... Ts>
   vertex_element_buffer(Container1<std_layout_tuple<Ts...>> const&,
      Container2 const&) -> vertex_element_buffer<Ts...>;
} // namespace doge

#endif // DOGE_GL_VERTEX_HPP
