#include <doge/gl/shader_binary.hpp>
#include <experimental/ranges/concepts>

namespace doge {
   namespace ranges = std::experimental::ranges;
   using std::vector, std::pair, std::string;

   shader_binary::shader_binary(const vector<shader_source>& shaders)
      : index_{gl::CreateProgram()}
   {
      for (const auto& i : shaders)
         gl::AttachShader(index_, i);
      gl::LinkProgram(index_);

      ranges::SignedIntegral successful = 0;
      if (gl::GetProgramiv(index_, gl::LINK_STATUS, &successful); not successful) {
         ranges::Regular log = std::string(512, '\0');
         gl::GetProgramInfoLog(index_, log.size(), nullptr, log.data());
         throw std::runtime_error{log};
      }
   }

   vector<shader_source>
   shader_binary::compile_shaders(const vector<pair<shader_source::type, string>>& paths)
   {
      ranges::Regular shaders = vector<shader_source>{};
      shaders.reserve(paths.size());
      for (const auto& i : paths)
         shaders.emplace_back(i.first, i.second);
      return shaders;
   }
} // namespace doge
