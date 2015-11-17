#pragma once
#include <stdexcept>

class assertion_failed: public std::runtime_error {
public:
	assertion_failed(const std::string& string)
	: std::runtime_error(string) { }
};
