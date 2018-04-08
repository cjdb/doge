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
#ifndef DOGE_GL_VERTEX_ARRAY_HPP
#define DOGE_GL_VERTEX_ARRAY_HPP

#include "doge/gl/buffer.hpp"
#include "doge/gl/memory.hpp"
#include "doge/meta/clear.hpp"
#include "doge/meta/partial_sum.hpp"
#include "doge/meta/rotate.hpp"
#include "doge/utility/std_layout_tuple.hpp"
#include "doge/utility/type_traits.hpp"
#include <experimental/ranges/concepts>
#include "gl/gl_core.hpp"
#include <gsl/gsl>
#include <functional>

#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Wsign-compare"
#include "unique_resource.h"
#pragma GCC diagnostic pop

namespace doge {
   template <basic_buffer_usage Usage, typename... Ts>
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
         gl::BindVertexArray(vao_[0]);
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
         vbo_.write(data);
         bind([this, data, f]{
            count_ = ranges::size(data);
            f();
            vertex_attrib_pointer(std::index_sequence_for<Ts...>{},
               meta::clear_t<0, meta::rotate_t<sizeof...(Ts) - 1, meta::partial_sum_t<(sizeof(Ts) / sizeof(underlying_type_t<Ts>))...>>>{});
          });
      }
   private:
      GLsizei count_ = 0;
      array_buffer<Usage, Ts...> vbo_;
      gpu_resource<resource_type::vertex_array> vao_;

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

   template <basic_buffer_usage Usage = basic_buffer_usage::static_draw, typename... Ts>
   auto make_vertex_array_buffer(gsl::span<std_layout_tuple<Ts...> const> const data) noexcept
   {
      return vertex_array_buffer<Usage, Ts...>{data};
   }

   template <basic_buffer_usage Usage = basic_buffer_usage::static_draw,
      template <typename> typename Container, typename... Ts>
   requires
      ranges::ext::ContiguousRange<Container<std_layout_tuple<Ts...>>>
   auto make_vertex_array_buffer(Container<std_layout_tuple<Ts...>> const& data) noexcept
   {
      return vertex_array_buffer<Usage, Ts...>{data};
   }

   template <basic_buffer_usage Usage = basic_buffer_usage::static_draw,
      template <typename, std::size_t> typename Container, typename... Ts, std::size_t N>
   requires
      ranges::ext::ContiguousRange<Container<std_layout_tuple<Ts...>, N>>
   auto make_vertex_array_buffer(Container<std_layout_tuple<Ts...>, N> const& data) noexcept
   {
      return vertex_array_buffer<Usage, Ts...>{data};
   }

   template <basic_buffer_usage Usage, typename... Ts>
   requires
      (StandardLayout<Ts> && ...) &&
      ((sizeof(Ts) / sizeof(underlying_type_t<Ts>) <= 4) && ...)
   class vertex_element_buffer : vertex_array_buffer<Usage, Ts...> {
   public:
      using vertex_array_buffer<Usage, Ts...>::bind;

      explicit vertex_element_buffer(gsl::span<std_layout_tuple<Ts...> const> const data,
         gsl::span<GLuint const> const elements) noexcept
         : vertex_array_buffer<Usage, Ts...>{data}
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
      element_array_buffer<Usage> ebo_;

      void write(gsl::span<GLuint const> const elements) noexcept
      {
         bind([this, elements]{
            this->count(ranges::size(elements));
            ebo_.write(elements);
         });
      }
   };

   template <basic_buffer_usage Usage = basic_buffer_usage::static_draw, typename... Ts>
   auto make_vertex_element_buffer(gsl::span<std_layout_tuple<Ts...> const> const data,
      gsl::span<GLuint const> const elements) noexcept
   {
      return vertex_element_buffer<Usage, Ts...>{data, elements};
   }

   template <basic_buffer_usage Usage = basic_buffer_usage::static_draw,
      template <typename> typename Container, typename... Ts>
   auto make_vertex_element_buffer(Container<std_layout_tuple<Ts...>> const& data,
      gsl::span<GLuint const> const elements) noexcept
   {
      return vertex_element_buffer<Usage, Ts...>{data, elements};
   }

   template <basic_buffer_usage Usage = basic_buffer_usage::static_draw,
      template <typename, std::size_t> typename Container, typename... Ts, std::size_t N>
   requires
      ranges::ext::ContiguousRange<Container<std_layout_tuple<Ts...>, N>>
   auto make_vertex_element_buffer(Container<std_layout_tuple<Ts...>, N> const& data,
      gsl::span<GLuint const> const elements) noexcept
   {
      return vertex_element_buffer<Usage, Ts...>{data, elements};
   }
} // namespace doge

#endif // DOGE_GL_VERTEX_ARRAY_HPP
