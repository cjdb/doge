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
   if constexpr (not (std::is_same_v<std::decay_t<F>, std::multiplies<>> &&
      doge::detail::is_glm_matrix<T1>)) {
      check_binary_arithmetic_operation_impl<T2, T1>(f, b, a);
   }
}

template <typename T1, typename T2, ranges::RegularInvocable<T1, T2> F>
// requires
//    RingWith<T1, T2>
void check_binary_arithmetic_operation(const F& f, const doge::uniform<T1>& a,
   const doge::uniform<T2>& b) noexcept
{
   check_binary_arithmetic_operation_impl<T1, T2>(f, a, b);

   if constexpr (not (std::is_same_v<std::decay_t<F>, std::multiplies<>> &&
      doge::detail::is_glm_matrix<T1>)) {
      check_binary_arithmetic_operation_impl<T2, T1>(f, b, a);
   }
}

template <typename T>
void check_is_same_on_device(const doge::uniform<T>& u, const doge::shader_binary& program,
   const std::string_view name) noexcept
{
   const auto device = doge::uniform<const T>{program, name};
   CHECK(u == device);
}

template <typename T1, typename T2>
void check_compound_assignment(doge::uniform<T1>& a, const T2& b,
   const doge::shader_binary& program, const uniform<T1>& first)
{
   {
      const auto expected = a + b;
      a += b;
      CHECK(a == expected);
      check_is_same_on_device(a, program, first.name);
   }
   {
      const auto expected = a - b;
      a -= b;
      CHECK(a == expected);
      check_is_same_on_device(a, program, first.name);
   }
   if constexpr (not doge::detail::is_glm_matrix<T1>) {
      {
         const auto expected = a * b;
         a *= b;
         CHECK(a == expected);
         check_is_same_on_device(a, program, first.name);
      }
      {
         const auto expected = a / b;
         a /= b;
         CHECK(a == expected);
         check_is_same_on_device(a, program, first.name);
      }

      if constexpr (std::is_integral_v<T1> ) {
         const auto expected = a % b;
         a %= b;
         CHECK(a == expected);
         check_is_same_on_device(a, program, first.name);
      }
   }
}

template <typename T>
auto make_uniform(const doge::shader_binary& program, const std::string_view name, const T& value)
{
   if constexpr (doge::detail::is_glm_matrix<T>) {
      return doge::uniform<T>{program, name, false, value};
   }
   else {
      return doge::uniform<T>{program, name, value};
   }
}

template <typename T>
auto check_constructor(const doge::shader_binary& program, const ::uniform<T>& u)
{
   auto a = make_uniform(program, u.name, u.value);

   auto c = doge::uniform<const T>{program, u.name};
   CHECK(static_cast<T>(a) == static_cast<T>(c));

   CHECK_THROWS_AS(make_uniform(program, "dne", u.value), doge::uniform_not_found);
   CHECK_THROWS_AS((doge::uniform<const T>{program, "dne"}), doge::uniform_not_found);

   CHECK_THROWS_AS(make_uniform(program, "bad_type", u.value), std::runtime_error);

   return std::make_pair(std::move(a), std::move(c));
}

template <typename T1, typename T2, typename T3, typename T4>
void check_equivalent(const T1& eq1, const T2& eq2, const T3& eq3, const T4& neq) noexcept
{
   // test reflexivity on operator==
   CHECK(eq1 == eq1);

   // test symmetry on operator==
   CHECK(eq1 == eq2);
   CHECK(not (eq1 == neq));

   CHECK(eq2 == eq1);
   CHECK(not (neq == eq1));

   // test transitivity on operator==
   CHECK(eq2 == eq3);
   CHECK(not (eq2 == neq));
   CHECK(eq1 == eq3);
   CHECK(not (neq == eq3));

   // test anti-reflexivity on operator!=
   CHECK(not (eq1 != eq1)); 

   // test symmetry on operator!=
   CHECK(eq1 != neq);
   CHECK(not (eq1 != eq2));

   CHECK(neq != eq1);
   CHECK(not (eq2 != eq1));

   // test transitivity on operator!=
   CHECK(eq2 != neq);
   CHECK(not (eq2 != eq3));

   CHECK(eq3 != neq);
   CHECK(not (eq1 != eq3));
}

