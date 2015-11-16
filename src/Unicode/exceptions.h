#pragma once
#include <stdexcept>
#include <Unicode/convert.h>

#define UNICODE_WRAP_EXCEPTION(name) \
	class name: public std::name { \
	public: \
		name(const std::wstring& s) \
		: std::name(encodeLocal(s)) { } \
	};

UNICODE_WRAP_EXCEPTION(logic_error)
UNICODE_WRAP_EXCEPTION(domain_error)
UNICODE_WRAP_EXCEPTION(invalid_argument)
UNICODE_WRAP_EXCEPTION(length_error)
UNICODE_WRAP_EXCEPTION(out_of_range)
UNICODE_WRAP_EXCEPTION(runtime_error)
UNICODE_WRAP_EXCEPTION(range_error)
UNICODE_WRAP_EXCEPTION(overflow_error)
UNICODE_WRAP_EXCEPTION(underflow_error)

#undef UNICODE_WRAP_EXCEPTION
