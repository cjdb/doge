#ifndef DOGE_UTILITY_STD_LAYOUT_TUPLE_HPP
#define DOGE_UTILITY_STD_LAYOUT_TUPLE_HPP

#include <experimental/ranges/concepts>
#include <experimental/ranges/utility>
#include <type_traits>

namespace doge {
   template <typename T> constexpr void explicitly_constructible_helper(T const&) noexcept;

   template <typename T, typename... Args>
   concept bool ImplicitlyConstructible = requires(Args&&... args) {
         {explicitly_constructible_helper<T>({std::forward<Args>(args)...})};
      };

   template <typename T, typename... Args>
   concept bool ExplicitlyConstructible = not ImplicitlyConstructible<T, Args...>;

   template <typename T>
   concept bool StandardLayout = std::is_standard_layout_v<T>;

   namespace ranges = std::experimental::ranges;

   template <typename... Ts>
   class std_layout_tuple;

   template <int I, typename T>
   class tuple_element;

   template <int I, typename T>
   using tuple_element_t = typename tuple_element<I, T>::type;

   // [std_layout_tuple.elem]
   template <int I, typename... Types>
   requires (I < sizeof...(Types))
   constexpr tuple_element_t<I, std_layout_tuple<Types...>>& get(std_layout_tuple<Types...>& t) noexcept
   {
      return t.template get<I>();
   }

   template <int I, typename... Types>
   requires (I < sizeof...(Types))
   constexpr tuple_element_t<I, std_layout_tuple<Types...>> const& get(std_layout_tuple<Types...> const& t) noexcept
   {
      return t.template get<I>();
   }

   template <int I, typename... Types>
   requires (I < sizeof...(Types))
   constexpr tuple_element_t<I, std_layout_tuple<Types...>>&& get(std_layout_tuple<Types...>&& t) noexcept
   {
      return t.template get<I>();
   }

   template <int I, typename... Types>
   requires (I < sizeof...(Types))
   constexpr tuple_element_t<I, std_layout_tuple<Types...>> const&& get(std_layout_tuple<Types...> const&& t) noexcept
   {
      return t.template get<I>();
   }

   template <typename Expected>
   constexpr int exactly_one_of_impl() noexcept
   {
      return 0;
   }

   template <typename Expected, typename Actual, typename... Ts>
   constexpr int exactly_one_of_impl() noexcept
   {
      if constexpr (std::is_same_v<Expected, Actual>) {
         return 1 + exactly_one_of_impl<Expected, Ts...>();
      }
      else {
         return exactly_one_of_impl<Expected, Ts...>();
      }
   }

   template <typename T, typename... Ts>
   constexpr bool exactly_one_of = exactly_one_of_impl<T, Ts...>() == 1;

   static_assert(exactly_one_of<int> == false);
   static_assert(exactly_one_of<int, double> == false);
   static_assert(exactly_one_of<int, const int> == false);
   static_assert(exactly_one_of<int, int> == true);
   static_assert(exactly_one_of<int, double, int> == true);
   static_assert(exactly_one_of<int, int, double> == true);
   static_assert(exactly_one_of<int, int, int> == false);

   template <typename T, typename... Ts>
   requires (exactly_one_of<T, Ts...>)
   constexpr T& get(std_layout_tuple<Ts...>& t) noexcept
   {
      t.template get<T>();
   }

   template <typename T, typename... Ts>
   requires (exactly_one_of<T, Ts...>)
   constexpr T const& get(std_layout_tuple<Ts...> const& t) noexcept
   {
      t.template get<T>();
   }

   template <typename T, typename... Ts>
   requires (exactly_one_of<T, Ts...>)
   constexpr T&& get(std_layout_tuple<Ts...>&& t) noexcept
   {
      t.template get<T>();
   }

   template <typename T, typename... Ts>
   requires (exactly_one_of<T, Ts...>)
   constexpr T const&& get(std_layout_tuple<Ts...> const&& t) noexcept
   {
      t.template get<T>();
   }

   template <typename... Ts>
   requires
      (StandardLayout<Ts> && ...)
   struct std_layout_tuple;
   
   template <>
   struct std_layout_tuple<> {
      constexpr void swap(std_layout_tuple& t) noexcept {}