template <typename T1, typename T2, typename T3, typename T4, typename T5>
void check_strict_total_order(const T1& low1, const T2& low2, const T3& low3, const T4& mid,
   const T5& high) noexcept
{
   const auto check_strict_order = [](auto f, const auto& a, const auto& b,
      const auto& c) noexcept {
      if (not std::is_same_v<decltype(f), std::less_equal<>> &&
         not std::is_same_v<decltype(f), std::greater_equal<>>) {
         // check anti-reflexivity
         CHECK(not ranges::invoke(f, a, a));
      }

      // check anti-symmetry
      CHECK(ranges::invoke(f, a, b));
      CHECK(not ranges::invoke(f, b, a));

      // check transitivity
      CHECK(ranges::invoke(f, b, c));
      CHECK(ranges::invoke(f, a, c));
   };

   check_strict_order(std::less<>{}, low1, mid, high);
   check_strict_order(std::less_equal<>{}, low1, mid, high);
   check_strict_order(std::greater_equal<>{}, high, mid, low1);
   check_strict_order(std::greater<>{}, high, mid, low1);

   const auto check_equivalent = [](const auto& f, const auto& a, const auto& b,
      const auto& c) noexcept {
      // reflexivity
      CHECK(ranges::invoke(f, a, a));

      // symmetry
      CHECK(ranges::invoke(f, a, b));
      CHECK(ranges::invoke(f, b, a));

      // transitivity
      CHECK(ranges::invoke(f, b, c));
      CHECK(ranges::invoke(f, a, c));
   };

   check_equivalent(std::less_equal<>{}, low1, low2, low3);
   check_equivalent(std::greater_equal<>{}, low1, low2, low3);
}

template <typename T1, typename T2>
void check_unary_expression(doge::uniform<T1>& a, const doge::uniform<T2>& b) noexcept
{
   {
      CHECK(+a == static_cast<T1>(a));
      CHECK(+b == static_cast<T2>(b));
   }
   {
      CHECK(-a == -static_cast<T1>(a));
      CHECK(-b == -static_cast<T2>(b));
   }
   {
      auto expected = static_cast<T1>(a);
      CHECK(++a == ++expected);
      CHECK(a++ == expected++);
      CHECK(a == expected); // make sure the result actually increased.

      CHECK(--a == --expected);
      CHECK(a-- == expected--);
      CHECK(a == expected); // make sure the result actually decreased.
   }
}

template <typename T1, typename T2, typename T3>
void check_additive_expression(doge::uniform<T1>& a, const doge::uniform<T2>& b, const T3& n) noexcept
{
   auto check = [&](const auto& op){
      check_binary_arithmetic_operation(op, a, a);
      check_binary_arithmetic_operation(op, a, b);
      check_binary_arithmetic_operation(op, a, n);
   };

   check(std::plus<>{});
   check(std::minus<>{});
}

template <typename>
constexpr bool self_multiplicable = true;

template <glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
requires
   C != R
constexpr bool self_multiplicable<glm::mat<C, R, T, Q>> = false;

template <typename T1, typename T2, typename T3>
void check_multiplicative_expression(doge::uniform<T1>& a, const T2& b, const T3& n) noexcept
{
   auto check = [&](const auto& op){
      if constexpr (self_multiplicable<T1>) {
         check_binary_arithmetic_operation(op, a, a);
         check_binary_arithmetic_operation(op, a, b);
      }
      check_binary_arithmetic_operation(op, a, n);
   };

   check(std::multiplies<>{});

   if constexpr (not doge::detail::is_glm_matrix<T2>) {
      check(std::divides<>{});
   }

   if constexpr (std::is_integral_v<T1>) {
      check_binary_arithmetic_operation(std::modulus<>{}, a, b);
   }
}

