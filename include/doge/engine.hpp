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
#ifndef DOGE_ENGINE_HPP
#define DOGE_ENGINE_HPP

#include <doge/hid.hpp>
#include <doge/utility/screen_data.hpp>
#include <experimental/ranges/concepts>
#include <experimental/ranges/functional>
#include <gl/gl_core.hpp>
#include <GLFW/glfw3.h>

namespace doge {
   namespace ranges = std::experimental::ranges;

   class engine {
   public:
      engine()
      {
         doge::hid::keyboard::init(screen_.window());
         doge::hid::mouse::init(screen_.window());
      }

      template <ranges::Invocable F>
      void play(const F& logic)
      {
         while (screen_.open()) {
            compute_frame_displacement();
            ranges::invoke(logic);
            hid::mouse::update();
            screen_.swap_buffers();
            glfwPollEvents();
         }
      }

      const screen_data& screen() const noexcept
      {
         return screen_;
      }

      void close() noexcept
      {
         screen_.close();
      }

      [[nodiscard]] static ranges::Regular frame_displacement() noexcept
      {
         return frame_displacement_;
      }
   private:
      screen_data screen_;
      static inline float previous_frame_ = glfwGetTime();
      static inline float frame_displacement_ = 0.0f;

      static void compute_frame_displacement() noexcept
      {
         const ranges::Regular current_frame = glfwGetTime();
         const ranges::Regular displacement = current_frame - previous_frame_;
         previous_frame_ = current_frame;
         frame_displacement_ = displacement;
      }
   };
} // namespace doge

#endif // DOGE_ENGINE_HPP
