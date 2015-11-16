#include "assert.h"
#include <sstream>

bool assert_fail(const char* expression, const char* file, const int line, const char* function)
{
	std::stringstream message;
	message << file << ":" << line << ": ";
	message << "assertion failed: ";
	message << expression;
	throw assertion_failed(message.str());
}
