#pragma once
#include <ostream>
#include <memory>
#include <vector>
#include <set>
#include <map>
#include <typeinfo>

template<class T, class D>
std::wostream& operator<<(std::wostream& out, const std::unique_ptr<T,D>& p)
{
	if(p == nullptr)
		return out << L"null";
	else
		return out << *p;
}

template<class T, class D>
std::ostream& operator<<(std::ostream& out, const std::unique_ptr<T,D>& p)
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
std::ostream& operator<<(std::ostream& out, const std::shared_ptr<T>& p)
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
std::ostream& operator<<(std::ostream& out, const std::weak_ptr<T>& p)
{
	return out << p.lock();
}

template<class A, class B>
std::wostream& operator<<(std::wostream& out, const std::pair<A, B>& p)
{
	return out << L"(" << p.first << L", " << p.second << L")";
}

template<class A, class B>
std::ostream& operator<<(std::ostream& out, const std::pair<A, B>& p)
{
	return out << L"(" << p.first << L", " << p.second << L")";
}

template<class T, class A>
std::wostream& operator<<(std::wostream& out, const std::vector<T,A>& v)
{
	typename std::vector<T,A>::const_iterator it = v.begin();
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

template<class T, class A>
std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& v)
{
	typename std::vector<T,A>::const_iterator it = v.begin();
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

template<class T, class C, class A>
std::wostream& operator<<(std::wostream& out, const std::set<T,C,A>& v)
{
	typename std::set<T,C,A>::const_iterator it = v.begin();
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

template<class T, class C, class A>
std::ostream& operator<<(std::ostream& out, const std::set<T,C,A>& v)
{
	typename std::set<T,C,A>::const_iterator it = v.begin();
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

template<class K, class V, class C, class A>
std::wostream& operator<<(std::wostream& out, const std::map<K,V,C,A>& v)
{
	typename std::map<K,V,C,A>::const_iterator it = v.begin();
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

template<class K, class V, class C, class A>
std::ostream& operator<<(std::ostream& out, const std::map<K,V,C,A>& v)
{
	typename std::map<K,V,C,A>::const_iterator it = v.begin();
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

std::wostream& operator<<(std::wostream& out, const std::type_info& p);

std::ostream& operator<<(std::ostream& out, const std::type_info& p);
