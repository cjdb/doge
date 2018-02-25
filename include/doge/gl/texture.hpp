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
#ifndef DOGE_GL_TEXTURE_HPP
#define DOGE_GL_TEXTURE_HPP

#include <array>
#include <doge/utility/reference_count.hpp>
#include <doge/utility/type_traits.hpp>
#include <experimental/ranges/algorithm>
#include <experimental/ranges/concepts>
#include <experimental/ranges/iterator>
#include <gl/gl_core.hpp>
#include <gsl/gsl>
#include <memory>
#include <stb_image.h>
#include <tuple>
#include <utility>

namespace doge {
   namespace ranges = std::experimental::ranges;

   enum class texture_t {
      texture_1d = gl::TEXTURE_1D,
      texture_2d = gl::TEXTURE_2D,
      texture_3d = gl::TEXTURE_3D
   };

   template <texture_t Kind>
   class basic_texture;

   template <texture_t Kind>
   void texture_parameter(const basic_texture<Kind>&, GLenum pname, GLfloat param) noexcept
   {
      gl::TexParameterf(static_cast<GLenum>(Kind), pname, param);
   }

   template <texture_t Kind, typename I>
   requires
      (ranges::Integral<I> ||
       std::is_enum_v<I>)
   void texture_parameter(const basic_texture<Kind>&, GLenum pname, I param) noexcept
   {
      Expects(std::numeric_limits<GLint>::min() <= static_cast<GLint>(param));
      Expects(static_cast<GLint>(param) <= std::numeric_limits<GLint>::max());
      gl::TexParameteri(static_cast<GLenum>(Kind), pname, static_cast<GLint>(param));
   }

   template <texture_t Kind, ranges::RandomAccessIterator Rng>
   requires
      requires(Rng rng) {
         ranges::Same<ranges::value_type_t<Rng>, GLfloat>;
         {ranges::data(rng)} -> ranges::value_type_t<Rng>*;
      }
   void texture_parameter(const basic_texture<Kind>&, GLenum pname, const Rng& params) noexcept
   {
      gl::TexParameterfv(static_cast<GLenum>(Kind), pname, ranges::data(params));
   }

   template <texture_t Kind, typename V>
   requires
      detail::type_is_glm_vec<GLfloat, V>
   void texture_parameter(const basic_texture<Kind>&, GLenum pname, const V& params) noexcept
   {
      gl::TexParameterfv(static_cast<GLenum>(Kind), pname, std::addressof(params));
   }

   template <texture_t Kind, ranges::RandomAccessIterator Rng>
   requires
      requires(Rng rng) {
         ranges::Same<ranges::value_type_t<Rng>, GLint>;
         {ranges::data(rng)} -> ranges::value_type_t<Rng>*;
      }
   void texture_parameter(const basic_texture<Kind>&, GLenum pname, const Rng& params) noexcept
   {
      gl::TexParameterIiv(static_cast<GLenum>(Kind), pname, ranges::data(params));
   }

   template <texture_t Kind, typename V>
   requires
      detail::type_is_glm_vec<GLint, V>
   void texture_parameter(const basic_texture<Kind>&, GLenum pname, const V& params) noexcept
   {
      gl::TexParameterIiv(static_cast<GLenum>(Kind), pname, std::addressof(params));
   }

   template <texture_t Kind, ranges::RandomAccessIterator Rng>
   requires requires(Rng rng) {
         ranges::Same<ranges::value_type_t<Rng>, GLuint>;
         {ranges::data(rng)} -> ranges::value_type_t<Rng>*;
   }
   void texture_parameter(const basic_texture<Kind>&, GLenum pname, const Rng& params) noexcept
   {
      gl::TexParameterIuiv(static_cast<GLenum>(Kind), pname, ranges::data(params));
   }

   template <texture_t Kind, typename V>
   requires
      detail::type_is_glm_vec<GLuint, V>
   void texture_parameter(const basic_texture<Kind>&, GLenum pname, const V& params) noexcept
   {
      gl::TexParameterIuiv(static_cast<GLenum>(Kind), pname, std::addressof(params));
   }

   using texture1d = basic_texture<texture_t::texture_1d>;
   using texture2d = basic_texture<texture_t::texture_2d>;
   using texture3d = basic_texture<texture_t::texture_3d>;

   enum class texture_wrap_t {
      clamp_to_edge = gl::CLAMP_TO_EDGE,
      clamp_to_border = gl::CLAMP_TO_BORDER,
      mirrored_repeat = gl::MIRRORED_REPEAT,
      repeat = gl::REPEAT
   };

   /**
    * @brief Sets the wrap parameter for a single-dimensional texture.
    *
    * @param tex
    * @param s Sets the wrap parameter for texture coordinate `s` to one of `gl::CLAMP_TO_EDGE`,
    *        `gl::CLAMP_TO_BORDER`, `gl::MIRRORED_REPEAT`, or `gl::REPEAT`.
    *
    * `doge::texture_wrap_t::clamp_to_edge` causes `s` coordinates to be clamped to the range
    * `[1/(2N), 1 - 1/(2N)]`, where `N` is the size of the texture in the direction you are
    * clamping.
    *
    * `doge::texture_wrap_t::clamp_to_border` evaluates `s` coordinates in a similar way to
    * `doge::texture_wrap_t::clamp_to_edge`. However, in cases where clamping would have occurred in
    * `doge::texture_wrap_t::clamp_to_edge` mode, the fetched texel data is substituted with the
    * values specified by `gl::TEXTURE_BORDER_COLOR`.
    *
    * `doge::texture_wrap_t::repeat` causes the integer part of the s coordinate to be ignored; the
    * GL uses only the fractional part, thereby creating a repeating pattern.
    *
    * `doge::texture_wrap_t::mirrored_repat` causes the `s` coordinate to be set to the fractional
    * part of the texture coordinate if the integer part of s is even; if the integer part of s is
    * odd, then the `s` texture coordinate is set to `1 - frac(s)`, where `frac(s)` represents the
    * fractional part of `s`.
    */
   void wrap(const texture1d& tex, texture_wrap_t) noexcept;

