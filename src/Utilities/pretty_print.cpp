#include "pretty_print.h"
#include <Unicode/convert.h>
#include <cxxabi.h>

std::wostream& operator<<(std::wostream& out, const std::type_info& p)
{
	const char* mangled = p.name();
	int status = 0;
	char* demangled = abi::__cxa_demangle(mangled, nullptr, nullptr, &status);
	if (demangled != nullptr) {
		out << decodeUtf8(std::string{demangled});
		free(demangled);
	} else {
		// demangling failed. Output function name as a C function with
		// no arguments.
		out << decodeUtf8(std::string{mangled});
	}
	return out;
}
