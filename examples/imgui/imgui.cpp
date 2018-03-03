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
#include "imgui/imgui.h"
#include "doge/engine.hpp"
#include "doge/hid.hpp"

namespace hid = doge::hid;

void renderer(ImDrawData* draw_data)
{
   ImGuiIO& io { ImGui::GetIO() };
   int fb_width { (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x) };
   int fb_height { (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y) };

   draw_data->ScaleClipRects(io.DisplayFramebufferScale);

   GLint last_texture;
   gl::GetIntegerv(gl::TEXTURE_BINDING_2D, &last_texture);

   GLint last_viewport[4];
   gl::GetIntegerv(gl::VIEWPORT, last_viewport);

   gl::PushAttrib(gl::ENABLE_BIT | gl::COLOR_BUFFER_BIT | gl::TRANSFORM_BIT);
   gl::Enable(gl::BLEND);
   gl::BlendFunc(gl::SRC_ALPHA, gl::ONE_MINUS_SRC_ALPHA);
   gl::Disable(gl::CULL_FACE);
   gl::Disable(gl::DEPTH_TEST);
   gl::Enable(gl::SCISSOR_TEST);
   gl::EnableClientState(gl::VERTEX_ARRAY);
   gl::EnableClientState(gl::TEXTURE_COORD_ARRAY);
   gl::EnableClientState(gl::COLOR_ARRAY);
   gl::Enable(gl::TEXTURE_2D);

   // Setup viewport, orthographic projection matrix
   gl::Viewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
   gl::MatrixMode(gl::PROJECTION);
   gl::PushMatrix();
   gl::LoadIdentity();
   gl::Ortho(0.0f, io.DisplaySize.x, io.DisplaySize.y, 0.0f, -1.0f, +1.0f);
   gl::MatrixMode(gl::MODELVIEW);
   gl::PushMatrix();
   gl::LoadIdentity();

   // Render command lists
   #define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
   for (int n = 0; n < draw_data->CmdListsCount; n++) {
      const ImDrawList* cmd_list = draw_data->CmdLists[n];
      const unsigned char* vtx_buffer = (const unsigned char*)&cmd_list->VtxBuffer.front();
      const ImDrawIdx* idx_buffer = &cmd_list->IdxBuffer.front();
      glVertexPointer(2, gl::FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer + OFFSETOF(ImDrawVert, pos)));
      glTexCoordPointer(2, gl::FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer + OFFSETOF(ImDrawVert, uv)));
      glColorPointer(4, gl::UNSIGNED_BYTE, sizeof(ImDrawVert), (void*)(vtx_buffer + OFFSETOF(ImDrawVert, col)));

      for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.size(); cmd_i++)
      {
         const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
         if (pcmd->UserCallback)
         {
               pcmd->UserCallback(cmd_list, pcmd);
         }
         else
         {
               glBindTexture(gl::TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
               glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
               glDrawElements(gl::TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? gl::UNSIGNED_SHORT : gl::UNSIGNED_INT, idx_buffer);
         }
         idx_buffer += pcmd->ElemCount;
      }
   }
   #undef OFFSETOF

   // Restore modified state
   gl::DisableClientState(gl::COLOR_ARRAY);
   gl::DisableClientState(gl::TEXTURE_COORD_ARRAY);
   gl::DisableClientState(gl::VERTEX_ARRAY);
   gl::BindTexture(gl::TEXTURE_2D, last_texture);
   gl::MatrixMode(gl::MODELVIEW);
   gl::PopMatrix();
   gl::MatrixMode(gl::PROJECTION);
   gl::PopMatrix();
   gl::PopAttrib();
   gl::Viewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
}

void setup(GLFWwindow* window)
{
   unsigned char* pixels;
   int width;
   int height;
   int display_width;
   int display_height;
   GLuint g_FontTexture;

   ImGuiIO& io { ImGui::GetIO() };

   io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);

   // Upload texture to graphics system
   GLint last_texture;
   gl::GetIntegerv(gl::TEXTURE_BINDING_2D, &last_texture);
   gl::GenTextures(1, &g_FontTexture);
   gl::BindTexture(gl::TEXTURE_2D, g_FontTexture);
   gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MIN_FILTER, gl::LINEAR);
   gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MAG_FILTER, gl::LINEAR);
   gl::TexImage2D(gl::TEXTURE_2D, 0, gl::ALPHA, width, height, 0, gl::ALPHA, gl::UNSIGNED_BYTE, pixels);

   // Get display size
   glfwGetWindowSize(window, &width, &height);
   glfwGetFramebufferSize(window, &display_width, &display_height);

   io.DisplaySize = ImVec2((float)width, (float)height);
   io.RenderDrawListsFn = renderer;
   io.Fonts->TexID = (void *)(intptr_t)g_FontTexture;

   // Restore state
   gl::BindTexture(gl::TEXTURE_2D, last_texture);
}

int main()
{
   auto engine = doge::engine{};
   setup(engine.screen().window())
   engine.play([&]{
      hid::on_key_down<hid::keyboard>(GLFW_KEY_ESCAPE, [&]{ engine.close(); });

      ImGui::NewFrame();
      ImGui::Begin("Foo bar");
      ImGui::Text("lol");
      ImGui::End();
      ImGui::Render();
   });
}