/*
 * L E S S P T R . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

/*
	Implement a class for comparing pointers so that they can be
	used in maps and sets. This is necessary because pointer
	comparisons have to be done differently on different
	platforms - see Andrew Koenig's "Traps and Pitfalls" article
	in C++ Report, November - December 1996.
*/

#ifndef _BASE_LESSPTR_HPP
#define _BASE_LESSPTR_HPP

#include "base/base.hpp"
//#include <function>



template< class T >
class less_ptr : public std::binary_function< T*, T*, bool >
{
public:
	bool operator()( T* a, T* b )
	{
		return a < b;
	}
	bool operator()( const T* a, const T* b ) const
	{
		return a < b;
	}
};

#endif
