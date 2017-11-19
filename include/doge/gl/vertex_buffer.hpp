//
// Copyright 2017 Christopher Di Bella
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
#ifndef DOGE_GL_VERTEX_BUFFER_HPP
#define DOGE_GL_VERTEX_BUFFER_HPP

#include <doge/gl/vertex_array.hpp>
#include <gl/gl_core.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <utility>
#include <vector>

namespace doge {
   template <typename T>
   class gl_buffer {
   public:
      gl_buffer(const GLsizei n, const GLenum target, std::vector<T> vertices) noexcept
         : size_{n},
           target_{target},
           vertices_{std::move(vertices)}
      {
         gl::GenBuffers(size_, &index_);
      }

      gl_buffer(gl_buffer&& b) noexcept
         : size_{b.size_},
           target_{b.target_},
           vertices_{std::move(b.vertices_)}
      {
         b.moved_ = true;
      }

      gl_buffer& operator=(gl_buffer&& b) noexcept
      {
         std::experimental::ranges::swap(b, *this);
         moved_ = false;
         return *this;
      }

      gl_buffer(const gl_buffer& b) noexcept
         : gl_buffer{b.size_, b.target_, b.vertices_}
      {}

      gl_buffer& operator=(const gl_buffer& b) noexcept
      {
         auto t = b;
         std::experimental::ranges::swap(t, *this);
         moved_ = false;
         return *this;
      }

      ~gl_buffer()
      {
         if (not moved_)
            gl::DeleteBuffers(size_, &index_);
      }

      constexpr GLenum target() const noexcept
      {
         return target_;
      }

      void bind(const GLenum usage) const noexcept
      {
         gl::BindBuffer(target_, index_);
         gl::BufferData(target_, vertices_.size() * sizeof(T), vertices_.data(), usage);
         //return {[this]{ gl::BindBuffer(target_, 0); }};
      }

      constexpr operator GLuint() const noexcept
      {
         return index_;
      }
   private:
      GLsizei size_;
      GLenum target_;
      std::vector<T> vertices_;
      GLuint index_;
      bool moved_ = false;
   };

   using vertex_buffer = gl_buffer<glm::vec3>;

   class element_buffer : public gl_buffer<GLuint> {
   public:
      element_buffer(const GLsizei n, std::vector<GLuint> indices)
         : gl_buffer<GLuint>{n, gl::ELEMENT_ARRAY_BUFFER, std::move(indices)}
      {}
   };

   template <typename T>
   void bind(const vertex_array<T>& a, const vertex_buffer& b, const GLenum usage) noexcept
   {
      a.bind();
      b.bind(usage);
      a.interpret();
      a.enable();
      gl::BindBuffer(b.target(), 0);
      gl::BindVertexArray(0);
   }

   template <typename T>
   void bind(const vertex_array<T>& a, const vertex_buffer& b, const element_buffer& e,
      const GLenum usage) noexcept
   {
      a.bind();
      b.bind(usage);
      e.bind(usage);
      a.interpret();
      a.enable();
      gl::BindBuffer(b.target(), 0);
      gl::BindVertexArray(0);
   }
} // namespace doge

#endif // DOGE_GL_VERTEX_BUFFER_HPP
