#pragma once
#include <vector>
#include <set>
#include <map>
#include <algorithm>


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


template<typename T>
int indexOf(const std::vector<T>& list, const T& item)
{
	auto i = std::find(list.begin(), list.end(), item);
	if(i == list.end())
		throw std::runtime_error(encodeLocal(L"Could not find index of item."));
	return std::distance(list.begin(), i);
}


template<class Key, class Value>
bool tryGet(const std::map<Key, Value>& map, const Key& key, Value& value)
{
	typename std::map<Key, Value>::const_iterator it = map.find(key);
	if(it == map.end())
		return false;
	value = it->second;
	return true;
}

template<class Key, class Value>
bool tryGetKey(const std::map<Key, Value>& map, const Value& value, Key& key)
{
	for(typename std::map<Key, Value>::const_iterator it = map.begin(); it != map.end(); ++it) {
		if(it->second == value) {
			key = it->first;
			return true;
		}
	}
	return false;
}


template<class T>
void insertUnion(std::set<T>& target, const std::set<T>& insert)
{
	for(T element: insert)
		target.insert(element);
}

template<class T>
std::set<T> setUnion(const std::set<T>& a, const std::set<T>& b)
{
	std::set<T> u = a;
	return insertUnion<T>(u, b);
}

template<class T>
std::set<T> intersection(const std::set<T>& a, const std::set<T>& b)
{
	std::set<T> i;
	for(T e: a)
		if(contains<T>(b, e))
			i.insert(e);
	return i;
}

template<class T>
std::set<T> setMinus(const std::set<T>& a, const std::set<T>& b)
{
	std::set<T> i;
	for(T e: a)
		if(!contains<T>(b, e))
			i.insert(e);
	return i;
}
