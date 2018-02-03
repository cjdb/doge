#include <array>
#include <doge/utility/type_traits.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat2x2.hpp>
#include <glm/mat2x3.hpp>
#include <glm/mat2x4.hpp>
#include <glm/mat3x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat3x4.hpp>
#include <glm/mat4x2.hpp>
#include <glm/mat4x3.hpp>
#include <glm/mat4x4.hpp>

int main()
{
   {
      using doge::is_one_of_v;
      static_assert(is_one_of_v<int, int>);
      static_assert(is_one_of_v<int, int, double>);
      static_assert(is_one_of_v<int, double, int>);
      static_assert(is_one_of_v<int, double, int, long>);

      static_assert(not is_one_of_v<int, double>);
      static_assert(not is_one_of_v<int, double, long>);
   }
   {
      using doge::detail::is_glm_vec;
      static_assert(not is_glm_vec<int>);
      static_assert(not is_glm_vec<std::array<int, 2>>);
      static_assert(is_glm_vec<glm::vec2>);
      static_assert(is_glm_vec<glm::vec3>);
      static_assert(is_glm_vec<glm::vec4>);
      static_assert(is_glm_vec<glm::ivec2>);
      static_assert(is_glm_vec<glm::ivec3>);
      static_assert(is_glm_vec<glm::ivec4>);
      static_assert(is_glm_vec<glm::uvec2>);
      static_assert(is_glm_vec<glm::uvec3>);
      static_assert(is_glm_vec<glm::uvec4>);
   }
   {
      using doge::detail::is_vec2;
      static_assert(is_vec2<glm::vec2>);
      static_assert(is_vec2<glm::ivec2>);
      static_assert(is_vec2<glm::uvec2>);

      static_assert(not is_vec2<int>);
      static_assert(not is_vec2<glm::vec3>);
      static_assert(not is_vec2<glm::ivec3>);
      static_assert(not is_vec2<glm::uvec3>);
      static_assert(not is_vec2<glm::vec4>);
      static_assert(not is_vec2<glm::ivec4>);
      static_assert(not is_vec2<glm::uvec4>);
   }
   {
      using doge::detail::is_vec3;
      static_assert(is_vec3<glm::vec3>);
      static_assert(is_vec3<glm::ivec3>);
      static_assert(is_vec3<glm::uvec3>);

      static_assert(not is_vec3<int>);
      static_assert(not is_vec3<glm::vec2>);
      static_assert(not is_vec3<glm::ivec2>);
      static_assert(not is_vec3<glm::uvec2>);
      static_assert(not is_vec3<glm::vec4>);
      static_assert(not is_vec3<glm::ivec4>);
      static_assert(not is_vec3<glm::uvec4>);
   }
   {
      using doge::detail::is_vec4;
      static_assert(is_vec4<glm::vec4>);
      static_assert(is_vec4<glm::ivec4>);
      static_assert(is_vec4<glm::uvec4>);

      static_assert(not is_vec4<int>);
      static_assert(not is_vec4<glm::vec2>);
      static_assert(not is_vec4<glm::ivec2>);
      static_assert(not is_vec4<glm::uvec2>);
      static_assert(not is_vec4<glm::vec3>);
      static_assert(not is_vec4<glm::ivec3>);
      static_assert(not is_vec4<glm::uvec3>);
   }
   {
      using doge::detail::type_is_glm_vec;
      static_assert(not type_is_glm_vec<int, int>);
      static_assert(not type_is_glm_vec<float, int>);

      static_assert(type_is_glm_vec<float, glm::vec2>);
      static_assert(type_is_glm_vec<int, glm::ivec3>);
      static_assert(type_is_glm_vec<unsigned int, glm::uvec4>);

      static_assert(not type_is_glm_vec<int, glm::vec4>);
      static_assert(not type_is_glm_vec<glm::ivec2, glm::ivec2>);
   }
   {
      using doge::detail::is_glm_matrix;
      static_assert(is_glm_matrix<glm::mat2x2>);
      static_assert(is_glm_matrix<glm::mat2x3>);
      static_assert(is_glm_matrix<glm::mat2x4>);
      static_assert(is_glm_matrix<glm::mat3x2>);
      static_assert(is_glm_matrix<glm::mat3x3>);
      static_assert(is_glm_matrix<glm::mat3x4>);
      static_assert(is_glm_matrix<glm::mat4x2>);
      static_assert(is_glm_matrix<glm::mat4x3>);
      static_assert(is_glm_matrix<glm::mat4x4>);

      static_assert(not is_glm_matrix<int>);
   }
   {
      using doge::detail::is_glm_GLfloat;

      static_assert(is_glm_GLfloat<GLfloat>);
      static_assert(is_glm_GLfloat<glm::vec2>);
      static_assert(is_glm_GLfloat<glm::vec3>);
      static_assert(is_glm_GLfloat<glm::vec4>);
      static_assert(is_glm_GLfloat<glm::mat2x2>);
      static_assert(is_glm_GLfloat<glm::mat2x3>);
      static_assert(is_glm_GLfloat<glm::mat2x4>);
      static_assert(is_glm_GLfloat<glm::mat3x2>);
      static_assert(is_glm_GLfloat<glm::mat3x3>);
      static_assert(is_glm_GLfloat<glm::mat3x4>);
      static_assert(is_glm_GLfloat<glm::mat4x2>);
      static_assert(is_glm_GLfloat<glm::mat4x3>);
      static_assert(is_glm_GLfloat<glm::mat4x4>);

      static_assert(not is_glm_GLfloat<glm::ivec2>);
      static_assert(not is_glm_GLfloat<glm::ivec3>);
      static_assert(not is_glm_GLfloat<glm::ivec4>);
      static_assert(not is_glm_GLfloat<glm::uvec2>);
      static_assert(not is_glm_GLfloat<glm::uvec3>);
      static_assert(not is_glm_GLfloat<glm::uvec4>);

      static_assert(not is_glm_GLfloat<GLhalf>);
      static_assert(not is_glm_GLfloat<GLdouble>);
      static_assert(not is_glm_GLfloat<long double>);
   }
   {
      using doge::detail::is_glm_GLint;
      static_assert(is_glm_GLint<GLint>);
      static_assert(is_glm_GLint<glm::ivec2>);
      static_assert(is_glm_GLint<glm::ivec3>);
      static_assert(is_glm_GLint<glm::ivec4>);

      static_assert(not is_glm_GLint<glm::vec2>);
      static_assert(not is_glm_GLint<glm::vec3>);
      static_assert(not is_glm_GLint<glm::vec4>);
      static_assert(not is_glm_GLint<glm::mat2x2>);
      static_assert(not is_glm_GLint<glm::mat2x3>);
      static_assert(not is_glm_GLint<glm::mat2x4>);
      static_assert(not is_glm_GLint<glm::mat3x2>);
      static_assert(not is_glm_GLint<glm::mat3x3>);
      static_assert(not is_glm_GLint<glm::mat3x4>);
      static_assert(not is_glm_GLint<glm::mat4x2>);
      static_assert(not is_glm_GLint<glm::mat4x3>);
      static_assert(not is_glm_GLint<glm::mat4x4>);

      static_assert(not is_glm_GLint<glm::uvec2>);
      static_assert(not is_glm_GLint<glm::uvec3>);
      static_assert(not is_glm_GLint<glm::uvec4>);

      static_assert(not is_glm_GLint<GLuint>);
      static_assert(not is_glm_GLint<GLint64>);
      static_assert(not is_glm_GLint<GLfloat>);
   }
   {
      using doge::detail::is_glm_GLuint;
      static_assert(is_glm_GLuint<GLuint>);
      static_assert(is_glm_GLuint<glm::uvec2>);
      static_assert(is_glm_GLuint<glm::uvec3>);
      static_assert(is_glm_GLuint<glm::uvec4>);

      static_assert(not is_glm_GLuint<glm::vec2>);
      static_assert(not is_glm_GLuint<glm::vec3>);
      static_assert(not is_glm_GLuint<glm::vec4>);
      static_assert(not is_glm_GLuint<glm::mat2x2>);
      static_assert(not is_glm_GLuint<glm::mat2x3>);
      static_assert(not is_glm_GLuint<glm::mat2x4>);
      static_assert(not is_glm_GLuint<glm::mat3x2>);
      static_assert(not is_glm_GLuint<glm::mat3x3>);
      static_assert(not is_glm_GLuint<glm::mat3x4>);
      static_assert(not is_glm_GLuint<glm::mat4x2>);
      static_assert(not is_glm_GLuint<glm::mat4x3>);
      static_assert(not is_glm_GLuint<glm::mat4x4>);

      static_assert(not is_glm_GLuint<glm::ivec2>);
      static_assert(not is_glm_GLuint<glm::ivec3>);
      static_assert(not is_glm_GLuint<glm::ivec4>);

      static_assert(not is_glm_GLuint<GLint>);
      static_assert(not is_glm_GLuint<GLuint64>);
      static_assert(not is_glm_GLuint<GLfloat>);
   }
}
