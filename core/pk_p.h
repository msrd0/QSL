#include "qslnamespace.h"

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

}
