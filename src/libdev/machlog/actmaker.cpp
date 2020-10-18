/*
 * A C T M A K E R . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/point3d.hpp"
#include "mathex/point2d.hpp"
#include "mathex/eulerang.hpp"

#include "machphys/beacon.hpp"
#include "machphys/factory.hpp"
#include "machphys/garrison.hpp"
#include "machphys/hwlab.hpp"
#include "machphys/pod.hpp"
#include "machphys/mine.hpp"
#include "machphys/missemp.hpp"
#include "machphys/smelter.hpp"
#include "machphys/machine.hpp"

#include "machlog/actmaker.hpp"
#include "machlog/administ.hpp"
#include "machlog/aggressr.hpp"
#include "machlog/apc.hpp"
#include "machlog/artefact.hpp"
#include "machlog/artfacts.hpp"
#include "machlog/beacon.hpp"
#include "machlog/construc.hpp"
#include "machlog/debris.hpp"
#include "machlog/factory.hpp"
#include "machlog/garrison.hpp"
#include "machlog/geolocat.hpp"
#include "machlog/hwlab.hpp"
#include "machlog/messbrok.hpp"
#include "machlog/mine.hpp"
#include "machlog/missilem.hpp"
#include "machlog/network.hpp"
#include "machlog/oreholo.hpp"
#include "machlog/planet.hpp"
//#include "machlog/plandoms.hpp"
#include "machlog/pod.hpp"
#include "machlog/races.hpp"
#include "machlog/rescarr.hpp"
#include "machlog/smelter.hpp"
#include "machlog/spacial.hpp"
#include "machlog/spy.hpp"
//#include "machlog/swlab.hpp"
#include "machlog/technici.hpp"
#include "machlog/vmman.hpp"
#include "machlog/vmdata.hpp"


//static
MachPhysConstruction* MachLogActorMaker::newPhysConstruction
(
    MachLog::ObjectType type, int subType, int hwLevel,
    W4dEntity* pParent, const MexTransform3d& localTransform, MachPhys::Race race
)
{

	ASSERT_INFO( type );
	ASSERT_INFO( subType );
	ASSERT_INFO( hwLevel );

    //Depends on type
    switch( type )
    {
        case MachLog::BEACON:
            return _NEW( MachPhysBeacon( pParent, localTransform, hwLevel, race) );

        case MachLog::FACTORY:
            return _NEW( MachPhysFactory( pParent, localTransform,
                                          (MachPhys::FactorySubType)subType, hwLevel, race ) );

        case MachLog::GARRISON:
            return _NEW( MachPhysGarrison( pParent, localTransform, hwLevel, race) );

        case MachLog::HARDWARE_LAB:
            return _NEW( MachPhysHardwareLab( pParent, localTransform,
            									(MachPhys::HardwareLabSubType)subType, hwLevel, race) );

        case MachLog::POD:
            return _NEW( MachPhysPod( pParent, localTransform, hwLevel, race ) );

        case MachLog::MINE:
            return _NEW( MachPhysMine( pParent, localTransform, hwLevel, race) );

        case MachLog::MISSILE_EMPLACEMENT:
		{
			MachPhys::WeaponCombo wc = MachPhys::T_FLAME_THROWER1;
			switch( subType )
			{
				case MachPhys::TURRET:
				{
					switch( hwLevel )
					{
						case 1: wc = MachPhys::T_FLAME_THROWER1; break;
						case 2: wc = MachPhys::LR_AUTO_CANNON_X2; break;
						case 3: wc = MachPhys::LR_PULSE_RIFLE_X2; break;
					}
				}
				break;
				case MachPhys::SENTRY:
				{
					switch( hwLevel )
					{
						case 3: wc = MachPhys::LR_MULTI_LAUNCHER5_X2; break;
						case 4: wc = MachPhys::LR_MULTI_LAUNCHER6_X2; break;
					}
				}
				break;
				case MachPhys::LAUNCHER:
				{
					switch( hwLevel )
					{
						case 4: wc = MachPhys::LR_LARGE_MISSILE_X2; break;
					}
				}
				break;
				case MachPhys::ICBM:
				{
			 		switch( hwLevel )
					{
				  		case 5: wc = MachPhys::T_NUCLEAR_MISSILE; break;
   					}
  				}
				break;
				DEFAULT_ASSERT_BAD_CASE( subType );
			}
            return _NEW( MachPhysMissileEmplacement( pParent, localTransform,
                                                     (MachPhys::MissileEmplacementSubType)subType,
                                                     hwLevel, race, wc ) );
		}

        case MachLog::SMELTER:
            return _NEW( MachPhysSmelter( pParent, localTransform, hwLevel, race) );

        DEFAULT_ASSERT_BAD_CASE( type );
    }

    return NULL;
}

//static
MachLogConstruction* MachLogActorMaker::newLogConstruction
(
    MachLog::ObjectType type, int subType, int hwLevel,
    const MexPoint3d& inLocation, const MexRadians& angle, MachPhys::Race race
)
{
    //Get the colony race pointer
    MachLogRace* pLogRace = &MachLogRaces::instance().race( race );
	MachLogConstruction* result = NULL;

    //Adjust the height for the terrain

//    MexPoint3d location = MachLogSpacialManipulation::heightAdjustedLocation( inLocation );

    const MexTransform3d  transform( MexEulerAngles( angle, 0.0, 0.0 ), inLocation );
    MexPoint3d location = MachLogSpacialManipulation::heightAdjustedLocation( type, subType, hwLevel, transform );

    //Depends on type
    switch( type )
    {
        case MachLog::BEACON:
            result = _NEW( MachLogBeacon( pLogRace, hwLevel, location, angle ) );
			break;

        case MachLog::FACTORY:
            result = _NEW( MachLogFactory( (MachPhys::FactorySubType)subType, pLogRace, hwLevel,
                                         location, angle ) );
			break;

        case MachLog::GARRISON:
            result = _NEW( MachLogGarrison( pLogRace, hwLevel, location, angle ) );
			break;

        case MachLog::HARDWARE_LAB:
            result = _NEW( MachLogHardwareLab( (MachPhys::HardwareLabSubType)subType,pLogRace,
            									hwLevel, location, angle ) );
			break;

        case MachLog::POD:
            result = _NEW( MachLogPod( pLogRace, hwLevel, location, angle, MachPhys::T_ION_ORBITAL_CANNON ) );
			break;

        case MachLog::MINE:
            result = _NEW( MachLogMine( pLogRace, hwLevel, location, angle, MachLogMine::IGNORE_DISCOVERED_FLAG ) );
			break;

        case MachLog::MISSILE_EMPLACEMENT:
		{
			MachPhys::WeaponCombo wc = MachPhys::T_FLAME_THROWER1;
			switch( subType )
			{
				case MachPhys::TURRET:
				{
					switch( hwLevel )
					{
						case 1: wc = MachPhys::T_FLAME_THROWER1; break;
						case 2: wc = MachPhys::LR_AUTO_CANNON_X2; break;
						case 3: wc = MachPhys::LR_PULSE_RIFLE_X2; break;
					}
				}
				break;
				case MachPhys::SENTRY:
				{
					switch( hwLevel )
					{
						case 3: wc = MachPhys::LR_MULTI_LAUNCHER5_X2; break;
						case 4: wc = MachPhys::LR_MULTI_LAUNCHER6_X2; break;
					}
				}
				break;
				case MachPhys::LAUNCHER:
				{
					switch( hwLevel )
					{
						case 4: wc = MachPhys::LR_LARGE_MISSILE_X2; break;
					}
				}
				break;
				case MachPhys::ICBM:
				{
			 		switch( hwLevel )
					{
				  		case 5: wc = MachPhys::T_NUCLEAR_MISSILE; break;
   					}
  				}
				break;
				DEFAULT_ASSERT_BAD_CASE( subType );
			}
            result = _NEW( MachLogMissileEmplacement( (MachPhys::MissileEmplacementSubType)subType,
                                                     pLogRace, hwLevel, location, angle, wc ) );
			break;
		}


        case MachLog::SMELTER:
            result = _NEW( MachLogSmelter( pLogRace, hwLevel, location, angle ) );
			break;

        DEFAULT_ASSERT_BAD_CASE( type );
    }

	// lay pads on terrain
	result->physConstruction().layPadsOnTerrain( *MachLogPlanet::instance().surface() );

	if( MachLogNetwork::instance().isNetworkGame() )
	{
		MachLogNetwork::instance().messageBroker().sendCreateActorMessage( race,
																			type,
																			subType,
																			hwLevel,
																			0,
																			location,
																			angle,
																			MachPhys::N_WEAPON_COMBOS,
																			result->id()
																			);
	}
    return result;
}

//static
MachLogConstruction* MachLogActorMaker::newLogConstruction
(
    MachLog::ObjectType type, int subType, int hwLevel,
    const MexPoint3d& inLocation, const MexRadians& angle, MachPhys::Race race, UtlId withId
)
{
	DEBUG_STREAM( DIAG_NETWORK, "MLActorMaker::newLogConstruction " << type << " withId " << withId << std::endl );
    //Get the colony race pointer
    MachLogRace* pLogRace = &MachLogRaces::instance().race( race );
	MachLogConstruction* result = NULL;

    //Adjust the height for the terrain
//    MexPoint3d location = MachLogSpacialManipulation::heightAdjustedLocation( inLocation );

    const MexTransform3d  transform( MexEulerAngles( angle, 0.0, 0.0 ), inLocation );
    MexPoint3d location = MachLogSpacialManipulation::heightAdjustedLocation( type, subType, hwLevel, transform );

    //Depends on type
    switch( type )
    {
        case MachLog::BEACON:
            result = _NEW( MachLogBeacon( pLogRace, hwLevel, location, angle, withId ) );
			break;

        case MachLog::FACTORY:
            result = _NEW( MachLogFactory( (MachPhys::FactorySubType)subType, pLogRace, hwLevel,
                                         location, angle, withId ) );
			break;

        case MachLog::GARRISON:
            result = _NEW( MachLogGarrison( pLogRace, hwLevel, location, angle, withId ) );
			break;

        case MachLog::HARDWARE_LAB:
            result = _NEW( MachLogHardwareLab( (MachPhys::HardwareLabSubType)subType,pLogRace,
            									hwLevel, location, angle, withId ) );
			break;

        case MachLog::POD:
            result = _NEW( MachLogPod( pLogRace, hwLevel, location, angle, MachPhys::T_ION_ORBITAL_CANNON, withId ) );
			break;

        case MachLog::MINE:
            result = _NEW( MachLogMine( pLogRace, hwLevel, location, angle, MachLogMine::IGNORE_DISCOVERED_FLAG, withId ) );
			break;

        case MachLog::MISSILE_EMPLACEMENT:
		{
			MachPhys::WeaponCombo wc = MachPhys::T_FLAME_THROWER1;
			switch( subType )
			{
				case MachPhys::TURRET:
				{
					switch( hwLevel )
					{
						case 1: wc = MachPhys::T_FLAME_THROWER1; break;
						case 2: wc = MachPhys::LR_AUTO_CANNON_X2; break;
						case 3: wc = MachPhys::LR_PULSE_RIFLE_X2; break;
					}
				}
				break;
				case MachPhys::SENTRY:
				{
					switch( hwLevel )
					{
						case 3: wc = MachPhys::LR_MULTI_LAUNCHER5_X2; break;
						case 4: wc = MachPhys::LR_MULTI_LAUNCHER6_X2; break;
					}
				}
				break;
				case MachPhys::LAUNCHER:
				{
					switch( hwLevel )
					{
						case 4: wc = MachPhys::LR_LARGE_MISSILE_X2; break;
					}
				}
				break;
				case MachPhys::ICBM:
				{
			 		switch( hwLevel )
					{
				  		case 5: wc = MachPhys::T_NUCLEAR_MISSILE; break;
   					}
  				}
				break;
				DEFAULT_ASSERT_BAD_CASE( subType );
			}
            result = _NEW( MachLogMissileEmplacement( (MachPhys::MissileEmplacementSubType)subType,
                                                     pLogRace, hwLevel, location, angle, wc, withId ) );
			break;
		}


        case MachLog::SMELTER:
            result = _NEW( MachLogSmelter( pLogRace, hwLevel, location, angle, withId ) );
			break;

        DEFAULT_ASSERT_BAD_CASE( type );
    }

	// lay pads on terrain
	result->physConstruction().layPadsOnTerrain( *MachLogPlanet::instance().surface() );

	DEBUG_STREAM( DIAG_NETWORK, "MLActorMaker::newLogConstruction DONE " << type << " withId " << withId << std::endl );

    return result;
}

//static
MachLogMachine* MachLogActorMaker::newLogMachine
(
    MachLog::ObjectType type, int subType, int hwLevel, int swLevel, MachPhys::Race race,
    const MexPoint3d& inLocation, MachPhys::WeaponCombo wc
)
{
    //Get the colony race pointer
    MachLogRace* pLogRace = &MachLogRaces::instance().race( race );
	MachLogMachine* result = NULL;

    //Adjust the height for the terrain

    MexVec3 normal;
    const MATHEX_SCALAR height = MachLogPlanet::instance().surface()->terrainHeight( inLocation.x(), inLocation.y(), &normal );
    const MexPoint3d location( inLocation.x(), inLocation.y(), height );

    //Depends on type
    switch( type )
    {
        case MachLog::AGGRESSOR:
            result = _NEW( MachLogAggressor( (MachPhys::AggressorSubType)subType, hwLevel, swLevel, pLogRace, location, wc ) );
			break;

        case MachLog::ADMINISTRATOR:
            result = _NEW( MachLogAdministrator( (MachPhys::AdministratorSubType)subType, hwLevel, swLevel, pLogRace, location, wc ) );
			break;

        case MachLog::APC:
            result = _NEW( MachLogAPC( hwLevel, swLevel, pLogRace, location ) );
			break;

        case MachLog::CONSTRUCTOR:
            result = _NEW( MachLogConstructor( (MachPhys::ConstructorSubType)subType, hwLevel, swLevel, pLogRace, location ) );
			break;

        case MachLog::GEO_LOCATOR:
            result = _NEW( MachLogGeoLocator( hwLevel, swLevel, pLogRace, location ) );
			break;

        case MachLog::RESOURCE_CARRIER:
            result = _NEW( MachLogResourceCarrier( hwLevel, swLevel, pLogRace, location ) );
			break;

        case MachLog::SPY_LOCATOR:
            result = _NEW( MachLogSpyLocator( hwLevel, swLevel, pLogRace, location ) );
			break;

        case MachLog::TECHNICIAN:
            result = _NEW( MachLogTechnician( (MachPhys::TechnicianSubType)subType, hwLevel, swLevel, pLogRace, location ) );
			break;

        DEFAULT_ASSERT_BAD_CASE( type );
    }

    //  Adjust the transform so that the machine sits correctly on the surface of the planet.
    const MexTransform3d& globalTransform = result->physMachine().globalTransform();
    const MexTransform3d newTransform( MexTransform3d::Z_ZX, normal, globalTransform.xBasis(), globalTransform.position() );

    result->physMachine().globalTransform( newTransform );

	if( MachLogNetwork::instance().isNetworkGame() )
	{
		MachLogNetwork::instance().messageBroker().sendCreateActorMessage( race,
																			type,
																			subType,
																			hwLevel,
																			swLevel,
																			location,
																			0,
																			wc,
																			result->id()
																			);
	}

    return result;
}

//static
MachLogMachine* MachLogActorMaker::newLogMachine
(
    MachLog::ObjectType type, int subType, int hwLevel, int swLevel, MachPhys::Race race,
    const MexPoint3d& inLocation, MachPhys::WeaponCombo wc, UtlId withId
)
{
	DEBUG_STREAM( DIAG_NETWORK, "MLActorMaker::newLogMachine " << type << " withId " << withId << std::endl );
    //Get the colony race pointer
    MachLogRace* pLogRace = &MachLogRaces::instance().race( race );
	MachLogMachine* result = NULL;

    //Adjust the height for the terrain
    MexVec3 normal;
    const MATHEX_SCALAR height = MachLogPlanet::instance().surface()->terrainHeight( inLocation.x(), inLocation.y(), &normal );
    const MexPoint3d location( inLocation.x(), inLocation.y(), height );

    //Depends on type
    switch( type )
    {
        case MachLog::AGGRESSOR:
            result = _NEW( MachLogAggressor( (MachPhys::AggressorSubType)subType, hwLevel, swLevel, pLogRace, location, wc, withId ) );
			break;

        case MachLog::ADMINISTRATOR:
            result = _NEW( MachLogAdministrator( (MachPhys::AdministratorSubType)subType, hwLevel, swLevel, pLogRace, location, wc, withId ) );
			break;

        case MachLog::APC:
            result = _NEW( MachLogAPC( hwLevel, swLevel, pLogRace, location, withId ) );
			break;

        case MachLog::CONSTRUCTOR:
            result = _NEW( MachLogConstructor( (MachPhys::ConstructorSubType)subType, hwLevel, swLevel, pLogRace, location, withId ) );
			break;

        case MachLog::GEO_LOCATOR:
            result = _NEW( MachLogGeoLocator( hwLevel, swLevel, pLogRace, location, withId ) );
			break;

        case MachLog::RESOURCE_CARRIER:
            result = _NEW( MachLogResourceCarrier( hwLevel, swLevel, pLogRace, location, withId ) );
			break;

        case MachLog::SPY_LOCATOR:
            result = _NEW( MachLogSpyLocator( hwLevel, swLevel, pLogRace, location, withId ) );
			break;

        case MachLog::TECHNICIAN:
            result = _NEW( MachLogTechnician( (MachPhys::TechnicianSubType)subType, hwLevel, swLevel, pLogRace, location, withId ) );
			break;

        DEFAULT_ASSERT_BAD_CASE( type );
    }

    //  Adjust the transform so that the machine sits correctly on the surface of the planet.
    const MexTransform3d& globalTransform = result->physMachine().globalTransform();
    const MexTransform3d newTransform( MexTransform3d::Z_ZX, normal, globalTransform.xBasis(), globalTransform.position() );

    result->physMachine().globalTransform( newTransform );

	DEBUG_STREAM( DIAG_NETWORK, "MLActorMaker::newLogMachine DONE " << type << " withId " << withId << std::endl );
    return result;
}

//static
MachLogOreHolograph* MachLogActorMaker::newLogOreHolograph
(
	MachPhys::Race race, int concentration, int quantity, const MexPoint3d& inLocation
)
{
	DEBUG_STREAM( DIAG_NETWORK, "MLActorMaker::newLogOreholograph " << std::endl );
    //Get the colony race pointer
    MachLogRace* pLogRace = &MachLogRaces::instance().race( race );

    //Adjust the height for the terrain
    MexPoint3d location = MachLogSpacialManipulation::heightAdjustedLocation( inLocation );

	MachLogOreHolograph* result = NULL;
	result = _NEW( MachLogOreHolograph( pLogRace, concentration, quantity, location ) );

	/*
	if( MachLogNetwork::instance().isNetworkGame() )
	{
		MachLogNetwork::instance().messageBroker().sendCreateActorMessage( race,
																			MachLog::ORE_HOLOGRAPH,
																			0,
																			concentration,
																			quantity,
																			location,
																			0,
																			MachPhys::N_WEAPON_COMBOS,
																			result->id()
																			);
	}
	*/

	DEBUG_STREAM( DIAG_NETWORK, "MLActorMaker::newLogOreholograph DONE " << std::endl );
	return result;
}

    //Return a new instance of a logical oreholo
