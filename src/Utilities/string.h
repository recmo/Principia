#pragma once
#include <Utilities/pretty_print.h>
#include <string>
#include <sstream>
#include <vector>

std::vector<std::wstring> tokenize(const std::wstring& string);

bool startsWith(const std::wstring& string, const std::wstring& prefix);

template<class T>
std::wstring toString(const T& value)
{
	std::wstringstream ss;
	ss << value;
	return ss.str();
}

template<class T>
std::wstring& operator<<(std::wstring& out, const T& value)
{
	out.append(toString(value));
	return out;
}

class parse_error: public std::invalid_argument {
public:
	parse_error()
	: std::invalid_argument("Could not parse argument.") { }
};

template<class T>
T parse(const std::wstring &string)
{
	std::wistringstream ss(string);
	T value;
	ss >> value;
	if(!ss.eof()) {
		throw parse_error{};
	}
	return value;
}

template<class T>
std::wstring typeName()
{
	return toString(typeid(T));
}
