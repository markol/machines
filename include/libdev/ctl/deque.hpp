/*
 * D E Q U E . H P P
 * (c) Charybdis Limited, 1996, 1997. All Rights Reserved.
 */

/*
    ctl_deque

    This is a version of the STL deque class that can be derived from (it
    has a virtual destructor).
*/

#ifndef _CTL_DEQUE_HPP
#define _CTL_DEQUE_HPP

//////////////////////////////////////////////////////////////////////

#include "base/persist.hpp"
#include <deque>

//////////////////////////////////////////////////////////////////////

template < class T >
class ctl_deque
: public std::deque< T >
{
public:

	typedef std::deque< T >							rep_type;

    using std::deque< T >::deque;

	virtual ~ctl_deque() {};

	//ctl_deque< T >& operator =( const ctl_deque< T >& x );


    PER_MEMBER_PERSISTENT_DEFAULT_VIRTUAL( ctl_deque );

private:

};

PER_DEFINE_PERSISTENT_INLINE_T1( ctl_deque );

//////////////////////////////////////////////////////////////////////
/*
template < class T >
bool operator ==( const ctl_deque< T >&, const ctl_deque< T >& );

template < class T >
bool operator <( const ctl_deque< T >&, const ctl_deque< T >& );
*/
//////////////////////////////////////////////////////////////////////

#ifdef  _INLINE
 //   #include "ctl/deque.itp"
#endif

//#ifdef _INSTANTIATE_TEMPLATE_CLASSES
    //#include "ctl/deque.ctp"
//#endif

//#ifdef _INSTANTIATE_TEMPLATE_FUNCTIONS
    #include "ctl/deque.ctf"
//#endif

//////////////////////////////////////////////////////////////////////

#endif	/* #ifndef _CTL_DEQUE_HPP	*/