      // [std_layout_tuple.rel]
      constexpr bool operator==(std_layout_tuple const&) const noexcept
      {
         return true;
      }

      constexpr bool operator!=(std_layout_tuple const& b) const noexcept
      {
         return not(*this == b);
      }

      constexpr bool operator<(std_layout_tuple const&) const noexcept
      {
         return false;
      }

      constexpr bool operator<=(std_layout_tuple const& b) const noexcept
      {
         return not(*this > b);
      }

      constexpr bool operator>=(std_layout_tuple const& b) const noexcept
      {
         return not(*this < b);
      }

      constexpr bool operator>(std_layout_tuple const& b) const noexcept
      {
         return b < *this;
      }
   };

   template <typename T, typename... Ts>
   requires
      StandardLayout<T> && (StandardLayout<Ts> && ...)
   class std_layout_tuple<T, Ts...> {
   public:
      // (1)
      constexpr explicit std_layout_tuple() noexcept(noexcept(
         std::is_nothrow_default_constructible_v<T> &&
         (std::is_nothrow_default_constructible_v<Ts> && ...)))
      requires
         ranges::DefaultConstructible<T> &&
         (ranges::DefaultConstructible<Ts> && ...) = default;

      // (2)
      constexpr explicit std_layout_tuple(T const& x, Ts const&... rest) noexcept(noexcept(
         std::is_nothrow_copy_constructible_v<T> &&
         (std::is_nothrow_copy_constructible_v<Ts> && ...)))
         : x_{x},
           rest_{rest...}
      {}

      // (3)
      template <typename U, typename... UTypes>
      requires (sizeof...(UTypes) == sizeof...(Ts))
      constexpr explicit std_layout_tuple(U&& x, UTypes&&... rest)
         : x_{std::forward<U>(x)},
           rest_{std::forward<UTypes>(rest)...}
      {}

      std_layout_tuple(std_layout_tuple const&)
      requires
         ranges::CopyConstructible<T> &&
         (ranges::CopyConstructible<Ts> && ...) = default;

      std_layout_tuple(std_layout_tuple&&)
      requires
         ranges::MoveConstructible<T> &&
         (ranges::MoveConstructible<Ts> && ...) = default;

      // (4)
      template <typename U, typename... UTypes>
      requires (sizeof...(UTypes) == sizeof...(Ts))
      constexpr explicit std_layout_tuple(std_layout_tuple<U, UTypes...> const& other) noexcept(noexcept(
         std::is_nothrow_constructible_v<T, U> &&
         (std::is_nothrow_constructible_v<Ts, UTypes> && ...)))
         : x_{other.x_},
           rest_{other.rest_}
      {}

      // (5)
      template <typename U, typename... UTypes>
      requires (sizeof...(UTypes) == sizeof...(Ts))
      constexpr explicit std_layout_tuple(std_layout_tuple<U, UTypes...>&& other) noexcept(noexcept(
         std::is_nothrow_constructible_v<T, U&&> &&
         (std::is_nothrow_constructible_v<Ts, UTypes&&> && ...)))
         : x_{std::forward<U>(other.x_)},
           rest_{std::forward<UTypes>(other.rest_)...}
      {}

      // (6)
      template <typename U1, typename U2>
      constexpr explicit std_layout_tuple(std::pair<U1, U2> const& p)
      : x_{p.first},
        rest_{p.second}
      {}

      // (7)
      template <typename U1, typename U2>
      constexpr explicit std_layout_tuple(std::pair<U1, U2>&& p)
         : x_{std::forward<U1>(p.first)},
           rest_{std::forward<U2>(p.second)}
      {}

      std_layout_tuple& operator=(std_layout_tuple const&)
      requires
         ranges::Assignable<T&, T const&> &&
         (ranges::Assignable<Ts&, Ts const&> && ...) = default;

      std_layout_tuple& operator=(std_layout_tuple&&) noexcept(noexcept(
         std::is_nothrow_move_assignable_v<T> &&
         (std::is_nothrow_move_assignable_v<Ts> && ...)))
      requires
         ranges::Assignable<T&, T&&> &&
         (ranges::Assignable<Ts&, Ts&&> && ...) = default;

