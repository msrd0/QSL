#pragma once

/// @file pk.h This file contains the `spis::PrimaryKey` class and the required
/// templated handler to increment and compare primary keys.

#include "spisnamespace.h"
#include "pk_p.h"

namespace spis {

/**
 * This class is used to store the last-used primary key of a table and to determine
 * the next primary key that should be used.
 */
template<typename T, typename H = PrimaryKeyHandlerImpl<T>>
class SPIS_PUBLIC PrimaryKey
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
