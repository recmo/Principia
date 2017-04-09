#include "Parser.h"
#include "Assembler.h"
#include <exception>
#include <iostream>

int main(int argc, char* argv[])
{
	try {
		std::cerr << "Principia 0.0.1\n";
		std::cerr << "Unicode " << Parser::unicode_version() << "\n";
		std::cerr << "ICU " << Parser::icu_version() << "\n";
		std::cerr << "AsmJit " << Assembler::asmjit_version() << "\n";

		Parser{}.parse_file("test");
	}
	catch(std::exception& e) {
    std::cerr << "Uncaught exception: " << e.what() << "\n"
      "Terminating!\n";
    std::exit(EXIT_FAILURE);
  }
  return EXIT_SUCCESS;
}
