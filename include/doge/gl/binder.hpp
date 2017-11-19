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
#ifndef DOGE_GL_BINDER_HPP
#define DOGE_GL_BINDER_HPP

#include <experimental/ranges/concepts>
#include <experimental/ranges/functional>
#include <utility>

namespace doge {
   class binder {
   public:
      template <std::experimental::ranges::Invocable F>
      binder(F f)
         : unbind_{std::move(f)}
      {}

      ~binder()
      {
         std::experimental::ranges::invoke(unbind_);
      }
   private:
      std::function<void()> unbind_;
   };
} // namespace doge

#endif // DOGE_GL_BINDER_HPP