//static
MachLogOreHolograph* MachLogActorMaker::newLogOreHolograph
(
	MachPhys::Race race, int concentration, int quantity, const MexPoint3d& inLocation,
          UtlId withId
)
{
	DEBUG_STREAM( DIAG_NETWORK, "MLActorMaker::newLogOreholograph withId " << withId << std::endl );
    //Get the colony race pointer
    MachLogRace* pLogRace = &MachLogRaces::instance().race( race );

    //Adjust the height for the terrain
    MexPoint3d location = MachLogSpacialManipulation::heightAdjustedLocation( inLocation );

	MachLogOreHolograph* result = NULL;
	result = _NEW( MachLogOreHolograph( pLogRace, concentration, quantity, location, withId ) );
	DEBUG_STREAM( DIAG_NETWORK," done\n" );
	return result;
}

//Return a new instance of a logical debris
//static
MachLogDebris* MachLogActorMaker::newLogDebris
    ( MachPhys::Race race, int quantity, const MexPoint3d& inLocation, const MexAlignedBox2d& boundary )
{
	DEBUG_STREAM( DIAG_NETWORK, "MLActorMaker::newLogDebris " << std::endl );
    //Get the colony race pointer
    MachLogRace* pLogRace = &MachLogRaces::instance().race( race );

    //Adjust the height for the terrain
    MexPoint3d location = MachLogSpacialManipulation::heightAdjustedLocation( inLocation );

	MachLogDebris* result = NULL;
	result = _NEW( MachLogDebris( pLogRace, quantity, location, boundary ) );
	if( MachLogNetwork::instance().isNetworkGame() )
	{
		MachLogNetwork::instance().messageBroker().sendCreateSpecialActorMessage( race,
																			MachLog::DEBRIS,
																			quantity,
																			location,
																			boundary,
																			result->id()
																			);
	}
	DEBUG_STREAM( DIAG_NETWORK, "MLActorMaker::newLogDebris DONE " << std::endl );
	return result;
}

