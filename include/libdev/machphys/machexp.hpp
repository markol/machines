/*
 * M A C H E X P . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysMachineExplosion

    Make a machine explode complete with fireballs, sound effects
    and flying parts
*/

#ifndef _MACHPHYS_MACHEXP_HPP
#define _MACHPHYS_MACHEXP_HPP

#include "base/base.hpp"

#include "phys/phys.hpp"
#include "ctl/vector.hpp"

#include "phys/motplan.hpp"
#include "phys/accetumb.hpp"

#include "machphys/mexpdata.hpp"
#include "machphys/plansurf.hpp"

class W4dLink;
class MachPhysMachine;

class MachPhysMachineExplosion
{
public:
    typedef ctl_vector< W4dLink* >    Links;

    MachPhysMachineExplosion(
        MachPhysMachine* pMachine,
        const MachPhysMachineExplosionData& data,
        const PhysAbsoluteTime& time );

    ~MachPhysMachineExplosion( void );

    void CLASS_INVARIANT;

private:
    // Operation deliberately revoked
    MachPhysMachineExplosion( const MachPhysMachineExplosion& );

    // Operation deliberately revoked
    MachPhysMachineExplosion& operator =( const MachPhysMachineExplosion& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysMachineExplosion& );

    void shootOffLink(
        MachPhysMachine* pMachine,
        const MachPhysMachineExplosionData::LinkData& linkData,
		const PhysRelativeTime& startTimeOffset,
        const PhysRelativeTime& time,
        MATHEX_SCALAR pseudoWeight,
        MATHEX_SCALAR maxSize,
        const MexPoint3d& explosionCenter );

    void createRandomTransform(
      const MexPoint3d& explosionPosition,
      const MexTransform3d& objectTransform,
      MexTransform3d* pResult );


    void createMainFireball(
        MachPhysMachine* pMachine,
        const PhysRelativeTime& startTimeOffset,
        const PhysRelativeTime& duration );

    void createSecondaryFireball(
        MachPhysMachine* pMachine,
        const MachPhysMachineExplosionData::LinkData& linkData,
        const PhysRelativeTime& startTimeOffset,
        const PhysRelativeTime& duration,
        MATHEX_SCALAR maxSize );


    void setupMotionPlans(
        MachPhysMachine* pMachine,
        const MachPhysMachineExplosionData::LinkData& linkData,
        const PhysRelativeTime& startTimeOffset,
        const PhysRelativeTime& explosionDuration,
        MATHEX_SCALAR pseudoWeight,
        const MexPoint3d& explosionCenter );

    double randomDouble(
        MATHEX_SCALAR lowerLimit,
        MATHEX_SCALAR upperLimit );
        
    int randomInt(
        int lowerLimit,
        int upperLimit );

    int randomInt(
        int upperLimit );

    friend  void MachPhysPlanetSurface::parseHeader( UtlLineTokeniser* pParser );
    static  MexVec3& acceleration( void );
};


#endif

/* End MACHEXP.HPP **************************************************/
