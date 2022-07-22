/*
 * F U N C T I O N . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved.
 */

// Extensions to standard library function objects.

#ifndef _CTL_FUNCTION_HPP
#define _CTL_FUNCTION_HPP

#include "ctl/utility.hpp"

//////////////////////////////////////////////////////////////////////

template < class FIRST, class SECOND >
struct ctl_select1st
: public std::unary_function< std::pair< FIRST, SECOND >, FIRST >
{
	FIRST& operator ()( std::pair< FIRST, SECOND >& p ) const
	{
		return p.first;
	}

	const FIRST& operator ()( const std::pair< FIRST, SECOND >& p ) const
	{
		return p.first;
	}
};

//////////////////////////////////////////////////////////////////////

template < class FIRST, class SECOND >
struct ctl_select2nd
: public std::unary_function< std::pair< FIRST, SECOND >, SECOND >
{
	SECOND& operator ()( std::pair< FIRST, SECOND >& p ) const
	{
		return p.second;
	}

	const SECOND& operator ()( const std::pair< FIRST, SECOND >& p ) const
	{
		return p.second;
	}
};

//////////////////////////////////////////////////////////////////////

#endif	// #ifndef _CTL_FUNCTION_HPP

/* End **************************************************************/
