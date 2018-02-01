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
#ifndef DOGE_UTILITY_FILE_IO_HPP
#define DOGE_UTILITY_FILE_IO_HPP

//#include <filesystem>
#include <string>

namespace doge {
   template <typename T>
   T from_file(const std::string&);

   template <>
   std::string from_file<std::string>(const std::string& path);
} // namespace doge

#endif // DOGE_UTILITY_FILE_IO_HPP
