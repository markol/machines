/*
 * S C E N A R I O . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */


/*
    Scenario

    A brief description of the class should go in here
*/

#ifndef _SCENARIO_HPP
#define _SCENARIO_HPP

#include "base/base.hpp"
#include "machlog/machlog.hpp"
#include "machlog/camera.hpp"

class MachLogGameCreationData;
class SysPathName;
class UtlLineTokeniser;

class MachLogScenario
{
public:
    MachLogScenario( ){};
    ~MachLogScenario( void );

	static void load( 	const SysPathName& scenarioFilePath, const MachLogGameCreationData& );

	static MachLog::ObjectType objectType( const string& type );
	static int objectSubType( MachLog::ObjectType, const string& type );
	static MachPhys::TechnicianSubType technicianSubType( const string& subType );
	static MachPhys::ConstructorSubType constructorSubType( const string& subType );
	static MachPhys::AggressorSubType aggressorSubType( const string& subType );
	static MachPhys::AdministratorSubType administratorSubType( const string& subType );

	static MachPhys::FactorySubType factorySubType( const string& subType );
	static MachPhys::HardwareLabSubType hardwareLabSubType( const string& subType );
	static MachPhys::MissileEmplacementSubType missileEmplacementSubType( const string& subType );

	static MachPhys::WeaponCombo	weaponCombo( const string& weaponCombo );
	static MachPhys::Race 			machPhysRace( const string& race );
	static MachLog::ObjectType 		physConstructionToLogObject ( MachPhys::ConstructionType);
	static MachLogCamera::Type 		cameraType( const string& cameraType );
	static MachLog::BeaconType		virtualBeaconType( const string& beaconType );
	static MachLog::ResourcesAvailable		resourceAvailable( const string& resourceAvailableToken );
	static int						startingResourcesToInt( MachLog::StartingResources );
	static void 					parseRestrictConstruction( const UtlLineTokeniser& );

    void CLASS_INVARIANT;

private:
    // Operation deliberately revoked
    MachLogScenario( const MachLogScenario& );

    // Operation deliberately revoked
    MachLogScenario& operator =( const MachLogScenario& );

    // Operation deliberately revoked
    bool operator ==( const MachLogScenario& );

};


#endif

/* End SCENARIO.HPP *************************************************/
