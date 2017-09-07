#pragma once
#include <string>

class Assembler {
public:
  static std::string asmjit_version();
  static std::string target();
};
