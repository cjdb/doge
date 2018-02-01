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
#include <catch/catch.hpp>
#include <doge/engine.hpp>
#include <doge/gl/shader_binary.hpp>
#include <doge/gl/shader_source.hpp>
#include <doge/gl/uniform.hpp>
#include <experimental/ranges/algorithm>
#include <experimental/ranges/concepts>
#include <experimental/ranges/functional>
#include <string_view>

namespace ranges = std::experimental::ranges;
using ranges::Regular;
using ranges::StrictTotallyOrdered;

namespace {
   template <typename T>
   struct uniform {
      std::string_view name;
      T value;
   };

   constexpr bool identity(const bool b) noexcept
   {
      return b;
   }
} // namespace <anonymous>

template <typename T1, typename T2, ranges::Predicate<bool> F = bool(&)(bool)>
void check_equivalent(const T1& a, const T2& b, const F& f = identity) noexcept
{
   CHECK(ranges::invoke(f, a == b));
   CHECK(not ranges::invoke(f, a != b));
}

template <typename T1, typename T2, ranges::Predicate<bool> F1, ranges::Predicate<bool> F2>
void check_strict_total_order(const T1& a, const T2& b, const F1& f1, const F2& f2) noexcept
{
   CHECK(ranges::invoke(f1, a < b));
   CHECK(ranges::invoke(f2, a <= b));
   CHECK(ranges::invoke(f2, b >= a));
   CHECK(ranges::invoke(f1, b > a));
}

template <typename A, typename B, typename T1, typename T2, typename F>
void check_binary_arithmetic_operation_impl(const F& f, const T1& a, const T2& b) noexcept
{
   const auto expected = ranges::invoke(f, static_cast<A>(a), static_cast<B>(b));
   const auto actual = ranges::invoke(f, a, b);
   CHECK(expected == actual);
}

template <typename T1, typename T2, ranges::RegularInvocable<T1, T2> F>
// requires
//    RingWith<T1, T2>
void check_binary_arithmetic_operation(const F& f, const doge::uniform<T1>& a, const T2& b) noexcept
{
   check_binary_arithmetic_operation_impl<T1, T2>(f, a, b);
   check_binary_arithmetic_operation_impl<T2, T1>(f, b, a);
}

template <typename T1, typename T2, ranges::RegularInvocable<T1, T2> F>
// requires
//    RingWith<T1, T2>
void check_binary_arithmetic_operation(const F& f, const T1& a, const doge::uniform<T2>& b) noexcept
{
   check_binary_arithmetic_operation_impl<T1, T2>(f, a, b);
   check_binary_arithmetic_operation_impl<T2, T1>(f, b, a);
}

template <typename T1, typename T2, ranges::RegularInvocable<T1, T2> F>
// requires
//    RingWith<T1, T2>
void check_binary_arithmetic_operation(const F& f, const doge::uniform<T1>& a,
   const doge::uniform<T2>& b) noexcept
{
   check_binary_arithmetic_operation_impl<T1, T2>(f, a, b);
   check_binary_arithmetic_operation_impl<T2, T1>(f, b, a);
}

template <typename T1, typename T2>
void check_compound_assignment(doge::uniform<T1>& a, const T2& b,
   const doge::shader_binary& program, const uniform<T1>& first)
{
   const auto check_same_on_device = [&program, &first](const auto& a) {
      const auto device = doge::uniform<const T1>{program, first.name};
      CHECK(a == device);
   };

   {
      const auto expected = a + b;
      a += b;
      CHECK(a == expected);
      check_same_on_device(a);
   }
   {
      const auto expected = a - b;
      a -= b;
      CHECK(a == expected);
      check_same_on_device(a);
      a = b;
   }
   {
      const auto expected = a * b;
      a *= b;
      CHECK(a == expected);
      check_same_on_device(a);
   }
   {
      const auto expected = a / b;
      a /= b;
      CHECK(a == expected);
      check_same_on_device(a);
   }

   if constexpr (std::is_integral_v<T1> ) {
      const auto expected = a % b;
      a %= b;
      CHECK(a == expected);
      check_same_on_device(a);
   }
}

