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
#include "doge/engine.hpp"
#include "doge/entity/basic_entity.hpp"
#include "doge/types.hpp"
#include "doge/units/angle.hpp"
#include <experimental/ranges/concepts>
#include <glm/gtc/matrix_transform.hpp>
#include <gsl/gsl>

namespace doge {
   namespace ranges = std::experimental::ranges;
   class camera : public basic_entity {
   public:
      camera() = default;

      camera(vec3 const& position, float const speed = default_speed) noexcept
         : basic_entity{position, speed}
      {}

      [[nodiscard]] mat4 view() const noexcept
      {
         return glm::lookAt(position(), direction() + position(), up);
      }

      void field_of_view(radians const a) noexcept
      {
         field_of_view_ = std::clamp(field_of_view() + a, 1.0_deg, 45.0_deg);
      }

      void mouselook(vec2 const delta) noexcept
      {
         yaw(doge::as_radians(delta.x));
         pitch(doge::as_radians(-delta.y));
      }

      radians field_of_view() const noexcept
      {
         return field_of_view_;
      }

      [[nodiscard]] mat4 project(float const aspect_ratio, float const min_view,
         const float max_view) noexcept
      {
         return glm::perspective(::doge::dimensionless_cast(field_of_view_), aspect_ratio, min_view,
            max_view);
      }
   private:
      radians field_of_view_ = 45.0_deg;
   };
} // namespace doge

#endif // DOGE_ENTITY_CAMERA_HPP
