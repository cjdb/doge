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

int main()
{
   auto engine = doge::engine{doge::depth_test::enabled};
   auto camera = doge::camera{};

   using namespace doge::angle_literals;
   auto light = doge::spot_lighting{doge::vec3{1.2f, 1.0f, 2.0f}, doge::unit<doge::vec3> * 0.2f,
      doge::unit<doge::vec3> * 0.5f, doge::unit<doge::vec3>, 1.0f, 0.09f, 0.032f, doge::front,
      5.5_deg, 9.5_deg};
   auto cube = demo::cube{cube_with_normal, "example.spot_light", "resources/container",
      light, "light.position", "light.ambience", "light.diffuse", "light.specular"};

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

      cube.program().use([&]{
         hid::on_key_down<hid::keyboard>('U', [&light]{
            light += doge::vec3{0.0f, 0.01f, 0.0f}; });
         hid::on_key_down<hid::keyboard>('I', [&light]{
            light -= doge::vec3{0.0f, 0.0f, 0.01f}; });
         hid::on_key_down<hid::keyboard>('O', [&light]{
            light -= doge::vec3{0.0f, 0.01f, 0.0f}; });
         hid::on_key_down<hid::keyboard>('J', [&light]{
            light -= doge::vec3{0.01f, 0.0f, 0.0f}; });
         hid::on_key_down<hid::keyboard>('K', [&light]{
            light += doge::vec3{0.0f, 0.0f, 0.01f}; });
         hid::on_key_down<hid::keyboard>('L', [&light]{
            light += doge::vec3{0.01f, 0.0f, 0.0f}; });
      });

      auto const camera_projection = camera.project(engine.screen().aspect_ratio(), 0.1f, 100.0f);
      cube.draw([&camera, &cube, &engine, &camera_projection, &light](auto& view,
         auto& light_position, auto& model, auto& projection) {
         view = camera.view();
         light_position = camera.position();
         model = doge::mat4{1.0f};
         projection = camera_projection;
         doge::uniform(cube.program(), "view_position", camera.position());
         doge::uniform(cube.program(), "light.direction", camera.direction());
      }, light);
   });
}
