/*
 * L E V E L S . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <algorithm>
//#include "stdlib/string.hpp"
#include "machphys/levels.hpp"

//  These all break our normal rules of no global statics etc. but
//  since they are just arrays of ints we will allow it in this case.
//  This is the master reference for all hardware / software levels -
//  everything else should use the functions provided in this class.

static  size_t aggressorGruntHardwareLevels[] = { 1, 2, 3 };
static  size_t aggressorGruntSoftwareLevels[] = { 1, 2, 3 };
static  size_t aggressorAssassinHardwareLevels[] = { 2, 3 };
static  size_t aggressorAssassinSoftwareLevels[] = { 1, 2, 3, 4, 5 };
static  size_t aggressorKnightHardwareLevels[] = { 3, 4, 5 };
static  size_t aggressorKnightSoftwareLevels[] = { 1, 2, 3, 4, 5 };
static  size_t aggressorBallistaHardwareLevels[] = { 3, 4 };
static  size_t aggressorBallistaSoftwareLevels[] = { 1, 2 };
static  size_t aggressorNinjaHardwareLevels[] = { 5 };
static  size_t aggressorNinjaSoftwareLevels[] = { 1, 2, 3, 4, 5 };

static  size_t geoLocatorHardwareLevels[] = { 1, 2, 3 };
static  size_t geoLocatorSoftwareLevels[] = { 1, 2, 3 };
static  size_t spyLocatorHardwareLevels[] = { 3, 5 };
static  size_t spyLocatorSoftwareLevels[] = { 1, 2, 3, 4, 5 };

static  size_t resourceCarrierHardwareLevels[] = { 1, 2, 3, 4, 5 };
static  size_t resourceCarrierSoftwareLevels[] = { 1, 2, 3, 4 };

static  size_t APCHardwareLevels[] = { 2, 4, 5 };
static  size_t personnelCarrierSoftwareLevels[] = { 1, 2, 3, 4, 5 };

static  size_t technicianLabTechHardwareLevels[] = { 2 };
static  size_t technicianLabTechSoftwareLevels[] = { 1, 2 };
static  size_t technicianTechBoyHardwareLevels[] = { 3 };
static  size_t technicianTechBoySoftwareLevels[] = { 3 };
static  size_t technicianBrainBoxHardwareLevels[] = { 5 };
static  size_t technicianBrainBoxSoftwareLevels[] = { 5 };

static  size_t constructorDozerHardwareLevels[] = { 1 };
static  size_t constructorDozerSoftwareLevels[] = { 1 };
static  size_t constructorBuilderHardwareLevels[] = { 3 };
static  size_t constructorBuilderSoftwareLevels[] = { 1 };
static  size_t constructorBehemothHardwareLevels[] = { 4, 5 };
static  size_t constructorBehemothSoftwareLevels[] = { 1, 2 };

static  size_t administratorBossHardwareLevels[] = { 1, 2 };
static  size_t administratorBossSoftwareLevels[] = { 1, 2, 3, 4, 5 };
static  size_t administratorOverseerHardwareLevels[] = { 2, 3 };
static  size_t administratorOverseerSoftwareLevels[] = { 1, 2, 3, 4, 5 };
static  size_t administratorCommanderHardwareLevels[] = { 4, 5 };
static  size_t administratorCommanderSoftwareLevels[] = { 1, 2, 3, 4, 5 };

static  size_t podHardwareLevels[] = { 1 };
static  size_t hardwareLabMilitaryHardwareLevels[] = { 1, 3 };
static  size_t hardwareLabCivilianHardwareLevels[] = { 1, 3 };
//static  size_t softwareLabHardwareLevels[] = { 1, 3, 5 };
//static  size_t weaponsLabHardwareLevels[] = { 1, 3, 5 };
static  size_t smelterHardwareLevels[] = { 1, 3 };
static  size_t factoryMilitaryHardwareLevels[] = {1, 3, 4, 5 };
static  size_t factoryCivilianHardwareLevels[] = {1, 3, 5 };

static  size_t missileEmplacementTurretHardwareLevels[] = { 1, 2, 3 };
static  size_t missileEmplacementSentryHardwareLevels[] = { 3, 4 };
static  size_t missileEmplacementLauncherHardwareLevels[] = { 4 };
static  size_t missileEmplacementICBMHardwareLevels[] = { 5 };

static  size_t garrisonHardwareLevels[] = { 1 };

static  size_t mineHardwareLevels[] = { 1, 3, 5 };
static  size_t beaconHardwareLevels[] = {1, 3 };




// static
MachPhysLevels& MachPhysLevels::instance()
{
    static MachPhysLevels instance_;
    return instance_;
}

MachPhysLevels::MachPhysLevels()
{

    TEST_INVARIANT;
}

MachPhysLevels::~MachPhysLevels()
{
    TEST_INVARIANT;

}

void MachPhysLevels::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

size_t  MachPhysLevels::nHardwareLevels( MachPhys::MachineType type ) const
{
    PRE( not subTyped( type ) );

    size_t  result = 0;
    size_t* pData = NULL;

    hardwareLevelData( type, &pData, &result );

    return result;
}
size_t  MachPhysLevels::nHardwareLevels( MachPhys::AdministratorSubType subType ) const
{
    size_t  result = 0;
    size_t* pData = NULL;

    hardwareLevelData( subType, &pData, &result );

    return result;
}
size_t  MachPhysLevels::nHardwareLevels( MachPhys::AggressorSubType subType ) const
{
    size_t  result = 0;
    size_t* pData = NULL;

    hardwareLevelData( subType, &pData, &result );

    return result;
}
size_t  MachPhysLevels::nHardwareLevels( MachPhys::ConstructorSubType subType ) const
{
    size_t  result = 0;
    size_t* pData = NULL;

    hardwareLevelData( subType, &pData, &result );

    return result;
}
size_t  MachPhysLevels::nHardwareLevels( MachPhys::TechnicianSubType subType ) const
{
    size_t  result = 0;
    size_t* pData = NULL;

    hardwareLevelData( subType, &pData, &result );

    return result;
}

size_t  MachPhysLevels::nHardwareLevels( MachPhys::ConstructionType type ) const
{
    PRE( type == MachPhys::SMELTER or
         type == MachPhys::GARRISON or type == MachPhys::MINE or type == MachPhys::BEACON or type == MachPhys::POD );

    size_t  result = 0;
    size_t* pData = NULL;

    hardwareLevelData( type, &pData, &result );

    return result;
}
size_t  MachPhysLevels::nHardwareLevels( MachPhys::FactorySubType subType ) const
{
    size_t  result = 0;
    size_t* pData = NULL;

    hardwareLevelData( subType, &pData, &result );

    return result;
}
size_t  MachPhysLevels::nHardwareLevels( MachPhys::HardwareLabSubType subType ) const
{
    size_t  result = 0;
    size_t* pData = NULL;

    hardwareLevelData( subType, &pData, &result );

    return result;
}
size_t  MachPhysLevels::nHardwareLevels( MachPhys::MissileEmplacementSubType subType ) const
{
    size_t  result = 0;
    size_t* pData = NULL;

    hardwareLevelData( subType, &pData, &result );

    return result;
}





size_t  MachPhysLevels::nSoftwareLevels( MachPhys::MachineType type ) const
{
    PRE( not subTyped( type ) );

    size_t  result = 0;
    size_t* pData = NULL;

    hardwareLevelData( type, &pData, &result );

    return result;
}
size_t  MachPhysLevels::nSoftwareLevels( MachPhys::AdministratorSubType subType ) const
{
    size_t  result = 0;
    size_t* pData = NULL;

    hardwareLevelData( subType, &pData, &result );

    return result;
}
size_t  MachPhysLevels::nSoftwareLevels( MachPhys::AggressorSubType subType ) const
{
    size_t  result = 0;
    size_t* pData = NULL;

    hardwareLevelData( subType, &pData, &result );

    return result;
}
size_t  MachPhysLevels::nSoftwareLevels( MachPhys::ConstructorSubType subType ) const
{
    size_t  result = 0;
    size_t* pData = NULL;

    hardwareLevelData( subType, &pData, &result );

    return result;
}
size_t  MachPhysLevels::nSoftwareLevels( MachPhys::TechnicianSubType subType ) const
{
    size_t  result = 0;
    size_t* pData = NULL;

    hardwareLevelData( subType, &pData, &result );

    return result;
}




size_t  MachPhysLevels::hardwareLevel( MachPhys::MachineType type, size_t index ) const
{
    PRE( index < nHardwareLevels( type ) );
    PRE( not subTyped( type ) );

    size_t  nElements = 0;
    size_t* levels = NULL;

    hardwareLevelData( type, &levels, &nElements );

    return levels[ index ];
}
size_t  MachPhysLevels::hardwareLevel( MachPhys::AdministratorSubType subType, size_t index ) const
{
    PRE( index < nHardwareLevels( subType ) );

    size_t  nElements = 0;
    size_t* levels = NULL;

    hardwareLevelData( subType, &levels, &nElements );

    return levels[ index ];
}
size_t  MachPhysLevels::hardwareLevel( MachPhys::AggressorSubType subType, size_t index ) const
{
    PRE( index < nHardwareLevels( subType ) );

    size_t  nElements = 0;
    size_t* levels = NULL;

    hardwareLevelData( subType, &levels, &nElements );

    return levels[ index ];
}
size_t  MachPhysLevels::hardwareLevel( MachPhys::ConstructorSubType subType, size_t index ) const
{
    PRE( index < nHardwareLevels( subType ) );

    size_t  nElements = 0;
    size_t* levels = NULL;

    hardwareLevelData( subType, &levels, &nElements );

    return levels[ index ];
}
size_t  MachPhysLevels::hardwareLevel( MachPhys::TechnicianSubType subType, size_t index ) const
{
    PRE( index < nHardwareLevels( subType ) );

    size_t  nElements = 0;
    size_t* levels = NULL;

    hardwareLevelData( subType, &levels, &nElements );

    return levels[ index ];
}

size_t  MachPhysLevels::hardwareLevel( MachPhys::ConstructionType type, size_t index ) const
{
    PRE( index < nHardwareLevels( type ) );
    PRE( not subTyped( type ) );

    size_t  nElements = 0;
    size_t* levels = NULL;

    hardwareLevelData( type, &levels, &nElements );

    return levels[ index ];
}
size_t  MachPhysLevels::hardwareLevel( MachPhys::FactorySubType subType, size_t index ) const
{
    PRE( index < nHardwareLevels( subType ) );

    size_t  nElements = 0;
    size_t* levels = NULL;

    hardwareLevelData( subType, &levels, &nElements );

    return levels[ index ];
}
size_t  MachPhysLevels::hardwareLevel( MachPhys::HardwareLabSubType subType, size_t index ) const
{
    PRE( index < nHardwareLevels( subType ) );

    size_t  nElements = 0;
    size_t* levels = NULL;

    hardwareLevelData( subType, &levels, &nElements );

    return levels[ index ];
}
size_t  MachPhysLevels::hardwareLevel( MachPhys::MissileEmplacementSubType subType, size_t index ) const
{
    PRE( index < nHardwareLevels( subType ) );

    size_t  nElements = 0;
    size_t* levels = NULL;

    hardwareLevelData( subType, &levels, &nElements );

    return levels[ index ];
}

size_t  MachPhysLevels::softwareLevel( MachPhys::MachineType type, size_t index ) const
{
    PRE( index < nSoftwareLevels( type ) );
    PRE( not subTyped( type ) );

    size_t  nElements = 0;
    size_t* levels = NULL;

    softwareLevelData( type, &levels, &nElements );

    return levels[ index ];
}
size_t  MachPhysLevels::softwareLevel( MachPhys::AdministratorSubType subType, size_t index ) const
{
    PRE( index < nSoftwareLevels( subType ) );

    size_t  nElements = 0;
    size_t* levels = NULL;

    softwareLevelData( subType, &levels, &nElements );

    return levels[ index ];
}
size_t  MachPhysLevels::softwareLevel( MachPhys::AggressorSubType subType, size_t index ) const
{
    PRE( index < nSoftwareLevels( subType ) );

    size_t  nElements = 0;
    size_t* levels = NULL;

    softwareLevelData( subType, &levels, &nElements );

    return levels[ index ];
}
size_t  MachPhysLevels::softwareLevel( MachPhys::ConstructorSubType subType, size_t index ) const
{
    PRE( index < nSoftwareLevels( subType ) );

    size_t  nElements = 0;
    size_t* levels = NULL;

    softwareLevelData( subType, &levels, &nElements );

    return levels[ index ];
}
size_t  MachPhysLevels::softwareLevel( MachPhys::TechnicianSubType subType, size_t index ) const
{
    PRE( index < nSoftwareLevels( subType ) );

    size_t  nElements = 0;
    size_t* levels = NULL;

    softwareLevelData( subType, &levels, &nElements );

    return levels[ index ];
}







bool    MachPhysLevels::levelValid( MachPhys::MachineType type, size_t hardwareLevel, size_t softwareLevel ) const
{
    PRE_INFO( type );
    PRE_INFO( hardwareLevel );
    PRE_INFO( softwareLevel );

    PRE( not subTyped( type ) );

    bool    hardwareLevelFound = false;
    bool    softwareLevelFound = false;
    size_t  nElements = 0;
    size_t* levels = NULL;

    hardwareLevelData( type, &levels, &nElements );
    hardwareLevelFound = levelFound( hardwareLevel, levels, nElements );

    if( hardwareLevelFound )
    {
        softwareLevelData( type, &levels, &nElements );
        softwareLevelFound = levelFound( softwareLevel, levels, nElements );
    }

    return hardwareLevelFound and softwareLevelFound;
}
bool    MachPhysLevels::levelValid( MachPhys::MachineType type, size_t hardwareLevel ) const
{
    PRE_INFO( type );
    PRE_INFO( hardwareLevel );

    PRE( not subTyped( type ) );

    bool    hardwareLevelFound = false;
    size_t  nElements = 0;
    size_t* levels = NULL;

    hardwareLevelData( type, &levels, &nElements );
    hardwareLevelFound = levelFound( hardwareLevel, levels, nElements );

    return hardwareLevelFound;
}
bool    MachPhysLevels::levelValid( MachPhys::AdministratorSubType subType, size_t hardwareLevel, size_t softwareLevel ) const
{
    PRE_INFO( subType );
    PRE_INFO( hardwareLevel );

    bool    hardwareLevelFound = false;
    bool    softwareLevelFound = false;
    size_t  nElements = 0;
    size_t* levels = NULL;

    hardwareLevelData( subType, &levels, &nElements );
    hardwareLevelFound = levelFound( hardwareLevel, levels, nElements );

    if( hardwareLevelFound )
    {
        softwareLevelData( subType, &levels, &nElements );
        softwareLevelFound = levelFound( softwareLevel, levels, nElements );
    }

    return hardwareLevelFound and softwareLevelFound;
}
bool    MachPhysLevels::levelValid( MachPhys::AdministratorSubType subType, size_t hardwareLevel ) const
{
    PRE_INFO( subType );
    PRE_INFO( hardwareLevel );

    bool    hardwareLevelFound = false;
    size_t  nElements = 0;
    size_t* levels = NULL;

    hardwareLevelData( subType, &levels, &nElements );
    hardwareLevelFound = levelFound( hardwareLevel, levels, nElements );

    return hardwareLevelFound;
}
bool    MachPhysLevels::levelValid( MachPhys::AggressorSubType subType, size_t hardwareLevel, size_t softwareLevel ) const
{
    PRE_INFO( subType );
    PRE_INFO( hardwareLevel );
    PRE_INFO( softwareLevel );

    bool    hardwareLevelFound = false;
    bool    softwareLevelFound = false;
    size_t  nElements = 0;
    size_t* levels = NULL;

    hardwareLevelData( subType, &levels, &nElements );
    hardwareLevelFound = levelFound( hardwareLevel, levels, nElements );

    if( hardwareLevelFound )
    {
        softwareLevelData( subType, &levels, &nElements );
        softwareLevelFound = levelFound( softwareLevel, levels, nElements );
    }

    return hardwareLevelFound and softwareLevelFound;
}
bool    MachPhysLevels::levelValid( MachPhys::AggressorSubType subType, size_t hardwareLevel ) const
{
    PRE_INFO( subType );
    PRE_INFO( hardwareLevel );

    bool    hardwareLevelFound = false;
    size_t  nElements = 0;
    size_t* levels = NULL;

    hardwareLevelData( subType, &levels, &nElements );
    hardwareLevelFound = levelFound( hardwareLevel, levels, nElements );

    return hardwareLevelFound;
}
bool    MachPhysLevels::levelValid( MachPhys::ConstructorSubType subType, size_t hardwareLevel, size_t softwareLevel ) const
{
    PRE_INFO( subType );
    PRE_INFO( hardwareLevel );
    PRE_INFO( softwareLevel );

    bool    hardwareLevelFound = false;
    bool    softwareLevelFound = false;
    size_t  nElements = 0;
    size_t* levels = NULL;

    hardwareLevelData( subType, &levels, &nElements );
    hardwareLevelFound = levelFound( hardwareLevel, levels, nElements );

    if( hardwareLevelFound )
    {
        softwareLevelData( subType, &levels, &nElements );
        softwareLevelFound = levelFound( softwareLevel, levels, nElements );
    }

    return hardwareLevelFound and softwareLevelFound;
}
bool    MachPhysLevels::levelValid( MachPhys::ConstructorSubType subType, size_t hardwareLevel ) const
{
    PRE_INFO( subType );
    PRE_INFO( hardwareLevel );

    bool    hardwareLevelFound = false;
    size_t  nElements = 0;
    size_t* levels = NULL;

    hardwareLevelData( subType, &levels, &nElements );
    hardwareLevelFound = levelFound( hardwareLevel, levels, nElements );

    return hardwareLevelFound;
}
bool    MachPhysLevels::levelValid( MachPhys::TechnicianSubType subType, size_t hardwareLevel, size_t softwareLevel ) const
{
    PRE_INFO( subType );
    PRE_INFO( hardwareLevel );
    PRE_INFO( softwareLevel );

    bool    hardwareLevelFound = false;
    bool    softwareLevelFound = false;
    size_t  nElements = 0;
    size_t* levels = NULL;

    hardwareLevelData( subType, &levels, &nElements );
    hardwareLevelFound = levelFound( hardwareLevel, levels, nElements );

    if( hardwareLevelFound )
    {
        softwareLevelData( subType, &levels, &nElements );
        softwareLevelFound = levelFound( softwareLevel, levels, nElements );
    }

    return hardwareLevelFound and softwareLevelFound;
}
bool    MachPhysLevels::levelValid( MachPhys::TechnicianSubType subType, size_t hardwareLevel ) const
{
    PRE_INFO( subType );
    PRE_INFO( hardwareLevel );

    bool    hardwareLevelFound = false;
    size_t  nElements = 0;
    size_t* levels = NULL;

    hardwareLevelData( subType, &levels, &nElements );
    hardwareLevelFound = levelFound( hardwareLevel, levels, nElements );

    return hardwareLevelFound;
}

bool    MachPhysLevels::levelValid( MachPhys::ConstructionType type, size_t hardwareLevel ) const
{
    PRE_INFO( type );
    PRE_INFO( hardwareLevel );

    PRE( not subTyped( type ) );

    bool    hardwareLevelFound = false;
    size_t  nElements = 0;
    size_t* levels = NULL;

    hardwareLevelData( type, &levels, &nElements );
    hardwareLevelFound = levelFound( hardwareLevel, levels, nElements );

    return hardwareLevelFound;
}
bool    MachPhysLevels::levelValid( MachPhys::FactorySubType subType, size_t hardwareLevel ) const
{
    PRE_INFO( subType );
    PRE_INFO( hardwareLevel );

    bool    hardwareLevelFound = false;
    size_t  nElements = 0;
    size_t* levels = NULL;

    hardwareLevelData( subType, &levels, &nElements );
    hardwareLevelFound = levelFound( hardwareLevel, levels, nElements );

    return hardwareLevelFound;
}
bool    MachPhysLevels::levelValid( MachPhys::HardwareLabSubType subType, size_t hardwareLevel ) const
{
    PRE_INFO( subType );
    PRE_INFO( hardwareLevel );

    bool    hardwareLevelFound = false;
    size_t  nElements = 0;
    size_t* levels = NULL;

    hardwareLevelData( subType, &levels, &nElements );
    hardwareLevelFound = levelFound( hardwareLevel, levels, nElements );

    return hardwareLevelFound;
}
bool    MachPhysLevels::levelValid( MachPhys::MissileEmplacementSubType subType, size_t hardwareLevel ) const
{
    PRE_INFO( subType );
    PRE_INFO( hardwareLevel );

    bool    hardwareLevelFound = false;
    size_t  nElements = 0;
    size_t* levels = NULL;

    hardwareLevelData( subType, &levels, &nElements );
    hardwareLevelFound = levelFound( hardwareLevel, levels, nElements );

    return hardwareLevelFound;
}




void  MachPhysLevels::hardwareLevelData( MachPhys::MachineType type, size_t** pDataArray, size_t* pNElements ) const
{
    PRE( not subTyped( type ) );

    switch( type )
    {
        case MachPhys::GEO_LOCATOR:
            *pNElements = elementsof( geoLocatorHardwareLevels );
            *pDataArray = geoLocatorHardwareLevels;
            break;
        case MachPhys::SPY_LOCATOR:
            *pNElements = elementsof( spyLocatorHardwareLevels );
            *pDataArray = spyLocatorHardwareLevels;
            break;
        case MachPhys::RESOURCE_CARRIER:
            *pNElements = elementsof( resourceCarrierHardwareLevels );
            *pDataArray = resourceCarrierHardwareLevels;
            break;
        case MachPhys::APC:
            *pNElements = elementsof( APCHardwareLevels );
            *pDataArray = APCHardwareLevels;
            break;
        default:
            ASSERT_BAD_CASE_INFO( type );
            break;
    }
}
void  MachPhysLevels::hardwareLevelData( MachPhys::AdministratorSubType subType, size_t** pDataArray, size_t* pNElements ) const
{
    switch( subType )
    {
        case MachPhys::BOSS:
            *pNElements = elementsof( administratorBossHardwareLevels );
            *pDataArray = administratorBossHardwareLevels;
            break;
        case MachPhys::OVERSEER:
            *pNElements = elementsof( administratorOverseerHardwareLevels );
            *pDataArray = administratorOverseerHardwareLevels;
            break;
        case MachPhys::COMMANDER:
            *pNElements = elementsof( administratorCommanderHardwareLevels );
            *pDataArray = administratorCommanderHardwareLevels;
            break;
        default:
            ASSERT_BAD_CASE_INFO( subType );
            break;
    }
}
void  MachPhysLevels::hardwareLevelData( MachPhys::AggressorSubType subType, size_t** pDataArray, size_t* pNElements ) const
{
    switch( subType )
    {
        case MachPhys::GRUNT:
            *pNElements = elementsof( aggressorGruntHardwareLevels );
            *pDataArray = aggressorGruntHardwareLevels;
            break;
        case MachPhys::ASSASSIN:
            *pNElements = elementsof( aggressorAssassinHardwareLevels );
            *pDataArray = aggressorAssassinHardwareLevels;
            break;
        case MachPhys::KNIGHT:
            *pNElements = elementsof( aggressorKnightHardwareLevels );
            *pDataArray = aggressorKnightHardwareLevels;
            break;
        case MachPhys::BALLISTA:
            *pNElements = elementsof( aggressorBallistaHardwareLevels );
            *pDataArray = aggressorBallistaHardwareLevels;
            break;
        case MachPhys::NINJA:
            *pNElements = elementsof( aggressorNinjaHardwareLevels );
            *pDataArray = aggressorNinjaHardwareLevels;
            break;
        default:
            ASSERT_BAD_CASE_INFO( subType );
            break;
    }
}
void  MachPhysLevels::hardwareLevelData( MachPhys::ConstructorSubType subType, size_t** pDataArray, size_t* pNElements ) const
{
    switch( subType )
    {
        case MachPhys::DOZER:
            *pNElements = elementsof( constructorDozerHardwareLevels );
            *pDataArray = constructorDozerHardwareLevels;
            break;
        case MachPhys::BUILDER:
            *pNElements = elementsof( constructorBuilderHardwareLevels );
            *pDataArray = constructorBuilderHardwareLevels;
            break;
        case MachPhys::BEHEMOTH:
            *pNElements = elementsof( constructorBehemothHardwareLevels );
            *pDataArray = constructorBehemothHardwareLevels;
            break;
        default:
            ASSERT_BAD_CASE_INFO( subType );
            break;
    }
}
void  MachPhysLevels::hardwareLevelData( MachPhys::TechnicianSubType subType, size_t** pDataArray, size_t* pNElements ) const
{
    switch( subType )
    {
        case MachPhys::LAB_TECH:
            *pNElements = elementsof( technicianLabTechHardwareLevels );
            *pDataArray = technicianLabTechHardwareLevels;
            break;
        case MachPhys::TECH_BOY:
            *pNElements = elementsof( technicianTechBoyHardwareLevels );
            *pDataArray = technicianTechBoyHardwareLevels;
            break;
        case MachPhys::BRAIN_BOX:
            *pNElements = elementsof( technicianBrainBoxHardwareLevels );
            *pDataArray = technicianBrainBoxHardwareLevels;
            break;
        default:
            ASSERT_BAD_CASE_INFO( subType );
            break;
    }
}

void  MachPhysLevels::hardwareLevelData( MachPhys::ConstructionType type, size_t** pDataArray, size_t* pNElements ) const
{
    PRE( not subTyped( type ) );

    switch( type )
    {
        case MachPhys::SMELTER:
            *pNElements = elementsof( smelterHardwareLevels );
            *pDataArray = smelterHardwareLevels;
            break;
        case MachPhys::GARRISON:
            *pNElements = elementsof( garrisonHardwareLevels );
            *pDataArray = garrisonHardwareLevels;
            break;
        case MachPhys::MINE:
            *pNElements = elementsof( mineHardwareLevels );
            *pDataArray = mineHardwareLevels;
            break;
        case MachPhys::BEACON:
            *pNElements = elementsof( beaconHardwareLevels );
            *pDataArray = beaconHardwareLevels;
            break;
        case MachPhys::POD:
            *pNElements = elementsof( podHardwareLevels );
            *pDataArray = podHardwareLevels;
            break;
        default:
            ASSERT_BAD_CASE_INFO( type );
            break;
    }
}
void  MachPhysLevels::hardwareLevelData( MachPhys::FactorySubType subType, size_t** pDataArray, size_t* pNElements ) const
{
    switch( subType )
    {
        case MachPhys::CIVILIAN:
            *pNElements = elementsof( factoryCivilianHardwareLevels );
            *pDataArray = factoryCivilianHardwareLevels;
            break;
        case MachPhys::MILITARY:
            *pNElements = elementsof( factoryMilitaryHardwareLevels );
            *pDataArray = factoryMilitaryHardwareLevels;
            break;
        default:
            ASSERT_BAD_CASE_INFO( subType );
            break;
    }
}
void  MachPhysLevels::hardwareLevelData( MachPhys::HardwareLabSubType subType, size_t** pDataArray, size_t* pNElements ) const
{
    switch( subType )
    {
        case MachPhys::LAB_CIVILIAN:
            *pNElements = elementsof( hardwareLabCivilianHardwareLevels );
            *pDataArray = hardwareLabCivilianHardwareLevels;
            break;
        case MachPhys::LAB_MILITARY:
            *pNElements = elementsof( hardwareLabMilitaryHardwareLevels );
            *pDataArray = hardwareLabMilitaryHardwareLevels;
            break;
        default:
            ASSERT_BAD_CASE_INFO( subType );
            break;
    }
}
void  MachPhysLevels::hardwareLevelData( MachPhys::MissileEmplacementSubType subType, size_t** pDataArray, size_t* pNElements ) const
{
    switch( subType )
    {
        case MachPhys::TURRET:
            *pNElements = elementsof( missileEmplacementTurretHardwareLevels );
            *pDataArray = missileEmplacementTurretHardwareLevels;
            break;
        case MachPhys::SENTRY:
            *pNElements = elementsof( missileEmplacementSentryHardwareLevels );
            *pDataArray = missileEmplacementSentryHardwareLevels;
            break;
        case MachPhys::LAUNCHER:
            *pNElements = elementsof( missileEmplacementLauncherHardwareLevels );
            *pDataArray = missileEmplacementLauncherHardwareLevels;
            break;
        case MachPhys::ICBM:
            *pNElements = elementsof( missileEmplacementICBMHardwareLevels );
            *pDataArray = missileEmplacementICBMHardwareLevels;
            break;
        default:
            ASSERT_BAD_CASE_INFO( subType );
            break;
    }
}

void  MachPhysLevels::softwareLevelData( MachPhys::MachineType type, size_t** pDataArray, size_t* pNElements ) const
{
    PRE( not subTyped( type ) );

    switch( type )
    {
        case MachPhys::GEO_LOCATOR:
            *pNElements = elementsof( geoLocatorSoftwareLevels );
            *pDataArray = geoLocatorSoftwareLevels;
            break;
        case MachPhys::SPY_LOCATOR:
            *pNElements = elementsof( spyLocatorSoftwareLevels );
            *pDataArray = spyLocatorSoftwareLevels;
            break;
        case MachPhys::RESOURCE_CARRIER:
            *pNElements = elementsof( resourceCarrierSoftwareLevels );
            *pDataArray = resourceCarrierSoftwareLevels;
            break;
        case MachPhys::APC:
            *pNElements = elementsof( personnelCarrierSoftwareLevels );
            *pDataArray = personnelCarrierSoftwareLevels;
            break;
        default:
            ASSERT_BAD_CASE_INFO( type );
            break;
    }
}
void  MachPhysLevels::softwareLevelData( MachPhys::AdministratorSubType subType, size_t** pDataArray, size_t* pNElements ) const
{
    switch( subType )
    {
        case MachPhys::BOSS:
            *pNElements = elementsof( administratorBossSoftwareLevels );
            *pDataArray = administratorBossSoftwareLevels;
            break;
        case MachPhys::OVERSEER:
            *pNElements = elementsof( administratorOverseerSoftwareLevels );
            *pDataArray = administratorOverseerSoftwareLevels;
            break;
        case MachPhys::COMMANDER:
            *pNElements = elementsof( administratorCommanderSoftwareLevels );
            *pDataArray = administratorCommanderSoftwareLevels;
            break;
        default:
            ASSERT_BAD_CASE_INFO( subType );
            break;
    }
}
void  MachPhysLevels::softwareLevelData( MachPhys::AggressorSubType subType, size_t** pDataArray, size_t* pNElements ) const
{
    switch( subType )
    {
        case MachPhys::GRUNT:
            *pNElements = elementsof( aggressorGruntSoftwareLevels );
            *pDataArray = aggressorGruntSoftwareLevels;
            break;
        case MachPhys::ASSASSIN:
            *pNElements = elementsof( aggressorAssassinSoftwareLevels );
            *pDataArray = aggressorAssassinSoftwareLevels;
            break;
        case MachPhys::KNIGHT:
            *pNElements = elementsof( aggressorKnightSoftwareLevels );
            *pDataArray = aggressorKnightSoftwareLevels;
            break;
        case MachPhys::BALLISTA:
            *pNElements = elementsof( aggressorBallistaSoftwareLevels );
            *pDataArray = aggressorBallistaSoftwareLevels;
            break;
        case MachPhys::NINJA:
            *pNElements = elementsof( aggressorNinjaSoftwareLevels );
            *pDataArray = aggressorNinjaSoftwareLevels;
            break;
        default:
            ASSERT_BAD_CASE_INFO( subType );
            break;
    }
}
void  MachPhysLevels::softwareLevelData( MachPhys::ConstructorSubType subType, size_t** pDataArray, size_t* pNElements ) const
{
    switch( subType )
    {
        case MachPhys::DOZER:
            *pNElements = elementsof( constructorDozerSoftwareLevels );
            *pDataArray = constructorDozerSoftwareLevels;
            break;
        case MachPhys::BUILDER:
            *pNElements = elementsof( constructorBuilderSoftwareLevels );
            *pDataArray = constructorBuilderSoftwareLevels;
            break;
        case MachPhys::BEHEMOTH:
            *pNElements = elementsof( constructorBehemothSoftwareLevels );
            *pDataArray = constructorBehemothSoftwareLevels;
            break;
        default:
            ASSERT_BAD_CASE_INFO( subType );
            break;
    }
}
void  MachPhysLevels::softwareLevelData( MachPhys::TechnicianSubType subType, size_t** pDataArray, size_t* pNElements ) const
{
    switch( subType )
    {
        case MachPhys::LAB_TECH:
            *pNElements = elementsof( technicianLabTechSoftwareLevels );
            *pDataArray = technicianLabTechSoftwareLevels;
            break;
        case MachPhys::TECH_BOY:
            *pNElements = elementsof( technicianTechBoySoftwareLevels );
            *pDataArray = technicianTechBoySoftwareLevels;
            break;
        case MachPhys::BRAIN_BOX:
            *pNElements = elementsof( technicianBrainBoxSoftwareLevels );
            *pDataArray = technicianBrainBoxSoftwareLevels;
            break;
        default:
            ASSERT_BAD_CASE_INFO( subType );
            break;
    }
}



bool    MachPhysLevels::levelFound( size_t level, const size_t* levels, size_t nLevels ) const
{
    bool    found = false;

    for( size_t i = 0; i < nLevels and not found; ++i )
    {
        if( level == levels[ i ] )
            found = true;
    }

    return found;
}

bool MachPhysLevels::subTyped( MachPhys::MachineType type ) const
{
    PRE_INFO( type );

    return type == MachPhys::AGGRESSOR or type == MachPhys::CONSTRUCTOR or
           type == MachPhys::ADMINISTRATOR or type == MachPhys::TECHNICIAN;
}

bool MachPhysLevels::subTyped( MachPhys::ConstructionType type ) const
{
    PRE_INFO( type );

    return 	type == MachPhys::FACTORY or
    		type == MachPhys::MISSILE_EMPLACEMENT or
    		type == MachPhys::HARDWARE_LAB;
}


// ********************************************************************************

MachPhysLevelIndexMap   MachPhysLevels::hardwareLevelIndexMap( MachPhys::MachineType type ) const
{
    PRE( not subTyped( type ) );

    size_t* dataArray = NULL;
    size_t  nElements = 0;

    hardwareLevelData( type, &dataArray, &nElements );

    return MachPhysLevelIndexMap( dataArray, nElements );
}

MachPhysLevelIndexMap   MachPhysLevels::hardwareLevelIndexMap( MachPhys::AdministratorSubType subType ) const
{
    size_t* dataArray = NULL;
    size_t  nElements = 0;

    hardwareLevelData( subType, &dataArray, &nElements );

    return MachPhysLevelIndexMap( dataArray, nElements );
}

MachPhysLevelIndexMap   MachPhysLevels::hardwareLevelIndexMap( MachPhys::AggressorSubType subType ) const
{
    size_t* dataArray = NULL;
    size_t  nElements = 0;

    hardwareLevelData( subType, &dataArray, &nElements );

    return MachPhysLevelIndexMap( dataArray, nElements );
}

MachPhysLevelIndexMap   MachPhysLevels::hardwareLevelIndexMap( MachPhys::ConstructorSubType subType ) const
{
    size_t* dataArray = NULL;
    size_t  nElements = 0;

    hardwareLevelData( subType, &dataArray, &nElements );

    return MachPhysLevelIndexMap( dataArray, nElements );
}

MachPhysLevelIndexMap   MachPhysLevels::hardwareLevelIndexMap( MachPhys::TechnicianSubType subType ) const
{
    size_t* dataArray = NULL;
    size_t  nElements = 0;

    hardwareLevelData( subType, &dataArray, &nElements );

    return MachPhysLevelIndexMap( dataArray, nElements );
}


MachPhysLevelIndexMap   MachPhysLevels::softwareLevelIndexMap( MachPhys::MachineType type ) const
{
    PRE( not subTyped( type ) );

    size_t* dataArray = NULL;
    size_t  nElements = 0;

    softwareLevelData( type, &dataArray, &nElements );

    return MachPhysLevelIndexMap( dataArray, nElements );
}

MachPhysLevelIndexMap   MachPhysLevels::softwareLevelIndexMap( MachPhys::AdministratorSubType subType ) const
{
    size_t* dataArray = NULL;
    size_t  nElements = 0;

    softwareLevelData( subType, &dataArray, &nElements );

    return MachPhysLevelIndexMap( dataArray, nElements );
}

MachPhysLevelIndexMap   MachPhysLevels::softwareLevelIndexMap( MachPhys::AggressorSubType subType ) const
{
    size_t* dataArray = NULL;
    size_t  nElements = 0;

    softwareLevelData( subType, &dataArray, &nElements );

    return MachPhysLevelIndexMap( dataArray, nElements );
}

MachPhysLevelIndexMap   MachPhysLevels::softwareLevelIndexMap( MachPhys::ConstructorSubType subType ) const
{
    size_t* dataArray = NULL;
    size_t  nElements = 0;

    softwareLevelData( subType, &dataArray, &nElements );

    return MachPhysLevelIndexMap( dataArray, nElements );
}

MachPhysLevelIndexMap   MachPhysLevels::softwareLevelIndexMap( MachPhys::TechnicianSubType subType ) const
{
    size_t* dataArray = NULL;
    size_t  nElements = 0;

    softwareLevelData( subType, &dataArray, &nElements );

    return MachPhysLevelIndexMap( dataArray, nElements );
}


MachPhysLevelIndexMap   MachPhysLevels::hardwareLevelIndexMap( MachPhys::ConstructionType type ) const
{
    PRE( not subTyped( type ) );

    size_t* dataArray = NULL;
    size_t  nElements = 0;

    hardwareLevelData( type, &dataArray, &nElements );

    return MachPhysLevelIndexMap( dataArray, nElements );
}

MachPhysLevelIndexMap   MachPhysLevels::hardwareLevelIndexMap( MachPhys::FactorySubType subType ) const
{
    size_t* dataArray = NULL;
    size_t  nElements = 0;

    hardwareLevelData( subType, &dataArray, &nElements );

    return MachPhysLevelIndexMap( dataArray, nElements );
}

MachPhysLevelIndexMap   MachPhysLevels::hardwareLevelIndexMap( MachPhys::HardwareLabSubType subType ) const
{
    size_t* dataArray = NULL;
    size_t  nElements = 0;

    hardwareLevelData( subType, &dataArray, &nElements );

    return MachPhysLevelIndexMap( dataArray, nElements );
}

MachPhysLevelIndexMap   MachPhysLevels::hardwareLevelIndexMap( MachPhys::MissileEmplacementSubType subType ) const
{
    size_t* dataArray = NULL;
    size_t  nElements = 0;

    hardwareLevelData( subType, &dataArray, &nElements );

    return MachPhysLevelIndexMap( dataArray, nElements );
}

// ********************************************************************************


size_t  MachPhysLevels::uniqueHardwareIndex( MachPhys::MachineType type, size_t hardwareLevel ) const
{
    PRE( not subTyped( type ) );
    PRE( levelValid( type, hardwareLevel ) );

    size_t result = hardwareLevelIndexMap( type ).index( hardwareLevel );

    POST( result < nHardwareIndices( type ) );
    return result;
}
size_t  MachPhysLevels::uniqueHardwareIndex( MachPhys::AdministratorSubType subType, size_t hardwareLevel ) const
{
    PRE( levelValid( subType, hardwareLevel ) );

    size_t result = 0;

    switch( subType )
    {
        case MachPhys::BOSS:
            result = hardwareLevelIndexMap( subType ).index( hardwareLevel );
            break;
        case MachPhys::OVERSEER:
            result = elementsof( administratorBossHardwareLevels ) +
                    hardwareLevelIndexMap( subType ).index( hardwareLevel );
            break;
        case MachPhys::COMMANDER:
            result = elementsof( administratorBossHardwareLevels ) +
                    elementsof( administratorOverseerHardwareLevels ) +
                    hardwareLevelIndexMap( subType ).index( hardwareLevel );
            break;
    }

    POST( result < nHardwareIndices( MachPhys::ADMINISTRATOR ) );
    return result;
}
size_t  MachPhysLevels::uniqueHardwareIndex( MachPhys::AggressorSubType subType, size_t hardwareLevel ) const
{
    PRE( levelValid( subType, hardwareLevel ) );

    size_t result = 0;

    switch( subType )
    {
        case MachPhys::GRUNT:
            result = hardwareLevelIndexMap( subType ).index( hardwareLevel );
            break;
        case MachPhys::ASSASSIN:
            result = elementsof( aggressorGruntHardwareLevels ) +
                    hardwareLevelIndexMap( subType ).index( hardwareLevel );
            break;
        case MachPhys::KNIGHT:
            result = elementsof( aggressorGruntHardwareLevels ) +
                    elementsof( aggressorAssassinHardwareLevels ) +
                    hardwareLevelIndexMap( subType ).index( hardwareLevel );
            break;
        case MachPhys::BALLISTA:
            result = elementsof( aggressorGruntHardwareLevels ) +
                    elementsof( aggressorAssassinHardwareLevels ) +
                    elementsof( aggressorKnightHardwareLevels ) +
                    hardwareLevelIndexMap( subType ).index( hardwareLevel );
            break;
        case MachPhys::NINJA:
            result = elementsof( aggressorGruntHardwareLevels ) +
                    elementsof( aggressorAssassinHardwareLevels ) +
                    elementsof( aggressorKnightHardwareLevels ) +
                    elementsof( aggressorBallistaHardwareLevels ) +
                    hardwareLevelIndexMap( subType ).index( hardwareLevel );
            break;
    }

    POST( result < nHardwareIndices( MachPhys::AGGRESSOR ) );
    return result;
}
size_t  MachPhysLevels::uniqueHardwareIndex( MachPhys::ConstructorSubType subType, size_t hardwareLevel ) const
{
    PRE( levelValid( subType, hardwareLevel ) );

    size_t result = 0;

    switch( subType )
    {
        case MachPhys::DOZER:
            result = hardwareLevelIndexMap( subType ).index( hardwareLevel );
            break;
        case MachPhys::BUILDER:
            result = elementsof( constructorDozerHardwareLevels ) +
                    hardwareLevelIndexMap( subType ).index( hardwareLevel );
            break;
        case MachPhys::BEHEMOTH:
            result = elementsof( constructorDozerHardwareLevels ) +
                     elementsof( constructorBuilderHardwareLevels ) +
                    hardwareLevelIndexMap( subType ).index( hardwareLevel );
            break;
    }

    POST( result < nHardwareIndices( MachPhys::CONSTRUCTOR ) );
    return result;
}
size_t  MachPhysLevels::uniqueHardwareIndex( MachPhys::TechnicianSubType subType, size_t hardwareLevel ) const
{
    PRE( levelValid( subType, hardwareLevel ) );

    size_t result = 0;

    switch( subType )
    {
        case MachPhys::LAB_TECH:
            result = hardwareLevelIndexMap( subType ).index( hardwareLevel );
            break;
        case MachPhys::TECH_BOY:
            result = elementsof( technicianLabTechHardwareLevels ) +
                    hardwareLevelIndexMap( subType ).index( hardwareLevel );
            break;
        case MachPhys::BRAIN_BOX:
            result = elementsof( technicianLabTechHardwareLevels ) +
                     elementsof( technicianTechBoyHardwareLevels ) +
                    hardwareLevelIndexMap( subType ).index( hardwareLevel );
            break;
    }

    POST( result < nHardwareIndices( MachPhys::TECHNICIAN ) );
    return result;
}

size_t  MachPhysLevels::uniqueHardwareIndex( MachPhys::ConstructionType type, size_t hardwareLevel ) const
{
    PRE( not subTyped( type ) );
    PRE( levelValid( type, hardwareLevel ) );

    size_t result = hardwareLevelIndexMap( type ).index( hardwareLevel );

    POST( result < nHardwareIndices( type ) );
    return result;
}
size_t  MachPhysLevels::uniqueHardwareIndex( MachPhys::FactorySubType subType, size_t hardwareLevel ) const
{
    PRE( levelValid( subType, hardwareLevel ) );

    size_t result = 0;

    switch( subType )
    {
        case MachPhys::MILITARY:
            result = hardwareLevelIndexMap( subType ).index( hardwareLevel );
            break;
        case MachPhys::CIVILIAN:
            result = elementsof( factoryMilitaryHardwareLevels ) +
                    hardwareLevelIndexMap( subType ).index( hardwareLevel );
            break;
    }

    POST( result < nHardwareIndices( MachPhys::FACTORY ) );
    return result;
}
size_t  MachPhysLevels::uniqueHardwareIndex( MachPhys::HardwareLabSubType subType, size_t hardwareLevel ) const
{
    PRE( levelValid( subType, hardwareLevel ) );

    size_t result = 0;

    switch( subType )
    {
        case MachPhys::MILITARY:
            result = hardwareLevelIndexMap( subType ).index( hardwareLevel );
            break;
        case MachPhys::CIVILIAN:
            result = elementsof( hardwareLabMilitaryHardwareLevels ) +
                    hardwareLevelIndexMap( subType ).index( hardwareLevel );
            break;
    }

    POST( result < nHardwareIndices( MachPhys::FACTORY ) );
    return result;
}
size_t  MachPhysLevels::uniqueHardwareIndex( MachPhys::MissileEmplacementSubType subType, size_t hardwareLevel ) const
{
    PRE( levelValid( subType, hardwareLevel ) );

    size_t result = 0;

    switch( subType )
    {
        case MachPhys::TURRET:
            result = hardwareLevelIndexMap( subType ).index( hardwareLevel );
            break;
        case MachPhys::SENTRY:
            result = elementsof( missileEmplacementTurretHardwareLevels ) +
                    hardwareLevelIndexMap( subType ).index( hardwareLevel );
            break;
        case MachPhys::LAUNCHER:
            result = elementsof( missileEmplacementTurretHardwareLevels ) +
                    elementsof( missileEmplacementSentryHardwareLevels ) +
                    hardwareLevelIndexMap( subType ).index( hardwareLevel );
            break;
        case MachPhys::ICBM:
            result = elementsof( missileEmplacementTurretHardwareLevels ) +
                    elementsof( missileEmplacementSentryHardwareLevels ) +
                    elementsof( missileEmplacementLauncherHardwareLevels ) +
                    hardwareLevelIndexMap( subType ).index( hardwareLevel );
            break;
    }

    POST( result < nHardwareIndices( MachPhys::MISSILE_EMPLACEMENT ) );
    return result;
}

size_t  MachPhysLevels::nHardwareIndices( MachPhys::MachineType type ) const
{
    size_t result = 0;

    switch( type )
    {
        case MachPhys::AGGRESSOR:
            result = elementsof( aggressorGruntHardwareLevels ) +
                    elementsof( aggressorAssassinHardwareLevels ) +
                    elementsof( aggressorKnightHardwareLevels ) +
                    elementsof( aggressorBallistaHardwareLevels ) +
                    elementsof( aggressorNinjaHardwareLevels );
            break;
        case MachPhys::GEO_LOCATOR:
            result = elementsof( geoLocatorHardwareLevels );
            break;
        case MachPhys::SPY_LOCATOR:
            result = elementsof( spyLocatorHardwareLevels );
            break;
        case MachPhys::CONSTRUCTOR:
            result = elementsof( constructorDozerHardwareLevels ) +
                    elementsof( constructorBuilderHardwareLevels ) +
                    elementsof( constructorBehemothHardwareLevels );
            break;
        case MachPhys::ADMINISTRATOR:
            result = elementsof( administratorBossHardwareLevels ) +
                    elementsof( administratorOverseerHardwareLevels ) +
                    elementsof( administratorCommanderHardwareLevels );
            break;
        case MachPhys::RESOURCE_CARRIER:
            result = elementsof( resourceCarrierHardwareLevels );
            break;
        case MachPhys::APC:
            result = elementsof( APCHardwareLevels );
            break;
        case MachPhys::TECHNICIAN:
            result = elementsof( technicianLabTechHardwareLevels ) +
                    elementsof( technicianTechBoyHardwareLevels ) +
                    elementsof( technicianBrainBoxHardwareLevels );
            break;
    }

    return result;
}

size_t  MachPhysLevels::nHardwareIndices( MachPhys::ConstructionType type ) const
{
    size_t result = 0;

    switch( type )
    {
        case MachPhys::HARDWARE_LAB:
            result = elementsof( hardwareLabMilitaryHardwareLevels ) +
					elementsof( hardwareLabCivilianHardwareLevels );
            break;
//Removed        case MachPhys::SOFTWARE_LAB:
//Removed            result = elementsof( softwareLabHardwareLevels );
//Removed            break;
//Removed        case MachPhys::WEAPONS_LAB:
//Removed            result = elementsof( weaponsLabHardwareLevels );
//Removed            break;
        case MachPhys::SMELTER:
            result = elementsof( smelterHardwareLevels );
            break;
        case MachPhys::FACTORY:
            result = elementsof( factoryMilitaryHardwareLevels ) +
                    elementsof( factoryCivilianHardwareLevels );
            break;
        case MachPhys::MISSILE_EMPLACEMENT:
            result = elementsof( missileEmplacementTurretHardwareLevels ) +
                    elementsof( missileEmplacementSentryHardwareLevels ) +
                    elementsof( missileEmplacementLauncherHardwareLevels ) +
                    elementsof( missileEmplacementICBMHardwareLevels );
            break;
        case MachPhys::GARRISON:
            result = elementsof( garrisonHardwareLevels );
            break;
        case MachPhys::MINE:
            result = elementsof( mineHardwareLevels );
            break;
        case MachPhys::BEACON:
            result = elementsof( beaconHardwareLevels );
            break;
        case MachPhys::POD:
            result = elementsof( podHardwareLevels );
            break;
    }

    return result;
}


size_t  MachPhysLevels::nSoftwareIndices( void ) const
{
    size_t result = 0;

    result = std::max( result, aggressorGruntSoftwareLevels[ elementsof( aggressorGruntSoftwareLevels ) - 1 ] );
    result = std::max( result, aggressorAssassinSoftwareLevels[ elementsof( aggressorAssassinSoftwareLevels ) - 1 ] );
    result = std::max( result, aggressorKnightSoftwareLevels[ elementsof( aggressorKnightSoftwareLevels ) - 1 ] );
    result = std::max( result, aggressorBallistaSoftwareLevels[ elementsof( aggressorBallistaSoftwareLevels ) - 1 ] );
    result = std::max( result, aggressorNinjaSoftwareLevels[ elementsof( aggressorNinjaSoftwareLevels ) - 1 ] );
    result = std::max( result, geoLocatorSoftwareLevels[ elementsof( geoLocatorSoftwareLevels ) - 1 ] );
    result = std::max( result, spyLocatorSoftwareLevels[ elementsof( spyLocatorSoftwareLevels ) - 1 ] );
    result = std::max( result, resourceCarrierSoftwareLevels[ elementsof( resourceCarrierSoftwareLevels ) - 1 ] );
    result = std::max( result, personnelCarrierSoftwareLevels[ elementsof( personnelCarrierSoftwareLevels ) - 1 ] );
    result = std::max( result, technicianLabTechSoftwareLevels[ elementsof( technicianLabTechSoftwareLevels ) - 1 ] );
    result = std::max( result, technicianTechBoySoftwareLevels[ elementsof( technicianTechBoySoftwareLevels ) - 1 ] );
    result = std::max( result, technicianBrainBoxSoftwareLevels[ elementsof( technicianBrainBoxSoftwareLevels ) - 1 ] );
    result = std::max( result, constructorDozerSoftwareLevels[ elementsof( constructorDozerSoftwareLevels ) - 1 ] );
    result = std::max( result, constructorBuilderSoftwareLevels[ elementsof( constructorBuilderSoftwareLevels ) - 1 ] );
    result = std::max( result, constructorBehemothSoftwareLevels[ elementsof( constructorBehemothSoftwareLevels ) - 1 ] );
    result = std::max( result, administratorBossSoftwareLevels[ elementsof( administratorBossSoftwareLevels ) - 1 ] );
    result = std::max( result, administratorOverseerSoftwareLevels[ elementsof( administratorOverseerSoftwareLevels ) - 1 ] );
    result = std::max( result, administratorCommanderSoftwareLevels[ elementsof( administratorCommanderSoftwareLevels ) - 1 ] );

    return result;
}

string  MachPhysLevels::name( MachPhys::MachineType type ) const
{
    PRE( not subTyped( type ) );

    string  result;

    switch( type )
    {
        case MachPhys::GEO_LOCATOR:
            result = "gl";
            break;
        case MachPhys::SPY_LOCATOR:
            result = "sl";
            break;
        case MachPhys::RESOURCE_CARRIER:
            result = "rc";
            break;
        case MachPhys::APC:
            result = "ap";
            break;
        DEFAULT_ASSERT_BAD_CASE( type );
    }

    return result;
}
string  MachPhysLevels::name( MachPhys::AdministratorSubType subType ) const
{
    string  result = "ad";

    switch( subType )
    {
        case MachPhys::BOSS:
            result += "b";
            break;
        case MachPhys::OVERSEER:
            result += "o";
            break;
        case MachPhys::COMMANDER:
            result += "c";
            break;
        DEFAULT_ASSERT_BAD_CASE( subType );
    }

    return result;
}
string  MachPhysLevels::name( MachPhys::AggressorSubType subType ) const
{
    string  result = "ag";

    switch( subType )
    {
        case MachPhys::GRUNT:
            result += "g";
            break;
        case MachPhys::ASSASSIN:
            result += "a";
            break;
        case MachPhys::KNIGHT:
            result += "k";
            break;
        case MachPhys::BALLISTA:
            result += "b";
            break;
        case MachPhys::NINJA:
            result += "n";
            break;
        DEFAULT_ASSERT_BAD_CASE( subType );
    }

    return result;
}
string  MachPhysLevels::name( MachPhys::ConstructorSubType subType ) const
{
    string  result = "co";

    switch( subType )
    {
        case MachPhys::DOZER:
            result += "d";
            break;
        case MachPhys::BUILDER:
            result += "b";
            break;
        case MachPhys::BEHEMOTH:
            result += "m";
            break;
        DEFAULT_ASSERT_BAD_CASE( subType );
    }

    return result;
}
string  MachPhysLevels::name( MachPhys::TechnicianSubType subType ) const
{
    string  result = "te";

    switch( subType )
    {
        case MachPhys::LAB_TECH:
            result += "l";
            break;
        case MachPhys::TECH_BOY:
            result += "t";
            break;
        case MachPhys::BRAIN_BOX:
            result += "b";
            break;
        DEFAULT_ASSERT_BAD_CASE( subType );
    }

    return result;
}

string  MachPhysLevels::name( MachPhys::ConstructionType type ) const
{
    PRE( not subTyped( type ) );

    string  result;

    switch( type )
    {
        case MachPhys::SMELTER:
            result = "sm";
            break;
        case MachPhys::GARRISON:
            result = "ga";
            break;
        case MachPhys::MINE:
            result = "mn";
            break;
        case MachPhys::BEACON:
            result = "bk";
            break;
        case MachPhys::POD:
            result = "pd";
            break;
    }

    return result;
}
string  MachPhysLevels::name( MachPhys::FactorySubType subType ) const
{
    string  result = "fa";

    switch( subType )
    {
        case MachPhys::CIVILIAN:
            result += "c";
            break;
        case MachPhys::MILITARY:
            result += "m";
            break;
        DEFAULT_ASSERT_BAD_CASE( subType );
    }

    return result;
}
string  MachPhysLevels::name( MachPhys::HardwareLabSubType subType ) const
{
    string  result = "hw";

    switch( subType )
    {
        case MachPhys::LAB_CIVILIAN:
            result += "c";
            break;
        case MachPhys::LAB_MILITARY:
            result += "m";
            break;
        DEFAULT_ASSERT_BAD_CASE( subType );
    }

    return result;
}
string  MachPhysLevels::name( MachPhys::MissileEmplacementSubType subType ) const
{
    string  result = "me";

    switch( subType )
    {
        case MachPhys::TURRET:
            result += "t";
            break;
        case MachPhys::SENTRY:
            result += "s";
            break;
        case MachPhys::LAUNCHER:
            result += "l";
            break;
        case MachPhys::ICBM:
            result += "i";
            break;
        DEFAULT_ASSERT_BAD_CASE( subType );
    }

    return result;
}

ostream& operator <<( ostream& o, const MachPhysLevels& t )
{

    o << "MachPhysLevels " << (void*)&t << " start" << std::endl;
    o << "MachPhysLevels " << (void*)&t << " end" << std::endl;

    return o;
}

/* End LEVELS.CPP ***************************************************/


