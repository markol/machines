/*
 * C S 2 Q W R I T . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PhysPathFindingQueueWriter

    This class exists purely to write out details of the
    current path finding queues.
*/

#ifndef _PHYS_CS2QWRIT_HPP
#define _PHYS_CS2QWRIT_HPP

#include "base/base.hpp"

class PhysConfigSpace2d;

class PhysPathFindingQueueWriter
// Canonical form revoked
{
public:
    PhysPathFindingQueueWriter( const PhysConfigSpace2d& configSpace );
    ~PhysPathFindingQueueWriter( void );

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const PhysPathFindingQueueWriter& t );

    PhysPathFindingQueueWriter( const PhysPathFindingQueueWriter& );
    PhysPathFindingQueueWriter& operator =( const PhysPathFindingQueueWriter& );

    const PhysConfigSpace2d& configSpace_;
};


#endif

/* End QWRITE.HPP ***************************************************/
