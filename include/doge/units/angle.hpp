#ifndef DOGE_UNITS_ANGLE_HPP
#define DOGE_UNITS_ANGLE_HPP

#include <experimental/ranges/concepts>
#include <experimental/ranges/functional>
#include <glm/glm.hpp>
#include <gsl/gsl>
#include <limits>

namespace doge {
   namespace ranges = std::experimental::ranges;

   template <typename T>
   requires
      std::numeric_limits<T>::is_iec559
   class basic_angle {
   public:
      using value_type = T;

      constexpr basic_angle() noexcept = default;
      explicit constexpr basic_angle(const T f) noexcept
         : angle_{f}
      {}

      constexpr operator T() const noexcept
      {
         return angle_;
      }

      constexpr basic_angle& operator+=(const basic_angle a) noexcept
      {
         angle_ += a.angle_;
         return *this;
      }

      constexpr basic_angle& operator-=(const basic_angle a) noexcept
      {
         angle_ -= a.angle_;
         return *this;
      }

      constexpr basic_angle& operator*=(const basic_angle a) noexcept
      {
         angle_ *= a.angle_;
         return *this;
      }

      template <typename U>
      requires
         std::is_fundamental_v<U>
      constexpr basic_angle& operator*=(const U scalar) noexcept
      {
         angle_ *= gsl::narrow_cast<value_type>(scalar);
         return *this;
      }

      constexpr basic_angle& operator/=(const basic_angle a) noexcept
      {
         angle_ /= a.angle_;
         return *this;
      }

      template <typename U>
      constexpr basic_angle& operator/=(const U scalar) noexcept
      {
         angle_ /= gsl::narrow_cast<value_type>(scalar);
         return *this;
      }

      constexpr basic_angle operator+() noexcept
      {
         return *this;
      }

      constexpr basic_angle operator-() noexcept
      {
         return basic_angle{-angle_};
      }

      constexpr basic_angle& operator++() noexcept
      {
         ++angle_;
         return *this;
      }

      constexpr basic_angle operator++(int) noexcept
      {
         auto temp = *this;
         ++*this;
         return temp;
      }

      constexpr basic_angle& operator--() noexcept
      {
         --angle_;
         return *this;
      }

      constexpr basic_angle operator--(int) noexcept
      {
         auto temp = *this;
         --*this;
         return temp;
      }

      friend constexpr basic_angle operator+(basic_angle a, const basic_angle b) noexcept
      {
         return a += b;
      }

      friend constexpr basic_angle operator-(basic_angle a, const basic_angle b) noexcept
      {
         return a -= b;
      }

      friend constexpr basic_angle operator*(basic_angle a, const basic_angle b) noexcept
      {
         return a *= b;
      }

      template <typename U>
      requires
         std::is_fundamental_v<U>
      friend constexpr basic_angle operator*(basic_angle a, const U b) noexcept
      {
         return a *= b;
      }

      template <typename U>
      requires
         std::is_fundamental_v<U>
      friend constexpr basic_angle operator*(const U b, const basic_angle a) noexcept
      {
         return a * b;
      }

      friend constexpr basic_angle operator/(basic_angle a, const basic_angle b) noexcept
      {
         return a /= b;
      }

      friend constexpr bool operator==(const basic_angle a, const basic_angle b) noexcept
      {
         return a.angle_ == b.angle_;
      }

      friend constexpr bool operator!=(const basic_angle a, const basic_angle b) noexcept
      {
         return not(a == b);
      }

      friend constexpr bool operator<(const basic_angle a, const basic_angle b) noexcept
      {
         return a.angle_ < b.angle_;
      }

      friend constexpr bool operator<=(const basic_angle a, const basic_angle b) noexcept
      {
         return not(b < a);
      }

      friend constexpr bool operator>(const basic_angle a, const basic_angle b) noexcept
      {
         return b < a;
      }

      friend constexpr bool operator>=(const basic_angle a, const basic_angle b) noexcept
      {
         return not(a < b);
      }
   private:
      T angle_ = {};
   };

   template <typename T>
   requires
      std::numeric_limits<T>::is_iec559
   basic_angle<T> as_radians(const T t) noexcept
   {
      return basic_angle<T>{glm::radians(t)};
   }

   template <typename T>
   requires
      std::numeric_limits<T>::is_iec559
   basic_angle<T> cos(const basic_angle<T> t) noexcept
   {
      return basic_angle<T>{std::cos(t)};
   }

   template <typename T>
   requires
      std::numeric_limits<T>::is_iec559
   basic_angle<T> sin(const basic_angle<T> t) noexcept
   {
      return basic_angle<T>{std::sin(t)};
   }

   template <typename T>
   requires
      std::numeric_limits<T>::is_iec559
   basic_angle<T> tan(const basic_angle<T> t) noexcept
   {
      return basic_angle<T>{std::tan(t)};
   }

   using angle = basic_angle<float>;
   using angled = basic_angle<double>;

   inline namespace angle_literals {
      constexpr angle operator""_deg(const long double d) noexcept
      {
         return angle{gsl::narrow_cast<float>(glm::radians(d))};
      }

      constexpr angled operator""_ddeg(const long double d) noexcept
      {
         return angled{gsl::narrow_cast<double>(glm::radians(d))};
      }

      constexpr angle operator""_rad(const long double d) noexcept
      {
         return angle{gsl::narrow_cast<float>(d)};
      }

      constexpr angled operator""_drad(const long double d) noexcept
      {
         return angled{gsl::narrow_cast<double>(d)};
      }
   } // inline namespace angle_literals
} // namespace doge

#endif // DOGE_UNITS_ANGLE_HPP
