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
#ifndef DOGE_ENTITY_CAMERA_HPP
#define DOGE_ENTITY_CAMERA_HPP

#include <cmath>
#include <doge/engine.hpp>
#include <doge/units/angle.hpp>
#include <experimental/ranges/concepts>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <gsl/gsl>

namespace doge {
   namespace ranges = std::experimental::ranges;

   inline constexpr auto up = glm::vec3{0.0f, 1.0f, 0.0f};

   enum class cardinality {
      north,
      south,
      east,
      west
   };

   class camera {
      static inline constexpr ranges::Regular default_speed = 10.0f;
      enum dir_t { // should probably clean this up
         forward = 1,
         backward = -forward,
         right = 1,
         left = -right
      };
   public:
      enum type { free, fps };
      camera() = default;

      camera(const glm::vec3& position, const float speed = default_speed) noexcept
         : position_{position},
           speed_{speed}
      {}

      [[nodiscard]] const glm::vec3& position() const noexcept
      {
         return position_;
      }

      void position(const glm::vec3& p) noexcept
      {
         position_ = p;
      }

      [[nodiscard]] const glm::vec3& direction() const noexcept
      {
         return direction_;
      }

      [[nodiscard]] glm::mat4 view() const noexcept
      {
         return glm::lookAt(position(), direction() + position(), up);
      }

      [[nodiscard]] float speed() const noexcept
      {
         return speed_;
      }

      void speed(const float s) noexcept
      {
         speed_ = s;
      }

      [[nodiscard]] float framed_speed() const noexcept
      {
         return speed() * engine::frame_displacement();
      }

      void move(const cardinality c, const type t) noexcept
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

      void pitch(const angle a) noexcept
      {
         pitch_ = std::clamp(pitch() + a, -89.0_deg, 89.0_deg);
         change_direction();
      }

      void yaw(const angle a) noexcept
      {
         yaw_ += a;
         change_direction();
      }

      void field_of_view(const angle a) noexcept
      {
         field_of_view_ = std::clamp(field_of_view() + a, 1.0_deg, 45.0_deg);
      }

      void mouselook(const glm::vec2 delta) noexcept
      {
         yaw(doge::as_radians(delta.x));
         pitch(doge::as_radians(-delta.y));
      }

      angle field_of_view() const noexcept
      {
         return field_of_view_;
      }

      [[nodiscard]] glm::mat4 project(const float aspect_ratio, const float min_view,
         const float max_view) noexcept
      {
         return glm::perspective(gsl::narrow_cast<float>(field_of_view_), aspect_ratio, min_view,
            max_view);
      }

      static inline constexpr ranges::Regular front = glm::vec3{0.0f, 0.0f, -1.0f};
   private:
      glm::vec3 position_ = glm::vec3{0.0f, 0.0f, 3.0f};
      glm::vec3 direction_ = front;
      float speed_ = default_speed;
      angle pitch_ = 0.0_deg;
      angle yaw_ = -90.0_deg;
      angle field_of_view_ = 45.0_deg;

      [[nodiscard]] glm::vec3 direction(const type t) const noexcept
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

      void advance(const dir_t d, const type t) noexcept
      {
         position_ += d * framed_speed() * direction(t);
      }

      void strafe(const dir_t d, const type t) noexcept
      {
         position_ += d * framed_speed() * glm::normalize(glm::cross(direction(t), up));
      }

      void change_direction() noexcept
      {
         direction_.x = std::cos(pitch_) * std::cos(yaw_);
         direction_.y = std::sin(pitch_);
         direction_.z = std::cos(pitch_) * std::sin(yaw_);
         direction_ = glm::normalize(direction_);
      }
   };
} // namespace doge

#endif // DOGE_ENTITY_CAMERA_HPP
