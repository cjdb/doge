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

#include <glm/mat4x4.hpp>

namespace doge {
   namespace ranges = std::experimental::ranges;

   using uniform = GLint;

   class basic_light_source {
   public:
      basic_light_source(uniform projection, uniform view, uniform model)
         : projection_{std::move(projection)},
           view_{std::move(view)},
           model_{std::move(model)}
      {}

      void draw(const glm::mat4& p, const glm::mat4& v, const glm::mat4& m)
      {
         projection_ = p;
         view_ = v;
         model_ m;
         draw_impl();
      }
   protected:
      ~basic_light_source() = default;

      const uniform& projection() const noexcept
      {
         return projection_;
      }

      const uniform& view() const noexcept
      {
         return view_;
      }

      const uniform& model() const noexcept
      {
         return model_;
      }
   private:
      uniform<glm::mat4> projection_;
      uniform<glm::mat4> view_;
      uniform<glm::mat4> model_;

      virtual void draw_impl() = 0;
   };

   class directional_light {};
   class point_light {};
   class spotlight {};
} // namespace doge

#endif // DOGE_ENTITY_LIGHT_SOURCE_HPP