      template <typename U, typename... UTypes>
      requires
         sizeof...(UTypes) == sizeof...(Ts) &&
         ranges::Assignable<T&, U const&>
      std_layout_tuple& operator=(std_layout_tuple<U, UTypes...> const& t)
      {
         static_assert((ranges::Assignable<Ts&, UTypes const&> && ...));
         x_ = t.x_;
         rest_ = t.rest_;
         return *this;
      }

      template <typename U, typename... UTypes>
      requires
         sizeof...(UTypes) != sizeof...(Ts) &&
         ranges::Assignable<T&, U&&>
      std_layout_tuple& operator=(std_layout_tuple<U, UTypes...>&& t)
      {
         static_assert((... && ranges::Assignable<Ts&, UTypes&&>));
         x_ = std::forward<T>(t.x_);
         rest_ = std::move(t.rest_);
         return *this;
      }

      template <typename U1, typename U2>
      requires
         sizeof...(Ts) == 1 &&
         ranges::Assignable<T&, U1 const&> &&
         (ranges::Assignable<Ts&, U2 const&> && ...)
      std_layout_tuple& operator=(std::pair<U1, U2> const& p)
      {
         get<0>() = p.first;
         get<1>() = p.second;
         return *this;
      }

      template <typename U1, typename U2>
      requires
         sizeof...(Ts) == 1 &&
         ranges::Assignable<T&, U1&&> &&
         (ranges::Assignable<Ts&, U2&&> && ...)
      std_layout_tuple& operator=(std::pair<U1, U2>&& p)
      {
         get<0>() = std::forward<U1>(p.first);
         get<1>() = std::forward<U2>(p.second);
         return *this;
      }

      constexpr void swap(std_layout_tuple& t) noexcept(noexcept(
         std::is_nothrow_swappable_v<T> &&
         (std::is_nothrow_swappable_v<Ts> && ...)))
      requires
         ranges::Swappable<T> &&
         (ranges::Swappable<Ts> && ...) 
      {
         ranges::swap(x_, t.x_);
         rest_.swap(t.rest_);
      }

      template <int I>
      friend constexpr tuple_element_t<I, std_layout_tuple>& get(std_layout_tuple&);
      template <int I>
      friend constexpr tuple_element_t<I, std_layout_tuple> const& get(std_layout_tuple const&);
      template <int I>
      friend constexpr tuple_element_t<I, std_layout_tuple>&& get(std_layout_tuple&&);
      template <int I>
      friend constexpr tuple_element_t<I, std_layout_tuple> const&& get(std_layout_tuple const&&);

      template <typename U>
      friend constexpr T& get(std_layout_tuple&);
      template <typename U>
      friend constexpr T const& get(std_layout_tuple const&);
      template <typename U>
      friend constexpr T&& get(std_layout_tuple&&);
      template <typename U>
      friend constexpr T const&& get(std_layout_tuple const&&);

      // [std_layout_tuple.rel]
      template <typename U, typename... UTypes>
      requires
         sizeof...(Ts) == sizeof...(UTypes) &&
         ranges::EqualityComparableWith<T, U> &&
         (ranges::EqualityComparableWith<Ts, UTypes> && ...)
      friend constexpr bool operator==(std_layout_tuple const& a, std_layout_tuple<UTypes...> const& b) noexcept
      {
         return a.x_ == b.x_ && a.rest_ == b.rest_;
      }

      template <typename U, typename... UTypes>
      requires
         sizeof...(Ts) == sizeof...(UTypes) &&
         ranges::EqualityComparableWith<T, U> &&
         (ranges::EqualityComparableWith<Ts, UTypes> && ...)
      friend constexpr bool operator!=(std_layout_tuple const& a, std_layout_tuple<UTypes...> const& b) noexcept
      {
         return not(a == b);
      }

      template <typename U, typename... UTypes>
      requires
         sizeof...(Ts) == sizeof...(UTypes) &&
         ranges::StrictTotallyOrderedWith<T, U> &&
         (ranges::StrictTotallyOrderedWith<Ts, UTypes> && ...)
      friend constexpr bool operator<(std_layout_tuple const& a, std_layout_tuple<UTypes...> const& b) noexcept
      {
         return ((bool)(a.x_ < b.x_) || not((bool)(b.x_ < a.x_))) && a.rest_ < b.rest_;
      }