template <typename T>
void check_scalar(const doge::shader_binary& program, const std::array<::uniform<T>, 2> scalar)
{
   const auto first = scalar[0];

   auto a = doge::uniform{program, first.name, first.value};
   CHECK(static_cast<T>(a) == first.value);

   const auto second = scalar[1];
   (void)second;
   SECTION("[uniform.scalar.comparison]") {
      // test operator== reflexivity
      check_equivalent(a, a);

      // test copy constructor and operator== symmetry
      auto b = doge::uniform{program, first.name, first.value}; // STO
      check_equivalent(a, b);
      check_equivalent(b, a);

      // test operator== transitivity
      auto c = doge::uniform{program, first.name, first.value}; // STO
      check_equivalent(b, c);
      check_equivalent(a, c);

      // test equivalent via total order (T)
      check_strict_total_order(a, b, std::logical_not<>{}, identity);
      check_strict_total_order(b, a, std::logical_not<>{}, identity);

      // test `const T` against T
      const auto ca = doge::uniform<const T>{program, first.name}; // STO
      CHECK(static_cast<T>(ca) == first.value);

      // test operator==<const T> reflexivity
      check_equivalent(ca, ca);

      // test operator==<const T> symmetry
      const auto cb = doge::uniform<const T>{program, first.name}; // STO
      check_equivalent(ca, cb);
      check_equivalent(cb, ca);

      // test operator==<const T, T> symmetry
      check_equivalent(ca, a);
      check_equivalent(a, ca);

      // test operator==<const T> transitivity
      const auto cc = doge::uniform<const T>{program, first.name}; // STO
      check_equivalent(cb, cc);
      check_equivalent(ca, cc);

      // test operator<const T, T> transitivity
      check_equivalent(cb, b);
      check_equivalent(ca, b);

      // test equivalent via strict total order (const T)
      check_strict_total_order(ca, cb, std::logical_not<>{}, identity);
      check_strict_total_order(cb, ca, std::logical_not<>{}, identity);

      // test equivalent via strict total order (const T, T)
      check_strict_total_order(ca, a, std::logical_not<>{}, identity);
      check_strict_total_order(a, ca, std::logical_not<>{}, identity);

      // test strict total order
      auto d = doge::uniform{program, second.name, second.value}; // STO
      CHECK(static_cast<T>(d) == second.value);

      check_strict_total_order(a, d, identity, identity);
      check_strict_total_order(d, a, std::logical_not<>{}, std::logical_not<>{});
   }

   SECTION("[uniform.scalar.assignment]") {
      // test assignment
      a = second.value;
      const auto cd = doge::uniform<const T>{program, first.name};
      check_equivalent(a, cd);

      // test equivalent to T
      check_equivalent(a, second.value);
      check_equivalent(second.value, a);

      // test equivalent via strict total order (const T)
      check_strict_total_order(a, second.value, std::logical_not<>{}, identity);
      check_strict_total_order(second.value, a, std::logical_not<>{}, identity);

      // test equivalent to T
      check_equivalent(cd, second.value);
      check_equivalent(second.value, cd);

      // test equivalent via strict total order (const T)
      check_strict_total_order(cd, second.value, std::logical_not<>{}, identity);
      check_strict_total_order(second.value, cd, std::logical_not<>{}, identity);
   }

   SECTION("[uniform.scalar.arithmetic]") {
      a = first.value + first.value;
      const auto b = doge::uniform<const T>{program, first.name};
      constexpr auto n = T(10);
      {
         check_equivalent(+a, static_cast<T>(a));
         check_equivalent(+b, static_cast<T>(b));
         check_equivalent(-a, -static_cast<T>(a));
         check_equivalent(-b, -static_cast<T>(b));
      }

      check_compound_assignment(a, a, program, first);
      // check_compound_assignment(a, b, program, first);
      // check_compound_assignment(a, n, program, first);

      auto check_binary_arithmetic = [&](const auto& f) {
         check_binary_arithmetic_operation(f, a, n);
         check_binary_arithmetic_operation(f, a, a);
         check_binary_arithmetic_operation(f, a, b);
      };

      check_binary_arithmetic(std::plus<>{});
      check_binary_arithmetic(std::minus<>{});
      check_binary_arithmetic(std::multiplies<>{});
      check_binary_arithmetic(std::divides<>{});

      if constexpr (std::is_integral_v<T>) {
         check_binary_arithmetic(std::modulus<>{});
      }
   }

   CHECK_THROWS(doge::uniform<GLfloat>{program, "float_dne", first.value});
}

template <int N, typename T>
void check_vector(const doge::shader_binary& program, const std::array<::uniform<T>, 2> vec)
{
   (void)program, (void)vec;
}

TEST_CASE("uniforms can be read and written to", "[uniform]") {
   auto engine = doge::engine{};
   auto program = doge::shader_binary{{
      std::make_pair(doge::shader_source::vertex, "test.uniform.vert.glsl"),
      std::make_pair(doge::shader_source::fragment, "test.uniform.frag.glsl")}};

   program.use([&program]{
      SECTION("[uniform.scalar]") {
         SECTION("[uniform.scalar.GLfloat]") {
            check_scalar(program, std::array<uniform<GLfloat>, 2>{{{"f.a", 0.05f}, {"f.b", 0.5f}}});
         }

         // SECTION("[uniform.scalar.GLint]") {
         //    check_scalar(program, std::array<uniform<GLint>, 2>{{{"i.a", 10}, {"i.b", 20}}});
         // }

         // SECTION("Testing GLuint", "[uniform.scalar.GLuint]") {
         //    check_scalar(program, std::array<uniform<GLuint>, 2>{{{"u.a", 15u}, {"u.b", 16u}}});
         // }
      }

      SECTION("[uniform.vec2]") {
         constexpr ranges::Integral dimensions = 2;
         SECTION("[uniform.vec2.GLfloat]") {
            check_vector<dimensions>(program, std::array<uniform<glm::vec2>, 2>{{
               {"v2.a", {0.5f, 0.8f}}, {"v2.b", {-0.5f, -0.8f}}}});
         }

         SECTION("[uniform.vec2.GLint") {
            check_vector<dimensions>(program, std::array<uniform<glm::ivec2>, 2>{{
               {"iv2.a", {10, 20}}, {"iv2.b", {30, 40}}}});
         }

         SECTION("[uniform.vec2.GLuint") {
            check_vector<dimensions>(program, std::array<uniform<glm::uvec2>, 2>{{
               {"uv2.a", {10, 20}}, {"uv2.b", {30, 40}}}});
         }
      }
   // SECTION("Testing three-dimensional vectors", "[uniform.vec3]") {}
   // SECTION("Testing four-dimensional vectors", "[uniform.vec4]") {}
   // SECTION("Testing 2xN matrix", "[uniform.mat2xN]") {}
   // SECTION("Testing Nx2 matrix", "[uniform.matNx2]") {}
   // SECTION("Testing 3xN matrix", "[uniform.mat3xN]") {}
   // SECTION("Testing Nx3 matrix", "[uniform.matNx3]") {}
   // SECTION("Testing 4xN matrix", "[uniform.mat4xN]") {}
   // SECTION("Testing Nx4 matrix", "[uniform.matNx4]") {}
   });
}