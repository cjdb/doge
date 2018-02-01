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
#ifndef DOGE_GL_VERTEX_ARRAY_HPP
#define DOGE_GL_VERTEX_ARRAY_HPP

#include <cassert>
#include <doge/gl/buffer_interpreter.hpp>
#include <doge/utility/reference_count.hpp>
#include <experimental/ranges/concepts>
#include <gl/gl_core.hpp>
#include <gsl/gsl>
#include <memory>
#include <optional>
#include <type_traits>
#include <variant>
#include <vector>

namespace doge {
   namespace ranges = std::experimental::ranges;

   class vertex {
   public:
      vertex(const GLenum target, const GLenum usage,
         gsl::not_null<std::shared_ptr<std::vector<GLfloat>>> data, const GLint size,
         const std::vector<GLint>& stride)
         : data_{std::move(data.get())}
      {
         Expects(not data_->empty());
         init(target, usage, size, stride, []{});
      }

      vertex(const GLenum target, const GLenum usage,
         gsl::not_null<std::shared_ptr<std::vector<GLfloat>>> data, std::vector<GLint> indices,
         const GLint size, const std::vector<GLint>& stride)
         : data_{std::move(data.get())},
           indices_{std::move(indices)}
      {
         Expects(not data_->empty());
         Expects(indices_ and not indices_->empty());

         init(target, usage, size, stride, [this, usage]{
            gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, *ebo_);
            gl::BufferData(gl::ELEMENT_ARRAY_BUFFER, indices_->size() * sizeof(GLint),
               indices_->data(), usage); });
      }

      enum draw_type { triangles = gl::TRIANGLES };

      void draw(const draw_type mode) const noexcept
      {
         gl::DrawElements(gsl::narrow_cast<GLint>(mode), indices_->size(), gl::UNSIGNED_INT,
            nullptr);
      }

      void draw(const draw_type mode, const GLint first, const GLint last) const noexcept
      {
         gl::DrawArrays(gsl::narrow_cast<GLint>(mode), first, last);
      }

      template <ranges::Invocable F>
      void bind(const F& f) const noexcept
      {
         gl::BindVertexArray(vao_);
         ranges::invoke(f);
      }
   private:
      GLuint array_size_ = 1;
      reference_count<GLuint> vao_ = make_reference_count(array_size_, gl::GenVertexArrays,
         gl::DeleteVertexArrays);

      GLuint buffer_size_ = 1;
      reference_count<GLuint> vbo_ = make_reference_count(buffer_size_, gl::GenBuffers,
         gl::DeleteBuffers);

      std::shared_ptr<std::vector<GLfloat>> data_;

      std::optional<std::vector<GLint>> indices_;
      std::optional<reference_count<GLuint>> ebo_ = indices_ ?
         make_reference_count(1, gl::GenBuffers, gl::DeleteBuffers) :
         std::optional<reference_count<GLuint>>{};

      template <typename F1, typename F2>
      static reference_count<GLuint> make_reference_count(GLuint size, F1 f1, F2 f2) noexcept
      {
         auto result = GLuint{};
         ranges::invoke(f1, size, &result);
         return {result, [size, f2 = std::move(f2)](const auto i) noexcept {
            ranges::invoke(f2, size, i); }};
      }

      void bind_buffer(const GLenum target, const GLenum usage) const noexcept
      {
         gl::BindBuffer(target, vbo_);
         gl::BufferData(target, data_->size() * sizeof(GLfloat), std::data(*data_), usage);
      }

      void interpret(const GLint size, const std::vector<GLint>& interpreter) noexcept
      {
         using ranges::Integral, ranges::SignedIntegral;
         Integral offset = GLsizeiptr{};
         for (Integral i = decltype(interpreter.size()){}; i != interpreter.size(); ++i) {
            const SignedIntegral index = static_cast<GLint>(i);
            const SignedIntegral stride = interpreter[i];
            gl::VertexAttribPointer(index, stride, gl::FLOAT, false, size * sizeof(GLfloat),
               reinterpret_cast<GLvoid*>(offset * sizeof(GLfloat)));
            gl::EnableVertexAttribArray(index);
            offset += stride;
         }
      }

      template <ranges::Invocable F>
      void init(const GLenum target, const GLenum usage, const GLint size,
         const std::vector<GLint>& stride, const F& ebo)
      {
         bind([&, this]{
            bind_buffer(target, usage);
            ranges::invoke(ebo);
            interpret(size, stride);
            unbind(target);
         });
      }

      static void unbind(const GLenum target) noexcept
      {
         gl::BindBuffer(target, 0);
         gl::BindVertexArray(0);
      }
   };
} // namespace doge

#endif // DOGE_GL_VERTEX_ARRAY_HPP
