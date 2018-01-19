#ifndef DOGE_GL_UNIFORM_HPP
#define DOGE_GL_UNIFORM_HPP

#include "doge/gl/cast.hpp"
#include "doge/gl/shader_binary.hpp"
#include <gl/gl_core.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat2x2.hpp>
#include <glm/mat2x3.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat3x2.hpp>
#include <glm/mat3x4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/mat4x2.hpp>
#include <glm/mat4x3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <gsl/gsl>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <utility>

namespace doge {
   namespace ranges = std::experimental::ranges;

   struct uniform_not_found : public std::runtime_error {
      uniform_not_found(const std::string_view id)
         : std::runtime_error{id.data()}
      {}
   };

   GLint uniform(const shader_binary& program, const std::string_view id)
   {
      if (ranges::SignedIntegral i = gl::GetUniformLocation(program.index(), id.data()); i >= 0)
         return i;
      throw uniform_not_found{id};
   }

   namespace detail {
      template <typename F, typename... Args>
      requires
         ranges::Invocable<F, GLint, Args...>
      void uniform_impl(const GLint location, const F& f, Args&&... args) noexcept
      {
         Expects(location >= 0);
         ranges::invoke(f, location, std::forward<Args>(args)...);
      }

      template <typename F, typename... Args>
      requires
         ranges::Invocable<F, GLint, Args...>
      void uniform_impl(const shader_binary& program, const std::string_view id, const F& f,
         Args&&... args)
      {
         uniform_impl(uniform(program, id), f, std::forward<Args>(args)...);
      }
   } // namespace detail

   void uniform(const shader_binary& program, const std::string_view id, const GLfloat v)
   {
      doge::detail::uniform_impl(program, id, gl::Uniform1f, v);
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, GLfloat>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const shader_binary& program, const std::string_view id, const Rng& rng)
   {
      doge::detail::uniform_impl(program, id, gl::Uniform1fv, 1, std::addressof(rng[0]));
   }

