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
#ifndef DOGE_META_CLEAR_HPP
#define DOGE_META_CLEAR_HPP

#include <utility>

namespace doge::meta {
   template <std::size_t Value, typename>
   struct clear;

   template <std::size_t Value, typename T>
   using clear_t = typename clear<Value, T>::type;

   template <std::size_t Value, std::size_t Old, std::size_t... Rest>
   struct clear<Value, std::index_sequence<Old, Rest...>> {
      using type = std::index_sequence<Value, Rest...>;
   };
} // namespace doge::meta

#endif // DOGE_META_CLEAR_HPP
