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
#ifndef DOGE_HID_HPP
#define DOGE_HID_HPP

#include <experimental/ranges/concepts>
#include <functional>
#include <gl/gl_core.hpp>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

namespace doge::hid {
   enum class key_state { up, release, press, down };
   using std::experimental::ranges::Regular;

   class hid {
   public:
      static bool shift() noexcept
      {
         return shift_;
      }

      static bool control() noexcept
      {
         return control_;
      }

      static bool alt() noexcept
      {
         return alt_;
      }

      static bool super() noexcept
      {
         return super_;
      }
   protected:
      static void modifiers(const int mods) noexcept
      {
         shift_ = mods & GLFW_MOD_SHIFT;
         control_ = mods & GLFW_MOD_CONTROL;
         alt_ = mods & GLFW_MOD_ALT;
         super_ = mods & GLFW_MOD_SUPER;
      }
   private:
      hid() = delete;
      ~hid() = delete;
      static inline Regular shift_ = false;
      static inline Regular control_ = false;
      static inline Regular alt_ = false;
      static inline Regular super_ = false;
   };

   class keyboard : public hid {
   public:
      static void init(GLFWwindow* w) noexcept
      {
         glfwSetInputMode(w, GLFW_STICKY_KEYS, true);
         glfwSetKeyCallback(w, keyboard::callback);
      }

      static key_state key(const int i) noexcept
      {
         return key_mask_.at(i + 1);
      }

      static void key(const int i, const key_state s) noexcept
      {
         key_mask_.at(i + 1) = s;
      }
   private:
      static inline Regular key_mask_ = std::vector(360, key_state::up);

      static void callback(GLFWwindow*, int key, int, int action, int mods) noexcept
      {
         ++key;
         if (action == GLFW_PRESS && key_mask_[key] != key_state::down)
            key_mask_[key] = key_state::press;
         else if (action == GLFW_RELEASE && key_mask_[key] != key_state::up)
            key_mask_[key] = key_state::release;

         modifiers(mods);
      }
   };

#define DOGE_INVOCABLE_NOEXCEPT(f, Args, args) \
   noexcept(noexcept(std::experimental::ranges::invoke(f, std::forward<Args>(args)...)))

   template <typename F, typename... Args>
   requires
      std::experimental::ranges::Invocable<F, Args...>
   using void_or_optional_invoke_result_t = std::conditional_t<
      std::is_same_v<std::invoke_result_t<F, Args...>, void>, void,
      std::optional<std::invoke_result_t<F, Args...>>>;

   template <typename F, typename... Args>
   requires
      std::experimental::ranges::Invocable<F, Args...>
   constexpr Regular returns_optional = not std::is_same_v<std::invoke_result_t<F, Args...>, void>;

   class mouse : public hid {
   public:
      enum index {
         previous,
         current
      };

      enum class cursor_type {
         normal = GLFW_CURSOR_NORMAL,
         hidden = GLFW_CURSOR_HIDDEN,
         disabled = GLFW_CURSOR_DISABLED
      };

      static void init(GLFWwindow* const w, const cursor_type c = cursor_type::disabled,
         const bool buttons = true, const bool cursor = true, const bool scroll = true,
         [[maybe_unused]] const bool enter = false) noexcept
      {
         glfwSetInputMode(w, GLFW_CURSOR, static_cast<int>(c));
         glfwSetInputMode(w, GLFW_STICKY_MOUSE_BUTTONS, true);

         if (buttons)
            glfwSetMouseButtonCallback(w, button_callback);
         if (cursor)
            glfwSetCursorPosCallback(w, cursor_callback);
         if (scroll)
            glfwSetScrollCallback(w, scroll_callback);
         //if (enter)
         //   glfwSetCursorEnterCallback(w, enter_callback);
      }

      static [[nodiscard]] glm::vec2 cursor(index i) noexcept
      {
         return cursor_[i];
      }

      static [[nodiscard]] glm::vec2 scroll(index i) noexcept
      {
         return scroll_[i];
      }

      static [[nodiscard]] float sensitivity() noexcept
      {
         return sensitivity_;
      }

      static void sensitivity(const float s) noexcept
      {
         sensitivity_ = s;
      }

      static glm::vec2 cursor_delta() noexcept
      {
         return (cursor(current) - cursor(previous)) * sensitivity_;
      }

      static glm::vec2 scroll_delta() noexcept
      {
         return scroll(current) - scroll(previous);
      }

      static void update() noexcept
      {
         cursor_[previous] = cursor_[current];
         scroll_[previous] = scroll_[current];
      }

      template <typename F, typename... Args>
      requires
         std::experimental::ranges::Invocable<F, Args...>
      static auto on_cursor_delta(const glm::vec2& d, const F& f, Args&&... args)
         DOGE_INVOCABLE_NOEXCEPT(f, Args, args)
      {
         return conditionally_invoke(cursor_delta() == d, f, std::forward<Args>(args)...);
      }

      template <typename F, typename... Args>
      requires
         std::experimental::ranges::Invocable<F, Args...>
      static auto on_scroll_delta(const glm::vec2& d, const F& f, Args&&... args)
         DOGE_INVOCABLE_NOEXCEPT(f, Args, args)
      {
         return conditionally_invoke(scroll_delta() == d, f, std::forward<Args>(args)...);
      }

