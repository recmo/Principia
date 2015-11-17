#pragma once
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <stdexcept>

template<class T, class A>
bool eraseByValue(std::vector<T,A>& v, const T& value)
{
	auto it = std::find(v.begin(), v.end(), value);
	if(it == v.end()) return false;
	v.erase(it);
	return true;
}

template<class T, class A>
bool contains(const std::vector<T,A>& v, const T& value)
{
	return std::find(v.begin(), v.end(), value) != v.end();
}

template<class T, class C, class A>
bool contains(const std::set<T,C,A>& set, const T& value)
{
	return set.find(value) != set.end();
}

template<class K, class V, class C, class A>
bool contains(const std::map<K,V,C,A>& map, const K& key)
{
	return map.find(key) != map.end();
}

class not_found: public std::out_of_range
{
public:
	not_found(): out_of_range("Could not find item.") { }
};

template<class T, class A>
int indexOf(const std::vector<T,A>& list, const T& item)
{
	auto i = std::find(list.begin(), list.end(), item);
	if(i == list.end())
		throw not_found{};
	return std::distance(list.begin(), i);
}

template<class K, class V, class C, class A>
bool tryGet(const std::map<K,V,C,A>& map, const K& key, V& value)
{
	typename std::map<K,V,C,A>::const_iterator it = map.find(key);
	if(it == map.end())
		return false;
	value = it->second;
	return true;
}

template<class K, class V, class C, class A>
bool tryGetKey(const std::map<K,V,C,A>& map, const V& value, K& key)
{
	for(typename std::map<K,V,C,A>::const_iterator it = map.begin();
		it != map.end(); ++it) {
		if(it->second == value) {
			key = it->first;
			return true;
		}
	}
	return false;
}

template<class T, class C, class A>
void insertUnion(std::set<T,C,A>& target, const std::set<T,C,A>& insert)
{
	for(T element: insert)
		target.insert(element);
}

template<class T, class C, class A>
std::set<T,C,A> setUnion(const std::set<T,C,A>& a, const std::set<T,C,A>& b)
{
	std::set<T,C,A> u = a;
	return insertUnion<T,C,A>(u, b);
}

template<class T, class C, class A>
std::set<T,C,A> intersection(const std::set<T,C,A>& a, const std::set<T,C,A>& b)
{
	std::set<T,C,A> i;
	for(T e: a)
		if(contains<T,C,A>(b, e))
			i.insert(e);
	return i;
}

template<class T, class C, class A>
std::set<T,C,A> setMinus(const std::set<T,C,A>& a, const std::set<T,C,A>& b)
{
	std::set<T,C,A> i;
	for(T e: a)
		if(!contains<T,C,A>(b, e))
			i.insert(e);
	return i;
}