//Return a new instance of a logical debris
//static
MachLogDebris* MachLogActorMaker::newLogDebris
    ( MachPhys::Race race, int quantity, const MexPoint3d& inLocation, const MexAlignedBox2d& boundary, UtlId withId )
{
	DEBUG_STREAM( DIAG_NETWORK, "MLActorMaker::newLogDebris withId " << withId << std::endl );
    //Get the colony race pointer
    MachLogRace* pLogRace = &MachLogRaces::instance().race( race );

    //Adjust the height for the terrain
    MexPoint3d location = MachLogSpacialManipulation::heightAdjustedLocation( inLocation );

    MachLogDebris* result = NULL;
	result = _NEW( MachLogDebris( pLogRace, quantity, location, boundary, withId ) );
	DEBUG_STREAM( DIAG_NETWORK," done\n" );
	return result;
}

//Return a new instance of a logical land mine
//static
MachLogLandMine* MachLogActorMaker::newLogLandMine
    ( MachPhys::Race , const MexPoint3d&  )
{
	return NULL;
}

//Return a new instance of a logical land mine
//static
MachLogLandMine* MachLogActorMaker::newLogLandMine
    ( MachPhys::Race , const MexPoint3d&, UtlId  )
{
	return NULL;
}

//static
MachLogArtefact* MachLogActorMaker::newLogArtefact
    ( int subType, const MexPoint3d& inLocation, const MexRadians& angle )
{
    //Adjust the height for the terrain
    MexPoint3d location = MachLogSpacialManipulation::heightAdjustedLocation( inLocation );

    MachLogArtefact* result = _NEW( MachLogArtefact( subType, location, angle ) );


	if( MachLogNetwork::instance().isNetworkGame() )
	{
		MachLogNetwork::instance().messageBroker().sendCreateActorMessage( MachPhys::NORACE,
																			MachLog::ARTEFACT,
																			subType,
																			0,
																			0,
																			location,
																			angle,
																			MachPhys::N_WEAPON_COMBOS,
																			result->id()
																			);
	}

    return result;
}

