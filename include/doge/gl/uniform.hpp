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
#ifndef DOGE_GL_UNIFORM_HPP
#define DOGE_GL_UNIFORM_HPP

#include <cassert>
#include "doge/detail/gl_invocable.hpp"
#include "doge/gl/cast.hpp"
#include "doge/gl/shader_binary.hpp"
#include "doge/gl/gl_error.hpp"
#include <gl/gl_core.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat2x2.hpp>
#include <glm/mat2x3.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat3x2.hpp>
#include <glm/mat3x4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/mat4x2.hpp>
#include <glm/mat4x3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <gsl/gsl>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <utility>

namespace doge {
   namespace ranges = std::experimental::ranges;

   struct uniform_not_found : public std::runtime_error {
      uniform_not_found(const std::string& id)
         : std::runtime_error{"uniform not found: " + id}
      {}
   };

   template <ranges::DefaultConstructible T>
   class uniform;

   namespace detail {
      inline GLint find_location(const shader_binary& program, const std::string_view id)
      {
         if (ranges::SignedIntegral i = gl::GetUniformLocation(static_cast<GLuint>(program), id.data()); i >= 0)
            return i;
         throw uniform_not_found{id.data()};
      }

      template <class T>
      constexpr bool is_uniform = false;

      template<ranges::DefaultConstructible T>
      constexpr bool is_uniform<uniform<T>> = true;

      template <typename T>
      struct uniform_data {
         detail::uniform_function_t<std::remove_const_t<T>> set_uniform_;
         GLuint program_ ;
         GLint location_ ;
         std::remove_const_t<T> value_;
      };

      template <typename T>
      requires
         not std::is_const_v<T> &&
         is_glm_matrix<T>
      struct uniform_data<T> {
         detail::uniform_function_t<std::remove_const_t<T>> set_uniform_;
         GLuint program_;
         GLint location_;
         GLsizei count_;
         GLboolean transpose_;
         std::remove_const_t<T> value_;
      };

      template <typename T>
      struct uniform_data<const T> {
         GLuint program_ = 0;
         GLint location_ = 0;
         std::remove_const_t<T> value_ = {};

         uniform_data(const shader_binary& program, const GLint location,
            const detail::get_uniform_function_t<std::remove_cv_t<T>>& f)
            : program_{program},
              location_{location},
              value_{[&, this]{
                    ranges::Regular result = std::remove_const_t<T>{};
                    ranges::Regular data = [&result]() mutable {
                       using T2 = decltype(result);
                       if constexpr (detail::is_glm_vec<T2> || detail::is_glm_matrix<T2>) {
                          return glm::value_ptr(result);
                       }
                       else {
                          return std::addressof(result);
                       }
                    }();
                    ranges::invoke(f, program_, location_, data);
                    return result;
                 }()
              }
         {}
      };
   }

   template <class T>
   constexpr bool is_uniform = detail::is_uniform<std::decay_t<T>>;

   template <ranges::DefaultConstructible T>
   class uniform {
   public:
      uniform(const shader_binary& program, const std::string_view id)
      requires
         std::is_const_v<T> &&
         detail::is_glm_GLfloat<T>
         : uniform{program, id, gl::GetUniformfv}
      {}

      uniform(const shader_binary& program, const std::string_view id)
      requires
         std::is_const_v<T> &&
         detail::is_glm_GLint<T>
         : uniform{program, id, gl::GetUniformiv}
      {}

      uniform(const shader_binary& program, const std::string_view id)
      requires
         std::is_const_v<T> &&
         detail::is_glm_GLuint<T>
         : uniform{program, id, gl::GetUniformuiv}
      {}

      template <ranges::ConvertibleTo<T> T2>
      uniform(const shader_binary& program, const std::string_view id, const T2& t2)
         : uniform{program, id, gsl::narrow_cast<T>(t2)}
      {}

      uniform(const shader_binary& program, const std::string_view id, const GLfloat v)
      requires
         ranges::Same<T, GLfloat>
         : uniform{program, id, gl::Uniform1f, v}
      {}

      // uniform(const shader_binary& program, const std::string_view id, const T& rng)
      // requires
      //    ranges::RandomAccessRange<T> &&
      //    ranges::Same<ranges::data_.value_type_t<T>, GLfloat>
      //    : uniform{program, id, gl::Uniform1fv, rng}
      // {}

