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
#ifndef DOGE_GLM_MATRIX_HPP
#define DOGE_GLM_MATRIX_HPP

#include "doge/detail/matrix.hpp"
#include "doge/types.hpp"

namespace doge {
   constexpr auto translate(const vec3& v) noexcept
   {
      return detail::translate_impl<vec3>{v};
   }

   constexpr auto rotate(const angle a, const vec3& v) noexcept
   {
      return detail::rotate_impl<vec3>{a, v};
   }

   constexpr auto scale(const glm::vec3& v) noexcept
   {
      return detail::scale_impl<vec3>{v};
   }

   inline const auto& invert = detail::invert_impl::construct();
   inline const auto& transpose = detail::transpose_impl::construct();
} // namespace doge

#endif // DOGE_GLM_MATRIX_HPP
