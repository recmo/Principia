#pragma once
#include <ostream>
#include <memory>
#include <vector>
#include <set>
#include <map>

template<class T>
std::wostream& operator<<(std::wostream& out, const std::unique_ptr<T>& p)
{
	if(p == nullptr)
		return out << L"null";
	else
		return out << *p;
}

template<class T>
std::wostream& operator<<(std::wostream& out, const std::shared_ptr<T>& p)
{
	if(p == nullptr)
		return out << L"null";
	else
		return out << *p;
}

template<class T>
std::wostream& operator<<(std::wostream& out, const std::weak_ptr<T>& p)
{
	return out << p.lock();
}

template<class T>
std::wostream& operator<<(std::wostream& out, const std::vector<T>& v)
{
	typename std::vector<T>::const_iterator it = v.begin();
	out << L"[";
	if(it != v.end()) for(;;) {
		out << *it;
		if(++it == v.end())
			break;
		out << L", ";
	}
	out << L"]";
	return out;
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
