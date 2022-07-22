/*
 * Q U E U E . H P P
 * (c) Charybdis Limited, 1995. All Rights Reserved.
 */

/*
    ctl_queue

    This is a version of the STL queue class that can be derived from (it
    has a virtual destructor).

    ctl_priority_queue

    This is not yet implemented due to problems with the Objectspace implementation.
*/

#ifndef _CTL_QUEUE_HPP
#define _CTL_QUEUE_HPP

//#include "stdlib/private/ospace.hpp"
#include <queue>

//////////////////////////////////////////////////////////////////////

template < class CONTAINER >
class ctl_queue
: public std::queue< CONTAINER >
{
private:

	typedef std::queue< CONTAINER >	rep_type;

public:

    using std::queue< CONTAINER >::queue;
	///////////////////////////////

	virtual ~ctl_queue() {};

	///////////////////////////////

};

//////////////////////////////////////////////////////////////////////
/*
template < class CONTAINER >
bool operator ==( const ctl_queue< CONTAINER >&,
					const ctl_queue< CONTAINER >& );

template < class CONTAINER >
bool operator <( const ctl_queue< CONTAINER >&,
					const ctl_queue< CONTAINER >& );
*/
//////////////////////////////////////////////////////////////////////

#ifdef _INSTANTIATE_TEMPLATE_CLASSES
    //#include "ctl/queue.ctp"
#endif

#ifdef _INSTANTIATE_TEMPLATE_FUNCTIONS
    //#include "ctl/queue.ctf"
#endif

//////////////////////////////////////////////////////////////////////

#endif	/*	#ifndef _CTL_QUEUE_HPP	*/