      uniform(const shader_binary& program, const std::string_view id, const glm::vec2& v)
      requires
         ranges::Same<T, glm::vec2>
         : uniform{program, id, gl::Uniform2f, v}
      {}

      // uniform(const shader_binary& program, const std::string_view id, const T& rng)
      // requires
      //    ranges::RandomAccessRange<T> &&
      //    ranges::Same<ranges::data_.value_type_t<T>, glm::vec2>
      //    : uniform{program, id, gl::Uniform2fv, rng}
      // {}

      uniform(const shader_binary& program, const std::string_view id, const glm::vec3& v)
      requires
         ranges::Same<T, glm::vec3>
         : uniform{program, id, gl::Uniform3f, v}
      {}

      // uniform(const shader_binary& program, const std::string_view id, const T& rng)
      // requires
      //    ranges::RandomAccessRange<T> &&
      //    ranges::Same<ranges::data_.value_type_t<T>, glm::vec3>
      //    : uniform{program, id, gl::Uniform3fv, rng}
      // {}

      uniform(const shader_binary& program, const std::string_view id, const glm::vec4& v)
      requires
         ranges::Same<T, glm::vec4>
         : uniform{program, id, gl::Uniform4f, v}
      {}

      // uniform(const shader_binary& program, const std::string_view id, const T& rng)
      // requires
      //    ranges::RandomAccessRange<T> &&
      //    ranges::Same<ranges::data_.value_type_t<T>, glm::vec4>
      //    : uniform{program, id, gl::Uniform4fv, rng}
      // {}

      uniform(const shader_binary& program, const std::string_view id, const GLint i)
      requires
         ranges::Same<T, GLint>
         : uniform{program, id, gl::Uniform1i, i}
      {}

      // uniform(const shader_binary& program, const std::string_view id, const T& rng)
      // requires
      //    ranges::RandomAccessRange<T> &&
      //    ranges::Same<ranges::data_.value_type_t<T>, GLint>
      //    : uniform{program, id, gl::Uniform1iv, rng}
      // {}

      uniform(const shader_binary& program, const std::string_view id, const glm::ivec2 v)
      requires
         ranges::Same<T, glm::ivec2>
         : uniform{program, id, gl::Uniform2i, v}
      {}

      // uniform(const shader_binary& program, const std::string_view id, const T& rng)
      // requires
      //    ranges::RandomAccessRange<T> &&
      //    ranges::Same<ranges::data_.value_type_t<T>, glm::ivec2>
      //    : uniform{program, id, gl::Uniform2iv, rng}
      // {}

      uniform(const shader_binary& program, const std::string_view id, const glm::ivec3 v)
      requires
         ranges::Same<T, glm::ivec3>
         : uniform{program, id, gl::Uniform3i, v}
      {}

      // uniform(const shader_binary& program, const std::string_view id, const T& rng)
      // requires
      //    ranges::RandomAccessRange<T> &&
      //    ranges::Same<ranges::data_.value_type_t<T>, glm::ivec3>
      //    : uniform{program, id, gl::Uniform3iv, rng}
      // {}

      uniform(const shader_binary& program, const std::string_view id, const glm::ivec4 v)
      requires
         ranges::Same<T, glm::ivec4>
         : uniform{program, id, gl::Uniform4i, v}
      {}

      // uniform(const shader_binary& program, const std::string_view id, const T& rng)
      // requires
      //    ranges::RandomAccessRange<T> &&
      //    ranges::Same<ranges::data_.value_type_t<T>, glm::ivec4>
      //    : uniform{program, id, gl::Uniform4iv, rng}
      // {}

      uniform(const shader_binary& program, const std::string_view id, const GLuint v)
      requires
         ranges::Same<T, GLuint>
         : uniform{program, id, gl::Uniform1ui, v}
      {}

      // uniform(const shader_binary& program, const std::string_view id, const T& rng)
      // requires
      //    ranges::RandomAccessRange<T> &&
      //    ranges::Same<ranges::data_.value_type_t<T>, GLuint>
      //    : uniform{program, id, gl::Uniform1uiv, rng}
      // {}

