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
#include "doge/units/angle.hpp"
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
         explicit lighting_impl(vec3 const& position, vec3 const& ambient, vec3 const& diffuse,
            vec3 const& specular, vec3 const& colour = unit<vec3>)
            : position_{position},
              ambient_{colour * ambient},
              diffuse_{colour * diffuse},
              specular_{colour * specular}
         {}

         vec3 const& ambient() const noexcept
         {
            return ambient_;
         }

         void ambient(vec3 const& a)
         {
            ambient_ = a;
         }

         vec3 const& diffuse() const noexcept
         {
            return diffuse_;
         }

         void diffuse(vec3 const& d)
         {
            diffuse_ = d;
         }

         vec3 const& specular() const noexcept
         {
            return specular_;
         }

         void specular(vec3 const& s)
         {
            specular_ = s;
         }
      protected:
         ~lighting_impl() = default;

         vec3 const& position() const noexcept
         {
            return position_;
         }

         void position(vec3 const& p)
         {
            position_ = p;
         }
      private:
         vec3 position_;
         vec3 ambient_;
         vec3 diffuse_;
         vec3 specular_;
      };

      template <>
      class lighting_impl<light_type::point> : public lighting_impl<light_type::directional>
      {
      public:
         explicit lighting_impl(vec3 const& position, vec3 const& ambient, vec3 const& diffuse,
            vec3 const& specular, GLfloat const constant, float const linear, float const quadratic,
            vec3 const& colour = unit<vec3>)
            : lighting_impl<light_type::directional>{position, ambient, diffuse, specular, colour},
              attenuation_constant_{::doge::gl_cast(constant)},
              attenuation_linear_{::doge::gl_cast(linear)},
              attenuation_quadratic_{::doge::gl_cast(quadratic)}
         {}

         float attenuation_constant() const noexcept
         {
            return static_cast<float>(attenuation_constant_);
         }

         void attenuation_constant(float const c)
         {
            attenuation_constant_ = ::doge::gl_cast(c);
         }

         float attenuation_linear() const noexcept
         {
            return static_cast<float>(attenuation_linear_);
         }

         void attenuation_linear(float const c)
         {
            attenuation_linear_ = ::doge::gl_cast(c);
         }

         float attenuation_quadratic() const noexcept
         {
            return static_cast<float>(attenuation_quadratic_);
         }

         void attenuation_quadratic(float const c)
         {
            attenuation_quadratic_ = ::doge::gl_cast(c);
         }
      protected:
         ~lighting_impl() = default;
      private:
         GLfloat attenuation_constant_;
         GLfloat attenuation_linear_;
         GLfloat attenuation_quadratic_;
      };

      template <>
      class lighting_impl<light_type::spot> : public lighting_impl<light_type::point> {
      public:
         explicit lighting_impl(vec3 const& position, vec3 const& ambient, vec3 const& diffuse,
            vec3 const& specular, GLfloat const constant, float const linear, float const quadratic,
            vec3 const& direction, angle const inner_cutoff, angle const outer_cutoff,
            vec3 const& colour = unit<vec3>)
            : lighting_impl<light_type::point>{position, ambient, diffuse, specular, constant,
                 linear, quadratic, colour},
              direction_{direction},
              inner_cutoff_{::doge::cos(inner_cutoff)},
              outer_cutoff_{::doge::cos(outer_cutoff)}
         {}

         vec3 const& direction() const noexcept
         {
            return direction_;
         }

         void direction(vec3 const& d) noexcept
         {
            direction_ = d;
         }

         angle inner_cutoff() const noexcept
         {
            return inner_cutoff_;
         }

         void inner_cutoff(angle const a) noexcept
         {
            inner_cutoff_ = a;
         }

         angle outer_cutoff() const noexcept
         {
            return outer_cutoff_;
         }

         void outer_cutoff(angle const a) noexcept
         {
            outer_cutoff_ = a;
         }
      private:
         vec3 direction_;
         angle inner_cutoff_;
         angle outer_cutoff_;
      };

      template <detail::light_type Light>
      class basic_lighting final : public detail::lighting_impl<Light> {
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
         requires(Light != detail::light_type::point)
         {
            if constexpr (Light == detail::light_type::directional) {
               return detail::lighting_impl<Light>::position();
            }
            else {
               return detail::lighting_impl<Light>::direction();
            }
         }

         void direction(vec3 const& d) noexcept
         requires(Light != detail::light_type::point)
         {
            if constexpr (Light == detail::light_type::directional) {
               detail::lighting_impl<Light>::position(d);
            }
            else {
               detail::lighting_impl<Light>::direction(d);
            }
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
   } // namespace detail

   using directional_lighting = detail::basic_lighting<detail::light_type::directional>;
   using point_lighting = detail::basic_lighting<detail::light_type::point>;
   using spot_lighting = detail::basic_lighting<detail::light_type::spot>;

   template <ranges::Invocable<vec3 const&, vec3 const&, vec3 const&, vec3 const&> F>
   void draw(directional_lighting const& lighting, F const& f)
   {
      ranges::invoke(f, lighting.direction(), lighting.ambient(), lighting.diffuse(),
         lighting.specular());
   }

   template <ranges::Invocable<vec3 const&, vec3 const&, vec3 const&, vec3 const&, float, float,
      float> F>
   void draw(point_lighting const& lighting, F const& f)
   {
      ranges::invoke(f, lighting.position(), lighting.ambient(), lighting.diffuse(),
         lighting.specular(), lighting.attenuation_constant(), lighting.attenuation_linear(),
         lighting.attenuation_quadratic());
   }

   template <ranges::Invocable<vec3 const&, vec3 const&, vec3 const&, vec3 const&, float, float,
      float, vec3 const&, float> F>
   void draw(spot_lighting const& lighting, F const& f)
   {
      ranges::invoke(f, lighting.position(), lighting.ambient(), lighting.diffuse(),
         lighting.specular(), lighting.attenuation_constant(), lighting.attenuation_linear(),
         lighting.attenuation_quadratic(), lighting.direction(), lighting.inner_cutoff(),
         lighting.outer_cutoff());
   }

   template <class T>
   requires requires(T const t) {
      {t.position()};
   }
   auto const& position(T const& t) noexcept
   {
      return t.position();
   }

   inline auto const& position(directional_lighting const& l) noexcept
   {
      return l.direction();
   }
} // namespace doge

#endif // DOGE_ENTITY_lighting_HPP
