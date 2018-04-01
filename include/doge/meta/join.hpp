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
#ifndef DOGE_META_JOIN_HPP
#define DOGE_META_JOIN_HPP

namespace doge::meta {
   template <typename... Ts>
   struct join;

   template <typename T>
   struct join<T> {
      using type = T;
   };

   template <typename... Ts>
   using join_t = typename join<Ts...>::type;
} // namespace doge::meta

#endif // DOGE_META_JOIN_HPP