      uniform(const shader_binary& program, const std::string_view id, const glm::uvec2 v)
      requires
         ranges::Same<T, glm::uvec2>
         : uniform{program, id, gl::Uniform2ui, v}
      {}

      // uniform(const shader_binary& program, const std::string_view id, const T& rng)
      // requires
      //    ranges::RandomAccessRange<T> &&
      //    ranges::Same<ranges::data_.value_type_t<T>, glm::uvec2>
      //    : uniform{program, id, gl::Uniform2uiv, rng}
      // {}

      uniform(const shader_binary& program, const std::string_view id, const glm::uvec3 v)
      requires
         ranges::Same<T, glm::uvec3>
         : uniform{program, id, gl::Uniform3ui, v}
      {}

      // uniform(const shader_binary& program, const std::string_view id, const T& rng)
      // requires
      //    ranges::RandomAccessRange<T> &&
      //    ranges::Same<ranges::data_.value_type_t<T>, glm::uvec3>
      //    : uniform{program, id, gl::Uniform3uiv, rng}
      // {}

      uniform(const shader_binary& program, const std::string_view id, const glm::uvec4 v)
      requires
         ranges::Same<T, glm::uvec4>
         : uniform{program, id, gl::Uniform4ui, v}
      {}

      // uniform(const shader_binary& program, const std::string_view id, const T& rng)
      // requires
      //    ranges::RandomAccessRange<T> &&
      //    ranges::Same<ranges::data_.value_type_t<T>, glm::uvec4>
      //    : uniform{program, id, gl::Uniform4uiv, rng}
      // {}

      uniform(const shader_binary& program, const std::string_view id, const bool transpose,
         const glm::mat2x2& m)
      requires
         ranges::Same<T, glm::mat2x2>
         : uniform{program, id, 1, transpose, gl::UniformMatrix2fv, m}
      {}

      // uniform(const shader_binary& program, const std::string_view id, const bool transpose,
      //    const T& rng)
      // requires
      //    ranges::RandomAccessRange<T> &&
      //    ranges::Same<ranges::data_.value_type_t<T>, glm::mat2>
      //    : uniform{program, id, gl::UniformMatrix2fv, transpose, rng}
      // {}

      uniform(const shader_binary& program, const std::string_view id, const bool transpose,
         const glm::mat3x3& m)
      requires
         ranges::Same<T, glm::mat3x3>
         : uniform{program, id, 1, transpose, gl::UniformMatrix3fv, m}
      {}

      // uniform(const shader_binary& program, const std::string_view id, const bool transpose,
      //    const T& rng)
      // requires
      //    ranges::RandomAccessRange<T> &&
      //    ranges::Same<ranges::data_.value_type_t<T>, glm::mat3>
      //    : uniform{program, id, gl::UniformMatrix3fv, transpose, rng}
      // {}

      uniform(const shader_binary& program, const std::string_view id, const bool transpose,
         const glm::mat4x4& m)
      requires
         ranges::Same<T, glm::mat4x4>
         : uniform{program, id, 1, transpose, gl::UniformMatrix4fv, m}
      {}

      // uniform(const shader_binary& program, const std::string_view id, const bool transpose,
      //    const T& rng)
      // requires
      //    ranges::RandomAccessRange<T> &&
      //    ranges::Same<ranges::data_.value_type_t<T>, glm::mat4>
      //    : uniform{program, id, gl::UniformMatrix4fv, transpose, rng}
      // {}

      uniform(const shader_binary& program, const std::string_view id, const bool transpose,
         const glm::mat2x3& m)
      requires
         ranges::Same<T, glm::mat2x3>
         : uniform{program, id, 1, transpose, gl::UniformMatrix2x3fv, m}
      {}

      // uniform(const shader_binary& program, const std::string_view id, const bool transpose,
      //    const T& rng)
      // requires
      //    ranges::RandomAccessRange<T> &&
      //    ranges::Same<ranges::data_.value_type_t<T>, glm::mat2x3>
      //    : uniform{program, id, gl::UniformMatrix2x3fv, transpose, rng}
      // {}

      uniform(const shader_binary& program, const std::string_view id, const bool transpose,
         const glm::mat3x2& m)
      requires
         ranges::Same<T, glm::mat3x2>
         : uniform{program, id, 1, transpose, gl::UniformMatrix3x2fv, m}
      {}

