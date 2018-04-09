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
#ifndef DOGE_DETAIL_MATRIX_HPP
#define DOGE_DETAIL_MATRIX_HPP

#include "doge/units/angle.hpp"
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace doge::detail {
   template <typename T>
   class translate_impl {
   public:
      explicit constexpr translate_impl(const T& v) noexcept
         : v_{v}
      {}

      translate_impl(const translate_impl&) = delete;
      translate_impl& operator=(const translate_impl&) = delete;

      friend auto operator|(const glm::mat4& m, translate_impl&& t) noexcept
      {
         return glm::translate(m, t.v_);
      }
   private:
      T v_;
   };

   template <typename T>
   class rotate_impl {
   public:
      explicit constexpr rotate_impl(const radians a, const T& v) noexcept
         : angle_{a},
            v_{v}
      {}

      rotate_impl(const rotate_impl&) = delete;
      rotate_impl& operator=(const rotate_impl&) = delete;

      friend auto operator|(const glm::mat4& m, rotate_impl&& r) noexcept
      {
         return glm::rotate(m, dimensionless_cast(r.angle_), r.v_);
      }
   private:
      radians angle_;
      T v_;
   };

   template <typename T>
   class scale_impl {
   public:
      explicit constexpr scale_impl(const glm::vec3& v) noexcept
         : v_{v}
      {}

      scale_impl(const scale_impl&) = delete;
      scale_impl& operator=(const scale_impl&) = delete;

      friend auto operator|(const glm::mat4& m, scale_impl&& r) noexcept
      {
         return glm::scale(m, r.v_);
      }
   private:
      T v_;
   };

   class invert_impl {
   public:
      invert_impl(const invert_impl&) = delete;
      invert_impl& operator=(const invert_impl&) = delete;

      template <typename T>
      requires requires(const T t) {
         glm::inverse(t);
      }
      friend auto operator|(const T& t, const invert_impl&) noexcept
      {
         return glm::inverse(t);
      }

      static const auto& construct() noexcept
      {
         static const auto invert = invert_impl{};
         return invert;
      }
   private:
      invert_impl() = default;
   };

   class transpose_impl {
   public:
      transpose_impl(const transpose_impl&) = delete;
      transpose_impl& operator=(const transpose_impl&) = delete;

      template <typename T>
      requires requires(const T t) {
         glm::transpose(t);
      }
      friend auto operator|(const T& t, const transpose_impl&) noexcept
      {
         return glm::transpose(t);
      }

      static const auto& construct() noexcept
      {
         static const auto transpose = transpose_impl{};
         return transpose;
      }
   private:
      transpose_impl() = default;
   };
} // namesapce doge::detail

#endif // DOGE_DETAIL_MATRIX_HPP