   /**
    * @brief Sets the wrap parameters for a two-dimensional texture.
    * @param tex
    * @param s See the description outlined for doge::wrap(const texture1d&, GLenum) noexcept.
    * @param t Sets the wrap parameter for texture coordinate `t`. See the description outlined for
    *          the texture coordinate `s`.
    * @seealso doge::wrap(const texture1d&, texture_wrap_t) noexcept;
    */
   void wrap(const texture2d& tex, texture_wrap_t, texture_wrap_t) noexcept;

   /**
    * @brief Sets the wrap parameters for a three-dimensional texture.
    * @param tex
    * @param s See the description outlined for doge::wrap(const texture1d&, GLenum) noexcept.
    * @param t Sets the wrap parameter for texture coordinate `t`. See the description outlined for
    *          the texture coordinate `s`.
    * @param r Sets the wrap parameter for texture coordinate `r`. See the description outlined for
    *          the texture coordinate `r`.
    * @seealso doge::wrap(const texture1d&, texture_wrap_t) noexcept;
    */
   void wrap(const texture3d& tex, texture_wrap_t, texture_wrap_t, texture_wrap_t) noexcept;

   enum class minmag_t {
      nearest = gl::NEAREST,
      linear = gl::LINEAR,
      nearest_mipmap_nearest = gl::NEAREST_MIPMAP_NEAREST,
      nearest_mipmap_linear = gl::NEAREST_MIPMAP_LINEAR,
      linear_mipmap_nearest = gl::LINEAR_MIPMAP_NEAREST,
      linear_mipmap_linear = gl::LINEAR_MIPMAP_LINEAR
   };

   /**
    * 
    *
    *
    *
    *
    */
   template <texture_t Kind>
   void min_filter(const basic_texture<Kind>& tex, const minmag_t filter) noexcept
   {
      doge::texture_parameter(tex, gl::TEXTURE_MIN_FILTER, static_cast<GLint>(filter));
   }

   /**
    * 
    *
    *
    *
    *
    */
   template <texture_t Kind>
   void mag_filter(const basic_texture<Kind>& tex, const minmag_t filter) noexcept
   {
      doge::texture_parameter(tex, gl::TEXTURE_MAG_FILTER, static_cast<GLint>(filter));
   }

   template <texture_t Kind>
   basic_texture<Kind> make_texture_map(std::string const& texture_path)
   {
      return {texture_path, {doge::texture_wrap_t::repeat, doge::texture_wrap_t::repeat},
         doge::minmag_t::linear, doge::minmag_t::linear};
   }

   template <texture_t Kind>
   class basic_texture {
      using wrapping_t =
         std::conditional_t<Kind == texture_t::texture_1d, std::tuple<texture_wrap_t>,
         std::conditional_t<Kind == texture_t::texture_2d, std::tuple<texture_wrap_t, texture_wrap_t>,
            std::tuple<texture_wrap_t, texture_wrap_t, texture_wrap_t>>>;
   public:
      static constexpr auto texture_type = Kind;

      basic_texture(const std::string_view path, const wrapping_t& wrapping, minmag_t min_filter,
         minmag_t mag_filter, int n = 0)
         : index_{
               [this]{
                 ranges::Integral object_ = GLuint{};
                 gl::GenTextures(size_, &object_);
                 return object_;
              }(),
              [this](GLuint* i) noexcept { gl::DeleteTextures(size_, i); }
           }
      {
         ranges::SignedIntegral width = 0;
         ranges::SignedIntegral height = 0;
         ranges::SignedIntegral channels = 0;
         stbi_set_flip_vertically_on_load(true);
         auto data = std::unique_ptr<unsigned char, void(*)(void*)>{
            stbi_load(path.data(), &width, &height, &channels, 0), stbi_image_free};

         using namespace std::string_literals;
         if (not data)
            throw std::runtime_error{"Unable to open texture "s + path.data()};

         GLenum format = channels == 1 ? gl::RED
                       : channels == 3 ? gl::RGB : gl::RGBA;

         bind(gl::TEXTURE0 + n);
         
         gl::TexImage2D(static_cast<GLenum>(Kind), 0, format, width, height, 0, format,
            gl::UNSIGNED_BYTE, data.get());
         gl::GenerateMipmap(static_cast<GLenum>(Kind));

         std::apply([this](auto&&... args) noexcept {
            doge::wrap(*this, std::forward<decltype(args)>(args)...); }, wrapping);

         doge::min_filter(*this, min_filter);
         doge::mag_filter(*this, mag_filter);
      }

      void bind(const GLenum active_texture) const noexcept
      {
         Expects(gl::TEXTURE0 <= active_texture && active_texture <= gl::TEXTURE16);
         gl::ActiveTexture(active_texture);
         gl::BindTexture(static_cast<GLenum>(Kind), index_);
      }

      template <ranges::Invocable F>
      void bind(const GLenum active_texture, F f) const noexcept
      {
         bind(active_texture);
         ranges::invoke(f);
      }
   private:
      static constexpr GLuint size_ = 1;
      //GLuint object_;
      reference_count<GLuint> index_;
   };
} // namespace doge

#endif // DOGE_GL_TEXTURE_HPP