   void uniform(const shader_binary& program, const std::string_view id, const glm::vec2 v)
   {
      doge::detail::uniform_impl(program, id, gl::Uniform2f, v.x, v.y);
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, glm::vec2>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const shader_binary& program, const std::string_view id, const Rng& rng)
   {
      doge::detail::uniform_impl(program, id, gl::Uniform2fv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), std::addressof(rng[0][0]));
   }

   void uniform(const shader_binary& program, const std::string_view id, const glm::vec3 v)
   {
      doge::detail::uniform_impl(program, id, gl::Uniform3f, v.x, v.y, v.z);
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, glm::vec3>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const shader_binary& program, const std::string_view id, const Rng& rng)
   {
      doge::detail::uniform_impl(program, id, gl::Uniform3fv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), std::addressof(rng[0][0]));
   }

   void uniform(const shader_binary& program, const std::string_view id, const glm::vec4& v)
   {
      doge::detail::uniform_impl(program, id, gl::Uniform4f, v.x, v.y, v.z, v.w);
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, glm::vec4>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const shader_binary& program, const std::string_view id, const Rng& rng)
   {
      doge::detail::uniform_impl(program, id, gl::Uniform4fv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), std::addressof(rng[0][0]));
   }

   void uniform(const shader_binary& program, const std::string_view id, const GLint i)
   {
      doge::detail::uniform_impl(program, id, gl::Uniform1i, i);
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, GLint>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const shader_binary& program, const std::string_view id, const Rng& rng)
   {
      doge::detail::uniform_impl(program, id, gl::Uniform1iv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), std::addressof(rng[0][0]));
   }

   void uniform(const shader_binary& program, const std::string_view id, const glm::ivec2 v)
   {
      doge::detail::uniform_impl(program, id, gl::Uniform2i, v.x, v.y);
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, glm::ivec2>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const shader_binary& program, const std::string_view id, const Rng& rng)
   {
      doge::detail::uniform_impl(program, id, gl::Uniform2iv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), std::addressof(rng[0][0]));
   }

   void uniform(const shader_binary& program, const std::string_view id, const glm::ivec3 v)
   {
      doge::detail::uniform_impl(program, id, gl::Uniform3i, v.x, v.y, v.z);
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, glm::ivec3>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const shader_binary& program, const std::string_view id, const Rng& rng)
   {
      doge::detail::uniform_impl(program, id, gl::Uniform3iv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), std::addressof(rng[0][0]));
   }

   void uniform(const shader_binary& program, const std::string_view id, const glm::ivec4 v)
   {
      doge::detail::uniform_impl(program, id, gl::Uniform4i, v.x, v.y, v.z, v.w);
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, glm::ivec4>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const shader_binary& program, const std::string_view id, const Rng& rng)
   {
      doge::detail::uniform_impl(program, id, gl::Uniform4iv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), std::addressof(rng[0][0]));
   }

   void uniform(const shader_binary& program, const std::string_view id, const GLuint v)
   {
      doge::detail::uniform_impl(program, id, gl::Uniform1ui, v);
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, GLuint>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const shader_binary& program, const std::string_view id, const Rng& rng)
   {
      doge::detail::uniform_impl(program, id, gl::Uniform1uiv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), std::addressof(rng[0][0]));
   }

   void uniform(const shader_binary& program, const std::string_view id, const glm::uvec2 v)
   {
      doge::detail::uniform_impl(program, id, gl::Uniform2ui, v.x, v.y);
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, glm::uvec2>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const shader_binary& program, const std::string_view id, const Rng& rng)
   {
      doge::detail::uniform_impl(program, id, gl::Uniform2uiv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), std::addressof(rng[0][0]));
   }

   void uniform(const shader_binary& program, const std::string_view id, const glm::uvec3 v)
   {
      doge::detail::uniform_impl(program, id, gl::Uniform3ui, v.x, v.y, v.z);
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, glm::uvec3>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const shader_binary& program, const std::string_view id, const Rng& rng)
   {
      doge::detail::uniform_impl(program, id, gl::Uniform3uiv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), std::addressof(rng[0][0]));
   }

   void uniform(const shader_binary& program, const std::string_view id, const glm::uvec4 v)
   {
      doge::detail::uniform_impl(program, id, gl::Uniform4ui, v.x, v.y, v.z, v.w);
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, glm::uvec4>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const shader_binary& program, const std::string_view id, const Rng& rng)
   {
      doge::detail::uniform_impl(program, id, gl::Uniform4uiv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), std::addressof(rng[0][0]));
   }

   void uniform(const shader_binary& program, const std::string_view id, const bool transpose,
      const glm::mat2& m)
   {
      doge::detail::uniform_impl(program, id, gl::UniformMatrix2fv, 1, transpose,
         glm::value_ptr(m));
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, glm::mat2>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const shader_binary& program, const std::string_view id, const bool transpose,
      const Rng& rng)
   {
      doge::detail::uniform_impl(program, id, gl::UniformMatrix2fv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), transpose, std::addressof(rng[0][0][0]));
   }

   void uniform(const shader_binary& program, const std::string_view id, const bool transpose,
      const glm::mat3& m)
   {
      doge::detail::uniform_impl(program, id, gl::UniformMatrix3fv, 1, transpose,
         glm::value_ptr(m));
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, glm::mat3>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const shader_binary& program, const std::string_view id, const bool transpose,
      const Rng& rng)
   {
      doge::detail::uniform_impl(program, id, gl::UniformMatrix3fv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), transpose, std::addressof(rng[0][0][0]));
   }

   void uniform(const shader_binary& program, const std::string_view id, const bool transpose,
      const glm::mat4& m)
   {
      doge::detail::uniform_impl(program, id, gl::UniformMatrix4fv, 1, transpose,
         glm::value_ptr(m));
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, glm::mat4>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const shader_binary& program, const std::string_view id, const bool transpose,
      const Rng& rng)
   {
      doge::detail::uniform_impl(program, id, gl::UniformMatrix4fv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), transpose, std::addressof(rng[0][0][0]));
   }

   void uniform(const shader_binary& program, const std::string_view id, const bool transpose,
      const glm::mat2x3& m)
   {
      doge::detail::uniform_impl(program, id, gl::UniformMatrix2x3fv, 1, transpose,
         glm::value_ptr(m));
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, glm::mat2x3>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const shader_binary& program, const std::string_view id, const bool transpose,
      const Rng& rng)
   {
      doge::detail::uniform_impl(program, id, gl::UniformMatrix2x3fv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), transpose, std::addressof(rng[0][0][0]));
   }

   void uniform(const shader_binary& program, const std::string_view id, const bool transpose,
      const glm::mat3x2& m)
   {
      doge::detail::uniform_impl(program, id, gl::UniformMatrix3x2fv, 1, transpose,
         glm::value_ptr(m));
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, glm::mat3x2>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const shader_binary& program, const std::string_view id, const bool transpose,
      const Rng& rng)
   {
      doge::detail::uniform_impl(program, id, gl::UniformMatrix3x2fv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), transpose, std::addressof(rng[0][0][0]));
   }

   void uniform(const shader_binary& program, const std::string_view id, const bool transpose,
      const glm::mat4x2& m)
   {
      doge::detail::uniform_impl(program, id, gl::UniformMatrix4x2fv, 1, transpose,
         glm::value_ptr(m));
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, glm::mat4x2>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const shader_binary& program, const std::string_view id, const bool transpose,
      const Rng& rng)
   {
      doge::detail::uniform_impl(program, id, gl::UniformMatrix4x2fv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), transpose, std::addressof(rng[0][0][0]));
   }

   void uniform(const shader_binary& program, const std::string_view id, const bool transpose,
      const glm::mat2x4& m)
   {
      doge::detail::uniform_impl(program, id, gl::UniformMatrix2x4fv, 1, transpose,
         glm::value_ptr(m));
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, glm::mat2x4>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const shader_binary& program, const std::string_view id, const bool transpose,
      const Rng& rng)
   {
      doge::detail::uniform_impl(program, id, gl::UniformMatrix2x4fv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), transpose, std::addressof(rng[0][0][0]));
   }

   void uniform(const shader_binary& program, const std::string_view id, const bool transpose,
      const glm::mat4x3& m)
   {
      doge::detail::uniform_impl(program, id, gl::UniformMatrix4x3fv, 1, transpose,
         glm::value_ptr(m));
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, glm::mat4x3>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const shader_binary& program, const std::string_view id, const bool transpose,
      const Rng& rng)
   {
      doge::detail::uniform_impl(program, id, gl::UniformMatrix4x3fv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), transpose, std::addressof(rng[0][0][0]));
   }

   void uniform(const shader_binary& program, const std::string_view id, const bool transpose,
      const glm::mat3x4& m)
   {
      doge::detail::uniform_impl(program, id, gl::UniformMatrix3x4fv, 1, transpose,
         glm::value_ptr(m));
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, glm::mat3x4>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const shader_binary& program, const std::string_view id, const bool transpose,
      const Rng& rng)
   {
      doge::detail::uniform_impl(program, id, gl::UniformMatrix3x4fv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), transpose, std::addressof(rng[0][0][0]));
   }

   void uniform(const GLint location, const GLfloat v) noexcept
   {
      doge::detail::uniform_impl(location, gl::Uniform1f, v);
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, GLfloat>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const GLint location, const Rng& rng) noexcept
   {
      doge::detail::uniform_impl(location, gl::Uniform1fv, ranges::size(rng),
         std::addressof(rng[0]));
   }

   void uniform(const GLint location, const glm::vec2 v) noexcept
   {
      doge::detail::uniform_impl(location, gl::Uniform2f, v.x, v.y);
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, glm::vec2>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const GLint location, const Rng& rng) noexcept
   {
      doge::detail::uniform_impl(location, gl::Uniform2fv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), std::addressof(rng[0][0]));
   }

   void uniform(const GLint location, const glm::vec3 v) noexcept
   {
      doge::detail::uniform_impl(location, gl::Uniform3f, v.x, v.y, v.z);
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, glm::vec3>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const GLint location, const Rng& rng) noexcept
   {
      doge::detail::uniform_impl(location, gl::Uniform3fv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), std::addressof(rng[0][0]));
   }

   void uniform(const GLint location, const glm::vec4 v) noexcept
   {
      doge::detail::uniform_impl(location, gl::Uniform4f, v.x, v.y, v.z, v.w);
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, glm::vec4>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const GLint location, const Rng& rng) noexcept
   {
      doge::detail::uniform_impl(location, gl::Uniform4fv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), std::addressof(rng[0][0]));
   }

   void uniform(const GLint location, const GLint i) noexcept
   {
      doge::detail::uniform_impl(location, gl::Uniform1i, i);
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, GLint>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const GLint location, const Rng& rng) noexcept
   {
      doge::detail::uniform_impl(location, gl::Uniform1iv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), std::addressof(rng[0]));
   }

   void uniform(const GLint location, const glm::ivec2 v) noexcept
   {
      doge::detail::uniform_impl(location, gl::Uniform2i, v.x, v.y);
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, glm::ivec2>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const GLint location, const Rng& rng) noexcept
   {
      doge::detail::uniform_impl(location, gl::Uniform2iv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), std::addressof(rng[0][0]));
   }

   void uniform(const GLint location, const glm::ivec3 v) noexcept
   {
      doge::detail::uniform_impl(location, gl::Uniform3i, v.x, v.y, v.z);
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, glm::ivec3>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const GLint location, const Rng& rng) noexcept
   {
      doge::detail::uniform_impl(location, gl::Uniform3iv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), std::addressof(rng[0][0]));
   }

   void uniform(const GLint location, const glm::ivec4 v) noexcept
   {
      doge::detail::uniform_impl(location, gl::Uniform4i, v.x, v.y, v.z, v.w);
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, glm::ivec4>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const GLint location, const Rng& rng) noexcept
   {
      doge::detail::uniform_impl(location, gl::Uniform4iv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), std::addressof(rng[0][0]));
   }

   void uniform(const GLint location, const GLuint v) noexcept
   {
      doge::detail::uniform_impl(location, gl::Uniform1ui, v);
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, GLuint>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const GLint location, const Rng& rng) noexcept
   {
      doge::detail::uniform_impl(location, gl::Uniform1uiv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), std::addressof(rng[0]));
   }

   void uniform(const GLint location, const glm::uvec2 v) noexcept
   {
      doge::detail::uniform_impl(location, gl::Uniform2ui, v.x, v.y);
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, glm::uvec2>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const GLint location, const Rng& rng) noexcept
   {
      doge::detail::uniform_impl(location, gl::Uniform2uiv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), std::addressof(rng[0][0]));
   }

   void uniform(const GLint location, const glm::uvec3 v) noexcept
   {
      doge::detail::uniform_impl(location, gl::Uniform3ui, v.x, v.y, v.z);
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, glm::uvec3>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const GLint location, const Rng& rng) noexcept
   {
      doge::detail::uniform_impl(location, gl::Uniform3uiv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), std::addressof(rng[0][0]));
   }

   void uniform(const GLint location, const glm::uvec4 v) noexcept
   {
      doge::detail::uniform_impl(location, gl::Uniform4ui, v.x, v.y, v.z, v.w);
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, glm::uvec4>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const GLint location, const Rng& rng) noexcept
   {
      doge::detail::uniform_impl(location, gl::Uniform4uiv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), std::addressof(rng[0][0]));
   }

   void uniform(const GLint location, const bool transpose, const glm::mat2& m) noexcept
   {
      doge::detail::uniform_impl(location, gl::UniformMatrix2fv, 1, transpose, glm::value_ptr(m));
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, glm::mat2>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const GLint location, const bool transpose, const Rng& rng) noexcept
   {
      doge::detail::uniform_impl(location, gl::UniformMatrix2fv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), transpose, std::addressof(rng[0][0][0]));
   }

   void uniform(const GLint location, const bool transpose, const glm::mat3& m) noexcept
   {
      doge::detail::uniform_impl(location, gl::UniformMatrix3fv, 1, transpose, glm::value_ptr(m));
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, glm::mat3>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const GLint location, const bool transpose, const Rng& rng) noexcept
   {
      doge::detail::uniform_impl(location, gl::UniformMatrix3fv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), transpose, std::addressof(rng[0][0][0]));
   }

   void uniform(const GLint location, const bool transpose, const glm::mat4& m) noexcept
   {
      doge::detail::uniform_impl(location, gl::UniformMatrix4fv, 1, transpose, glm::value_ptr(m));
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, glm::mat4>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const GLint location, const bool transpose, const Rng& rng) noexcept
   {
      doge::detail::uniform_impl(location, gl::UniformMatrix4fv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), transpose, std::addressof(rng[0][0][0]));
   }

   void uniform(const GLint location, const bool transpose, const glm::mat2x3& m) noexcept
   {
      doge::detail::uniform_impl(location, gl::UniformMatrix2x3fv, 1, transpose, glm::value_ptr(m));
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, glm::mat2x3>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const GLint location, const bool transpose, const Rng& rng) noexcept
   {
      doge::detail::uniform_impl(location, gl::UniformMatrix2x3fv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), transpose, std::addressof(rng[0][0][0]));
   }

   void uniform(const GLint location, const bool transpose, const glm::mat3x2& m) noexcept
   {
      doge::detail::uniform_impl(location, gl::UniformMatrix3x2fv, 1, transpose, glm::value_ptr(m));
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, glm::mat3x2>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const GLint location, const bool transpose, const Rng& rng) noexcept
   {
      doge::detail::uniform_impl(location, gl::UniformMatrix3x2fv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), transpose, std::addressof(rng[0][0][0]));
   }

   void uniform(const GLint location, const bool transpose, const glm::mat4x2& m) noexcept
   {
      doge::detail::uniform_impl(location, gl::UniformMatrix4x2fv, 1, transpose, glm::value_ptr(m));
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, glm::mat4x2>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const GLint location, const bool transpose, const Rng& rng) noexcept
   {
      doge::detail::uniform_impl(location, gl::UniformMatrix4x2fv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), transpose, std::addressof(rng[0][0][0]));
   }

   void uniform(const GLint location, const bool transpose, const glm::mat2x4& m) noexcept
   {
      doge::detail::uniform_impl(location, gl::UniformMatrix2x4fv, 1, transpose, glm::value_ptr(m));
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, glm::mat2x4>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const GLint location, const bool transpose, const Rng& rng) noexcept
   {
      doge::detail::uniform_impl(location, gl::UniformMatrix2x4fv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), transpose, std::addressof(rng[0][0][0]));
   }

   void uniform(const GLint location, const bool transpose, const glm::mat4x3& m) noexcept
   {
      doge::detail::uniform_impl(location, gl::UniformMatrix4x3fv, 1, transpose, glm::value_ptr(m));
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, glm::mat4x3>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const GLint location, const bool transpose, const Rng& rng) noexcept
   {
      doge::detail::uniform_impl(location, gl::UniformMatrix4x3fv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), transpose, std::addressof(rng[0][0][0]));
   }

   void uniform(const GLint location, const bool transpose, const glm::mat3x4& m) noexcept
   {
      doge::detail::uniform_impl(location, gl::UniformMatrix3x4fv, 1, transpose, glm::value_ptr(m));
   }

   template <ranges::RandomAccessRange Rng>
   requires requires(const Rng rng) {
      ranges::Same<ranges::value_type_t<Rng>, glm::mat3x4>;
      {rng[0]} -> const ranges::value_type_t<Rng>&;
   }
   void uniform(const GLint location, const bool transpose, const Rng& rng) noexcept
   {
      doge::detail::uniform_impl(location, gl::UniformMatrix3x4fv,
         gsl::narrow_cast<GLsizei>(ranges::size(rng)), transpose, std::addressof(rng[0][0][0]));
   }
} // namespace doge

#endif // DOGE_GL_UNIFORM_HPP
