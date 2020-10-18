/*
 * G E N E D A T A . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysGeneralData

    A data object for storing generic data defined in parmdata.dat
*/

#ifndef _MACHPHYS_GENEDATA_HPP
#define _MACHPHYS_GENEDATA_HPP

#include "base/base.hpp"
#include "mathex/mathex.hpp"
#include "phys/phys.hpp"
#include "machphys/machphys.hpp"

class MachPhysGeneralDataImpl;
class MexRadians;

class MachPhysGeneralData
// Canonical form revoked
{
public:
    ~MachPhysGeneralData();

    //Controls rate at which camera turns when looking up/down in 1st person
    MexRadians firstPersonLookUpDownRate() const;

    //Min/Max angle camera turns when looking up/down in 1st person
    MexRadians firstPersonLookUpDownMinAngle() const;
    MexRadians firstPersonLookUpDownMaxAngle() const;

    //Limiting values for the machine turn rates in 1st person
    MexRadians firstPersonMaxFastTurnRate() const;
    MexRadians firstPersonMaxSlowTurnRate() const;

    //	The distance before a turn point to start
    //  rotating to face the new direction
    MATHEX_SCALAR startRotationDistance() const;

	// the time after a direct user order in which a machine is to be treated as though in defcon HIGH	
	PhysRelativeTime virtualDefConInterval() const;

    //  The maximum gradient a non-glider machine can move up
    MexRadians maxMoveGradient() const;

    //  The maximum unevenness of terrain
    MATHEX_SCALAR    maxTerrainUnevenness() const;
    
    // The change in priority to use for player controlled machines when pathfinding
    PhysPathFindingPriority pcPathFindingPriority() const;
	
	// The BMU value it costs to launch a nuclear missile
	MachPhys::BuildingMaterialUnits nukeLaunchCost() const;

	//maximum actors that any given race may build
	size_t maxUnitsPerRace() const;
	//Used as an additional value to the above maxUnitsPerRace.
	size_t campaignExtraUnitsPerRace() const;
    	 
    void CLASS_INVARIANT;

private:
	friend class MachPhysDataParser;

    MachPhysGeneralData();

    friend ostream& operator <<( ostream& o, const MachPhysGeneralData& t );

    //Set methods private so data parser only can do it
    void firstPersonLookUpDownRate( MexRadians rate );
    void firstPersonLookUpDownMinAngle( MexRadians angle );
    void firstPersonLookUpDownMaxAngle( MexRadians angle );
    void firstPersonMaxFastTurnRate( MexRadians angle );
    void firstPersonMaxSlowTurnRate( MexRadians angle );
    void startRotationDistance( MATHEX_SCALAR distance );
    void maxMoveGradient( MexRadians gradient );
    void maxTerrainUnevenness( MATHEX_SCALAR );
	void virtualDefConInterval( PhysRelativeTime interval );
    void pcPathFindingPriority( PhysPathFindingPriority );
	void nukeLaunchCost( MachPhys::BuildingMaterialUnits cost );
	void maxUnitsPerRace( size_t );
	void campaignExtraUnitsPerRace( size_t );

    //revoked
    MachPhysGeneralData( const MachPhysGeneralData& );
    MachPhysGeneralData& operator =( const MachPhysGeneralData& );

    //data members
    MachPhysGeneralDataImpl* pImpl_; //Data implementation object
};


#endif

/* End GENEDATA.HPP *************************************************/
