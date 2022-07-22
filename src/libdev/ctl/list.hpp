/*
 * L I S T . H P P
 * (c) Charybdis Limited, 1996, 1997. All Rights Reserved.
 */

/*
    ctl_list

    This is a version of the STL list class that can be derived from (it
    has a virtual destructor). It also performs error checking on the
    debug version.
*/

#ifndef _CTL_LIST_HPP
#define _CTL_LIST_HPP

//////////////////////////////////////////////////////////////////////

#include "base/persist.hpp"
#include <list>

//////////////////////////////////////////////////////////////////////

template < class T >
class ctl_list
: public std::list< T >
{
protected:

	typedef std::list< T >					rep_type;

public:

    using std::list< T >::list;

	virtual ~ctl_list(){};

	//ctl_list< T >& operator =( const ctl_list< T >& );

	///////////////////////////////

    PER_MEMBER_PERSISTENT_DEFAULT_VIRTUAL( ctl_list );

};

PER_DEFINE_PERSISTENT_INLINE_T1( ctl_list );

//////////////////////////////////////////////////////////////////////

//template < class T >
//bool operator ==( const ctl_list< T >&, const ctl_list< T >& );

//template < class T >
//bool operator <( const ctl_list< T >&, const ctl_list< T >& );

//////////////////////////////////////////////////////////////////////

#ifdef   _INLINE
    #include "ctl/list.itf"
    #ifndef NDEBUG
        #include "ctl/private/listbit.itp"
        #include "ctl/private/listit.itp"
        #include "ctl/private/listcit.itp"
    #endif
#endif

#ifdef _INSTANTIATE_TEMPLATE_CLASSES
 //   #include "ctl/list.ctp"
#endif

//#ifdef _INSTANTIATE_TEMPLATE_FUNCTIONS
    #include "ctl/list.ctf"
//#endif

//////////////////////////////////////////////////////////////////////

#endif	/* ifndef	_CTL_LIST_HPP	*/

