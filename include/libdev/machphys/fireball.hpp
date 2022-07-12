/*
 * F I R E B A L L . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysFireball

    A fireball consisting of a turn to face polygon and a set of bitmaps
*/

#ifndef _MACHPHYS_FIREBALL_HPP
#define _MACHPHYS_FIREBALL_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "world4d/sprite3d.hpp"

#include "machphys/machphys.hpp"

class MachPhysFireballImpl;

class MachPhysFireball : public W4dSprite3d
{
public:
    //  Construct the fireball and set it going at the given startTime
    MachPhysFireball(
        W4dEntity* pParent,
        const MexTransform3d& localTransform,
        MachPhysFireballType fireballType,
        MATHEX_SCALAR size,
        MATHEX_SCALAR depthOffset,
        const PhysAbsoluteTime& startTime,
        const PhysRelativeTime& duration );

    //  Construct the fireball but don't start it going yet        
    MachPhysFireball(
        W4dEntity* pParent,
        const MexTransform3d& localTransform,
        MachPhysFireballType fireballType,
        MATHEX_SCALAR size,
        MATHEX_SCALAR depthOffset,
        const PhysRelativeTime& duration );
        
    ~MachPhysFireball();

    //  Start the fireball at the given time
    void startFireball( const PhysAbsoluteTime& startTime );
    
    //  Load all of the fireball bitmaps in advance so that the
    //  fireball can be created 'instantly'
    
    static  void preload();

    //  return a random fireball type out of those available
    static  MachPhysFireballType randomFireball();
    
    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT( MachPhysFireball );
    PER_FRIEND_READ_WRITE( MachPhysFireball );
    
private:
    // Operations deliberately revoked
    MachPhysFireball( const MachPhysFireball& );
    MachPhysFireball& operator =( const MachPhysFireball& );
    bool operator ==( const MachPhysFireball& );

	MachPhysFireballImpl* pImpl_;
};

PER_DECLARE_PERSISTENT( MachPhysFireball );

#endif

/* End FIREBALL.HPP *************************************************/
