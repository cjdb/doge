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
#include <doge/types.hpp>
#include <doge/utility/screen_data.hpp>
#include <experimental/ranges/concepts>
#include <experimental/ranges/functional>
#include <gl/gl_core.hpp>
#include <GLFW/glfw3.h>

namespace doge {
   namespace ranges = std::experimental::ranges;

   enum class depth_test { disabled, enabled };

   class engine {
   public:
      engine(depth_test const enable_depth_test = depth_test::disabled)
         : clear_flags_{set_clear_flags(static_cast<bool>(enable_depth_test))}
      {
         doge::hid::keyboard::init(screen_.window()); // TODO get this into a constructor >:(
         doge::hid::mouse::init(screen_.window()); // TODO get this into a constructor >:(
      }

      template <ranges::Invocable F>
      void play(F const& logic)
      {
         while (screen_.open()) {
            compute_frame_displacement();
            clear_screen();
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

      void clear_colour(vec4 const& colour) noexcept
      {
         clear_colour_ = colour;
         gl::ClearColor(clear_colour_.r, clear_colour_.g, clear_colour_.b, clear_colour_.a);
      }

      void clear_colour(float const r, float const g, float const b, float const a = 1.0f) noexcept
      {
         clear_colour({r, g, b, a});
      }

      void clear_screen() const noexcept
      {
         gl::Clear(clear_flags_);
      }

      [[nodiscard]] static ranges::Regular frame_displacement() noexcept
      {
         return frame_displacement_;
      }
   private:
      screen_data screen_;
      vec4 clear_colour_ = {1.0f, 1.0f, 1.0f, 1.0f};
      GLenum clear_flags_;

      static inline float previous_frame_ = glfwGetTime();
      static inline float frame_displacement_ = 0.0f;

      static GLenum set_clear_flags(bool const depth_buffer) noexcept
      {
         auto flags = GLenum{gl::COLOR_BUFFER_BIT};
         if (depth_buffer) {
            gl::Enable(gl::DEPTH_TEST);
            flags |= gl::DEPTH_BUFFER_BIT;
         }

         return flags;
      }

      static void compute_frame_displacement() noexcept
      {
         ranges::Regular const current_frame = glfwGetTime();
         ranges::Regular const displacement = current_frame - previous_frame_;
         previous_frame_ = current_frame;
         frame_displacement_ = displacement;
      }
   };
} // namespace doge

#endif // DOGE_ENGINE_HPP
