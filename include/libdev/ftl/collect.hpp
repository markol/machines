/*
 * C O L L E C T . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#ifndef _FTL_COLLECT_HPP
#define _FTL_COLLECT_HPP

#include "ftl/ftl.hpp"
#include "ftl/contain.hpp"

/* //////////////////////////////////////////////////////////////// */

// FtlCollection is a Ftl Container which supports insertion
// and removal of items.

template <	class KEY,
			class VALUE,
			class KEY_VALUE_ASSOCIATION >
class FtlCollection
: public FtlContainer< KEY >
{
public:

	typedef VALUE					Value;
	typedef KEY_VALUE_ASSOCIATION	KeyValueAssoc;

	FtlCollection() {};

	virtual ~FtlCollection() {};

	// inherited from FtlContainer...
	// virtual bool contains( const Key& key ) const = 0;

	virtual void add( const KeyValueAssoc& addMe ) = 0;
	// PRE( not contains( addMe ) );
	// POST( contains( addMe ) );

	virtual void remove( const typename FtlContainer< KEY>::Key& removeMe ) = 0;
	// PRE( contains( removeMe ) );
	// POST( not contains( removeMe ) );
};

/* //////////////////////////////////////////////////////////////// */

// FtlValueCollection is a collection of items of type Value.

template < class VALUE >
class FtlValueCollection
: public FtlCollection< VALUE, VALUE, VALUE >
{
public:

	FtlValueCollection() {};

	virtual ~FtlValueCollection() {};

	// inherited from FtlContainer...
	// virtual bool contains( const Value& ) const = 0;

	// inherited from FtlCollection...
	// virtual void add( const Value& addMe ) = 0;
	// PRE( not contains( addMe ) );
	// POST( contains( addMe ) );

	// virtual void remove( const Value& removeMe ) = 0;
	// PRE( contains( removeMe ) );
	// POST( not contains( removeMe ) );
};

/* //////////////////////////////////////////////////////////////// */

// FtlAssociativeCollection is a collection of items of type
// Value, indexed by type Key.

template < class KEY, class VALUE >
class FtlAssociativeCollection
: public FtlCollection< KEY, VALUE, FtlPair< KEY, VALUE > >
{
public:

	FtlAssociativeCollection() {};

	virtual ~FtlAssociativeCollection() {};

	// inherited from FtlContainer...
	// virtual bool contains( const Key& ) const = 0;

	// inherited from FtlCollection...
	// virtual void add( const FtlPair< KEY, VALUE >& addMe ) = 0;
	// PRE( not contains( addMe ) );
	// POST( contains( addMe ) );

	// virtual void remove( const Key& removeMe ) = 0;
	// PRE( contains( removeMe ) );
	// POST( not contains( removeMe ) );
};

/* //////////////////////////////////////////////////////////////// */

#endif	/* #ifndef _FTL_COLLECT_HPP	*/

/* End COLLECT.HPP **************************************************/
