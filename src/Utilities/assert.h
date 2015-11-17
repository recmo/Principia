// No "#pragma once" because we want to be able to override
// other assert implementations. This requires multiple inclusions.

#include <Utilities/assertion_failed.h>

/// Assert implementation that will throw a assertion_failed
/// @returns true if the expression succeeded
#ifdef assert
#undef assert
#endif
#define assert(expr) ( \
		(expr) ? true : \
		assert_fail(__STRING(expr), __FILE__, __LINE__, __PRETTY_FUNCTION__) \
	)

bool assert_fail(const char* expression,
	const char* file, const int line, const char* function);
