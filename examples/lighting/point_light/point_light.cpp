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
#include "../../cube.hpp"
#include "../../static_objects.hpp"
#include "doge/doge.hpp"
#include <string>

class lamp {
public:
   template <ranges::Invocable F>
   void draw(F const& f)
   {
      program_.use([this, &f]{
         ranges::invoke(f);
         vertices_.bind([this]{
            vertices_.draw(doge::vertex::triangles, 0, 36);
         });
      });
   }
private:
   doge::shader_binary program_ = doge::make_shader("light_source");
   doge::vertex vertices_{gl::ARRAY_BUFFER, gl::STATIC_DRAW, cube_with_normal, 8, {3}};
};

int main()
{
   auto engine = doge::engine{doge::depth_test::enabled};
   auto cube = demo::cube{cube_with_normal, "example.point_light", "resources/container"};
   auto camera = doge::camera{};

   auto light = doge::point_light{cube.program(),
      {"light.position", doge::vec3{}},
      {"light.ambient", doge::unit<doge::vec3> * 0.2f},
      {"light.diffuse", doge::unit<doge::vec3> * 0.5f},
      {"light.specular", doge::unit<doge::vec3> * 0.5f},
      {"light.attenuation.constant", 1.0f},
      {"light.attenuation.linear", 0.09f},
      {"light.attenuation.quadratic", 0.032f}
   };
   auto l = lamp{};

   engine.clear_colour(0.1f, 0.1f, 0.1f);
   engine.play([&]{
      namespace hid = doge::hid;
      hid::on_key_press<hid::keyboard>(GLFW_KEY_ESCAPE, [&engine]{ engine.close(); });
      camera.mouselook(hid::mouse::cursor_delta());

      hid::on_key_down<hid::keyboard>('W', [&camera]{
         camera.move(doge::cardinality::north, doge::camera::free); });
      hid::on_key_down<hid::keyboard>('A', [&camera]{
         camera.move(doge::cardinality::west, doge::camera::free); });
      hid::on_key_down<hid::keyboard>('S', [&camera]{
         camera.move(doge::cardinality::south, doge::camera::free); });
      hid::on_key_down<hid::keyboard>('D', [&camera]{
         camera.move(doge::cardinality::east, doge::camera::free); });

      hid::on_key_down<hid::keyboard>(GLFW_KEY_UP, [&light]{
         light = -doge::vec3{0.2f, 1.0f, 0.3f}; });
      hid::on_key_down<hid::keyboard>(GLFW_KEY_DOWN, [&light]{
         light = doge::vec3{0.2f, 1.0f, 0.3f}; });
      hid::on_key_down<hid::keyboard>(GLFW_KEY_LEFT, [&light]{
         light = -doge::vec3{1.0f, 0.3f, 0.2f}; });
      hid::on_key_down<hid::keyboard>(GLFW_KEY_RIGHT, [&light]{
         light = doge::vec3{0.3f, 0.2f, 1.0f}; });

      cube.draw([&camera, &cube, &engine](auto& view, auto& position, auto& model) {
         view = camera.view();
         position = camera.position();
         model = doge::mat4{1.0f};
         doge::uniform(cube.program(), "projection", false,
            camera.project(engine.screen().aspect_ratio(), 0.1f, 100.0f));
      });
      l.draw([]{});
   });
}
