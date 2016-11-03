#pragma once

/// @file pk.h This file contains the `qsl::PrimaryKey` class and the required
/// templated handler to increment and compare primary keys.

#include <qslnamespace.h>

namespace qsl {

/**
 * This class handles the value of a primary key. To implement a primary key
 * handler of a type that doesn't work with this implementation, write your
 * own `PrimaryKeyHandlerImpl<YourType>` instead.
 */
template<typename T>
class QSL_PRIVATE PrimaryKeyHandler
{
public:
	/** Constructs a new primary key handler with a default start value. */
	PrimaryKeyHandler()
	{
	}
	
	/** Constructs a new primary key handler with the given start value. */
	PrimaryKeyHandler(const T &pk)
		: _pk(pk)
	{
	}
	
	/** Checks whether the current primary key is valid. Currenntly implemented
	 * by checking if the value is greater or equal to 0. */
	virtual bool isValid() const
	{
		return (_pk >= 0);
	}
	
	/** Returns the current primary key. */
	virtual T pk() const
	{
		return _pk;
	}
	
	/** Returns the current primary key. */
	operator T() const
	{
		return pk();
	}
	
	/** Increments the primary key. */
	virtual PrimaryKeyHandler<T>& operator++()
	{
		_pk++;
		return *this;
	}
	
	/** Compares two primary keys. */
	virtual bool operator<(const PrimaryKeyHandler<T> &other) const
	{
		return pk() < other.pk();
	}
	
protected:
	/** The current primary key. */
	T _pk;
};

/**
 * This class is the default implementation of a primary key handler. In fact
 * it simply overwrites `PrimaryKeyHandler<T>` without changing it's behaviour.
 * To implement a primary key handler for a different type of primary keys, write
 * your own `PrimaryKeyHandlerImpl<YourType>`.
 */
template<typename T>
class QSL_PRIVATE PrimaryKeyHandlerImpl : public PrimaryKeyHandler<T>
{
public:
	/** Constructs a new primary key handler with a default start value. */
	PrimaryKeyHandlerImpl()
		: PrimaryKeyHandler<T>()
	{
	}
	
	/** Constructs a new primary key handler with the given start value. */
	PrimaryKeyHandlerImpl(const T &pk)
		: PrimaryKeyHandler<T>(pk)
	{
	}
};


/**
 * This class is used to store the last-used primary key of a table and to determine
 * the next primary key that should be used.
 */
template<typename T, typename H = PrimaryKeyHandlerImpl<T>>
class QSL_PUBLIC PrimaryKey
{
public:
	/** Constructs a new primary key with the handler-default start value. */
	PrimaryKey()
	{
	}
	
	/** Marks the given key as used. If the key is greater that the current one,
	 * the last-used primary key is adjusted. */
	void used(const T &t)
	{
		H used = t;
		if (used.isValid() && (handler < used))
			handler = used;
	}
	
	/** Increments the primary key and returns it. */
	T next()
	{
		++handler;
		return handler;
	}
	
private:
	H handler;
};

}
