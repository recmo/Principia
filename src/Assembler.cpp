#include "Assembler.h"
#include <asmjit/asmjit.h>

std::string Assembler::asmjit_version()
{
  return ASMJIT_VERSION_STRING;
  // TODO: Add build configuration
}

std::string Assembler::target()
{
  return "TODO";
}
