/*
 * S E T . H P P
 * Copyright (c) Charybdis Limited, 1996, 1997. All Rights Reserved.
 */

/*
    ctl_set

    version of the STL set class which can be derived from - i.e.
    it has a virtual destructor

    ctl_multiset

    version of the STL set class which can be derived from - i.e.
    it has a virtual destructor
*/

#ifndef _CTL_SET_HPP
#define _CTL_SET_HPP

//////////////////////////////////////////////////////////////////////

#include <set>

//////////////////////////////////////////////////////////////////////

template < class KEY, class COMPARE >
class ctl_set
: public std::set< KEY, COMPARE >
{
protected:

	typedef std::set< KEY, COMPARE > rep_type;

public:


	///////////////////////////////

	using std::set< KEY, COMPARE >::set;

	virtual ~ctl_set(){};

	ctl_set< KEY, COMPARE >&
	operator =( const ctl_set< KEY, COMPARE >& );
};

//////////////////////////////////////////////////////////////////////

template < class KEY, class COMPARE >
class ctl_multiset
: public std::multiset< KEY, COMPARE >
{
protected:

	typedef std::multiset< KEY, COMPARE > rep_type;

public:

	///////////////////////////////
    using std::multiset< KEY, COMPARE >::multiset;

	virtual ~ctl_multiset() {};

	ctl_multiset< KEY, COMPARE >&
	operator =( const ctl_multiset< KEY, COMPARE >& );

    // accessors //////////////////
};

//////////////////////////////////////////////////////////////////////

#ifdef  _INLINE
//    #include "ctl/set.itp"
 //   #include "ctl/set.itf"
#endif

#ifdef _INSTANTIATE_TEMPLATE_CLASSES
 //   #include "ctl/set.ctp"
#endif

#ifdef _INSTANTIATE_TEMPLATE_FUNCTIONS
//    #include "ctl/set.ctf"
#endif

//////////////////////////////////////////////////////////////////////

#endif	/* #ifndef _CTL_SET_HPP	*/


