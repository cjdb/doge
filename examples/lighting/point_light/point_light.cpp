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
#include "../../static_objects.hpp"
#include "doge/doge.hpp"
#include <filesystem>

namespace fs = std::filesystem;

namespace doge {
   shader_binary make_shader(fs::path const& basic_shader_path)
   {
      return shader_binary{{
         std::make_pair(shader_source::vertex, basic_shader_path + ".vert"),
         std::make_pair(shader_source::fragment, basic_shader_path + ".frag")
      }};
   }

   texture2d make_texture_map(fs::path const& texture_path)
   {
      return {texture_path, {doge::texture_wrap_t::repeat, doge::texture_wrap_t::repeat},
         doge::minmag_t::linear, doge::minmag_t::linear}};
   }
} // namespace doge

namespace demo {
   class cube {
   public:
      cube(std::vector<float> const& vertices, fs::path const& basic_shader_path,
         fs::path const& basic_map_path)
         : vertices_{gl::ARRAY_BUFFER, gl::STATIC_DRAW, vertices, 8, {3, 3, 2}},
           program_{doge::make_shader(basic_shader_path)},
           diffuse_{doge::make_texture_map(basic_map_path + "_diffuse.png")},
           specular_{doge::make_texture_map(basic_map_path + "_specular.png")}
      {}

      template <ranges::Invocable F>
      void draw(const F& f)
      {
         program_.use([&, this]{
            ranges::invoke(f);

            diffuse_map_.bind(gl::TEXTURE0);
            specular_map_.bind(gl::TEXTURE1);

            vertices_.bind([&]{
               for (auto i = doge::zero(cube_positions); i < ranges::size(cube_positions); ++i) {
                  model = doge::mat4{1.0f}
                        | doge::translate(cube_positions[i])
                        | doge::rotate(doge::as_radians<float>(20.0f * i), {0.5f, 1.0f, 0.5f});
                  vertices_.draw(doge::vertex::triangles, 0, 36);
               }
            });
         });
      }
   private:
      doge::vertex vertices_;
      doge::shader_binary program_;
      doge::texture2d diffuse_map_;
      doge::texture2d specular_map_;
   };
}

int main()
{
   auto engine = doge::engine{};
   auto cube = demo::cube{};
}