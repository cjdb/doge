#ifndef DOGE_GL_MEMORY_HPP
#define DOGE_GL_MEMORY_HPP

#include <experimental/ranges/concepts>
#include <experimental/ranges/functional>
#include "gl/gl_core.hpp"

#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Wsign-compare"
#include "unique_resource.h"
#pragma GCC diagnostic pop

namespace doge {
   auto allocate_gl_resource(void(* allocator)(GLsizei, GLuint*), 
                             void(* deleter)  (GLsizei, GLuint const*)) noexcept
   {
      ranges::UnsignedIntegral result = GLuint{};
      ranges::invoke(allocator, 1, &result);
      return std::experimental::unique_resource<GLuint, std::function<void(GLuint)>>(result,
         [deleter](auto const i) mutable noexcept { ranges::invoke(deleter, GLsizei{1}, &i); });
   }
} // namespace doge

#endif // DOGE_GL_MEMORY_HPP
