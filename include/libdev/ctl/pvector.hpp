/*
 * P V E C T O R . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

/*
    ctl_pvector

    This is a version of the STL vector class that only contains
    pointers to objects.
*/

#ifndef _CTL_PVECTOR_HPP
#define _CTL_PVECTOR_HPP

#include "ctl/nbvector.hpp"
#include "base/persist.hpp"

//	Just inherit everything from ctl_nb_vector

template < class T >
class ctl_pvector : public ctl_nb_vector< T* >
{
public:

    using ctl_nb_vector< T* >::ctl_nb_vector;

    //  This used to be marked inline due to a watcom bug.
	//  this appears to have been fixed (inadvertantly) incase
	//  it comes back the comment remains.
    PER_MEMBER_PERSISTENT_DEFAULT_VIRTUAL( ctl_pvector );
};

PER_DEFINE_PERSISTENT_INLINE_T1( ctl_pvector );

template < class T >
bool operator ==( const ctl_pvector< T >&, const ctl_pvector< T >& );

//#ifdef _INSTANTIATE_TEMPLATE_FUNCTIONS
    #include "ctl/pvector.ctf"
//#endif

#endif
