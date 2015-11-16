#pragma once
#include <stdexcept>

class assertion_failed: public std::runtime_error {
public:
	assertion_failed(const std::string& string)
	: std::runtime_error(string) { }
};

/// Assert implementation that will throw a assertion_failed
/// @returns true if the expression succeeded
#define assert(expr) ( \
		(expr) ? true : \
		assert_fail(__STRING(expr), __FILE__, __LINE__, __PRETTY_FUNCTION__) \
	)

bool assert_fail(const char* expression,
	const char* file, const int line, const char* function);
