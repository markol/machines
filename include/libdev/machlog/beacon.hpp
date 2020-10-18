/*
 * BEACON . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachLogBeacon

    Handle logical operations for a Beacon construction
*/

#ifndef _MACHLOG_BEACON_HPP
#define _MACHLOG_BEACON_HPP

#include "base/base.hpp"

#include "machlog/constron.hpp"

//Forward declarations
class MachPhysBeacon;
class MachLogRace;
class MexPoint3d;
class MexRadians;
class MachPhysBeaconData;

//Orthodox canonical (revoked)
class MachLogBeacon : public MachLogConstruction
{
public:
    //Construct smelter of deisgnated race and level at location, rotated thru angle about
    //z axis.
    MachLogBeacon( MachLogRace* pRace, uint level,
                    const MexPoint3d& location, const MexRadians& angle );

    MachLogBeacon( MachLogRace* pRace, uint level,
                    const MexPoint3d& location, const MexRadians& angle, UtlId );

    ~MachLogBeacon( void );

    //Inherited from SimActor
    virtual PhysRelativeTime update( const PhysRelativeTime& maxCPUTime,
                                     MATHEX_SCALAR clearanceFromDisplayedVolume );

	virtual const MachPhysConstructionData& constructionData() const;
	const MachPhysBeaconData& data() const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogBeacon& t );

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogBeacon );
	PER_FRIEND_READ_WRITE( MachLogBeacon );

private:
    // Operations deliberately revoked
    MachLogBeacon( const MachLogBeacon& );
    MachLogBeacon& operator =( const MachLogBeacon& );
    bool operator ==( const MachLogBeacon& );

    //Construct a new physical smelter. Used in ctor initializer list
    static MachPhysBeacon* pNewPhysBeacon( MachLogRace* pRace, uint level,
                     const MexPoint3d& location, const MexRadians& angle );

    //The physical beacon
    MachPhysBeacon* pPhysBeacon();
    const MachPhysBeacon* pPhysBeacon() const;

    //Data members
};

PER_DECLARE_PERSISTENT( MachLogBeacon );

#ifdef _INLINE
    #include "machlog/beacon.ipp"
#endif


#endif

/* End BEACON.HPP **************************************************/