template <glm::length_t C, glm::length_t R, typename T, glm::qualifier Q, typename Matrix>
void check_arithmetic(doge::uniform<glm::mat<C, R, T, Q>>& a, const doge::shader_binary& program,
   const uniform<Matrix> first, const uniform<Matrix> second) noexcept
{
   const auto b = doge::uniform<const Matrix>{program, first.name};
   check_unary_expression(a, b);
   const auto [sum, product] = []{
      [[maybe_unused]] const auto m2x3 = glm::mat2x3{0, 1, 2, 3, 4, 5};
      [[maybe_unused]] const auto m2x4 = glm::mat2x4{0, 10, 21, 32, 43, 54, 65, 76};
      [[maybe_unused]] const auto m3x2 = glm::mat3x2{1, 11, 12, 13, 14, 15};
      [[maybe_unused]] const auto m3x3 = glm::mat3x3{0, 1, 2, 3, 4, 5, 6, 7, 8};
      [[maybe_unused]] const auto m3x4 = glm::mat3x4{1, 2, 3, 4, 5, 6, -1, -2, -3, -4, -5, -6};
      [[maybe_unused]] const auto m4x3 = glm::mat4x3{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -2};

      if constexpr (std::is_same_v<Matrix, glm::mat2x2>) {
         return std::make_pair(glm::mat2x2{1, 2, 3, 4}, m3x2);
      }
      else if constexpr (std::is_same_v<Matrix, glm::mat2x3>) {
         return std::make_pair(m2x3, m2x4);
      }
      else if constexpr (std::is_same_v<Matrix, glm::mat2x4>) {
         return std::make_pair(m2x4, m4x3);
      }
      else if constexpr (std::is_same_v<Matrix, glm::mat3x2>) {
         return std::make_pair(m3x2, m2x4);
      }
      else if constexpr (std::is_same_v<Matrix, glm::mat3x3>) {
         return std::make_pair(m3x3, m2x3);
      }
      else if constexpr (std::is_same_v<Matrix, glm::mat3x4>) {
         return std::make_pair(m3x4, m4x3);
      }
      else if constexpr (std::is_same_v<Matrix, glm::mat4x2>) {
         return std::make_pair(glm::mat4x2{76, 65, 54, 43, 32, 21, 10, 0}, m2x4);
      }
      else if constexpr (std::is_same_v<Matrix, glm::mat4x3>) {
         return std::make_pair(m4x3, m3x2);
      }
      else if constexpr (std::is_same_v<Matrix, glm::mat4x4>) {
         return std::make_pair(glm::mat4x4{-8, -7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7},
                               m3x4);
      }
   }();

   check_compound_assignment(a, make_uniform(program, second.name, second.value), program, first);
   check_compound_assignment(a, doge::uniform<const Matrix>{program, second.name}, program, first);
   check_compound_assignment(a, sum, program, first);

   check_additive_expression(a, b, sum);
   check_multiplicative_expression(a, product, 3.0f);
}

template <typename T>
requires
   not doge::detail::is_glm_matrix<T>
void check_arithmetic(doge::uniform<T>& a, const doge::shader_binary& program,
   const uniform<T> first, const uniform<T> second) noexcept
{
   a = first.value + first.value;
   const auto b = doge::uniform<const T>{program, first.name};
   const auto n = []{
      if constexpr (doge::is_one_of_v<T, GLfloat, GLint, GLuint>) {
         return T{10};
      }
      else if constexpr (doge::detail::is_vec2<T>) {
         return T{10, 10};
      }
      else if constexpr (doge::detail::is_vec3<T>) {
         return T{10, 10, 10};
      }
      else if constexpr (doge::detail::is_vec4<T>) {
         return T{10, 10, 10, 10};
      }
   }();

   check_unary_expression(a, b);

   check_compound_assignment(a, make_uniform(program, second.name, second.value), program, first);
   check_compound_assignment(a, doge::uniform<const T>{program, second.name}, program, first);
   check_compound_assignment(a, n, program, first);

   check_additive_expression(a, b, n);
   check_multiplicative_expression(a, b, n);
}

