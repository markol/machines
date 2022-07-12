/*
 * S T F L A M E S . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */


/*
    Flames

    A brief description of the class should go in here
*/

#ifndef _STFLAMES_HPP
#define _STFLAMES_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "phys/phys.hpp"
#include "render/render.hpp"
#include "world4d/spin.hpp"
#include "machphys/flames.hpp"

class RenTexture;
class MexTransform3d;
class MexVec3;

class MachPhysSTFFlame : public W4dSpinSprite
{
public:

    MachPhysSTFFlame(
        W4dEntity* pParent,
        const MexTransform3d& localTransform,
        MachPhysFlameType flameType,
        MATHEX_SCALAR width,
        MATHEX_SCALAR height,
        const MexVec3& spinAxis,
        const PhysRelativeTime& duration );
        
    ~MachPhysSTFFlame();

    //  Start the flame at the given time, and turning off after the duration
    //defined in the constructor.
    void startFlame( const PhysAbsoluteTime& startTime );

    //Applies the material plan to get the flame burning from startTime,
    //and makes immediately visible. However does not apply a visibility plan to turn
    //the flame on/off. The client is left to do this.
    void startBurning( const PhysAbsoluteTime& startTime );
    
    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT( MachPhysSTFFlame );
    PER_FRIEND_READ_WRITE( MachPhysSTFFlame );

private:
    // Operations deliberately revoked
    MachPhysSTFFlame( const MachPhysSTFFlame& );
    MachPhysSTFFlame& operator =( const MachPhysSTFFlame& );
    bool operator ==( const MachPhysSTFFlame& );

    //data members
    PhysRelativeTime duration_;
    MachPhysFlameType flameType_;

	//Has this flame started ie is it visible?
	bool startedBurning_;
};

PER_DECLARE_PERSISTENT( MachPhysSTFFlame );

#endif

/* End STFLAMES.HPP ***************************************************/
