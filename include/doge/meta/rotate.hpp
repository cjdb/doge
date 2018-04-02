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
#ifndef DOGE_META_ROTATE_HPP
#define DOGE_META_ROTATE_HPP

#include <utility>

namespace doge::meta {
   template <std::size_t Rotations, typename>
   struct rotate;

   template <std::size_t Rotations, typename T>
   using rotate_t = typename rotate<Rotations, T>::type;

   template <std::size_t Rotations, std::size_t N, std::size_t... Range>
   struct rotate<Rotations, std::index_sequence<N, Range...>> {
      using type = rotate_t<Rotations - 1, std::index_sequence<Range..., N>>;
   };

   template <std::size_t N, std::size_t... Range>
   struct rotate<1, std::index_sequence<N, Range...>> {
      using type = std::index_sequence<Range..., N>;
   };

   template <std::size_t N, std::size_t... Range>
   struct rotate<0, std::index_sequence<N, Range...>> {
      using type = std::index_sequence<N, Range...>;
   };
} // namespace doge::meta

#endif // DOGE_META_ROTATE_HPP
