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
#include <gl/gl_core.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

namespace doge {
   class invalid_enum_error final : public std::logic_error {
   public:
      invalid_enum_error(std::string const& message)
         : std::logic_error{"invalid enum: " + message}
      {}
   };

   class invalid_value_error final : public std::logic_error {
   public:
      invalid_value_error(std::string const& message)
         : std::logic_error{"invalid value: " + message}
      {}
   };

   class invalid_operation_error final : public std::logic_error {
   public:
      invalid_operation_error(std::string const& message)
         : std::logic_error{"invalid operation error: " + message}
      {}
   };

   class stack_overflow_error final : public std::runtime_error {
   public:
      stack_overflow_error(std::string const& message)
         : std::runtime_error{"stack overflow error: " + message}
      {}
   };

   class stack_underflow_error final : public std::runtime_error {
   public:
      stack_underflow_error(std::string const& message)
         : std::runtime_error{"stack underflow error: " + message}
      {}
   };

   class out_of_memory_error final : public std::runtime_error {
   public:
      out_of_memory_error(std::string const& message)
         : std::runtime_error{"out of memory: " + message}
      {}
   };

   class invalid_framebuffer_operation_error final : public std::logic_error {
   public:
      invalid_framebuffer_operation_error(std::string const& message)
         : std::logic_error{"ivalid framebuffer operation: " + message}
      {}
   };

   void check_error(std::string const& extra_message = "")
   {
      switch (const auto error = gl::GetError(); error) {
      case gl::NO_ERROR_:
         return;
      case gl::INVALID_ENUM:
         throw invalid_enum_error{extra_message};
      case gl::INVALID_VALUE:
         throw invalid_value_error{extra_message};
      case gl::INVALID_OPERATION:
         throw invalid_operation_error{extra_message};
      case gl::STACK_OVERFLOW:
         throw stack_overflow_error{extra_message};
      case gl::STACK_UNDERFLOW:
         throw stack_underflow_error{extra_message};
      case gl::OUT_OF_MEMORY:
         throw out_of_memory_error{extra_message};
      case gl::INVALID_FRAMEBUFFER_OPERATION:
         throw invalid_framebuffer_operation_error{extra_message};
      default:
         std::cerr << "Unknown error " << error << ". The program will now terminate.\n";
         std::terminate();
      }
   }
} // namespace doge
