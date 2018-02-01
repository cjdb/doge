#include <doge/gl/texture.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace {
   template <doge::texture_t Kind>
   void wrap(const doge::basic_texture<Kind>& tex, const doge::texture_wrap_t s) noexcept
   {
      doge::texture_parameter(tex, gl::TEXTURE_WRAP_S, static_cast<GLint>(s));
   }

   template <doge::texture_t Kind>
   void wrap(const doge::basic_texture<Kind>& tex, const doge::texture_wrap_t s,
      const doge::texture_wrap_t t) noexcept
   {
      ::wrap(tex, s);
      doge::texture_parameter(tex, gl::TEXTURE_WRAP_T, static_cast<GLint>(t));
   }
} // namespace <anonymous>

namespace doge {
   void wrap(const texture1d& tex, const texture_wrap_t s) noexcept
   {
      ::wrap(tex, s);
   }

   void wrap(const texture2d& tex, const texture_wrap_t s, const texture_wrap_t t) noexcept
   {
      ::wrap(tex, s, t);
   }

   void wrap(const texture3d& tex, const texture_wrap_t s, const texture_wrap_t t,
      const texture_wrap_t r) noexcept
   {
      ::wrap(tex, s, t);
      doge::texture_parameter(tex, gl::TEXTURE_WRAP_R, static_cast<GLint>(r));
   }
} // namespace doge
