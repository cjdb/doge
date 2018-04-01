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