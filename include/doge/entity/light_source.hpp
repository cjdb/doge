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
#ifndef DOGE_ENTITY_LIGHT_SOURCE_HPP
#define DOGE_ENTITY_LIGHT_SOURCE_HPP

#include "doge/gl.hpp"
#include "doge/types.hpp"
#include <gsl/gsl>
#include <variant>

namespace doge {
   namespace ranges = std::experimental::ranges;

   template <typename Light>
   class basic_light_source {
   public:
      basic_light_source(shader_binary const& program,
         std::pair<std::string_view, vec3> const& position,
         std::pair<std::string_view, vec3> const& ambient,
         std::pair<std::string_view, vec3> const& diffuse,
         std::pair<std::string_view, vec3> const& specular,
         vec3 const& colour = unit<vec3>)
         : position_{program, position.first, position.second},
           ambient_{program, ambient.first, colour * ambient.second},
           diffuse_{program, diffuse.first, colour * diffuse.second},
           specular_{program, specular.first, colour * specular.second}
      {}

      Light& operator+=(vec3 const& delta) noexcept
      {
         position_ += delta;
         return static_cast<Light&>(*this);
      }

      Light& operator-=(vec3 const& delta) noexcept
      {
         position_ -= delta;
         return static_cast<Light&>(*this);
      }

      Light& operator*=(float const scalar) noexcept
      {
         position_ *= scalar;
         return static_cast<Light&>(*this);
      }

      Light& operator/=(float const scalar) noexcept
      {
         Expects(scalar != 0.0f);
         position_ /= scalar;
         return static_cast<Light&>(*this);
      }

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
   protected:
      ~basic_light_source() = default;

      vec3 const& position() const noexcept
      {
         return position_.get();
      }

      void position(vec3 const& p)
      {
         position_ = p;
      }

      void assign(Light const& light) noexcept
      {
         position_ = light.position();
         ambient_ = light.ambient();
         diffuse_ = light.diffuse();
         specular_ = light.specular();
      }

      void assign(vec3 const& position) noexcept
      {
         position_ = position;
      }
   private:
      uniform<vec3> position_;
      uniform<vec3> ambient_;
      uniform<vec3> diffuse_;
      uniform<vec3> specular_;
   };

   class directional_light final : public basic_light_source<directional_light> {
   public:
      using basic_light_source<directional_light>::basic_light_source;
      using basic_light_source<directional_light>::operator+=;

      decltype(auto) operator=(directional_light const& light) noexcept
      {
         assign(light);
         return *this;
      }

      decltype(auto) operator=(vec3 const& position) noexcept
      {
         assign(position);
         return *this;
      }

      friend vec3 operator-(directional_light const& light) noexcept
      {
         return light.direction();
      }

      vec3 const& direction() const noexcept
      {
         return position();
      }

      void direction(vec3 const& d) noexcept
      {
         position(d);
      }
   };

   inline void draw(directional_light const&) noexcept {}

   class point_light final : public basic_light_source<point_light> {
   public:
      explicit point_light(shader_binary const& program,
         std::pair<std::string, vec3> const& position,
         std::pair<std::string, vec3> const& ambient,
         std::pair<std::string, vec3> const& diffuse,
         std::pair<std::string, vec3> const& specular,
         std::pair<std::string, GLfloat> const constant,
         std::pair<std::string, GLfloat> const linear,
         std::pair<std::string, GLfloat> const quadratic)
         : basic_light_source<point_light>{program, position, ambient, diffuse, specular},
           constant_{program, constant.first, constant.second},
           linear_{program, linear.first, linear.second},
           quadratic_{program, quadratic.first, quadratic.second}
      {}

      decltype(auto) operator=(point_light const& light) noexcept
      {
         assign(light);
         constant_ = light.constant_;
         linear_ = light.linear_;
         quadratic_ = light.quadratic_;
         return *this;
      }

      decltype(auto) operator=(vec3 const& position) noexcept
      {
         assign(position);
         return *this;
      }
   private:
      uniform<GLfloat> constant_;
      uniform<GLfloat> linear_;
      uniform<GLfloat> quadratic_;
   };
   // void draw(const point_light&);

   // class spotlight final : public basic_light_source<spotlight> {};
   // void draw(const spotlight&);

   // using light_source = std::variant<directional_light, point_light, spotlight>;
   // void draw(const light_source&);
} // namespace doge

#endif // DOGE_ENTITY_LIGHT_SOURCE_HPP
