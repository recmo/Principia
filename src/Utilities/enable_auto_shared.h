#pragma once
#include <Utilities/not_null.h>
#include <memory>

template<class T>
class enable_auto_shared: public std::enable_shared_from_this<T>
{
public:
	operator not_null<std::shared_ptr<T>>()
	{ return std::enable_shared_from_this<T>::shared_from_this(); }
	operator not_null<std::shared_ptr<const T>>() const
	{ return std::enable_shared_from_this<T>::shared_from_this(); }
	operator std::shared_ptr<T>()
	{ return std::enable_shared_from_this<T>::shared_from_this(); }
	operator std::shared_ptr<const T>() const
	{ return std::enable_shared_from_this<T>::shared_from_this(); }
};
