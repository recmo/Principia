#pragma once
#include<cstring>
#include<iostream>
#include<vector>
#include<map>
#include<set>
#include<exception>
#include<stdexcept>
#include<stdint.h>
#include<locale>
#include<sstream>
#include<iterator>
#include<algorithm>
#include<typeinfo>

/// @brief Assert implementation that throws exceptions
#define assert(expr) ((expr) ? true : assert_fail(__STRING(expr), __FILE__, __LINE__, __PRETTY_FUNCTION__));

/// @brief Foreach implementation
/// Given a STL style collection with begin() and end()
/// and a variable name var it will itterate trough all
/// items and set var to each item.
/// Additionally the current itterator and current numerical
/// index can be accessed at var_itterator and var_index.
#define foreach(var, collection) for(auto var##_collection = collection, var##_itterator = var##_collection.begin(), var = *var##_itterator, var##_index = 0; var##_itterator != var##_collection.end(); ++var##_itterator, var = *var##_itterator, ++var##_index)

/// @brief Define a zero pointer
#define null 0

// Remove the _t's from elementary types…
typedef uint8_t uint8;
typedef uint32_t uint32;
typedef int32_t sint32;
typedef uint64_t uint64;
typedef int64_t sint64;
typedef wchar_t wchar;

typedef std::wstring string;
using std::vector;
using std::map;
using std::exception;
using std::wcin;
using std::wcout;
using std::wcerr;
using std::endl;
using std::flush;

template<class T>
T min(const T& a, const T& b)
{
	return (a < b) ? a : b;
}

template<class T>
T max(const T& a, const T& b)
{
	return (a > b) ? a : b;
}

template<class T>
std::wostream& operator<<(std::wostream& out, const std::vector<T>& v)
{
	typename std::vector<T>::const_iterator it = v.begin();
	out << L"[";
	if(it != v.end()) for(;;)
	{
		out << *it;
		if(++it == v.end()) break;
		out << L", ";
	}
	out << L"]";
	return out;
}

template<class T>
bool contains(const std::vector<T>& v, const T& value)
{
	return std::find(v.begin(), v.end(), value) != v.end();
}

template<class Key, class Value>
bool contains(const std::map<Key, Value>& map, const Key& value)
{
	return map.find(value) != map.end();
}

template<class T>
std::wostream& operator<<(std::wostream& out, const std::set<T>& v)
{
	typename std::set<T>::const_iterator it = v.begin();
	out << L"[";
	if(it != v.end()) for(;;)
	{
		out << *it;
		if(++it == v.end()) break;
		out << L", ";
	}
	out << L"]";
	return out;
}

template<class K, class V>
std::wostream& operator<<(std::wostream& out, const std::map<K,V>& v)
{
	typename std::map<K,V>::const_iterator it = v.begin();
	out << L"{";
	if(it != v.end()) for(;;)
	{
		out << it->first << ": " << it->second;
		if(++it == v.end()) break;
		out << L", ";
	}
	out << L"}";
	return out;
}

std::wstring decodeUtf8(const std::string& encoded);
std::string encodeUtf8(const std::wstring& plaintext);
std::wstring decodeLocal(const std::string& encoded);
std::string encodeLocal(const std::wstring& plaintext);

std::vector<std::wstring> tokenize(const std::wstring& string);

template<class T>
T parse(const std::wstring &string)
{
	T value;
	std::wstringstream ss(string);
	ss >> value;
	/// TODO: Catch parsing errors
	/// TODO: Ensure the whole string is parsed
	return value;
}

template<class T>
std::wstring toString(const T& value)
{
	std::wstringstream ss;
	ss << value;
	return ss.str();
}

sint32 Main(const std::vector<std::wstring>& args);
sint32 main(sint32 argc, char* argv[]);
bool assert_fail(const char* expression, const char* file, const int line, const char* function);