template <typename T>
void check_uniform(const doge::shader_binary& program, const std::array<uniform<T>, 3> u)
{
   const auto [first, second, third] = u;
   auto [a, const_a] = check_constructor(program, first);
   auto [b, const_b] = check_constructor(program, second);
   auto [c, const_c] = check_constructor(program, third);

   SECTION("[uniform.comparison]") {
      auto a_equivalent = std::array{check_constructor(program, first),
         check_constructor(program, first)};

      check_equivalent(a, a_equivalent[0].first, a_equivalent[1].first, b);
      check_equivalent(a, a_equivalent[0].second, first.value, const_b);

      if constexpr (ranges::StrictTotallyOrdered<T>) {
         check_strict_total_order(a, a_equivalent[0].first, a_equivalent[1].first, b, c);
         check_strict_total_order(a, a_equivalent[0].second, first.value, const_b, third.value);
      }
   }

   SECTION("[uniform.assignment]") {
      // test assignment
      a = second.value;
      check_is_same_on_device(a, program, first.name);

      // test equivalent to T
      CHECK(a == second.value);
   }

   SECTION("[uniform.arithmetic]") {
      check_arithmetic(a, program, first, second);
   }
}

TEST_CASE("uniforms can be read and written to", "[uniform]") {
   auto engine = doge::engine{};
   auto program = doge::shader_binary{{
      std::make_pair(doge::shader_source::vertex, "test.uniform.vert.glsl"),
      std::make_pair(doge::shader_source::fragment, "test.uniform.frag.glsl")}};

   program.use([&program]{
      SECTION("[uniform.scalar.GLfloat]") {
         check_uniform(program, std::array<uniform<GLfloat>, 3>{{{"f.a", 0.05f}, {"f.b", 0.5f},
            {"f.c", 5.0f}}});
      }

      SECTION("[uniform.scalar.GLint]") {
         check_uniform(program, std::array<uniform<GLint>, 3>{{{"i.a", -32'767}, {"i.b", 65'536},
            {"i.c", 650'356}}});
      }

      SECTION("Testing GLuint", "[uniform.scalar.GLuint]") {
         check_uniform(program, std::array<uniform<GLuint>, 3>{{{"u.a", 15u}, {"u.b", 16u},
            {"u.c", 352u}}});
      }

      SECTION("[uniform.vec2.GLfloat]") {
         check_uniform(program, std::array<uniform<glm::vec2>, 3>{{{"v2.a", {0.05f, 0.08f}},
            {"v2.b", {0.5f, 0.8f}}, {"v2.c", {5.0f, 8.0f}}}});
      }

      SECTION("[uniform.vec2.GLint") {
         check_uniform(program, std::array<uniform<glm::ivec2>, 3>{{{"iv2.a", {7, 20}},
            {"iv2.b", {30, 40}}, {"iv2.c", {0, 0}}}});
      }

      SECTION("[uniform.vec2.GLuint") {
         check_uniform(program, std::array<uniform<glm::uvec2>, 3>{{{"uv2.a", {10, 20}},
            {"uv2.b", {30, 40}}, {"uv2.c", {0, 0}}}});
      }

      SECTION("[uniform.vec3.GLfloat]") {
         check_uniform(program, std::array<uniform<glm::vec3>, 3>{{{"v3.a", {0.05f, 0.08f, 0.02f}},
            {"v3.b", {0.5f, 0.8f, 0.2f}}, {"v3.c", {5.0f, 8.0f, 2.0f}}}});
      }

      SECTION("[uniform.vec3.GLint") {
         check_uniform(program, std::array<uniform<glm::ivec3>, 3>{{{"iv3.a", {7, 20, 123}},
            {"iv3.b", {30, 40, 5342}}, {"iv3.c", {0, 0, 0}}}});
      }

      SECTION("[uniform.vec3.GLuint") {
         check_uniform(program, std::array<uniform<glm::uvec3>, 3>{{{"uv3.a", {10, 20, 123}},
            {"uv3.b", {30, 40, 5342}}, {"uv3.c", {0, 0, 0}}}});
      }

      SECTION("[uniform.vec4.GLfloat]") {
         check_uniform(program, std::array<uniform<glm::vec4>, 3>{{{"v4.a", {0.05f, 0.08f, 0.02f, 0.06f}},
            {"v4.b", {0.5f, 0.8f, 0.2f, 0.6f}}, {"v4.c", {5.0f, 8.0f, 2.0f, 6.0f}}}});
      }

      SECTION("[uniform.vec4.GLint") {
         check_uniform(program, std::array<uniform<glm::ivec4>, 3>{{{"iv4.a", {7, 20, 123, 42}},
            {"iv4.b", {30, 40, 5342, 135}}, {"iv4.c", {0, 0, 0, 0}}}});
      }

      SECTION("[uniform.vec4.GLuint") {
         check_uniform(program, std::array<uniform<glm::uvec4>, 3>{{{"uv4.a", {10, 20, 123, 21}},
            {"uv4.b", {30, 40, 5342, 32}}, {"uv4.c", {0, 0, 0, 0}}}});
      }

      SECTION("[uniform.mat2x2]") {
         check_uniform(program, std::array<uniform<glm::mat2x2>, 3>{{
            {"m2x2.a", {1.0f, 2.0f,
                        3.0f, 4.0f}},
            {"m2x2.b", {0.1f, 0.2f,
                        0.3f, 0.4f}},
            {"m2x2.c", {1.6f, 44.f,
                       -9.0f, 7.3f}}
         }});
      }

      SECTION("[uniform.mat2x3]") {
         check_uniform(program, std::array<uniform<glm::mat2x3>, 3>{{
            {"m2x3.a", {1.0f, 2.0f, 3.0f,
                        3.0f, 4.0f, 5.0f}},
            {"m2x3.b", {0.1f, 0.2f, 0.3f,
                        0.3f, 0.4f, 0.5f}},
            {"m2x3.c", {1.6f, 44.f, 7.8f,
                       -9.0f, 7.3f, 95.f}}
         }});
      }

      SECTION("[uniform.mat2x4]") {
         check_uniform(program, std::array<uniform<glm::mat2x4>, 3>{{
            {"m2x4.a", {1.0f, 2.0f, 3.0f, 4.0f,
                        3.0f, 4.0f, 5.0f, 6.0f}},
            {"m2x4.b", {0.1f, 0.2f, 0.3f, 0.4f,
                        0.3f, 0.4f, 0.5f, 0.6f}},
            {"m2x4.c", {1.6f, 44.f, 89.f, 95.f,
                       -9.0f, 7.3f, -6.f, -2.f}}
         }});
      }

      SECTION("[uniform.mat3x2]") {
         check_uniform(program, std::array<uniform<glm::mat3x2>, 3>{{
            {"m3x2.a", {1.0f, 2.0f,
                        3.0f, 4.0f,
                        5.0f, 6.0f}},
            {"m3x2.b", {0.1f, 0.2f,
                        0.3f, 0.4f,
                        0.5f, 0.6f}},
            {"m3x2.c", {1.6f, 44.f,
                       -9.0f, 7.3f,
                        8.5f, 8.5f}}
         }});
      }

      SECTION("[uniform.mat3x3]") {
         check_uniform(program, std::array<uniform<glm::mat3x3>, 3>{{
            {"m3x3.a", {1.0f, 2.0f, 3.0f,
                        3.0f, 4.0f, 5.0f,
                        5.0f, 6.0f, 7.0f}},
            {"m3x3.b", {0.1f, 0.2f, 0.3f,
                        0.3f, 0.4f, 0.5f,
                        0.5f, 0.6f, 0.7f}},
            {"m3x3.c", {1.6f, 44.f, 65.f,
                       -9.0f, 7.3f, -2.f,
                        8.5f, 8.5f, 3.8f}}
         }});
      }

      SECTION("[uniform.mat3x4]") {
         check_uniform(program, std::array<uniform<glm::mat3x4>, 3>{{
            {"m3x4.a", {1.0f, 2.0f, 3.0f,
                        3.0f, 4.0f, 5.0f,
                        5.0f, 6.0f, 7.0f,
                        8.0f, 9.0f, 10.f}},
            {"m3x4.b", {0.1f, 0.2f, 0.3f,
                        0.3f, 0.4f, 0.5f,
                        0.5f, 0.6f, 0.7f,
                        0.8f, 0.9f, 1.0f}},
            {"m3x4.c", {1.6f, 44.f, 65.f,
                       -9.0f, 7.3f, -2.f,
                        8.5f, 8.5f, 3.8f,
                        89.f, 31.f, 4.0f}}
         }});
      }

      SECTION("[uniform.mat4x2]") {
         check_uniform(program, std::array<uniform<glm::mat4x2>, 3>{{
            {"m4x2.a", {1.0f, 2.0f,
                        3.0f, 4.0f,
                        5.0f, 6.0f,
                        7.0f, 8.0f}},
            {"m4x2.b", {0.1f, 0.2f,
                        0.3f, 0.4f,
                        0.5f, 0.6f,
                        0.7f, 0.8f}},
            {"m4x2.c", {1.6f, 44.f,
                       -9.0f, 7.3f,
                        8.5f, 8.5f,
                        9.2f, 89.f}}
         }});
      }

      SECTION("[uniform.mat4x3]") {
         check_uniform(program, std::array<uniform<glm::mat4x3>, 3>{{
            {"m4x3.a", {1.0f, 2.0f, 3.0f,
                        3.0f, 4.0f, 5.0f,
                        5.0f, 6.0f, 7.0f,
                        8.0f, 9.0f, 10.f}},
            {"m4x3.b", {0.1f, 0.2f, 0.3f,
                        0.3f, 0.4f, 0.5f,
                        0.5f, 0.6f, 0.7f,
                        0.8f, 0.9f, 1.0f}},
            {"m4x3.c", {1.6f, 44.f, 65.f,
                       -9.0f, 7.3f, -2.f,
                        8.5f, 8.5f, 3.8f,
                        89.f, 31.f, 4.0f}}
         }});
      }

      SECTION("[uniform.mat4x4]") {
         check_uniform(program, std::array<uniform<glm::mat4x4>, 3>{{
            {"m4x4.a", {1.0f, 2.0f, 3.0f, 4.0f,
                        3.0f, 4.0f, 5.0f, 6.0f,
                        5.0f, 6.0f, 7.0f, 8.0f,
                        8.0f, 9.0f, 10.f, 11.f}},
            {"m4x4.b", {0.1f, 0.2f, 0.3f, 4.0f,
                        0.3f, 0.4f, 0.5f, 5.0f,
                        0.5f, 0.6f, 0.7f, 6.0f,
                        0.8f, 0.9f, 1.0f, 7.0f}},
            {"m4x4.c", {1.6f, 44.f, 65.f, 8.0f,
                       -9.0f, 7.3f, -2.f, 9.0f,
                        8.5f, 8.5f, 3.8f, 10.f,
                        89.f, 31.f, 4.0f, 11.f}}
         }});
      }
   });
}