      // uniform(const shader_binary& program, const std::string_view id, const bool transpose,
      //    const T& rng)
      // requires
      //    ranges::RandomAccessRange<T> &&
      //    ranges::Same<ranges::data_.value_type_t<T>, glm::mat3x2>
      //    : uniform{program, id, gl::UniformMatrix3x2fv, transpose, rng}
      // {}

      uniform(const shader_binary& program, const std::string_view id, const bool transpose,
         const glm::mat4x2& m)
      requires
         ranges::Same<T, glm::mat4x2>
         : uniform{program, id, 1, transpose, gl::UniformMatrix4x2fv, m}
      {}

      // uniform(const shader_binary& program, const std::string_view id, const bool transpose,
      //    const T& rng)
      // requires
      //    ranges::RandomAccessRange<T> &&
      //    ranges::Same<ranges::data_.value_type_t<T>, glm::mat4x2>
      //    : uniform{program, id, gl::UniformMatrix4x2fv, transpose, rng}
      // {}

      uniform(const shader_binary& program, const std::string_view id, const bool transpose,
         const glm::mat2x4& m)
      requires
         ranges::Same<T, glm::mat2x4>
         : uniform{program, id, 1, transpose, gl::UniformMatrix2x4fv, m}
      {}

      // uniform(const shader_binary& program, const std::string_view id, const bool transpose,
      //    const T& rng)
      // requires
      //    ranges::RandomAccessRange<T> &&
      //    ranges::Same<ranges::data_.value_type_t<T>, glm::mat2x4>
      //    : uniform{program, id, gl::UniformMatrix2x4fv, transpose, rng}
      // {}

      uniform(const shader_binary& program, const std::string_view id, const bool transpose,
         const glm::mat4x3& m)
      requires
         ranges::Same<T, glm::mat4x3>
         : uniform{program, id, 1, transpose, gl::UniformMatrix4x3fv, m}
      {}

      // uniform(const shader_binary& program, const std::string_view id, const bool transpose,
      //    const T& rng)
      // requires
      //    ranges::RandomAccessRange<T> &&
      //    ranges::Same<ranges::data_.value_type_t<T>, glm::mat4x3>
      //    : uniform{program, id, gl::UniformMatrix4x3fv, transpose, rng}
      // {}

      uniform(const shader_binary& program, const std::string_view id, const bool transpose,
         const glm::mat3x4& m)
      requires
         ranges::Same<T, glm::mat3x4>
         : uniform{program, id, 1, transpose, gl::UniformMatrix3x4fv, m}
      {}

      // uniform(const shader_binary& program, const std::string_view id, const bool transpose,
      //    const T& rng)
      // requires
      //    ranges::RandomAccessRange<T> &&
      //    ranges::Same<ranges::data_.value_type_t<T>, glm::mat3x4>
      //    : uniform{program, id, gl::UniformMatrix3x4fv, transpose, rng}
      // {}

      uniform(const uniform&) = delete;

      template <ranges::ConvertibleTo<T> T2>
      uniform(const uniform<T2>& t) = delete;

      uniform(uniform&& t) noexcept(noexcept(std::is_nothrow_move_constructible_v<T>))
         : data_{std::move(t.data_)}
      {
         t.data_.program_ = {};
         t.data_.location_ = {};
         t.data_.value_ = {};
      }

      template <ranges::ConvertibleTo<T> T2>
      uniform(uniform<T2>&& t) noexcept(noexcept(std::is_nothrow_constructible_v<T, T2>))
         : data_{std::move(t.data_)}
      {
         t.data_.program_ = {};
         t.data_.location_ = {};
         t.data_.value_ = {};
      }

      uniform& operator=(const uniform& t) noexcept(noexcept(std::is_nothrow_copy_assignable_v<T>))
      requires
         not std::is_const_v<T>
      {
         return *this = t.data_.value_;
      }

      template <ranges::ConvertibleTo<T> T2>
      requires
         not std::is_const_v<T>
      uniform& operator=(const uniform<T2>& t) noexcept(noexcept(std::is_nothrow_assignable_v<T, T2>))
      {
         return *this = t.data_.value_;
      }

