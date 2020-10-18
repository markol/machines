/*
 * O B J E X P L O . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysObjectExplosion

    Implements a generic exploding animation
*/

#ifndef _MACHPHYS_OBJEXPLO_HPP
#define _MACHPHYS_OBJEXPLO_HPP

#include "base/base.hpp"

#include "machphys/machphys.hpp"

//forward refs
class W4dEntity;

class MachPhysObjectExplosion
// Canonical form revoked
{
public:
    //ctor. pObject is the item being exploded. Does NOT delete pObject at any time.
    MachPhysObjectExplosion( W4dEntity* pObject );

    ~MachPhysObjectExplosion();

    //Initiate the explosion animation at startTime, returning its duration.
    PhysRelativeTime explode( const PhysAbsoluteTime& startTime, MachPhys::DemolitionType type); 
    PhysRelativeTime explode( const PhysAbsoluteTime& startTime); 

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachPhysObjectExplosion& t );

    MachPhysObjectExplosion( const MachPhysObjectExplosion& );
    MachPhysObjectExplosion& operator =( const MachPhysObjectExplosion& );

    //Do the sinking into ground thing
    void scaleObject( PhysAbsoluteTime startTime, PhysRelativeTime duration, MATHEX_SCALAR startScale, MATHEX_SCALAR endScale );
    void sinkObject( PhysAbsoluteTime startTime, const PhysRelativeTime& duration, const MATHEX_SCALAR& height );

    //data members
    W4dEntity* pObject_; //The object being exploded
};

#ifdef _INLINE
    #include "machphys/objexplo.ipp"
#endif


#endif

/* End OBJEXPLO.HPP *************************************************/
