/*
 * D A T A I M P L . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/internal/dataimpl.hpp"
#include "machphys/internal/datapars.hpp"
#include "machphys/internal/dataimpl.ctf"

#include "machphys/levels.hpp"

#include "system/pathname.hpp"

// Machine type data classes
#include "machphys/aggrdata.hpp"
#include "machphys/admndata.hpp"
#include "machphys/apcdata.hpp"
#include "machphys/rcardata.hpp"
#include "machphys/glocdata.hpp"
#include "machphys/slocdata.hpp"
#include "machphys/techdata.hpp"
#include "machphys/ctordata.hpp"
// Construction type data classes
#include "machphys/harddata.hpp"
#include "machphys/smeldata.hpp"
#include "machphys/factdata.hpp"
#include "machphys/msemdata.hpp"
#include "machphys/garrdata.hpp"
#include "machphys/minedata.hpp"
#include "machphys/beacdata.hpp"
#include "machphys/poddata.hpp"
// Specials
#include "machphys/orehdata.hpp"
#include "machphys/wepdata.hpp"
#include "machphys/genedata.hpp"

// static
MachPhysDataImplementation& MachPhysDataImplementation::instance()
{
    static MachPhysDataImplementation instance_;
    return instance_;
}

MachPhysDataImplementation::MachPhysDataImplementation()
: administratorBossData_( STORE_HARDWARE_SIZE,  ctl_fixed_vector< MachPhysAdministratorData* >( STORE_SOFTWARE_SIZE, NULL ) ),
  administratorOverseerData_( STORE_HARDWARE_SIZE,  ctl_fixed_vector< MachPhysAdministratorData* >( STORE_SOFTWARE_SIZE, NULL ) ),
  administratorCommanderData_( STORE_HARDWARE_SIZE,  ctl_fixed_vector< MachPhysAdministratorData* >( STORE_SOFTWARE_SIZE, NULL ) ),
  aggressorGruntData_( STORE_HARDWARE_SIZE, ctl_fixed_vector< MachPhysAggressorData* >( STORE_SOFTWARE_SIZE, NULL ) ),
  aggressorAssassinData_( STORE_HARDWARE_SIZE, ctl_fixed_vector< MachPhysAggressorData* >( STORE_SOFTWARE_SIZE, NULL ) ),
  aggressorKnightData_( STORE_HARDWARE_SIZE, ctl_fixed_vector< MachPhysAggressorData* >( STORE_SOFTWARE_SIZE, NULL ) ),
  aggressorBallistaData_( STORE_HARDWARE_SIZE, ctl_fixed_vector< MachPhysAggressorData* >( STORE_SOFTWARE_SIZE, NULL ) ),
  aggressorNinjaData_( STORE_HARDWARE_SIZE, ctl_fixed_vector< MachPhysAggressorData* >( STORE_SOFTWARE_SIZE, NULL ) ),
  constructorDozerData_( STORE_HARDWARE_SIZE, ctl_fixed_vector< MachPhysConstructorData* >( STORE_SOFTWARE_SIZE, NULL ) ),
  constructorBuilderData_( STORE_HARDWARE_SIZE, ctl_fixed_vector< MachPhysConstructorData* >( STORE_SOFTWARE_SIZE, NULL ) ),
  constructorBehemothData_( STORE_HARDWARE_SIZE, ctl_fixed_vector< MachPhysConstructorData* >( STORE_SOFTWARE_SIZE, NULL ) ),
  geoLocatorData_( STORE_HARDWARE_SIZE, ctl_fixed_vector< MachPhysGeoLocatorData* >( STORE_SOFTWARE_SIZE, NULL ) ),
  APCData_( STORE_HARDWARE_SIZE, ctl_fixed_vector< MachPhysAPCData* >( STORE_SOFTWARE_SIZE, NULL ) ),
  resourceCarrierData_( STORE_HARDWARE_SIZE, ctl_fixed_vector< MachPhysResourceCarrierData* >( STORE_SOFTWARE_SIZE, NULL ) ),
  spyLocatorData_( STORE_HARDWARE_SIZE, ctl_fixed_vector< MachPhysSpyLocatorData* >( STORE_SOFTWARE_SIZE, NULL ) ),
  technicianLabTechData_( STORE_HARDWARE_SIZE, ctl_fixed_vector< MachPhysTechnicianData* >( STORE_SOFTWARE_SIZE, NULL ) ),
  technicianTechBoyData_( STORE_HARDWARE_SIZE, ctl_fixed_vector< MachPhysTechnicianData* >( STORE_SOFTWARE_SIZE, NULL ) ),
  technicianBrainBoxData_( STORE_HARDWARE_SIZE, ctl_fixed_vector< MachPhysTechnicianData* >( STORE_SOFTWARE_SIZE, NULL ) ),
  hardwareLabCivilianData_( STORE_HARDWARE_SIZE, NULL ),
  hardwareLabMilitaryData_( STORE_HARDWARE_SIZE, NULL ),
  smelterData_( STORE_HARDWARE_SIZE, NULL ),
  mineData_( STORE_HARDWARE_SIZE, NULL ),
  garrisonData_( STORE_HARDWARE_SIZE, NULL ),
  beaconData_( STORE_HARDWARE_SIZE, NULL ),
  podData_( STORE_HARDWARE_SIZE, NULL ),
  factoryCivilianData_( STORE_HARDWARE_SIZE, NULL ),
  factoryMilitaryData_( STORE_HARDWARE_SIZE, NULL ),
  missileEmplacementTurretData_( STORE_HARDWARE_SIZE, NULL ),
  missileEmplacementSentryData_( STORE_HARDWARE_SIZE, NULL ),
  missileEmplacementLauncherData_( STORE_HARDWARE_SIZE, NULL ),
  missileEmplacementICBMData_( STORE_HARDWARE_SIZE, NULL ),
  weaponData_( MachPhys::N_WEAPON_TYPES, NULL ),
  pGeneralData_( NULL )
{
    MachPhysDataParser::instance().read( "data/parmdata.dat", this );

    TEST_INVARIANT;
}

MachPhysDataImplementation::~MachPhysDataImplementation()
{
    TEST_INVARIANT;

    delete2D( administratorBossData_ );
    delete2D( administratorOverseerData_ );
    delete2D( administratorCommanderData_ );
    delete2D( aggressorGruntData_ );
    delete2D( aggressorAssassinData_ );
    delete2D( aggressorKnightData_ );
    delete2D( aggressorBallistaData_ );
    delete2D( aggressorNinjaData_ );
    delete2D( constructorDozerData_ );
    delete2D( constructorBuilderData_ );
    delete2D( constructorBehemothData_ );
    delete2D( geoLocatorData_ );
    delete2D( APCData_ );
    delete2D( resourceCarrierData_ );
    delete2D( spyLocatorData_ );
    delete2D( technicianLabTechData_ );
    delete2D( technicianTechBoyData_ );
    delete2D( technicianBrainBoxData_ );

    delete1D( hardwareLabCivilianData_ );
    delete1D( hardwareLabMilitaryData_ );
    delete1D( smelterData_ );
    delete1D( mineData_ );
    delete1D( garrisonData_ );
    delete1D( beaconData_ );
    delete1D( podData_ );
    delete1D( factoryCivilianData_ );
    delete1D( factoryMilitaryData_ );
    delete1D( missileEmplacementTurretData_ );
    delete1D( missileEmplacementSentryData_ );
    delete1D( missileEmplacementLauncherData_ );
    delete1D( missileEmplacementICBMData_ );
    delete1D( weaponData_ );
    _DELETE( pGeneralData_ );
}

void MachPhysDataImplementation::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysDataImplementation& t )
{

    o << "MachPhysDataImplementation " << (void*)&t << " start" << std::endl;
    o << "MachPhysDataImplementation " << (void*)&t << " end" << std::endl;

    return o;
}

const MachPhysAggressorData& MachPhysDataImplementation::aggressorData( MachPhys::AggressorSubType subType, size_t hwLevel, size_t swLevel ) const
{
    PRE( MachPhysLevels::instance().levelValid( subType, hwLevel, swLevel ) );

    return *((*pDataStore( subType ))[ hwLevel ][ swLevel ]);
}

const MachPhysAdministratorData& MachPhysDataImplementation::administratorData( MachPhys::AdministratorSubType subType, size_t hwLevel, size_t swLevel ) const
{
    PRE( MachPhysLevels::instance().levelValid( subType, hwLevel, swLevel ) );

    return *((*pDataStore( subType ))[ hwLevel ][ swLevel ]);
}

const MachPhysAPCData& MachPhysDataImplementation::APCData( size_t hwLevel, size_t swLevel ) const
{
    PRE( MachPhysLevels::instance().levelValid( MachPhys::APC, hwLevel, swLevel ) );

	return *APCData_[ hwLevel ][ swLevel ];
}

const MachPhysResourceCarrierData& MachPhysDataImplementation::resourceCarrierData( size_t hwLevel, size_t swLevel ) const
{
    PRE( MachPhysLevels::instance().levelValid( MachPhys::RESOURCE_CARRIER, hwLevel, swLevel ) );

	return *resourceCarrierData_[ hwLevel ][ swLevel ];
}

const MachPhysGeoLocatorData& MachPhysDataImplementation::geoLocatorData( size_t hwLevel, size_t swLevel ) const
{
    PRE( MachPhysLevels::instance().levelValid( MachPhys::GEO_LOCATOR, hwLevel, swLevel ) );

	return *geoLocatorData_[ hwLevel ][ swLevel ];
}

const MachPhysSpyLocatorData& MachPhysDataImplementation::spyLocatorData( size_t hwLevel, size_t swLevel ) const
{
    PRE( MachPhysLevels::instance().levelValid( MachPhys::SPY_LOCATOR, hwLevel, swLevel ) );

	return *spyLocatorData_[ hwLevel ][ swLevel ];
}

const MachPhysConstructorData& MachPhysDataImplementation::constructorData( MachPhys::ConstructorSubType subType, size_t hwLevel, size_t swLevel ) const
{
    PRE( MachPhysLevels::instance().levelValid( subType, hwLevel, swLevel ) );

    return *((*pDataStore( subType ))[ hwLevel ][ swLevel ]);
}

const MachPhysTechnicianData& MachPhysDataImplementation::technicianData( MachPhys::TechnicianSubType subType, size_t hwLevel, size_t swLevel ) const
{
    PRE( MachPhysLevels::instance().levelValid( subType, hwLevel, swLevel ) );

    return *((*pDataStore( subType ))[ hwLevel ][ swLevel ]);
}

const MachPhysHardwareLabData& MachPhysDataImplementation::hardwareLabData( MachPhys::HardwareLabSubType subType, size_t hwLevel ) const
{
    PRE( MachPhysLevels::instance().levelValid( subType, hwLevel ) );

	return *((*pDataStore( subType ))[ hwLevel ]);

}

const MachPhysSmelterData& MachPhysDataImplementation::smelterData( size_t hwLevel ) const
{
    PRE( MachPhysLevels::instance().levelValid( MachPhys::SMELTER, hwLevel ) );

	return *smelterData_[ hwLevel ];
}

const MachPhysFactoryData& MachPhysDataImplementation::factoryData( MachPhys::FactorySubType subType, size_t hwLevel ) const
{
    PRE( MachPhysLevels::instance().levelValid( subType, hwLevel ) );

    return *((*pDataStore( subType ))[ hwLevel ]);
}

const MachPhysMissileEmplacementData& MachPhysDataImplementation::missileEmplacementData( MachPhys::MissileEmplacementSubType subType, size_t hwLevel ) const
{
    PRE( MachPhysLevels::instance().levelValid( subType, hwLevel ) );

    return *((*pDataStore( subType ))[ hwLevel ]);
}

const MachPhysGarrisonData& MachPhysDataImplementation::garrisonData( size_t hwLevel ) const
{
    PRE( MachPhysLevels::instance().levelValid( MachPhys::GARRISON, hwLevel ) );

	return *garrisonData_[ hwLevel ];
}

const MachPhysMineData& MachPhysDataImplementation::mineData( size_t hwLevel ) const
{
    PRE( MachPhysLevels::instance().levelValid( MachPhys::MINE, hwLevel ) );

	return *mineData_[ hwLevel ];
}

const MachPhysBeaconData& MachPhysDataImplementation::beaconData( size_t hwLevel ) const
{
    PRE( MachPhysLevels::instance().levelValid( MachPhys::BEACON, hwLevel ) );

	return *beaconData_[ hwLevel ];
}

const MachPhysPodData& MachPhysDataImplementation::podData( size_t hwLevel ) const
{
    PRE( MachPhysLevels::instance().levelValid( MachPhys::POD, hwLevel ) );

	return *podData_[ hwLevel ];
}

const MachPhysOreHolographData& MachPhysDataImplementation::oreHolographData() const
{
    PRE( pOreHolographData_ != NULL );

	return *pOreHolographData_;
}

const MachPhysDataImplementation::AdministratorDataStore* MachPhysDataImplementation::pDataStore( MachPhys::AdministratorSubType subType ) const
{
    const AdministratorDataStore* pStore = NULL;

    switch( subType )
    {
        case MachPhys::BOSS:
            pStore = &administratorBossData_;
            break;
        case MachPhys::OVERSEER:
            pStore = &administratorOverseerData_;
            break;
        case MachPhys::COMMANDER:
            pStore = &administratorCommanderData_;
            break;
        default:
            ASSERT_BAD_CASE_INFO( subType );
            break;
    }
    return pStore;
}

const MachPhysDataImplementation::AggressorDataStore* MachPhysDataImplementation::pDataStore( MachPhys::AggressorSubType subType ) const
{
    const AggressorDataStore* pStore = NULL;

    switch( subType )
    {
        case MachPhys::GRUNT:
            pStore = &aggressorGruntData_;
            break;
        case MachPhys::ASSASSIN:
            pStore = &aggressorAssassinData_;
            break;
        case MachPhys::KNIGHT:
            pStore = &aggressorKnightData_;
            break;
        case MachPhys::BALLISTA:
            pStore = &aggressorBallistaData_;
            break;
        case MachPhys::NINJA:
            pStore = &aggressorNinjaData_;
            break;
        default:
            ASSERT_BAD_CASE_INFO( subType );
            break;
    }
    return pStore;
}

const MachPhysDataImplementation::ConstructorDataStore* MachPhysDataImplementation::pDataStore( MachPhys::ConstructorSubType subType ) const
{
    const ConstructorDataStore* pStore = NULL;

    switch( subType )
    {
        case MachPhys::DOZER:
            pStore = &constructorDozerData_;
            break;
        case MachPhys::BUILDER:
            pStore = &constructorBuilderData_;
            break;
        case MachPhys::BEHEMOTH:
            pStore = &constructorBehemothData_;
            break;
        default:
            ASSERT_BAD_CASE_INFO( subType );
            break;
    }
    return pStore;
}

const MachPhysDataImplementation::TechnicianDataStore* MachPhysDataImplementation::pDataStore( MachPhys::TechnicianSubType subType ) const
{
    const TechnicianDataStore* pStore = NULL;

    switch( subType )
    {
        case MachPhys::LAB_TECH:
            pStore = &technicianLabTechData_;
            break;
        case MachPhys::TECH_BOY:
            pStore = &technicianTechBoyData_;
            break;
        case MachPhys::BRAIN_BOX:
            pStore = &technicianBrainBoxData_;
            break;
        default:
            ASSERT_BAD_CASE_INFO( subType );
            break;
    }
    return pStore;
}

const MachPhysDataImplementation::FactoryDataStore* MachPhysDataImplementation::pDataStore( MachPhys::FactorySubType subType ) const
{
    const FactoryDataStore* pStore = NULL;

    switch( subType )
    {
        case MachPhys::MILITARY:
            pStore = &factoryMilitaryData_;
            break;
        case MachPhys::CIVILIAN:
            pStore = &factoryCivilianData_;
            break;
        default:
            ASSERT_BAD_CASE_INFO( subType );
            break;
    }
    return pStore;
}

const MachPhysDataImplementation::HardwareLabDataStore* MachPhysDataImplementation::pDataStore( MachPhys::HardwareLabSubType subType ) const
{
    const HardwareLabDataStore* pStore = NULL;

    switch( subType )
    {
        case MachPhys::LAB_MILITARY:
            pStore = &hardwareLabMilitaryData_;
            break;
        case MachPhys::LAB_CIVILIAN:
            pStore = &hardwareLabCivilianData_;
            break;
        default:
            ASSERT_BAD_CASE_INFO( subType );
            break;
    }
    return pStore;
}

const MachPhysDataImplementation::MissileEmplacementDataStore* MachPhysDataImplementation::pDataStore( MachPhys::MissileEmplacementSubType subType ) const
{
    const MissileEmplacementDataStore* pStore = NULL;

    switch( subType )
    {
        case MachPhys::TURRET:
            pStore = &missileEmplacementTurretData_;
            break;
        case MachPhys::SENTRY:
            pStore = &missileEmplacementSentryData_;
            break;
        case MachPhys::LAUNCHER:
            pStore = &missileEmplacementLauncherData_;
            break;
        case MachPhys::ICBM:
            pStore = &missileEmplacementICBMData_;
            break;
        default:
            ASSERT_BAD_CASE_INFO( subType );
            break;
    }
    return pStore;
}


MachPhysDataImplementation::AdministratorDataStore* MachPhysDataImplementation::pDataStore( MachPhys::AdministratorSubType subType )
{
    AdministratorDataStore* pStore = NULL;

    switch( subType )
    {
        case MachPhys::BOSS:
            pStore = &administratorBossData_;
            break;
        case MachPhys::OVERSEER:
            pStore = &administratorOverseerData_;
            break;
        case MachPhys::COMMANDER:
            pStore = &administratorCommanderData_;
            break;
        default:
            ASSERT_BAD_CASE_INFO( subType );
            break;
    }
    return pStore;
}

MachPhysDataImplementation::AggressorDataStore* MachPhysDataImplementation::pDataStore( MachPhys::AggressorSubType subType )
{
    AggressorDataStore* pStore = NULL;

    switch( subType )
    {
        case MachPhys::GRUNT:
            pStore = &aggressorGruntData_;
            break;
        case MachPhys::ASSASSIN:
            pStore = &aggressorAssassinData_;
            break;
        case MachPhys::KNIGHT:
            pStore = &aggressorKnightData_;
            break;
        case MachPhys::BALLISTA:
            pStore = &aggressorBallistaData_;
            break;
        case MachPhys::NINJA:
            pStore = &aggressorNinjaData_;
            break;
        default:
            ASSERT_BAD_CASE_INFO( subType );
            break;
    }
    return pStore;
}

MachPhysDataImplementation::ConstructorDataStore* MachPhysDataImplementation::pDataStore( MachPhys::ConstructorSubType subType )
{
    ConstructorDataStore* pStore = NULL;

    switch( subType )
    {
        case MachPhys::DOZER:
            pStore = &constructorDozerData_;
            break;
        case MachPhys::BUILDER:
            pStore = &constructorBuilderData_;
            break;
        case MachPhys::BEHEMOTH:
            pStore = &constructorBehemothData_;
            break;
        default:
            ASSERT_BAD_CASE_INFO( subType );
            break;
    }
    return pStore;
}

MachPhysDataImplementation::TechnicianDataStore* MachPhysDataImplementation::pDataStore( MachPhys::TechnicianSubType subType )
{
    TechnicianDataStore* pStore = NULL;

    switch( subType )
    {
        case MachPhys::LAB_TECH:
            pStore = &technicianLabTechData_;
            break;
        case MachPhys::TECH_BOY:
            pStore = &technicianTechBoyData_;
            break;
        case MachPhys::BRAIN_BOX:
            pStore = &technicianBrainBoxData_;
            break;
        default:
            ASSERT_BAD_CASE_INFO( subType );
            break;
    }
    return pStore;
}

MachPhysDataImplementation::FactoryDataStore* MachPhysDataImplementation::pDataStore( MachPhys::FactorySubType subType )
{
    FactoryDataStore* pStore = NULL;

    switch( subType )
    {
        case MachPhys::MILITARY:
            pStore = &factoryMilitaryData_;
            break;
        case MachPhys::CIVILIAN:
            pStore = &factoryCivilianData_;
            break;
        default:
            ASSERT_BAD_CASE_INFO( subType );
            break;
    }
    return pStore;
}

MachPhysDataImplementation::HardwareLabDataStore* MachPhysDataImplementation::pDataStore( MachPhys::HardwareLabSubType subType )
{
    HardwareLabDataStore* pStore = NULL;

    switch( subType )
    {
        case MachPhys::LAB_MILITARY:
            pStore = &hardwareLabMilitaryData_;
            break;
        case MachPhys::LAB_CIVILIAN:
            pStore = &hardwareLabCivilianData_;
            break;
        default:
            ASSERT_BAD_CASE_INFO( subType );
            break;
    }
    return pStore;
}

MachPhysDataImplementation::MissileEmplacementDataStore* MachPhysDataImplementation::pDataStore( MachPhys::MissileEmplacementSubType subType )
{
    MissileEmplacementDataStore* pStore = NULL;

    switch( subType )
    {
        case MachPhys::TURRET:
            pStore = &missileEmplacementTurretData_;
            break;
        case MachPhys::SENTRY:
            pStore = &missileEmplacementSentryData_;
            break;
        case MachPhys::LAUNCHER:
            pStore = &missileEmplacementLauncherData_;
            break;
        case MachPhys::ICBM:
            pStore = &missileEmplacementICBMData_;
            break;
        default:
            ASSERT_BAD_CASE_INFO( subType );
            break;
    }
    return pStore;
}

const MachPhysWeaponData& MachPhysDataImplementation::weaponData( MachPhys::WeaponType type ) const
{
    PRE( type < MachPhys::N_WEAPON_TYPES );
    PRE( weaponData_[ type ] != NULL );

    return *weaponData_[ type ];
}

MachPhysWeaponData* MachPhysDataImplementation::pWeaponData( MachPhys::WeaponType type )
{
    PRE( type < MachPhys::N_WEAPON_TYPES );
    PRE( weaponData_[ type ] != NULL );

    return weaponData_[ type ];
}

MachPhysDataImplementation::WeaponDataStore* MachPhysDataImplementation::pWeaponDataStore()
{
    return &weaponData_;
}

MachPhysGeneralData** MachPhysDataImplementation::pGeneralDataStore()
{
    return &pGeneralData_;
}

const MachPhysGeneralData& MachPhysDataImplementation::generalData()
{
    return *pGeneralData_;
}
/* End DATAIMPL.CPP *************************************************/