      template <ranges::ConvertibleTo<T> T2 = T>
      requires
         not std::is_const_v<T>
      uniform& operator=(T2&& t)
         noexcept(noexcept(std::is_nothrow_constructible_v<T, T2> && std::is_nothrow_copy_assignable_v<T>))
      {
         data_.value_ = gsl::narrow_cast<T>(std::forward<T2>(t));
         set_uniform("");
         return *this;
      }

      T operator+() const noexcept
      {
         return data_.value_;
      }

      T operator-() const noexcept
      {
         return -data_.value_;
      }

      template <typename T2>
      requires
         not std::is_const_v<T>
      //    RingWith<T, T2>
      uniform& operator+=(const uniform<T2>& u) noexcept
      {
         return *this += u.data_.value_;
      }

      template <typename T2>
      requires
         not std::is_const_v<T>
      //    RingWith<T, T2>
      uniform& operator+=(const T2& t2) noexcept
      {
         data_.value_ += t2;
         set_uniform("");
         return *this;
      }

      template <typename T2>
      requires
         not std::is_const_v<T>
      //    RingWith<T, T2>
      uniform& operator-=(const uniform<T2>& u) noexcept
      {
         return *this -= u.data_.value_;
      }

      template <typename T2>
      requires
         not std::is_const_v<T>
      //    RingWith<T, T2>
      uniform& operator-=(const T2& t2) noexcept
      {
         data_.value_ -= t2;
         set_uniform("");
         return *this;
      }

      template <typename T2>
      requires
         not std::is_const_v<T>
      //    RingWith<T, T2>
      uniform& operator*=(const uniform<T2>& u) noexcept
      {
         return *this *= u.data_.value_;
      }

      template <typename T2>
      requires
         not std::is_const_v<T>
      //    RingWith<T, T2>
      uniform& operator*=(const T2& t2) noexcept
      {
         data_.value_ *= t2;
         set_uniform("");
         return *this;
      }

      template <typename T2>
      requires
         not std::is_const_v<T>
      //    RingWith<T, T2>
      uniform& operator/=(const uniform<T2>& u) noexcept
      {
         return *this /= u.data_.value_;
      }

      template <typename T2>
      requires
         not std::is_const_v<T>
      //    RingWith<T, T2>
      uniform& operator/=(const T2& t2) noexcept
      {
         Expects(t2 != T2{});
         data_.value_ /= t2;
         set_uniform("");
         return *this;
      }

      template <typename T2>
      requires
         not std::is_const_v<T> &&
         ranges::Integral<T> &&
         ranges::Integral<std::remove_const_t<T2>>
      //    RingWith<T, T2>
      uniform& operator%=(const uniform<T2>& u) noexcept
      {
         return *this %= u.data_.value_;
      }

      template <typename T2>
      requires
         not std::is_const_v<T> &&
         ranges::Integral<T> &&
         ranges::Integral<std::remove_const_t<T2>>
      //    RingWith<T, T2>
      uniform& operator%=(const T2& t2) noexcept
      {
         Expects(t2 != T2{});
         data_.value_ %= t2;
         set_uniform("");
         return *this;
      }

      uniform& operator++() noexcept
      requires
         not std::is_const_v<T> &&
         requires(std::remove_const_t<T> t) {
         {++t} -> T&;
      }
      {
         ++data_.value_;
         set_uniform("");
         return *this;
      }

      T operator++(int) noexcept
      requires
         not std::is_const_v<T> &&
         requires(std::remove_const_t<T> t) {
         {t++} -> T;
      }
      {
         auto result = data_.value_;
         ++*this;
         return result;
      }

      uniform& operator--() noexcept
      requires
         not std::is_const_v<T> &&
         requires(std::remove_const_t<T> t) {
         {--t} -> T&;
      }
      {
         --data_.value_;
         set_uniform("");
         return *this;
      }

      T operator--(int) noexcept
      requires
         not std::is_const_v<T> &&
         requires(std::remove_const_t<T> t) {
         {t--} -> T;
      }
      {
         auto result = data_.value_;
         --*this;
         return result;
      }

      explicit operator T() const noexcept(noexcept(std::is_nothrow_copy_assignable_v<T>))
      {
         return data_.value_;
      }

