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
   /// @brief Determines the type of buffer.
   /// @note basic_buffer_type::invalid is for internal use only.
   ///
   enum class basic_buffer_type {
      invalid,
      array = gl::ARRAY_BUFFER,
      element_array = gl::ELEMENT_ARRAY_BUFFER,
      uniform = gl::UNIFORM_BUFFER,
   };

   /// @brief Determines how the buffer will be typically used.
   /// @note basic_buffer_usage::invalid is for internal use only.
   ///
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

   /// @brief Determines whether the buffer will be laid out as an array of structures or a
   /// structure of arrays.
   /// @note basic_buffer_layout::invalid is for internal use only.
   ///
   enum class basic_buffer_layout {
      invalid,
      array_of_structures,
      structure_of_arrays
   };

   /// @brief 
   /// @tparam Resource Determines the operations that the 
   /// @tparam T
   /// @tparam Usage
   /// @tparam Layout
   /// @tparam Types...
   ///
   template <resource_type Resource, basic_buffer_type T, basic_buffer_usage Usage,
      basic_buffer_layout Layout = basic_buffer_layout::array_of_structures, typename... Types>
   requires
      (StandardLayout<Types> && ...)
   class basic_buffer {
   public:
      /// @brief Writes data to the buffer.
      ///
      /// TODO: add more
      /// @tparam UTypes... 
      /// @param data
      /// @note Only participates in overload resolution when sizeof...(Types) == sizeof...(UTypes),
      ///    Layout == basic_buffer_layout::array_of_structures, and for each [T, U] in
      ///    (Types, UTypes), T is constructible from U.
      ///
      template <typename... UTypes>
      requires
         sizeof...(Types) == sizeof...(UTypes) &&
         Layout == basic_buffer_layout::array_of_structures
      void write(gsl::span<std_layout_tuple<UTypes...> const> const data) noexcept
      {
         static_assert((ranges::Constructible<Types, UTypes> && ...));
         write_impl<(sizeof(UTypes) + ...)>(data);
      }

      /// @brief Writes data to the buffer.
      /// @tparam UTypes...
      /// @param utypes...
      /// @note Only participates in overload resolution if sizeof...(Types) == sizeof...(UTypes),
      ///    Layout == basic_buffer_layout::structure_of_arrays, for each U in UTypes is a
      ///    contiguous range and for each T in Types, T is constructible from U.
      ///
      template <typename... UTypes>
      requires
         sizeof...(Types) == sizeof...(UTypes) &&
         Layout == basic_buffer_layout::structure_of_arrays &&
         (ranges::ext::ContiguousRange<UTypes> && ...)
      void write(UTypes&&... utypes) noexcept
      {
         static_assert((ranges::Constructible<Types, UTypes> && ...));
         write_impl(std::forward<UTypes>(utypes)..., std::index_sequence_for<UTypes...>{});
      }

      /// @brief
      /// @param
      void write(gsl::span<GLuint const> const elements) noexcept
      requires
         T == basic_buffer_type::element_array
      {
         write_impl<sizeof(GLuint)>(elements);
      }

      GLuint operator[](int const i) const noexcept
      {
         Expects(i >= 0);
         return buffer_[i];
      }

      ranges::RandomAccessRange const& get() const noexcept
      {
         return buffer_.get();
      }
   private:
      gpu_resource<Resource,
         Layout == basic_buffer_layout::array_of_structures ? 1 : sizeof...(Types)> buffer_;

      template <std::size_t Size, typename U>
      void write_impl(gsl::span<U const> const data) noexcept
      {
         constexpr ranges::UnsignedIntegral type = static_cast<GLuint>(T);
         gl::BindBuffer(type, buffer_[0]);
         gl::BufferData(type, ranges::size(data) * Size, ranges::data(data), static_cast<GLuint>(Usage));
      }

      template <typename... UTypes, std::size_t... IndexSequence>
      requires
         Layout == basic_buffer_layout::structure_of_arrays
      void write(UTypes&&... utypes, std::index_sequence<IndexSequence...>)
      {
         static_assert((ranges::ext::ContiguousRange<UTypes> && ...));
         static_asserT((ranges::Constructible<Types, UTypes> && ...));
         constexpr ranges::UnsignedIntegral type = static_cast<GLuint>(T);
         ((gl::BindBuffer(type, buffer_[IndexSequence]),
           gl::BufferData(type, ranges::size(utypes) * sizeof(ranges::value_type_t<std::decay_t<UTypes>>),
              ranges::data(utypes), static_cast<GLuint>(Usage))), ...);
      }
   };

   template <basic_buffer_usage Usage, typename... Types>
   using array_buffer = basic_buffer<resource_type::buffer, basic_buffer_type::array, Usage,
      basic_buffer_layout::array_of_structures, Types...>;

   template <basic_buffer_usage Usage>
   using element_array_buffer =
      basic_buffer<resource_type::buffer, basic_buffer_type::element_array, Usage>;

   template <basic_buffer_usage Usage, typename... Types>
   using uniform_buffer = basic_buffer<resource_type::buffer, basic_buffer_type::uniform, Usage,
      basic_buffer_layout::array_of_structures, Types...>;
} // namespace doge

#endif // DOGE_GL_VERTEX_BUFFER_HPP
