#pragma once

#include <qsl_global.h>

namespace qsl {

template<typename T>
class PrimaryKeyHandler
{
public:
	PrimaryKeyHandler()
	{
	}
	
	PrimaryKeyHandler(const T &pk)
		: _pk(pk)
	{
	}
	
	virtual bool isValid() const
	{
		return (_pk >= 0);
	}
	
	virtual T pk() const
	{
		return _pk;
	}
	
	operator T() const
	{
		return pk();
	}
	
	virtual PrimaryKeyHandler<T>& operator++()
	{
		_pk++;
		return *this;
	}
	
	virtual bool operator<(const PrimaryKeyHandler<T> &other) const
	{
		return pk() < other.pk();
	}
	
protected:
	T _pk;
};

template<typename T>
class PrimaryKeyHandlerImpl : public PrimaryKeyHandler<T>
{
public:
	PrimaryKeyHandlerImpl()
		: PrimaryKeyHandler<T>()
	{
	}
	
	PrimaryKeyHandlerImpl(const T &pk)
		: PrimaryKeyHandler<T>(pk)
	{
	}
};


template<typename T, typename H = PrimaryKeyHandlerImpl<T>>
class PrimaryKey
{
public:
	PrimaryKey()
	{
	}
	
	void used(const T &t)
	{
		H used = t;
		if (used.isValid() && (handler < used))
			handler = used;
	}
	
	T next()
	{
		++handler;
		return handler;
	}
	
private:
	H handler;
};

}
