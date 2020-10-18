/*
 * F T L . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#ifndef _FTL_FTL_HPP
#define _FTL_FTL_HPP

/* //////////////////////////////////////////////////////////////// */

template < class KEY, class VALUE >
class FtlPair
// parameterwise cannonical
: public std::pair< KEY, VALUE >
{
public:

	typedef	KEY		Key;
	typedef	VALUE	Value;

	FtlPair( const Key& k, const Value& v )
	: std::pair< KEY, VALUE >( k, v ) {}

	//operator std::pair< const Key, Value >() const
	operator std::pair< const Key, Value >() const
	{
		//return std::pair< const Key, Value >( std::pair::first, std::pair::second );
		return std::pair< const Key, Value >
            ( std::pair< const Key, Value >::first, std::pair< const Key, Value >::second );
	}
};

/* //////////////////////////////////////////////////////////////// */

#endif	/* #ifndef _FTL_FTL_HPP	*/

/* End FTL.HPP ******************************************************/
