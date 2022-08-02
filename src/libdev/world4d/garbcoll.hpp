/*
 * G A R B C O L L . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dGarbageCollector

    This singleton class is used to queue the deletion of W4dEntities at or soon after
    a specified time.
    This is especially useful for example, for temporary objects which appear as part
    of special effects.
    The client must ensure that any entity registered here is not deleted by any other
    means, or double deletion will occur. However, this object does not delete its
    entities on destruction at the end of the program - they would be destructed
    automatically by their parent destructors.
*/

#ifndef _WORLD4D_GARBCOLL_HPP
#define _WORLD4D_GARBCOLL_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"
#include "ctl/vector.hpp"
#include "ctl/pvector.hpp"

//forward refs
class W4dEntity;

class W4dGarbageCollector
// Canonical form revoked
{
public:
    //  Singleton class
    static W4dGarbageCollector& instance();
    ~W4dGarbageCollector();

    //queues pEntity to be deleted at or soon after time
    void add( W4dEntity* pEntity, const PhysAbsoluteTime& time );

    //dequeues pEntity 
    void remove( W4dEntity* pEntity);

    //Deletes any queued objects.
    //Called as required by Wd4Manager::update()
    void collect();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const W4dGarbageCollector& t );

private:
    W4dGarbageCollector( const W4dGarbageCollector& );
    W4dGarbageCollector& operator =( const W4dGarbageCollector& );
    bool operator ==( const W4dGarbageCollector& );

    W4dGarbageCollector();

    //data members
    PhysAbsoluteTime nextDeleteTime_; //Earliest scheduled time of a deletion
    ctl_vector< PhysAbsoluteTime > times_; //Time of deletion for each entry
    ctl_pvector< W4dEntity > entities_; //Objects to be deleted
};

#endif

/* End GARBCOLL.HPP *************************************************/
