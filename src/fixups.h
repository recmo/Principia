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
#include<boost/foreach.hpp>

/// @brief Assert implementation that throws exceptions
#ifdef DEBUG
#define assert(expr) ((expr) ? true : assert_fail(__STRING(expr), __FILE__, __LINE__, __PRETTY_FUNCTION__));
#else
#define assert(expr) ;
#endif

/// @brief Foreach implementation using boost
#define foreach BOOST_FOREACH
/*
/// Given a STL style collection with begin() and end()
/// and a variable name var it will itterate trough all
/// items and set var to each item, as a value.
/// Additionally the current itterator and current numerical
/// index can be accessed at var_itterator and var_index.
/// No nonexisting itterators are dereferenced
/// The expression collection is evaluated only once
/// begin() and end() are called only once
/// Does not break case lables since there are no local vars
/// Supports break and continue statements
/// All overhead easily optimized away
/// Requires "auto"
// #define foreach(var, collection) for(auto var##_collection = collection, var##_begin = var##_collection.begin(), var##_end = var##_collection.end(), var##_itterator = var##_begin, var##_outer_run = true; var##_outer_run; var##_outer_run = false) if(var##_itterator != var##_end) for(auto var = *var##_itterator, var##_index = 0; var##_itterator != var##_end; var = (++var##_itterator != var##_end) ? *var##_itterator : var, ++var##_index)
*/

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
using std::set;
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
bool eraseByValue(std::vector<T>& v, const T& value)
{
	auto it = std::find(v.begin(), v.end(), value);
	if(it == v.end()) return false;
	v.erase(it);
	return true;
}

template<class T>
bool contains(const std::vector<T>& v, const T& value)
{
	return std::find(v.begin(), v.end(), value) != v.end();
}

template<class T>
bool contains(const std::set<T>& v, const T& value)
{
	return std::find(v.begin(), v.end(), value) != v.end();
}

template<class Key, class Value>
bool contains(const std::map<Key, Value>& map, const Key& key)
{
	return map.find(key) != map.end();
}

template<class Key, class Value>
bool tryGet(const std::map<Key, Value>& map, const Key& key, Value& value)
{
	typename std::map<Key, Value>::const_iterator it = map.find(key);
	if(it == map.end()) return false;
	value = it->second;
	return true;
}

template<class Key, class Value>
bool tryGetKey(const std::map<Key, Value>& map, const Value& value, Key& key)
{
	for(typename std::map<Key, Value>::const_iterator it = map.begin(); it != map.end(); ++it)
	{
		if(it->second == value)
		{
			key = it->first;
			return true;
		}
	}
	return false;
}


template<class T>
void insertUnion(set<T>& target, const set<T>& insert)
{
	foreach(T element, insert)
		target.insert(element);
}

template<class T>
set<T> setUnion(const set<T>& a, const set<T>& b)
{
	set<T> u = a;
	return insertUnion<T>(u, b);
}

template<class T>
set<T> intersection(const set<T>& a, const set<T>& b)
{
	set<T> i;
	foreach(T e, a)
		if(contains<T>(b, e))
			i.insert(e);
	return i;
}

template<class T>
set<T> setMinus(const set<T>& a, const set<T>& b)
{
	set<T> i;
	foreach(T e, a)
		if(!contains<T>(b, e))
			i.insert(e);
	return i;
}

template<class T>
std::wostream& operator<<(std::wostream& out, const std::set<T>& v)
{
	typename std::set<T>::const_iterator it = v.begin();
	out << L"{";
	if(it != v.end()) for(;;)
	{
		out << *it;
		if(++it == v.end()) break;
		out << L", ";
	}
	out << L"}";
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
bool parse(const std::wstring &string, T& value)
{
	std::wistringstream ss(string);
	return (ss >> value) && ss.eof();
}

template<class T>
std::wstring toString(const T& value)
{
	std::wstringstream ss;
	ss << value;
	return ss.str();
}

inline bool startsWith(const string& a, const string& b)
{
	if(b.size() < a.size()) return false;
	for(string::const_iterator i = a.begin(), j = b.begin(); i != a.end(); i++, j++)
		if(*i != *j) return false;
	return true;
}

sint32 Main(const std::vector<std::wstring>& args);
sint32 main(sint32 argc, char* argv[]);
bool assert_fail(const char* expression, const char* file, const int line, const char* function);