      template <typename U, typename... UTypes>
      requires
         sizeof...(Ts) == sizeof...(UTypes) &&
         ranges::StrictTotallyOrderedWith<T, U> &&
         (ranges::StrictTotallyOrderedWith<Ts, UTypes> && ...)
      friend constexpr bool operator>(std_layout_tuple const& a, std_layout_tuple<UTypes...> const& b) noexcept
      {
         return b < a;
      }

      template <typename U, typename... UTypes>
      requires
         sizeof...(Ts) == sizeof...(UTypes) &&
         ranges::StrictTotallyOrderedWith<T, U> &&
         (ranges::StrictTotallyOrderedWith<Ts, UTypes> && ...)
      friend constexpr bool operator<=(std_layout_tuple const& a, std_layout_tuple<UTypes...> const& b) noexcept
      {
         return not(a > b);
      }

      template <typename U, typename... UTypes>
      requires
         sizeof...(Ts) == sizeof...(UTypes) &&
         ranges::StrictTotallyOrderedWith<T, U> &&
         (ranges::StrictTotallyOrderedWith<Ts, UTypes> && ...)
      friend constexpr bool operator>=(std_layout_tuple const& a, std_layout_tuple<UTypes...> const& b) noexcept
      {
         return not(a < b);
      }

      friend constexpr void swap(std_layout_tuple& x, std_layout_tuple& y) noexcept(noexcept(x.swap(y)))
      {
         x.swap(y);
      }
   private:
      T x_{};
      std_layout_tuple<Ts...> rest_;

      template <int I>
      constexpr tuple_element_t<I, std_layout_tuple>& get() & noexcept
      {
         return get_impl<I>(*this);
      }

      template <int I>
      constexpr tuple_element_t<I, std_layout_tuple> const& get() const& noexcept
      {
         return get_impl<I>(*this);
      }

      template <int I>
      constexpr tuple_element_t<I, std_layout_tuple>&& get() && noexcept
      {
         return get_impl<I>(*this);
      }

      template <int I>
      constexpr tuple_element_t<I, std_layout_tuple> const&& get() const&& noexcept
      {
         return get_impl<I>(*this);
      }

      template <typename U>
      constexpr U& get() & noexcept
      {
         return get_impl<U>(*this);
      }

      template <typename U>
      constexpr U const& get() const& noexcept
      {
         return get_impl<U>(*this);
      }

      template <typename U>
      constexpr U&& get() && noexcept
      {
         return get_impl<U>(*this);
      }

      template <typename U>
      constexpr U const&& get() const&& noexcept
      {
         return get_impl<U>(*this);
      }

      template <int I, typename U>
      static constexpr auto&& get_impl(U&& u) noexcept
      {
         if constexpr (I == 0) {
            return u.x_;
         }
         else {
            return get_impl<I - 1>(u.rest_);
         }
      }

      template <typename U, typename Tuple>
      static constexpr U&& get_impl(Tuple&& t) noexcept
      {
         if constexpr (std::is_same_v<T, U>) {
            return t.x_;
         }
         else {
            return get_impl<U>(t.rest_);
         }
      }
   };

   // [std_layout_tuple.creation]
   constexpr std_layout_tuple<> make_tuple() noexcept
   {
      return std_layout_tuple<>{};
   }

   template <typename T>
   constexpr T unwrap(T&& t) noexcept
   {
      return t;
   }

   template <typename T>
   constexpr T& unwrap(std::reference_wrapper<T> const r)
   {
      return r.get();
   }

   template <typename T>
   struct reference_unwrapper {
      using type = T;
   };

   template <typename T>
   using reference_unwrapper_t = typename reference_unwrapper<T>::type;

   template <typename T>
   struct reference_unwrapper<T&> {
      using type = reference_unwrapper_t<T>;
   };

   template <typename T>
   struct reference_unwrapper<T&&> {
      using type = reference_unwrapper_t<T>;
   };

   template <typename T>
   struct reference_unwrapper<std::reference_wrapper<T>> {
      using type = T&;
   };

   template <typename T, typename... Ts>
   constexpr auto make_tuple(T&& t, Ts&&... ts)
   {
      return std_layout_tuple<reference_unwrapper_t<T>, reference_unwrapper_t<Ts>...>{unwrap(t), std::forward<Ts>(ts)...};
   }

