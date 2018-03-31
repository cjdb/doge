#ifndef DOGE_GL_VERTEX_HPP
#define DOGE_GL_VERTEX_HPP

#include "doge/type_traits.hpp"

namespace doge {
   namespace gpu {
      enum class object_type { array, buffer, element_buffer, framebuffer, uniform_buffer };

      template <object_type>
      class object {
      public:
         using create_t = (void)(*)(GLsizei, GLuint*) noexcept;
         using destroy_t = (void)(*)(GLsizei, GLuint const*) noexcept;

         gpu_object(create_t create, destroy_t destroy) noexcept
            : impl_{create, destroy}
         {}

         object(object const&) = delete;
         object& operator=(object const&) = delete;

         GLuint get() const noexcept
         {
            return impl_->get();
         }

         explicit operator GLuint() const noexcept
         {
            return get();
         }
      private:
         class impl {
            static constexpr ranges::UnsignedIntegral capacity = 1;
         public:
            impl(create_t create, destroy_t destroy) noexcept
               : buffer_{[]() noexcept {
                     ranges::UnsignedIntegral result = GLuint{};
                     ranges::invoke(create, capacity, &result);
                     return result;
                  }()},
               destroy_{destroy}
            {}

            GLuint get() const noexcept
            {
               return buffer_;
            }

            ~buffer_instance()
            {
               ranges::invoke(destroy_, capacity, &buffer_);
            }
         private:
            GLuint buffer_;
            F2 destroy_;
         };

         std::optional<impl> impl_;
      };

      using vertex_array = object<object_type::array>;
      using vertex_buffer = object<object_type::buffer>;
      using element_buffer = object<object_type::element_buffer>;
      using framebuffer = object<object_type::framebuffer>
      using uniform_buffer = object<object_type::uniform_buffer>;

      auto make_vertex_buffer()
      {
         return vertex_buffer{gl::GenBuffers, gl::DeleteBuffers};
      }

      auto make_array_buffer()
      {
         return array_buffer{gl::GenVertexArrays, gl::DeleteVertexArrays};
      }
   } // namespace gpu

   template <typename... Ts>
   requires
      (StandardLayout<Ts> && ...)
   class vertex_array {
   public:
      vertex_array(gsl::span<std_layout_tuple<Ts...>> const data)
      {
         auto buffer_binding = vbo_->bind();
         vertex_attrib_pointer(std::index_sequence_for<Ts...>{},
            meta::partial_sum<sizeof(Ts)...>{});
      }
   private:
      gpu::vertex_buffer vbo_ = ::doge::gpu::make_vertex_buffer();
      gpu::vertex_array vao_ = ::doge::gpu::make_array_buffer();

      template <std::size_t... Index, std::size_t Stride>
      void vertex_attrib_pointer(std::index_sequence<Index...>)
      {
         ((gl::VertexAttribPointer(Index, sizeof(Ts) / sizeof(underlying_type<Ts>), glsl_type<Ts>,
              PartialSum),
           gl::EnableVertexAttribArray(Index))...);
      }
   };
} // namespace doge

#endif // DOGE_GL_VERTEX_HPP
