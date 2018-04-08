#ifndef DOGE_GL_MEMORY_HPP
#define DOGE_GL_MEMORY_HPP

#include <array>
#include <experimental/ranges/concepts>
#include <experimental/ranges/functional>
#include <functional>
#include "gl/gl_core.hpp"

#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Wsign-compare"
#include "unique_resource.h"
#pragma GCC diagnostic pop

namespace doge {
   namespace ranges = std::experimental::ranges;
   enum class resource_type { buffer, framebuffer, vertex_array };

   template <resource_type T, int N = 1>
   requires
      N > 0
   class gpu_ptr {
      using unique_resource = std::experimental::unique_resource<std::array<GLuint, N>,
         std::function<void(std::array<GLuint, N> const&)>>;
   public:
      GLuint operator[](int const i) const noexcept
      requires
         N > 1
      {
         Expects(i >= 0);
         Expects(i < N);
         return get()[i];
      }

      GLuint operator*() const noexcept
      requires
         N == 1
      {
         return get()[0];
      }

      auto const& get() const noexcept
      {
         return resource_.get();
      }
   private:
      unique_resource resource_ = []{
         auto const [allocator, deleter] = gpu_ptr_allocator();
         ranges::Regular result = std::array<GLuint, N>{};
         allocator(ranges::size(result), ranges::data(result));

         return unique_resource(std::move(result),
            [deleter](ranges::RandomAccessRange const& resource) noexcept {
               deleter(ranges::size(resource), ranges::data(resource));
         });
      }();

      static auto gpu_ptr_allocator() noexcept
      requires
         T == resource_type::buffer
      {
         return std::make_pair(gl::GenBuffers, gl::DeleteBuffers);
      }

      static auto gpu_ptr_allocator() noexcept
      requires
         T == resource_type::framebuffer
      {
         return std::make_pair(gl::GenFramebuffers, gl::DeleteFramebuffers);
      }

      static auto gpu_ptr_allocator() noexcept
      requires
         T == resource_type::vertex_array
      {
         return std::make_pair(gl::GenVertexArrays, gl::DeleteVertexArrays);
      }
   };
} // namespace doge

#endif // DOGE_GL_MEMORY_HPP