      template <typename T2, std::enable_if_t<not is_uniform<T2>>* = nullptr>
      // requires
      //    RingWith<T, T2>
      friend std::common_type_t<T, T2> operator+(const T2& a, const uniform& b) noexcept
      {
         return a + b.data_.value_;
      }

      template <typename T2, std::enable_if_t<not is_uniform<T2>>* = nullptr>
      // requires
      //    RingWith<T, T2>
      friend std::common_type_t<T, T2> operator+(const uniform& a, const T2& b) noexcept
      {
         return b + a;
      }

      template <typename T2>
      // requires
      //    RingWith<T, T2>
      friend std::common_type_t<T, T2> operator+(const uniform& a, const uniform<T2>& b) noexcept
      {
         return a + b.data_.value_;
      }

      template <typename T2>
      // requires
      //    RingWith<T, T2>
      friend std::enable_if_t<not is_uniform<T2>, std::common_type_t<T, T2>>
      operator-(const T2& a, const uniform& b) noexcept
      {
         return a - b.data_.value_;
      }

      template <typename T2>
      // requires
      //    RingWith<T, T2>
      friend std::enable_if_t<not is_uniform<T2>, std::common_type_t<T, T2>>
      operator-(const uniform& a, const T2& b) noexcept
      {
         return a.data_.value_ - b;
      }

      template <typename T2>
      // requires
      //    RingWith<T, T2>
      friend std::common_type_t<T, T2> operator-(const uniform& a, const uniform<T2>& b) noexcept
      {
         return a - b.data_.value_;
      }

      template <typename T2, std::enable_if_t<not is_uniform<T2>>* = nullptr>
      // requires
      //    RingWith<T, T2>
      friend auto operator*(const T2& a, const uniform& b) noexcept
      {
         return a * b.data_.value_;
      }

      template <typename T2, std::enable_if_t<not is_uniform<T2>>* = nullptr>
      // requires
      //    RingWith<T, T2>
      friend auto operator*(const uniform& a, const T2& b) noexcept
      {
         return a.data_.value_ * b;
      }

      template <typename T2>
      // requires
      //    RingWith<T, T2>
      friend auto operator*(const uniform& a, const uniform<T2>& b) noexcept
      {
         return a * b.data_.value_;
      }

      template <typename T2>
      // requires
      //    RingWith<T, T2>
      friend std::enable_if_t<not is_uniform<T2>, std::common_type_t<T, T2>>
      operator/(const T2& a, const uniform& b) noexcept
      {
         Expects(b != T{});
         return a / b.data_.value_;
      }

      template <typename T2>
      // requires
      //    RingWith<T, T2>
      friend std::enable_if_t<not is_uniform<T2>, std::common_type_t<T, T2>>
      operator/(const uniform& a, const T2& b) noexcept
      {
         Expects(b != T2{});
         return a.data_.value_ / b;
      }

      template <typename T2>
      // requires
      //    RingWith<T, T2>
      friend std::common_type_t<T, T2> operator/(const uniform& a, const uniform<T2>& b) noexcept
      {
         return a / b.data_.value_;
      }

      template <typename T2>
      // requires
      //    IntegralRingWith<T, T2>
      friend std::enable_if_t<not is_uniform<T2>, std::common_type_t<T, T2>>
      operator%(const T2& a, const uniform& b) noexcept
      {
         Expects(b != T{});
         return a % b.data_.value_;
      }

      template <typename T2>
      // requires
      //    IntegralRingWith<T, T2>
      friend std::enable_if_t<not is_uniform<T2>, std::common_type_t<T, T2>>
      operator%(const uniform& a, const T2& b) noexcept
      {
         Expects(b != T2{});
         return a.data_.value_ % b;
      }

      template <typename T2>
      // requires
      //    IntegralRingWith<T, T2>
      friend std::common_type_t<T, T2> operator%(const uniform& a, const uniform<T2>& b) noexcept
      {
         return a % b.data_.value_;
      }

      template <ranges::EqualityComparableWith<T> T2>
      friend bool operator==(const uniform& a, const uniform<T2>& b) noexcept
      {
         const auto result = a.data_.value_ == b.data_.value_;
         assert(a.data_.program_ != b.data_.program_ || a.data_.location_ != b.data_.location_ || result);
         return result;
      }

