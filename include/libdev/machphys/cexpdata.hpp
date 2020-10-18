/*
 * C E X P D A T A . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysConstructionExplosionData

    Data specific to a particular level of construction
    that will cause it to explode in a pleasing manner.
*/

#ifndef _MACHPHYS_CEXPDATA_HPP
#define _MACHPHYS_CEXPDATA_HPP

#include "base/base.hpp"

#include "mathex/point3d.hpp"
#include "ctl/vector.hpp"
#include "phys/phys.hpp"

class MachPhysConstructionExplosionData
{
public:
    MachPhysConstructionExplosionData( const PhysRelativeTime& duration );
    ~MachPhysConstructionExplosionData( void );

    #include "machphys/private/exppoint.hpp"

    void    addPoint( const ExplosionPoint& point );

    typedef ctl_vector< ExplosionPoint >    ExplosionPoints;
    
    const ExplosionPoints& explosionPoints( void ) const;

    const PhysRelativeTime&    duration( void ) const;
        
    void CLASS_INVARIANT;

private:
    // Operation deliberately revoked
    MachPhysConstructionExplosionData( const MachPhysConstructionExplosionData& );

    // Operation deliberately revoked
    MachPhysConstructionExplosionData& operator =( const MachPhysConstructionExplosionData& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysConstructionExplosionData& );

    ExplosionPoints explosionPoints_;
    PhysRelativeTime        duration_;
};


#endif

/* End CEXPDATA.HPP *************************************************/

