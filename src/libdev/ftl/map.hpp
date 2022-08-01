/*
 * M A P . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#ifndef _FTL_MAP_HPP
#define _FTL_MAP_HPP

#include "ftl/collect.hpp"

/* //////////////////////////////////////////////////////////////// */

// FtlMap represents an abstract mapping bwteen items
// of type Key and items of type Value.

template <	class KEY, class VALUE >
class FtlMap
: public FtlAssociativeCollection< KEY, VALUE >
{
public:

	FtlMap() {};

	typedef typename FtlAssociativeCollection< KEY, VALUE >::Key Key;
	typedef typename FtlAssociativeCollection< KEY, VALUE >::Value Value;

	// inherited from FtlContainer...
	// virtual bool contains( const Key& ) const = 0;

	Value& operator []( const Key& key )
	{
		PRE( this->contains( key ) );
		return (Value&)doSubscript( key );
	}

	const Value& operator []( const Key& key ) const
	{
		PRE( this->contains( key ) );
		return doSubscript( key );
	}

	// inherited from FtlAssociativeCollection...
	// virtual void add( const FtlPair< KEY, VALUE >& addMe ) = 0;
	// PRE( not contains( addMe ) );
	// POST( contains( addMe ) );

	// virtual void remove( const Key& removeMe ) = 0;
	// PRE( contains( removeMe ) );
	// POST( not contains( removeMe ) );

protected:

	// PRE( contains( key ) );
	virtual const Value& doSubscript( const Key& ) const = 0;
};

/* //////////////////////////////////////////////////////////////// */

#endif	/* #ifndef _FTL_MAP_HPP	*/

/* End MAP.HPP ******************************************************/
