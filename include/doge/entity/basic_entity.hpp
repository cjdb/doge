//
//  Copyright 2017 Christopher Di Bella
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
#ifndef DOGE_ENTITY_BASIC_ENTITY_HPP
#define DOGE_ENTITY_BASIC_ENTITY_HPP

#include <cmath>
#include "doge/engine.hpp"
#include "doge/types.hpp"
#include "doge/units/angle.hpp"
#include <experimental/ranges/concepts>
#include <glm/gtc/matrix_transform.hpp>
#include <gsl/gsl>

namespace doge {
   namespace ranges = std::experimental::ranges;

   inline constexpr ranges::Regular front = vec3{0.0f, 0.0f, -1.0f};
   inline constexpr ranges::Regular up = vec3{0.0f, 1.0f, 0.0f};

   enum class cardinality {
      north,
      south,
      east,
      west
   };

   class basic_entity {
      enum dir_t { // should probably clean this up
         forward = 1,
         backward = -forward,
         right = 1,
         left = -right
      };
   public:
      enum type { free, fps };
      [[nodiscard]] vec3 const& position() const noexcept
      {
         return position_;
      }

      void position(vec3 const& p) noexcept
      {
         position_ = p;
      }

      [[nodiscard]] vec3 const& direction() const noexcept
      {
         return direction_;
      }

      void move(cardinality const c, type const t) noexcept
      {
         switch (c) {
         case cardinality::north:
            advance(forward, t);
            break;
         case cardinality::south:
            advance(backward, t);
            break;
         case cardinality::east:
            strafe(right, t);
            break;
         case cardinality::west:
            strafe(left, t);
            break;
         }
      }

      void pitch(angle const a) noexcept
      {
         pitch_ = std::clamp(pitch() + a, -89.0_deg, 89.0_deg);
         change_direction();
      }

      void yaw(angle const a) noexcept
      {
         yaw_ += a;
         change_direction();
      }

      [[nodiscard]] float speed() const noexcept
      {
         return speed_;
      }

      void speed(float const s) noexcept
      {
         speed_ = s;
      }

      [[nodiscard]] float framed_speed() const noexcept
      {
         return speed() * engine::frame_displacement();
      }
   protected:
      basic_entity() = default;

      basic_entity(vec3 const& position, float const speed)
         : position_{position},
           speed_{speed}
      {}

      ~basic_entity() = default;

      static inline constexpr ranges::Regular default_speed = 10.0f;
   private:
      vec3 position_{0.0f, 0.0f, 3.0f};
      vec3 direction_ = front;
      float speed_ = default_speed;
      angle pitch_ = 0.0_deg;
      angle yaw_ = -90.0_deg;

      [[nodiscard]] vec3 direction(type const t) const noexcept
      {
         auto d = direction();
         if (t == fps)
            d.y = 0.0f;
         return d;
      }

      [[nodiscard]] angle pitch() const noexcept
      {
         return pitch_;
      }

      void advance(dir_t const d, type const t) noexcept
      {
         position_ += d * framed_speed() * direction(t);
      }

      void strafe(dir_t const d, type const t) noexcept
      {
         position_ += d * framed_speed() * glm::normalize(glm::cross(direction(t), up));
      }

      void change_direction() noexcept
      {
         direction_.x = ::doge::cos(pitch_) * ::doge::cos(yaw_);
         direction_.y = ::doge::sin(pitch_);
         direction_.z = ::doge::cos(pitch_) * ::doge::sin(yaw_);
         direction_ = glm::normalize(direction_);
      }
   };
} // namespace doge

#endif // DOGE_ENTITY_BASIC_ENTITY_HPP
