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
   class engine {
   public:
      engine()
      {
         doge::hid::keyboard::init(screen_.window());
      }

      template <std::experimental::ranges::Invocable F>
      void play(const F& logic)
      {
         while (screen_.open()) {
            std::experimental::ranges::invoke(logic);
            screen_.swap_buffers();
            glfwPollEvents();
         }
      }

      void close() noexcept
      {
         screen_.close();
      }
   private:
      screen_data screen_;
   };
} // namespace doge

#endif // DOGE_ENGINE_HPP
