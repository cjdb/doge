//
// Copyright 2017 Christopher Di Bella
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
#ifndef DOGE_UTILITY_REFERENCE_COUNT_HPP
#define DOGE_UTILITY_REFERENCE_COUNT_HPP

#include <experimental/ranges/concepts>
#include <experimental/ranges/iterator>
#include <experimental/ranges/functional>
#include <utility>

namespace doge {
   namespace ranges = std::experimental::ranges;

   template <ranges::Semiregular T>
   class reference_count {
      static constexpr ranges::Regular small_object = sizeof(T) <= sizeof(decltype(sizeof(0)));
   public:
      reference_count() = default;

      template <ranges::Invocable<T*> F>
      reference_count(T t, F f)
         : object_{std::move(t)},
           ptr_{std::addressof(*object_),
            [f = std::move(f), this](auto p) {
               if (object_)
                  ranges::invoke(f, p);
            }}
      {}

      operator T() const
      requires small_object
      {
         return object_.value();
      }

      operator const T&() const
      requires not small_object
      {
         return object_.value();
      }
   private:
      std::optional<T> object_ = {};
      std::shared_ptr<T> ptr_ = {};
   };
} // namespace doge

#endif // DOGE_UTILITY_REFERENCE_COUNT_HPP
