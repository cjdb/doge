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
