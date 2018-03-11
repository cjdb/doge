//
//  Copyright 2018 Christopher Di Bella
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
#ifndef DOGE_ENTITY_lighting_HPP
#define DOGE_ENTITY_lighting_HPP

#include "doge/gl.hpp"
#include "doge/types.hpp"
#include <gsl/gsl>
#include <variant>

namespace doge {
   namespace ranges = std::experimental::ranges;


   namespace detail {
      template <class T>
      using uniform_type_info = std::pair<std::string_view, T>;

      enum class light_type { directional, point, spot };

      template <light_type> struct lighting_impl;

      template <>
      class lighting_impl<light_type::directional> {
         explicit lighting_impl(shader_binary const& program,
            uniform_type_info<vec3> const& position, uniform_type_info<vec3> const& ambient,
            uniform_type_info<vec3> const& diffuse, uniform_type_info<vec3> const& specular,
            vec3 const& colour = unit<vec3>)
            : position_{program, position.first, position.second},
              ambient_{program, ambient.first, colour * ambient.second},
              diffuse_{program, diffuse.first, colour * diffuse.second},
              specular_{program, specular.first, colour * specular.second}
         {}

         vec3 const& ambient() const noexcept
         {
            return ambient_.get();
         }

         void ambient(vec3 const& a)
         {
            ambient_ = a;
         }

         vec3 const& diffuse() const noexcept
         {
            return diffuse_.get();
         }

         void diffuse(vec3 const& d)
         {
            diffuse_ = d;
         }

         vec3 const& specular() const noexcept
         {
            return specular_.get();
         }

         void specular(vec3 const& s)
         {
            specular_ = s;
         }

         lighting_impl& operator=(lighting_impl const& light) noexcept
         {
            position_ = light.position();
            ambient_ = light.ambient();
            diffuse_ = light.diffuse();
            specular_ = light.specular();
            return *this;
         }
      protected:
         ~lighting_impl() = default;

         vec3 const& position() const noexcept
         {
            return position_.get();
         }

         void position(vec3 const& p)
         {
            position_ = p;
         }
      private:
         uniform<vec3> position_;
         uniform<vec3> ambient_;
         uniform<vec3> diffuse_;
         uniform<vec3> specular_;
      };

      template <>
      class lighting_impl<light_type::point> : public lighting_impl<light_type::directional>
      {
      public:
         explicit lighting_impl(shader_binary const& program,
            uniform_type_info<vec3> const& position, uniform_type_info<vec3> const& ambient,
            uniform_type_info<vec3> const& diffuse, uniform_type_info<vec3> const& specular,
            uniform_type_info<GLfloat> const constant, uniform_type_info<GLfloat> const linear,
            uniform_type_info<GLfloat> const quadratic, vec3 const& colour = unit<vec3>)
            : lighting_impl<light_type::directional>{program, position, ambient, diffuse,
                 specular, colour},
              attenuation_constant_{program, constant.first, constant.second},
              attenuation_linear_{program, linear.first, linear.second},
              attenuation_quadratic_{program, quadratic.first, quadratic.second}
         {}

         lighting_impl& operator=(lighting_impl const& l)
         {
            lighting_impl<light_type::directional>::operator=(l);
            attenuation_constant_ = l.attenuation_constant_;
            attenuation_linear_ = l.attenuation_linear_;
            attenuation_quadratic_ = l.attenuation_quadratic_;
            return *this;
         }

         float attenuation_constant() const noexcept
         {
            return static_cast<float>(attenuation_constant_.get());
         }

         void attenuation_constant(float const c)
         {
            attenuation_constant_ = gl_cast(c);
         }

         float attenuation_linear() const noexcept
         {
            return static_cast<float>(attenuation_linear_.get());
         }

         void attenuation_linear(float const c)
         {
            attenuation_linear_ = gl_cast(c);
         }

         float attenuation_quadratic() const noexcept
         {
            return static_cast<float>(attenuation_quadratic_.get());
         }

         void attenuation_quadratic(float const c)
         {
            attenuation_quadratic_ = gl_cast(c);
         }
      protected:
         ~lighting_impl() = default;
      private:
         uniform<GLfloat> attenuation_constant_;
         uniform<GLfloat> attenuation_linear_;
         uniform<GLfloat> attenuation_quadratic_;
      };

      template <>
      struct lighting_impl<light_type::spot> : lighting_impl<light_type::point> {};
   } // namespace detail

   template <detail::light_type Light>
   class basic_lighting final : detail::lighting_impl<Light> {
   public:
      using detail::lighting_impl<Light>::lighting_impl;

      basic_lighting& operator=(vec3 const& p) noexcept
      {
         position(p);
         return *this;
      }

      basic_lighting& operator+=(vec3 const& delta) noexcept
      {
         position(position() + delta);
         return *this;
      }

      basic_lighting& operator-=(vec3 const& delta) noexcept
      {
         position(position() - delta);
         return *this;
      }

      basic_lighting& operator*=(float const scalar) noexcept
      {
         position(position() * scalar);
         return *this;
      }

      basic_lighting& operator/=(float const scalar) noexcept
      {
         Expects(scalar != 0.0f);
         position(position() / scalar);
         return *this;
      }

      vec3 const& direction() const noexcept
      requires(Light == detail::light_type::directional)
      {
         return position();
      }

      void direction(vec3 const& d) noexcept
      requires(Light == detail::light_type::directional)
      {
         return position(d);
      }

      vec3 const& position() const noexcept
      {
         return detail::lighting_impl<Light>::position();
      }

      void position(vec3 const& d) noexcept
      {
         detail::lighting_impl<Light>::position(d);
      }
   };

   using directional_lighting = basic_lighting<detail::light_type::directional>;
   using point_lighting = basic_lighting<detail::light_type::point>;
   using spot_lighting = basic_lighting<detail::light_type::spot>;

   template <ranges::Invocable<vec3 const&, vec3 const&, vec3 const&, vec3 const&> F>
   void draw(F const& f, directional_lighting const& lighting)
   {
      ranges::invoke(f, lighting.direction(), lighting.ambient(), lighting.diffuse(),
         lighting.specular());
   }
} // namespace doge

#endif // DOGE_ENTITY_lighting_HPP