      template <typename F, typename... Args>
      requires
         std::experimental::ranges::Invocable<F, Args...>
      static auto on_cursor_position(const glm::vec2& v, const F& f, Args&&... args)
         DOGE_INVOCABLE_NOEXCEPT(f, Args, args)
      {
         return conditionally_invoke(cursor(current) == v, f, std::forward<Args>(args)...);
      }

      template <typename F, typename... Args>
      requires
         std::experimental::ranges::Invocable<F, Args...>
      static auto on_scroll_position(const glm::vec2& v, const F& f, Args&&... args)
         DOGE_INVOCABLE_NOEXCEPT(f, Args, args)
      {
         return conditionally_invoke(scroll(current) == v, f, std::forward<Args>(args)...);
      }
   private:
      static inline Regular key_mask_ = std::vector(8, key_state::up);
      static inline Regular cursor_ = std::array<glm::vec2, 2>{{{960.0f, 540.0f}, {960.0f, 540.0f}}};
      static inline Regular scroll_ = std::array<glm::vec2, 2>{{{0.0f, 0.0f}, {0.0f, 0.0f}}};
      static inline Regular sensitivity_ = 0.1f;

      static void button_callback(GLFWwindow*, int key, int action, int mods) noexcept
      {
         if (action == GLFW_PRESS && key_mask_[key] != key_state::down)
            key_mask_[key] = key_state::press;
         else if (action == GLFW_RELEASE && key_mask_[key] != key_state::up)
            key_mask_[key] = key_state::release;

         modifiers(mods);
      }

      static void cursor_callback(GLFWwindow*, double x, double y) noexcept
      {
         cursor_[previous] = cursor_[current];
         cursor_[current].x = static_cast<float>(x);
         cursor_[current].y = static_cast<float>(y);
      }

      static void scroll_callback(GLFWwindow*, double x, double y) noexcept
      {
         scroll_[previous] = scroll_[current];
         scroll_[current].x = static_cast<float>(x);
         scroll_[current].y = static_cast<float>(y);
      }

      template <typename F, typename... Args>
      requires
         std::experimental::ranges::Invocable<F, Args...>
      static void_or_optional_invoke_result_t<F, Args...>
      conditionally_invoke(const bool b, const F& f, Args&&... args)
         DOGE_INVOCABLE_NOEXCEPT(f, Args, args)
      {
         if (b)
            return std::experimental::ranges::invoke(f, std::forward<Args>(args)...);
         if constexpr (returns_optional<F, Args...>)
            return {};
      }
   };

   template <typename T, typename F, typename... Args>
   requires
      std::experimental::ranges::Invocable<F, Args...> &&
      std::experimental::ranges::DerivedFrom<T, hid>
   void_or_optional_invoke_result_t<F, Args...>
      on_key_down(const int i, const F& f, Args&&... args) DOGE_INVOCABLE_NOEXCEPT(f, Args, args)
   {
      if (T::key(i) >= key_state::press) {
         T::key(i, key_state::down);
         return std::experimental::ranges::invoke(f, std::forward<Args>(args)...);
      }

      if constexpr (returns_optional<F, Args...>)
         return {};
   }

   template <typename T, typename F, typename... Args>
   requires
      std::experimental::ranges::Invocable<F, Args...> &&
      std::experimental::ranges::DerivedFrom<T, hid>
   void_or_optional_invoke_result_t<F, Args...>
      on_key_press(const int i, const F& f, Args&&... args) DOGE_INVOCABLE_NOEXCEPT(f, Args, args)
   {
      if (T::key(i) == key_state::press)
         return on_key_down<T>(i, f, std::forward<Args>(args)...);

      if constexpr (returns_optional<F, Args...>)
         return {};
   }

   template <typename T, typename F, typename... Args>
   requires
      std::experimental::ranges::Invocable<F, Args...> &&
      std::experimental::ranges::DerivedFrom<T, hid>
   void_or_optional_invoke_result_t<F, Args...>
      on_key_up(const int i, const F& f, Args&&... args) DOGE_INVOCABLE_NOEXCEPT(f, Args, args)
   {
      if (T::key(i) <= key_state::release) {
         T::key(i, key_state::up);
         return std::experimental::ranges::invoke(f, std::forward<Args>(args)...);
      }

      if constexpr (returns_optional<F, Args...>)
         return {};
   }

   template <typename T, typename F, typename... Args>
   requires
      std::experimental::ranges::Invocable<F, Args...> &&
      std::experimental::ranges::DerivedFrom<T, hid>
   void_or_optional_invoke_result_t<F, Args...>
      on_key_release(const int i, const F& f, Args&&... args) DOGE_INVOCABLE_NOEXCEPT(f, Args, args)
   {
      if (T::key(i) == key_state::release)
         return on_key_up<T>(i, f, std::forward<Args>(args)...);

      if constexpr (returns_optional<F, Args...>)
         return {};
   }
} // namespace doge::hid

#undef DOGE_INVOCABLE_NOEXCEPT
#endif // DOGE_HID_HPP
