/*
 * R B T R E E . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#ifndef _FTL_RBTREE_HPP
#define _FTL_RBTREE_HPP

#include "ctl/map.hpp"

#include "ftl/map.hpp"

/* //////////////////////////////////////////////////////////////// */

// FtlRedBlackTree uses a search tree structure to map
// from items of type Key to items of type Value.

template <	class KEY, class VALUE >
class FtlRedBlackTree
: public FtlMap< KEY, VALUE >
{
private:

	typedef ctl_map< KEY, VALUE, less< KEY > > RepType;
	RepType	rep_;

public:
	
	FtlRedBlackTree() {};

	typedef RepType::const_iterator const_iterator;

	const_iterator begin() const { return rep_.begin(); }
	const_iterator end() const { return rep_.end(); }

	// inherited from FtlContainer...
	virtual bool contains( const Key& key ) const
	{
		return rep_.find( key ) != rep_.end();
	}

	// inherited from FtlAssociativeCollection...
	virtual void add( const FtlPair< Key, Value >& addMe )
	{
		PRE( not contains( addMe.first ) );
		rep_.insert( RepType::value_type( addMe ) );
		POST( contains( addMe.first ) );
	}

	virtual void remove( const Key& key )
	{
		PRE( contains( key ) );
		RepType::iterator i = rep_.find( key );
		ASSERT( i != rep_.end(), logic_error() );
		rep_.erase( i );
		POST( not contains( key ) );
	}

protected:

	// PRE( contains( key ) );
	virtual const Value& doSubscript( const Key& key ) const
	{
		return rep_[ key ];
	};

};

/* //////////////////////////////////////////////////////////////// */

#endif	/* #ifndef _FTL_RBTREE_HPP	*/

/* End RBTREE.HPP ***************************************************/