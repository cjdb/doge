//
//  Copyright 2017 Christopher Di Bella
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
#ifndef DOGE_META_PARTIAL_SUM_HPP
#define DOGE_META_PARTIAL_SUM_HPP

#include "doge/meta/join.hpp"
#include <utility>

namespace doge::meta {
   template <std::size_t... Is>
   struct partial_sum;

   template <std::size_t... Is>
   using partial_sum_t = typename partial_sum<Is...>::type;

   template <std::size_t... Is1, std::size_t... Is2>
   struct join<std::index_sequence<Is1...>, std::index_sequence<Is2...>> {
      using type = std::index_sequence<Is1..., Is2...>;
   };

   template <std::size_t A>
   struct partial_sum<A> {
      using type = std::index_sequence<A>;
   };

   template <std::size_t A, std::size_t B>
   struct partial_sum<A, B> {
      using type = std::index_sequence<A, A + B>;
   };

   template <std::size_t A, std::size_t B, std::size_t C, std::size_t... Is>
   struct partial_sum<A, B, C, Is...> {
      using type = join_t<partial_sum_t<A, B>, partial_sum_t<A + B + C, Is...>>;
   };
} // namespace doge::meta

#endif // DOGE_META_PARTIAL_SUM_HPP
