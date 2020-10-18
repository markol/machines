/*
 * M P H Y D A T A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "base/error.hpp"
#include "system/pathname.hpp"
#include "utility/linetok.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/levels.hpp"
#include "machphys/station.hpp"
#include "machphys/stations.hpp"
#include "machphys/internal/datapars.hpp"
#include "machphys/internal/dataimpl.hpp"

#include "machphys/beacdata.hpp"
#include "machphys/consdata.hpp"
#include "machphys/factdata.hpp"
#include "machphys/garrdata.hpp"
#include "machphys/harddata.hpp"
#include "machphys/minedata.hpp"
#include "machphys/msemdata.hpp"
#include "machphys/poddata.hpp"
#include "machphys/smeldata.hpp"

#include <stdio.h>

// static
MachPhysData& MachPhysData::instance()
{
    static MachPhysData instance_;
    return instance_;
}

MachPhysData::MachPhysData()
{
    TEST_INVARIANT;
}

MachPhysData::~MachPhysData()
{
    TEST_INVARIANT;
}

void MachPhysData::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

const MachPhysAggressorData& MachPhysData::aggressorData( MachPhys::AggressorSubType subType, size_t hwLevel, size_t swLevel ) const
{
    PRE( MachPhysLevels::instance().levelValid( subType, hwLevel, swLevel ) );
    ALWAYS_ASSERT( MachPhysLevels::instance().levelValid( subType, hwLevel, swLevel ), "Machine Aggressor level is not valid" );

    return MachPhysDataImplementation::instance().aggressorData( subType, hwLevel, swLevel );
}

const MachPhysAdministratorData& MachPhysData::administratorData( MachPhys::AdministratorSubType subType, size_t hwLevel, size_t swLevel ) const
{
    PRE( MachPhysLevels::instance().levelValid( subType, hwLevel, swLevel ) );
    ALWAYS_ASSERT( MachPhysLevels::instance().levelValid( subType, hwLevel, swLevel ), "Machine Administrator level is not valid" );

    return MachPhysDataImplementation::instance().administratorData( subType, hwLevel, swLevel );
}

const MachPhysAPCData& MachPhysData::APCData( size_t hwLevel, size_t swLevel ) const
{
    PRE( MachPhysLevels::instance().levelValid( MachPhys::APC, hwLevel, swLevel ) );
    ALWAYS_ASSERT( MachPhysLevels::instance().levelValid( MachPhys::APC, hwLevel, swLevel ), "Machine APC level is not valid" );

    return MachPhysDataImplementation::instance().APCData( hwLevel, swLevel );
}

const MachPhysResourceCarrierData& MachPhysData::resourceCarrierData( size_t hwLevel, size_t swLevel ) const
{
    PRE( MachPhysLevels::instance().levelValid( MachPhys::RESOURCE_CARRIER, hwLevel, swLevel ) );
    ALWAYS_ASSERT( MachPhysLevels::instance().levelValid( MachPhys::RESOURCE_CARRIER, hwLevel, swLevel ), "Machine ResourceCarrier level is not valid" );

    return MachPhysDataImplementation::instance().resourceCarrierData( hwLevel, swLevel );
}

const MachPhysGeoLocatorData& MachPhysData::geoLocatorData( size_t hwLevel, size_t swLevel ) const
{
    PRE( MachPhysLevels::instance().levelValid( MachPhys::GEO_LOCATOR, hwLevel, swLevel ) );
    ALWAYS_ASSERT( MachPhysLevels::instance().levelValid( MachPhys::GEO_LOCATOR, hwLevel, swLevel ), "Machine GeoLocator level is not valid" );

    return MachPhysDataImplementation::instance().geoLocatorData( hwLevel, swLevel );
}

const MachPhysSpyLocatorData& MachPhysData::spyLocatorData( size_t hwLevel, size_t swLevel ) const
{
    PRE( MachPhysLevels::instance().levelValid( MachPhys::SPY_LOCATOR, hwLevel, swLevel ) );
    ALWAYS_ASSERT( MachPhysLevels::instance().levelValid( MachPhys::SPY_LOCATOR, hwLevel, swLevel ), "Machine Spy level is not valid" );

    return MachPhysDataImplementation::instance().spyLocatorData( hwLevel, swLevel );
}

const MachPhysConstructorData& MachPhysData::constructorData( MachPhys::ConstructorSubType subType, size_t hwLevel, size_t swLevel ) const
{
    PRE( MachPhysLevels::instance().levelValid( subType, hwLevel, swLevel ) );
    ALWAYS_ASSERT( MachPhysLevels::instance().levelValid( subType, hwLevel, swLevel ), "Machine Constructor level is not valid" );

    return MachPhysDataImplementation::instance().constructorData( subType, hwLevel, swLevel );
}

const MachPhysTechnicianData& MachPhysData::technicianData( MachPhys::TechnicianSubType subType, size_t hwLevel, size_t swLevel ) const
{
    PRE( MachPhysLevels::instance().levelValid( subType, hwLevel, swLevel ) );
    ALWAYS_ASSERT( MachPhysLevels::instance().levelValid( subType, hwLevel, swLevel ), "Machine Technican level is not valid" );

    return MachPhysDataImplementation::instance().technicianData( subType, hwLevel, swLevel );
}

const MachPhysHardwareLabData& MachPhysData::hardwareLabData( MachPhys::HardwareLabSubType subType, size_t hwLevel ) const
{
    PRE( MachPhysLevels::instance().levelValid( subType, hwLevel ) );
    ALWAYS_ASSERT( MachPhysLevels::instance().levelValid( subType, hwLevel ), "Construction HardwareLab level is not valid" );

    return MachPhysDataImplementation::instance().hardwareLabData( subType, hwLevel );
}

const MachPhysSmelterData& MachPhysData::smelterData( size_t hwLevel ) const
{
    PRE( MachPhysLevels::instance().levelValid( MachPhys::SMELTER, hwLevel ) );
    ALWAYS_ASSERT( MachPhysLevels::instance().levelValid( MachPhys::SMELTER, hwLevel ), "Construction Smelter level is not valid" );

    return MachPhysDataImplementation::instance().smelterData( hwLevel );
}

const MachPhysFactoryData& MachPhysData::factoryData( MachPhys::FactorySubType subType, size_t hwLevel ) const
{
    PRE( MachPhysLevels::instance().levelValid( subType, hwLevel ) );
    ALWAYS_ASSERT( MachPhysLevels::instance().levelValid( subType, hwLevel ), "Construction Factory level is not valid" );

    return MachPhysDataImplementation::instance().factoryData( subType, hwLevel );
}

const MachPhysMissileEmplacementData& MachPhysData::missileEmplacementData( MachPhys::MissileEmplacementSubType subType, size_t hwLevel ) const
{
    PRE( MachPhysLevels::instance().levelValid( subType, hwLevel ) );
    ALWAYS_ASSERT( MachPhysLevels::instance().levelValid( subType, hwLevel ), "Construction MissileEmplacement level is not valid" );

    return MachPhysDataImplementation::instance().missileEmplacementData( subType, hwLevel );
}

const MachPhysGarrisonData& MachPhysData::garrisonData( size_t hwLevel ) const
{
    PRE( MachPhysLevels::instance().levelValid( MachPhys::GARRISON, hwLevel ) );
    ALWAYS_ASSERT( MachPhysLevels::instance().levelValid( MachPhys::GARRISON, hwLevel ), "Construction Garrison level is not valid" );

    return MachPhysDataImplementation::instance().garrisonData( hwLevel );
}

const MachPhysMineData& MachPhysData::mineData( size_t hwLevel ) const
{
    PRE( MachPhysLevels::instance().levelValid( MachPhys::MINE, hwLevel ) );
    ALWAYS_ASSERT( MachPhysLevels::instance().levelValid( MachPhys::MINE, hwLevel ), "Construction Mine level is not valid" );

    return MachPhysDataImplementation::instance().mineData( hwLevel );
}

const MachPhysBeaconData& MachPhysData::beaconData( size_t hwLevel ) const
{
    PRE( MachPhysLevels::instance().levelValid( MachPhys::BEACON, hwLevel ) );
    ALWAYS_ASSERT( MachPhysLevels::instance().levelValid( MachPhys::BEACON, hwLevel ), "Construction Beacon level is not valid" );

    return MachPhysDataImplementation::instance().beaconData( hwLevel );
}

const MachPhysPodData& MachPhysData::podData( size_t hwLevel ) const
{
    PRE( MachPhysLevels::instance().levelValid( MachPhys::POD, hwLevel ) );
    ALWAYS_ASSERT( MachPhysLevels::instance().levelValid( MachPhys::POD, hwLevel ), "Construction Pod level is not valid" );

    return MachPhysDataImplementation::instance().podData( hwLevel );
}

const MachPhysOreHolographData& MachPhysData::oreHolographData() const
{
    return MachPhysDataImplementation::instance().oreHolographData();
}

const MachPhysWeaponData& MachPhysData::weaponData( MachPhys::WeaponType type ) const
{
    return MachPhysDataImplementation::instance().weaponData( type );
}

const MachPhysGeneralData& MachPhysData::generalData() const
{
    return MachPhysDataImplementation::instance().generalData();
}

const MachPhysConstructionData& MachPhysData::constructionData(
    MachPhys::ConstructionType type, int subType, int hwLevel ) const
{
    const MachPhysConstructionData* pConstructionData = NULL;

    switch( type )
    {
        case MachPhys::BEACON:
            pConstructionData = &MachPhysData::instance().beaconData( hwLevel );
			break;

        case MachPhys::FACTORY:
            pConstructionData = &MachPhysData::instance().factoryData(
              _STATIC_CAST( MachPhys::FactorySubType, subType ), hwLevel );
			break;

        case MachPhys::GARRISON:
            pConstructionData = &MachPhysData::instance().garrisonData( hwLevel );
			break;

        case MachPhys::HARDWARE_LAB:
            pConstructionData = &MachPhysData::instance().hardwareLabData(
              _STATIC_CAST( MachPhys::HardwareLabSubType, subType ), hwLevel );
			break;

        case MachPhys::POD:
            pConstructionData = &MachPhysData::instance().podData( hwLevel );
			break;

        case MachPhys::MINE:
            pConstructionData = &MachPhysData::instance().mineData( hwLevel );
			break;

        case MachPhys::MISSILE_EMPLACEMENT:
            pConstructionData = &MachPhysData::instance().missileEmplacementData(
              _STATIC_CAST( MachPhys::MissileEmplacementSubType, subType ), hwLevel );
			break;

        case MachPhys::SMELTER:
            pConstructionData = &MachPhysData::instance().smelterData( hwLevel );
			break;
    }

    return *pConstructionData;
}

/* End MPHYDATA.CPP *************************************************/