   template <typename... TTypes>
   constexpr std_layout_tuple<TTypes&&...> forward_as_tuple(TTypes&&... ts) noexcept
   {
      return std_layout_tuple<TTypes&&...>(std::forward<TTypes>(ts)...);
   }

   struct ignore_t {
      template <typename T>
      constexpr ignore_t const& operator=(T&&) const noexcept
      {
         return *this;
      }
   };

   inline constexpr auto ignore = ignore_t{};

   template <typename... TTypes>
   constexpr std_layout_tuple<TTypes&...> tie(TTypes&... ts) noexcept
   {
      return std_layout_tuple<TTypes&...>(ts...);
   }

   // template <typename... Ts>
   // struct tuple_cat_impl;

   // template <typename... Ts1, typename... Ts2>
   // struct tuple_cat_impl<std_layout_tuple<Ts1...>, std_layout_tuple<Ts2...>> {
      
   // };

   // template <typename... Tuples>
   // constexpr auto tuple_cat(Tuples&&... tpls);

   // [std_layout_tuple.apply]
   // template <typename F, typename Tuple>
   // constexpr bool is_invocable_on = false;

   // template <typename F, typename... Ts>
   // constexpr bool is_invocable_on<F, std_layout_tuple<Ts...>> = ranges::Invocable<F, Ts...>;

   // template <typename F, typename... Ts>
   // constexpr bool is_invocable_on<F, std_layout_tuple<Ts...> const> = ranges::Invocable<F, Ts const...>;

   // template <typename F, typename... Ts>
   // constexpr bool is_invocable_on<F, std_layout_tuple<Ts...>&> = ranges::Invocable<F, Ts...>;

   // template <typename F, typename Tuple>
   // requires (is_invocable_on<F, Tuple>)
   // constexpr decltype(auto) apply(F&& f, Tuple&& t);

   // [std_layout_tuple.helper]
   template <typename T>
   struct tuple_size;

   template <typename T>
   constexpr ranges::Integral tuple_size_v = tuple_size<T>::value;

   template <typename... Types>
   struct tuple_size<std_layout_tuple<Types...>> : public std::integral_constant<std::size_t, sizeof...(Types)> {};

   template <typename T>
   struct tuple_size<T const> : public std::integral_constant<std::size_t, tuple_size<T>::value> {};

   template <typename T>
   struct tuple_size<T volatile> : public std::integral_constant<std::size_t, tuple_size<T>::value> {};

   template <typename T>
   struct tuple_size<T const volatile> : public std::integral_constant<std::size_t, tuple_size<T>::value> {};

   template <int I, typename T>
   struct tuple_element<I, T const> {
      using type = std::add_const_t<tuple_element_t<I, T>>;
   };

   template <int I, typename T>
   struct tuple_element<I, T volatile> {
      using type = std::add_volatile_t<tuple_element_t<I, T>>;
   };

   template <int I, typename T>
   struct tuple_element<I, T const volatile> {
      using type = std::add_cv_t<tuple_element_t<I, T>>;
   };

   template <int I, typename... Types>
   requires (0 <= I && I < sizeof...(Types))
   struct tuple_element_helper;

   template <int I>
   struct tuple_element_helper<I> {
      static_assert(I == I + 1, "Size of std_layout_tuple is smaller than requested element index.");
   };

   template <int I, typename T, typename... Types>
   struct tuple_element_helper<I, T, Types...> {
      using type = typename tuple_element_helper<I - 1, Types...>::type;
   };

   template <typename T, typename... Types>
   struct tuple_element_helper<0, T, Types...> {
      using type = T;
   };

   template <int I, typename... Types>
   struct tuple_element<I, std_layout_tuple<Types...>> {
      using type = typename tuple_element_helper<I, Types...>::type;
   };

   // static_assert(std::is_standard_layout_v<std_layout_tuple<int, float>>);
   // static_assert(std::is_same_v<tuple_element_t<0, std_layout_tuple<int, float>>, int>);
   // static_assert(std::is_same_v<tuple_element_t<1, std_layout_tuple<int, float>>, float>);
   // static_assert(tuple_size_v<std_layout_tuple<int, float>> == 2);
} // namespace doge

#endif // DOGE_UTILITY_STD_LAYOUT_TUPLE_HPP
