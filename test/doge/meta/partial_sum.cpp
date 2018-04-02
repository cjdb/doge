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
#include "doge/meta/partial_sum.hpp"
#include <array>
#include <vector>
#include <utility>

template <std::size_t... Sequence, std::size_t... Actual>
constexpr bool test_partial_sum2(std::index_sequence<Sequence...>, std::index_sequence<Actual...>) noexcept
{
   auto const sequence = std::array{Sequence...};
   auto const actual = std::array{Actual...};
   auto sum = std::size_t{};
   for (auto i = decltype(actual.size()){}; i < size(actual); ++i) {
      sum += sequence[i];
      if (sum != actual[i]) {
         return false;
      }
   }

   return true;
}

template <typename... Ts>
constexpr bool test_partial_sum(Ts&&...) noexcept
{
   return test_partial_sum2(std::index_sequence<sizeof(Ts)...>{}, doge::meta::partial_sum_t<sizeof(Ts)...>{});
}

int main()
{
   static_assert(test_partial_sum(0));
   static_assert(test_partial_sum(0, 0));
   static_assert(test_partial_sum(0, 0, 0));
   static_assert(test_partial_sum(0, 0, 0, 0));
   static_assert(test_partial_sum(0, 0, 0, 0, 0));
   static_assert(test_partial_sum(0, 0, 0, 0, 0, 0));
   static_assert(test_partial_sum(0, 0, 0, 0, 0, 0, 0));
   static_assert(test_partial_sum(0, 0.0, 'a', L'a', U'a', "foo bar"));
   static_assert(test_partial_sum(std::vector<double>{}, 0.0f, 0, "foo, bar"));
}