//static
MachLogArtefact* MachLogActorMaker::newLogArtefact
    ( int subType, const MexPoint3d& inLocation, const MexRadians& angle, UtlId withId )
{
    //Adjust the height for the terrain
    MexPoint3d location = MachLogSpacialManipulation::heightAdjustedLocation( inLocation );

    MachLogArtefact* pResult = _NEW( MachLogArtefact( subType, location, angle, withId ) );

    return pResult;
}

//static
W4dEntity* MachLogActorMaker::newDumbArtefact
    ( int subType, const MexPoint3d& inLocation, const MexRadians& angle )
{
    //Adjust the height for the terrain
    MexPoint3d location = MachLogSpacialManipulation::heightAdjustedLocation( inLocation );

    W4dEntity* pResult =
        MachLogRaces::instance().artefacts().newPhysArtefact( subType, location, angle );

	if( MachLogNetwork::instance().isNetworkGame() and MachLogNetwork::instance().isNodeLogicalHost() )
	{
		MachLogNetwork::instance().messageBroker().sendCreateActorMessage( MachPhys::NORACE,
																			MachLog::ARTEFACT,
																			subType,
																			1,
																			0,
																			location,
																			angle,
																			MachPhys::N_WEAPON_COMBOS,
																			0
																			);
	}

    return pResult;
}

/* End ACTMAKER.CPP *************************************************/
