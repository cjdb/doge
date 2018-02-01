#ifndef STATIC_OBJECTS_HPP
#define STATIC_OBJECTS_HPP

#include "doge/gl/shader_binary.hpp"
#include "doge/gl/texture.hpp"
#include "doge/gl/uniform.hpp"
#include "experimental/ranges/concepts"
#include "gl/gl_core.hpp"
#include <glm/vec3.hpp>
#include <initializer_list>
#include <string>
#include <vector>

namespace ranges = std::experimental::ranges;
using ranges::Regular;

inline Regular triangle = std::vector{
   std::make_shared<std::vector<GLfloat>>(std::vector<GLfloat>{
         0.5f,  0.5f, 0.0f,
         1.0f, -0.5f, 0.0f,
         0.0f, -0.5f, 0.0f
   }),
   std::make_shared<std::vector<GLfloat>>(std::vector<GLfloat>{
         0.0f, -0.5f, 0.0f,
      -1.0f, -0.5f, 0.0f,
      -0.5f,  0.5f, 0.0f
   })
};

inline Regular binary_triangles = std::make_shared<std::vector<GLfloat>>(std::vector<GLfloat>{
    0.5f,  0.5f, 0.0f,
    1.0f, -0.5f, 0.0f,
    0.0f, -0.5f, 0.0f,
   -0.5f,  0.5f, 0.0f,
   -1.0f, -0.5f, 0.0f
});

inline Regular coloured_triangle = std::make_shared<std::vector<GLfloat>>(std::vector<GLfloat>{
   // vertex             // colour
    0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f,
   -0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,
    0.0f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f
});

inline Regular coloured_rectangle = std::make_shared<std::vector<GLfloat>>(std::vector<GLfloat>{
   // positions         // colours         // texture coords
    0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,  1.0f, 1.0f,   // top right
    0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,  1.0f, 0.0f,   // bottom right
   -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,  0.0f, 0.0f,   // bottom left
   -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,  0.0f, 1.0f    // top left
});

inline Regular rectangle = std::make_shared<std::vector<GLfloat>>(std::vector<GLfloat>{
   // positions         // texture coords
    0.5f,  0.5f, 0.0f,   1.0f, 1.0f,   // top right
    0.5f, -0.5f, 0.0f,   1.0f, 0.0f,   // bottom right
   -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,   // bottom left
   -0.5f,  0.5f, 0.0f,   0.0f, 1.0f    // top left
});

inline Regular cube = std::make_shared<std::vector<GLfloat>>(std::initializer_list<GLfloat>{
   -0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
   -0.5f,  0.5f, -0.5f,
   -0.5f, -0.5f, -0.5f,

   -0.5f, -0.5f,  0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
   -0.5f,  0.5f,  0.5f,
   -0.5f, -0.5f,  0.5f,

   -0.5f,  0.5f,  0.5f,
   -0.5f,  0.5f, -0.5f,
   -0.5f, -0.5f, -0.5f,
   -0.5f, -0.5f, -0.5f,
   -0.5f, -0.5f,  0.5f,
   -0.5f,  0.5f,  0.5f,

    0.5f,  0.5f,  0.5f,
    0.5f,  0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,

   -0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f, -0.5f,  0.5f,
   -0.5f, -0.5f,  0.5f,
   -0.5f, -0.5f, -0.5f,

   -0.5f,  0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
    0.5f,  0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
   -0.5f,  0.5f,  0.5f,
   -0.5f,  0.5f, -0.5f,
});

inline Regular textured_cubes = std::make_shared<std::vector<GLfloat>>(std::vector<GLfloat>{
   // positions         // texture coords
   -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
   -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
   -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
   -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

   -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
   -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
   -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

   -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
   -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
   -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
});

inline Regular cube_with_normal = std::make_shared<std::vector<GLfloat>>(std::vector<GLfloat>{
    // positions            // normals              // texture coords
   -0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     0.0f, 0.0f,
    0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f, 1.0f,
    0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f, 1.0f,
   -0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     0.0f, 1.0f,
   -0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     0.0f, 0.0f,

   -0.5f, -0.5f,  0.5f,     0.0f,  0.0f, 1.0f,      0.0f, 0.0f,
    0.5f, -0.5f,  0.5f,     0.0f,  0.0f, 1.0f,      1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,     0.0f,  0.0f, 1.0f,      1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,     0.0f,  0.0f, 1.0f,      1.0f, 1.0f,
   -0.5f,  0.5f,  0.5f,     0.0f,  0.0f, 1.0f,      0.0f, 1.0f,
   -0.5f, -0.5f,  0.5f,     0.0f,  0.0f, 1.0f,      0.0f, 0.0f,

   -0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,     1.0f, 0.0f,
   -0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,     1.0f, 1.0f,
   -0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,     0.0f, 1.0f,
   -0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,     0.0f, 1.0f,
   -0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,     0.0f, 0.0f,
   -0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,     1.0f, 0.0f,

    0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     1.0f, 1.0f,
    0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f, 1.0f,
    0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f, 1.0f,
    0.5f, -0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     0.0f, 0.0f,
    0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f, 0.0f,

   -0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,     0.0f, 1.0f,
    0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,     1.0f, 1.0f,
    0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f, 0.0f,
    0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f, 0.0f,
   -0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     0.0f, 0.0f,
   -0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,     0.0f, 1.0f,

   -0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,     0.0f, 1.0f,
    0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,     1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f, 0.0f,
   -0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     0.0f, 0.0f,
   -0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,     0.0f, 1.0f
});

inline Regular cube_positions = std::vector<glm::vec3>{
   glm::vec3( 0.0f,  0.0f,  0.0f),
   glm::vec3( 2.0f,  5.0f, -15.0f),
   glm::vec3(-1.5f, -2.2f, -2.5f),
   glm::vec3(-3.8f, -2.0f, -12.3f),
   glm::vec3( 2.4f, -0.4f, -3.5f),
   glm::vec3(-1.7f,  3.0f, -7.5f),
   glm::vec3( 1.3f, -2.0f, -2.5f),
   glm::vec3( 1.5f,  2.0f, -2.5f),
   glm::vec3( 1.5f,  0.2f, -1.5f),
   glm::vec3(-1.3f,  1.0f, -1.5f)
};

inline auto make_awesomeface(const doge::shader_binary& program)
{
   auto tex = std::vector<doge::texture2d>{
      {"resources/container.jpg", {doge::texture_wrap_t::repeat, doge::texture_wrap_t::repeat},
         doge::minmag_t::linear, doge::minmag_t::linear, 0},
      {"resources/awesomeface.png", {doge::texture_wrap_t::repeat, doge::texture_wrap_t::repeat},
         doge::minmag_t::linear, doge::minmag_t::linear, 1}
   };

   program.use([&tex, &program]{
      for (ranges::Integral i = decltype(tex.size()){}; i != tex.size(); ++i) {
         doge::uniform(program, ("texture" + std::to_string(i)).c_str(), static_cast<GLint>(i));
      }
   });

   return tex;
}

#endif // STATIC_OBJECTS_HPP
