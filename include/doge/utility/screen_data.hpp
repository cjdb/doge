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
#ifndef DOGE_UTILITY_SCREEN_DATA_HPP
#define DOGE_UTILITY_SCREEN_DATA_HPP

#include <gl/gl_core.hpp>
#include <GLFW/glfw3.h>
#include <memory>

namespace doge {
   class screen_data {
   public:
      enum class backend_t { gl, vulkan };

      screen_data() = default;

      screen_data(const backend_t b, const int width, const int height, const int antialiasing,
        const bool fullscreen)
         : backend_{b},
           width_{width},
           height_{height},
           antialiasing_{antialiasing},
           fullscreen_{fullscreen}
      {}

      constexpr backend_t backend() const noexcept
      {
         return backend_;
      }

      constexpr int width() const noexcept
      {
         return width_;
      }

      constexpr int height() const noexcept
      {
         return height_;
      }

      constexpr int antialiasing() const noexcept
      {
         return antialiasing_;
      }

      constexpr bool fullscreen() const noexcept
      {
         return fullscreen_;
      }

      GLFWwindow* window() noexcept
      {
         return window_.get();
      }

      const GLFWwindow* window() const noexcept
      {
         return window_.get();
      }

      void make_window()
      {
         window_ = make_window_impl();
      }

      bool open() noexcept
      {
         return not glfwWindowShouldClose(window());
      }

      void close() noexcept
      {
         glfwSetWindowShouldClose(window(), true);
      }

      void swap_buffers() noexcept
      {
         glfwSwapBuffers(window());
      }
   private:
      backend_t backend_ = backend_t::gl;
      int width_ = 1920;
      int height_ = 1080;
      int antialiasing_ = 4;
      bool fullscreen_ = false;
      std::unique_ptr<GLFWmonitor, void(*)(void*)> monitor_ = []{
         if (not glfwInit())
            throw std::runtime_error{"Unable to initialise GLFW3"};
         return std::unique_ptr<GLFWmonitor, void(*)(void*)>{glfwGetPrimaryMonitor(),
            [](void*){ glfwTerminate(); }};
      }();
      const GLFWvidmode* vidmode_ = glfwGetVideoMode(monitor_.get());
      std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)> window_ = make_window_impl();

      std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)> make_window_impl()
      {
         if (backend_ == backend_t::gl) {
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_SAMPLES, antialiasing_);
         }

         auto w = std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)>{
            glfwCreateWindow(width_, height_, "doge", fullscreen_ ? monitor_.get() : nullptr, nullptr),
            [](GLFWwindow* p){ if (p) glfwDestroyWindow(p); }};
         glfwMakeContextCurrent(window_.get());
         glfwSetFramebufferSizeCallback(window_.get(), screen_data::framebuffer_size_callback);
         if (auto gl_load_gen = gl::sys::LoadFunctions(); not (window_ || gl_load_gen))
            throw std::runtime_error{"Could not open window with glfw3."};
         return w;
      }

      static void framebuffer_size_callback(GLFWwindow*, const int width, const int height) noexcept
      {
         gl::Viewport(0, 0, width, height);
      }
   };
}

#endif // DOGE_UTILITY_SCREEN_DATA_HPP
