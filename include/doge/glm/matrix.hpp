#ifndef DOGE_GLM_MATRIX_HPP
#define DOGE_GLM_MATRIX_HPP

#include "doge/detail/matrix.hpp"

namespace doge {
   constexpr auto translate(const glm::vec3& v) noexcept
   {
      return detail::translate_impl<glm::vec3>{v};
   }

   constexpr auto rotate(const angle a, const glm::vec3& v) noexcept
   {
      return detail::rotate_impl<glm::vec3>{a, v};
   }

   constexpr auto scale(const glm::vec3& v) noexcept
   {
      return detail::scale_impl<glm::vec3>{v};
   }

   inline const auto& invert = detail::invert_impl::construct();
   inline const auto& transpose = detail::transpose_impl::construct();
} // namespace doge

#endif // DOGE_GLM_MATRIX_HPP
