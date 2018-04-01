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