      template <ranges::EqualityComparableWith<T> T2>
      requires
         not is_uniform<T2>
      friend std::enable_if_t<not is_uniform<T2>, bool>
      operator==(const uniform& a, const T2& b) noexcept
      {
         return a.data_.value_ == b;
      }

      template <ranges::EqualityComparableWith<T> T2>
      requires
         not is_uniform<T2>
      friend std::enable_if_t<not is_uniform<T2>, bool>
      operator==(const T2& a, const uniform& b) noexcept
      {
         return b == a;
      }

      template <ranges::EqualityComparableWith<T> T2>
      friend bool operator!=(const uniform& a, const uniform<T2>& b) noexcept
      {
         return not(a == b);
      }

      template <ranges::EqualityComparableWith<T> T2>
      requires
         not is_uniform<T2>
      friend std::enable_if_t<not is_uniform<T2>, bool>
      operator!=(const uniform& a, const T2& b) noexcept
      {
         return not(a == b);
      }

      template <ranges::EqualityComparableWith<T> T2>
      requires
         not is_uniform<T2>
      friend std::enable_if_t<not is_uniform<T2>, bool>
      operator!=(const T2& a, const uniform& b) noexcept
      {
         return not(a == b);
      }

      template <ranges::StrictTotallyOrderedWith<T> T2>
      friend std::enable_if_t<ranges::StrictTotallyOrderedWith<T, T2>, bool>
      operator<(const uniform& a, const uniform<T2>& b) noexcept
      {
         const auto result = a.data_.value_ < b.data_.value_;
         assert(a.data_.program_ != b.data_.program_ || a.data_.location_ != b.data_.location_ || not result);
         return result;
      }

      template <ranges::StrictTotallyOrderedWith<T> T2>
      friend std::enable_if_t<ranges::StrictTotallyOrderedWith<T, T2> && not is_uniform<T2>, bool>
      operator<(const T2& a, const uniform& b) noexcept
      {
         return a < b.data_.value_;
      }

      template <ranges::StrictTotallyOrderedWith<T> T2>
      friend std::enable_if_t<ranges::StrictTotallyOrderedWith<T, T2> && not is_uniform<T2>, bool>
      operator<(const uniform& a, const T2& b) noexcept
      {
         return a.data_.value_ < b;
      }

      template <ranges::StrictTotallyOrderedWith<T> T2>
      friend std::enable_if_t<ranges::StrictTotallyOrderedWith<T, T2>, bool>
      operator>(const uniform& a, const uniform<T2>& b) noexcept
      {
         return b < a;
      }

      template <ranges::StrictTotallyOrderedWith<T> T2>
      friend std::enable_if_t<ranges::StrictTotallyOrderedWith<T, T2> && not is_uniform<T2>, bool>
      operator>(const T2& a, const uniform& b) noexcept
      {
         return b < a;
      }

      template <ranges::StrictTotallyOrderedWith<T> T2>
      friend std::enable_if_t<ranges::StrictTotallyOrderedWith<T, T2> && not is_uniform<T2>, bool>
      operator>(const uniform& a, const T2& b) noexcept
      {
         return b < a;
      }

      template <ranges::StrictTotallyOrderedWith<T> T2>
      friend std::enable_if_t<ranges::StrictTotallyOrderedWith<T, T2>, bool>
      operator<=(const uniform& a, const uniform<T2>& b) noexcept
      {
         return not(a > b);
      }

      template <ranges::StrictTotallyOrderedWith<T> T2>
      friend std::enable_if_t<ranges::StrictTotallyOrderedWith<T, T2> && not is_uniform<T2>, bool>
      operator<=(const T2& a, const uniform& b) noexcept
      {
         return not(a > b);
      }

      template <ranges::StrictTotallyOrderedWith<T> T2>
      friend std::enable_if_t<ranges::StrictTotallyOrderedWith<T, T2> && not is_uniform<T2>, bool>
      operator<=(const uniform& a, const T2& b) noexcept
      {
         return not(a > b);
      }

      template <ranges::StrictTotallyOrderedWith<T> T2>
      friend std::enable_if_t<ranges::StrictTotallyOrderedWith<T, T2>, bool>
      operator>=(const uniform& a, const uniform<T2>& b) noexcept
      {
         return not(a < b);
      }

