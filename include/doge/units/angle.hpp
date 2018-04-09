#ifndef DOGE_UNITS_ANGLE_HPP
#define DOGE_UNITS_ANGLE_HPP

#include <experimental/ranges/functional>
#include <glm/glm.hpp>
#include <gsl/gsl>
#include <limits>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#include <boost/units/systems/angle/degrees.hpp>
#include <boost/units/systems/si/plane_angle.hpp>
#include <boost/units/quantity.hpp>
#pragma GCC diagnostic pop

namespace doge {
   using radians = boost::units::quantity<boost::units::si::plane_angle>;

   inline namespace angle_literals {
      constexpr radians operator""_rad(long double const r) noexcept
      {
         return static_cast<double>(r) * boost::units::si::radians;
      }

      constexpr radians operator""_deg(long double const r) noexcept
      {
         return static_cast<radians>(static_cast<double>(r) * boost::units::degree::degrees);
      }
   } // inline namespace angle_literals

   template <typename Unit>
   constexpr auto dimensionless_cast(boost::units::quantity<Unit> const& q) noexcept
   {
      return q / Unit{};
   }

   constexpr radians sin(radians const& r) noexcept
   {
      return ::std::sin(dimensionless_cast(r)) * boost::units::si::radians;
   }

   constexpr radians cos(radians const& r) noexcept
   {
      return ::std::cos(dimensionless_cast(r)) * boost::units::si::radians;
   }

   constexpr radians tan(radians const& r) noexcept
   {
      return ::std::tan(dimensionless_cast(r)) * boost::units::si::radians;
   }
} // namespace doge

#endif // DOGE_UNITS_ANGLE_HPP
