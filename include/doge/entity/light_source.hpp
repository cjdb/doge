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
#ifndef DOGE_ENTITY_LIGHT_SOURCE_HPP
#define DOGE_ENTITY_LIGHT_SOURCE_HPP

#include "doge/types.hpp"
#include <gsl/gsl>
#include <variant>

namespace doge {
   namespace ranges = std::experimental::ranges;

   class basic_light_source {
   public:
   protected:
      ~basic_light_source() = default;
   };

   class directional_light final : public basic_light_source {
   public:
      directional_light(uniform<glm::vec3> direction)
         : direction_{std::move(direction)}
      {}

      directional_light& operator=(directional_light const& light) noexcept
      {
         direction_ = light.direction_;
         return *this;
      }

      directional_light& operator=(vec3 const& direction) noexcept
      {
         direction_ = direction;
         return *this;
      }

      directional_light& operator+=(vec3 const& v) noexcept
      {
         direction_ += v;
         return *this;
      }

      directional_light& operator-=(vec3 const& v) noexcept
      {
         direction_ -= v;
         return *this;
      }

      directional_light& operator*=(float const scalar) noexcept
      {
         direction_ *= scalar;
         return *this;
      }

      directional_light& operator/=(float const scalar) noexcept
      {
         Expects(scalar != 0);
         direction_ /= scalar;
         return *this;
      }
   private:
      uniform<glm::vec3> direction_;
   };

   inline void draw(directional_light const&) noexcept {}

   class point_light final : public basic_light_source {};
   void draw(const point_light&);

   class spotlight final : public basic_light_source {};
   void draw(const spotlight&);

   using light_source = std::variant<directional_light, point_light, spotlight>;
   void draw(const light_source&);
} // namespace doge

#endif // DOGE_ENTITY_LIGHT_SOURCE_HPP