      template <ranges::StrictTotallyOrderedWith<T> T2>
      friend std::enable_if_t<ranges::StrictTotallyOrderedWith<T, T2> && not is_uniform<T2>, bool>
      operator>=(const uniform& a, const T2& b) noexcept
      {
         return not(a < b);
      }

      template <ranges::StrictTotallyOrderedWith<T> T2>
      friend std::enable_if_t<ranges::StrictTotallyOrderedWith<T, T2> && not is_uniform<T2>, bool>
      operator>=(const T2& a, const uniform& b) noexcept
      {
         return not(a < b);
      }
   private:
      detail::uniform_data<T> data_;

      uniform(const shader_binary& program, const std::string_view id,
         const detail::get_uniform_function_t<std::remove_cv_t<T>>& f)
         : data_{program, ::doge::detail::find_location(program, id), f}
      {}

      template <typename F>
      uniform(const shader_binary&, const std::string_view, const F&)
      requires
         ranges::RandomAccessRange<T> = delete; // TODO

      struct set_constructor_t {
      };

      template <typename F1, typename... Args>
      uniform(const set_constructor_t, const shader_binary& program, const std::string_view id,
         const F1& f1, Args&&... args)
         : data_{f1, static_cast<GLuint>(program), ::doge::detail::find_location(program, id),
            std::forward<Args>(args)...}
      {
         set_uniform(id);
      }

      template <typename F, glm::length_t C, glm::length_t R, typename T2, glm::qualifier Q>
      uniform(const shader_binary& program, const std::string_view id, const GLsizei count,
         const bool transpose, const F& f, const glm::mat<C, R, T2, Q>& matrix)
         : data_{f, static_cast<GLuint>(program), ::doge::detail::find_location(program, id),
              count, transpose, matrix}
      {
         set_uniform(id);
      }

      template <typename F1>
      uniform(const shader_binary& program, const std::string_view id, const F1& f1,
         const T t)
         : uniform{set_constructor_t{}, program, id, f1, t}
      {}

      template <typename F1>
      uniform(const shader_binary& program, const std::string_view id, const F1& f1,
         const glm::tvec2<T>& v)
         : uniform{set_constructor_t{}, program, id, f1, v.x, v.y}
      {}

      template <typename F1>
      uniform(const shader_binary& program, const std::string_view id, const F1& f1,
         const glm::tvec3<T>& v)
         : uniform{set_constructor_t{}, program, id, f1, v.x, v.y, v.z}
      {}

      template <typename F1>
      uniform(const shader_binary& program, const std::string_view id, const F1& f1,
         const glm::tvec4<T>& v)
         : uniform{set_constructor_t{}, program, id, f1, v.x, v.y, v.z, v.w}
      {}

      void set_uniform(const std::string_view id)
      {
         if constexpr (is_one_of_v<T, GLfloat, GLint, GLuint>) {
            ranges::invoke(data_.set_uniform_, data_.location_, data_.value_);
         }
         else if constexpr (detail::is_vec2<T>) {
            ranges::invoke(data_.set_uniform_, data_.location_, data_.value_.x, data_.value_.y);
         }
         else if constexpr (detail::is_vec3<T>) {
            ranges::invoke(data_.set_uniform_, data_.location_, data_.value_.x, data_.value_.y,
               data_.value_.z);
         }
         else if constexpr (detail::is_vec4<T>) {
            ranges::invoke(data_.set_uniform_, data_.location_, data_.value_.x, data_.value_.y,
               data_.value_.z, data_.value_.w);
         }
         else if constexpr (detail::is_glm_matrix<T>) {
            ranges::invoke(data_.set_uniform_, data_.location_, data_.count_, data_.transpose_,
               glm::value_ptr(data_.value_));
         }

         check_error(std::string{id});
      }

      // template <typename F, typename T>
      // uniform(const shader_program& program, const std::string_view id, const F& f,
      //    const glm::detail::tmat)
   };

   template <ranges::DefaultConstructible T>
   uniform(const shader_binary&, std::string_view, T) -> uniform<T>;
} // namespace doge

#endif // DOGE_GL_UNIFORM_HPP
