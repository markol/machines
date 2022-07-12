/*
 * D A T A P A R S . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "system/pathname.hpp"
#include "utility/linetok.hpp"
#include "machphys/internal/datapars.hpp"
#include "machphys/station.hpp"
#include "machphys/stations.hpp"
#include "machphys/levels.hpp"
#include "machphys/levindmp.hpp"
#include "machphys/machphys.hpp"
#include "machphys/symparse.hpp"
#include "machphys/internal/consdati.hpp"

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
#include "machphys/wepdata.hpp"
// Specials
#include "machphys/orehdata.hpp"
#include "machphys/epps.hpp"
#include "machphys/genedata.hpp"

#include "mathex/degrees.hpp"
#include "mathex/point3d.hpp"


#include "ctl/fixedvec.hpp"
#include "ctl/vector.hpp"

#include "system/metafile.hpp"
#include "system/metaistr.hpp"
#include <memory>

#include <stdio.h>

// static
MachPhysDataParser& MachPhysDataParser::instance()
{
    static MachPhysDataParser instance_;
    return instance_;
}

MachPhysDataParser::MachPhysDataParser()
{
    TEST_INVARIANT;
}

MachPhysDataParser::~MachPhysDataParser()
{
    TEST_INVARIANT;
}

void MachPhysDataParser::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

void MachPhysDataParser::read( const SysPathName& pathname, MachPhysDataImplementation* pData )
{
    PRE_INFO( pathname.pathname() );
    //  Not necessarily true in the presense of metafiles
    // PRE( pathname.existsAsFile() );
    PRE( pData != NULL );

    pData_ = pData;

    initialiseDataStores();
    readParameterisedDataFile( pathname );
}

void MachPhysDataParser::initialiseDataStores()
{
    MachPhysLevelIndexMap hardwareMap = MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::BOSS );
    MachPhysLevelIndexMap softwareMap = MachPhysLevels::instance().softwareLevelIndexMap( MachPhys::BOSS );

    for( size_t hwIndex = 0; hwIndex < hardwareMap.nLevels(); ++hwIndex )
        for( size_t swIndex = 0; swIndex < softwareMap.nLevels(); ++swIndex )
        {
            size_t hwLevel = hardwareMap.level( hwIndex );
            size_t swLevel = softwareMap.level( swIndex );
            MachPhysAdministratorData* pItem = _NEW( MachPhysAdministratorData );
            pItem->machineType( MachPhys::ADMINISTRATOR );
            pItem->subType( MachPhys::BOSS );
            pItem->hwLevel( hwLevel );
            pItem->swLevel( swLevel );
            pData_->administratorBossData_[ hwLevel ][ swLevel ] = pItem;
        }

    hardwareMap = MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::OVERSEER );
    softwareMap = MachPhysLevels::instance().softwareLevelIndexMap( MachPhys::OVERSEER );

    for( size_t hwIndex = 0; hwIndex < hardwareMap.nLevels(); ++hwIndex )
        for( size_t swIndex = 0; swIndex < softwareMap.nLevels(); ++swIndex )
        {
            size_t hwLevel = hardwareMap.level( hwIndex );
            size_t swLevel = softwareMap.level( swIndex );
            MachPhysAdministratorData* pItem = _NEW( MachPhysAdministratorData );
            pItem->machineType( MachPhys::ADMINISTRATOR );
            pItem->subType( MachPhys::OVERSEER );
            pItem->hwLevel( hwLevel );
            pItem->swLevel( swLevel );
            pData_->administratorOverseerData_[ hwLevel ][ swLevel ] = pItem;
        }

    hardwareMap = MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::COMMANDER );
    softwareMap = MachPhysLevels::instance().softwareLevelIndexMap( MachPhys::COMMANDER );

    for( size_t hwIndex = 0; hwIndex < hardwareMap.nLevels(); ++hwIndex )
        for( size_t swIndex = 0; swIndex < softwareMap.nLevels(); ++swIndex )
        {
            size_t hwLevel = hardwareMap.level( hwIndex );
            size_t swLevel = softwareMap.level( swIndex );
            MachPhysAdministratorData* pItem = _NEW( MachPhysAdministratorData );
            pItem->machineType( MachPhys::ADMINISTRATOR );
            pItem->subType( MachPhys::COMMANDER );
            pItem->hwLevel( hwLevel );
            pItem->swLevel( swLevel );
            pData_->administratorCommanderData_[ hwLevel ][ swLevel ] = pItem;
        }

    hardwareMap = MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::GRUNT );
    softwareMap = MachPhysLevels::instance().softwareLevelIndexMap( MachPhys::GRUNT );

    for( size_t hwIndex = 0; hwIndex < hardwareMap.nLevels(); ++hwIndex )
        for( size_t swIndex = 0; swIndex < softwareMap.nLevels(); ++swIndex )
        {
            size_t hwLevel = hardwareMap.level( hwIndex );
            size_t swLevel = softwareMap.level( swIndex );
            MachPhysAggressorData* pItem = _NEW( MachPhysAggressorData );
            pItem->machineType( MachPhys::AGGRESSOR );
            pItem->subType( MachPhys::GRUNT );
            pItem->hwLevel( hwLevel );
            pItem->swLevel( swLevel );
            pData_->aggressorGruntData_[ hwLevel ][ swLevel ] = pItem;
        }

    hardwareMap = MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::ASSASSIN );
    softwareMap = MachPhysLevels::instance().softwareLevelIndexMap( MachPhys::ASSASSIN );

    for( size_t hwIndex = 0; hwIndex < hardwareMap.nLevels(); ++hwIndex )
        for( size_t swIndex = 0; swIndex < softwareMap.nLevels(); ++swIndex )
        {
            size_t hwLevel = hardwareMap.level( hwIndex );
            size_t swLevel = softwareMap.level( swIndex );
            MachPhysAggressorData* pItem = _NEW( MachPhysAggressorData );
            pItem->machineType( MachPhys::AGGRESSOR );
            pItem->subType( MachPhys::ASSASSIN );
            pItem->hwLevel( hwLevel );
            pItem->swLevel( swLevel );
            pData_->aggressorAssassinData_[ hwLevel ][ swLevel ] = pItem;
        }

    hardwareMap = MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::KNIGHT );
    softwareMap = MachPhysLevels::instance().softwareLevelIndexMap( MachPhys::KNIGHT );

    for( size_t hwIndex = 0; hwIndex < hardwareMap.nLevels(); ++hwIndex )
        for( size_t swIndex = 0; swIndex < softwareMap.nLevels(); ++swIndex )
        {
            size_t hwLevel = hardwareMap.level( hwIndex );
            size_t swLevel = softwareMap.level( swIndex );
            MachPhysAggressorData* pItem = _NEW( MachPhysAggressorData );
            pItem->machineType( MachPhys::AGGRESSOR );
            pItem->subType( MachPhys::KNIGHT );
            pItem->hwLevel( hwLevel );
            pItem->swLevel( swLevel );
            pData_->aggressorKnightData_[ hwLevel ][ swLevel ] = pItem;
        }

    hardwareMap = MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::BALLISTA );
    softwareMap = MachPhysLevels::instance().softwareLevelIndexMap( MachPhys::BALLISTA );

    for( size_t hwIndex = 0; hwIndex < hardwareMap.nLevels(); ++hwIndex )
        for( size_t swIndex = 0; swIndex < softwareMap.nLevels(); ++swIndex )
        {
            size_t hwLevel = hardwareMap.level( hwIndex );
            size_t swLevel = softwareMap.level( swIndex );
            MachPhysAggressorData* pItem = _NEW( MachPhysAggressorData );
            pItem->machineType( MachPhys::AGGRESSOR );
            pItem->subType( MachPhys::BALLISTA );
            pItem->hwLevel( hwLevel );
            pItem->swLevel( swLevel );
            pData_->aggressorBallistaData_[ hwLevel ][ swLevel ] = pItem;
        }

    hardwareMap = MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::NINJA );
    softwareMap = MachPhysLevels::instance().softwareLevelIndexMap( MachPhys::NINJA );

    for( size_t hwIndex = 0; hwIndex < hardwareMap.nLevels(); ++hwIndex )
        for( size_t swIndex = 0; swIndex < softwareMap.nLevels(); ++swIndex )
        {
            size_t hwLevel = hardwareMap.level( hwIndex );
            size_t swLevel = softwareMap.level( swIndex );
            MachPhysAggressorData* pItem = _NEW( MachPhysAggressorData );
            pItem->machineType( MachPhys::AGGRESSOR );
            pItem->subType( MachPhys::NINJA );
            pItem->hwLevel( hwLevel );
            pItem->swLevel( swLevel );
            pData_->aggressorNinjaData_[ hwLevel ][ swLevel ] = pItem;
        }

    hardwareMap = MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::DOZER );
    softwareMap = MachPhysLevels::instance().softwareLevelIndexMap( MachPhys::DOZER );

    for( size_t hwIndex = 0; hwIndex < hardwareMap.nLevels(); ++hwIndex )
        for( size_t swIndex = 0; swIndex < softwareMap.nLevels(); ++swIndex )
        {
            size_t hwLevel = hardwareMap.level( hwIndex );
            size_t swLevel = softwareMap.level( swIndex );
            MachPhysConstructorData* pItem = _NEW( MachPhysConstructorData );
            pItem->machineType( MachPhys::CONSTRUCTOR );
            pItem->subType( MachPhys::DOZER );
            pItem->hwLevel( hwLevel );
            pItem->swLevel( swLevel );
            pData_->constructorDozerData_[ hwLevel ][ swLevel ] = pItem;
        }

    hardwareMap = MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::BUILDER );
    softwareMap = MachPhysLevels::instance().softwareLevelIndexMap( MachPhys::BUILDER );

    for( size_t hwIndex = 0; hwIndex < hardwareMap.nLevels(); ++hwIndex )
        for( size_t swIndex = 0; swIndex < softwareMap.nLevels(); ++swIndex )
        {
            size_t hwLevel = hardwareMap.level( hwIndex );
            size_t swLevel = softwareMap.level( swIndex );
            MachPhysConstructorData* pItem = _NEW( MachPhysConstructorData );
            pItem->machineType( MachPhys::CONSTRUCTOR );
            pItem->subType( MachPhys::BUILDER );
            pItem->hwLevel( hwLevel );
            pItem->swLevel( swLevel );
            pData_->constructorBuilderData_[ hwLevel ][ swLevel ] = pItem;
        }

    hardwareMap = MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::BEHEMOTH );
    softwareMap = MachPhysLevels::instance().softwareLevelIndexMap( MachPhys::BEHEMOTH );

    for( size_t hwIndex = 0; hwIndex < hardwareMap.nLevels(); ++hwIndex )
        for( size_t swIndex = 0; swIndex < softwareMap.nLevels(); ++swIndex )
        {
            size_t hwLevel = hardwareMap.level( hwIndex );
            size_t swLevel = softwareMap.level( swIndex );
            MachPhysConstructorData* pItem = _NEW( MachPhysConstructorData );
            pItem->machineType( MachPhys::CONSTRUCTOR );
            pItem->subType( MachPhys::BEHEMOTH );
            pItem->hwLevel( hwLevel );
            pItem->swLevel( swLevel );
            pData_->constructorBehemothData_[ hwLevel ][ swLevel ] = pItem;
        }

    hardwareMap = MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::GEO_LOCATOR );
    softwareMap = MachPhysLevels::instance().softwareLevelIndexMap( MachPhys::GEO_LOCATOR );

    for( size_t hwIndex = 0; hwIndex < hardwareMap.nLevels(); ++hwIndex )
        for( size_t swIndex = 0; swIndex < softwareMap.nLevels(); ++swIndex )
        {
            size_t hwLevel = hardwareMap.level( hwIndex );
            size_t swLevel = softwareMap.level( swIndex );
            MachPhysGeoLocatorData* pItem = _NEW( MachPhysGeoLocatorData );
            pItem->machineType( MachPhys::GEO_LOCATOR );
            pItem->subType( 0 );
            pItem->hwLevel( hwLevel );
            pItem->swLevel( swLevel );
            pData_->geoLocatorData_[ hwLevel ][ swLevel ] = pItem;
        }

    hardwareMap = MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::SPY_LOCATOR );
    softwareMap = MachPhysLevels::instance().softwareLevelIndexMap( MachPhys::SPY_LOCATOR );

    for( size_t hwIndex = 0; hwIndex < hardwareMap.nLevels(); ++hwIndex )
        for( size_t swIndex = 0; swIndex < softwareMap.nLevels(); ++swIndex )
        {
            size_t hwLevel = hardwareMap.level( hwIndex );
            size_t swLevel = softwareMap.level( swIndex );
            MachPhysSpyLocatorData* pItem = _NEW( MachPhysSpyLocatorData );
            pItem->machineType( MachPhys::SPY_LOCATOR );
            pItem->subType( 0 );
            pItem->hwLevel( hwLevel );
            pItem->swLevel( swLevel );
            pData_->spyLocatorData_[ hwLevel ][ swLevel ] = pItem;
        }

    hardwareMap = MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::APC );
    softwareMap = MachPhysLevels::instance().softwareLevelIndexMap( MachPhys::APC );

    for( size_t hwIndex = 0; hwIndex < hardwareMap.nLevels(); ++hwIndex )
        for( size_t swIndex = 0; swIndex < softwareMap.nLevels(); ++swIndex )
        {
            size_t hwLevel = hardwareMap.level( hwIndex );
            size_t swLevel = softwareMap.level( swIndex );
            MachPhysAPCData* pItem = _NEW( MachPhysAPCData );
            pItem->machineType( MachPhys::APC );
            pItem->subType( 0 );
            pItem->hwLevel( hwLevel );
            pItem->swLevel( swLevel );
            pData_->APCData_[ hwLevel ][ swLevel ] = pItem;
        }

    hardwareMap = MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::RESOURCE_CARRIER );
    softwareMap = MachPhysLevels::instance().softwareLevelIndexMap( MachPhys::RESOURCE_CARRIER );

    for( size_t hwIndex = 0; hwIndex < hardwareMap.nLevels(); ++hwIndex )
        for( size_t swIndex = 0; swIndex < softwareMap.nLevels(); ++swIndex )
        {
            size_t hwLevel = hardwareMap.level( hwIndex );
            size_t swLevel = softwareMap.level( swIndex );
            MachPhysResourceCarrierData* pItem = _NEW( MachPhysResourceCarrierData );
            pItem->machineType( MachPhys::RESOURCE_CARRIER );
            pItem->subType( 0 );
            pItem->hwLevel( hwLevel );
            pItem->swLevel( swLevel );
            pData_->resourceCarrierData_[ hwLevel ][ swLevel ] = pItem;
        }

    hardwareMap = MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::LAB_TECH );
    softwareMap = MachPhysLevels::instance().softwareLevelIndexMap( MachPhys::LAB_TECH );

    for( size_t hwIndex = 0; hwIndex < hardwareMap.nLevels(); ++hwIndex )
        for( size_t swIndex = 0; swIndex < softwareMap.nLevels(); ++swIndex )
        {
            size_t hwLevel = hardwareMap.level( hwIndex );
            size_t swLevel = softwareMap.level( swIndex );
            MachPhysTechnicianData* pItem = _NEW( MachPhysTechnicianData );
            pItem->machineType( MachPhys::TECHNICIAN );
            pItem->subType( MachPhys::LAB_TECH );
            pItem->hwLevel( hwLevel );
            pItem->swLevel( swLevel );
            pData_->technicianLabTechData_[ hwLevel ][ swLevel ] = pItem;
        }

    hardwareMap = MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::TECH_BOY );
    softwareMap = MachPhysLevels::instance().softwareLevelIndexMap( MachPhys::TECH_BOY );

    for( size_t hwIndex = 0; hwIndex < hardwareMap.nLevels(); ++hwIndex )
        for( size_t swIndex = 0; swIndex < softwareMap.nLevels(); ++swIndex )
        {
            size_t hwLevel = hardwareMap.level( hwIndex );
            size_t swLevel = softwareMap.level( swIndex );
            MachPhysTechnicianData* pItem = _NEW( MachPhysTechnicianData );
            pItem->machineType( MachPhys::TECHNICIAN );
            pItem->subType( MachPhys::TECH_BOY );
            pItem->hwLevel( hwLevel );
            pItem->swLevel( swLevel );
            pData_->technicianTechBoyData_[ hwLevel ][ swLevel ] = pItem;
        }

    hardwareMap = MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::BRAIN_BOX );
    softwareMap = MachPhysLevels::instance().softwareLevelIndexMap( MachPhys::BRAIN_BOX );

    for( size_t hwIndex = 0; hwIndex < hardwareMap.nLevels(); ++hwIndex )
        for( size_t swIndex = 0; swIndex < softwareMap.nLevels(); ++swIndex )
        {
            size_t hwLevel = hardwareMap.level( hwIndex );
            size_t swLevel = softwareMap.level( swIndex );
            MachPhysTechnicianData* pItem = _NEW( MachPhysTechnicianData );
            pItem->machineType( MachPhys::TECHNICIAN );
            pItem->subType( MachPhys::BRAIN_BOX );
            pItem->hwLevel( hwLevel );
            pItem->swLevel( swLevel );
            pData_->technicianBrainBoxData_[ hwLevel ][ swLevel ] = pItem;
        }

    hardwareMap = MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::LAB_CIVILIAN );

    for( size_t hwIndex = 0; hwIndex < hardwareMap.nLevels(); ++hwIndex )
    {
        size_t hwLevel = hardwareMap.level( hwIndex );
        MachPhysHardwareLabData* pItem = _NEW( MachPhysHardwareLabData );
        pItem->constructionType( MachPhys::HARDWARE_LAB );
        pItem->subType( MachPhys::LAB_CIVILIAN );
        pItem->hwLevel( hwLevel );
        pData_->hardwareLabCivilianData_[ hwLevel ] = pItem;
    }

    hardwareMap = MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::LAB_MILITARY );

    for( size_t hwIndex = 0; hwIndex < hardwareMap.nLevels(); ++hwIndex )
    {
        size_t hwLevel = hardwareMap.level( hwIndex );
        MachPhysHardwareLabData* pItem = _NEW( MachPhysHardwareLabData );
        pItem->constructionType( MachPhys::HARDWARE_LAB );
        pItem->subType( MachPhys::LAB_MILITARY );
        pItem->hwLevel( hwLevel );
        pData_->hardwareLabMilitaryData_[ hwLevel ] = pItem;
    }

    hardwareMap = MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::SMELTER );

    for( size_t hwIndex = 0; hwIndex < hardwareMap.nLevels(); ++hwIndex )
    {
        size_t hwLevel = hardwareMap.level( hwIndex );
        MachPhysSmelterData* pItem = _NEW( MachPhysSmelterData );
        pItem->constructionType( MachPhys::SMELTER );
        pItem->subType( 0 );
        pItem->hwLevel( hwLevel );
        pData_->smelterData_[ hwLevel ] = pItem;
    }

    hardwareMap = MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::MINE );

    for( size_t hwIndex = 0; hwIndex < hardwareMap.nLevels(); ++hwIndex )
    {
        size_t hwLevel = hardwareMap.level( hwIndex );
        MachPhysMineData* pItem = _NEW( MachPhysMineData );
        pItem->constructionType( MachPhys::MINE );
        pItem->subType( 0 );
        pItem->hwLevel( hwLevel );
        pData_->mineData_[ hwLevel ] = pItem;
    }

    hardwareMap = MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::GARRISON );

    for( size_t hwIndex = 0; hwIndex < hardwareMap.nLevels(); ++hwIndex )
    {
        size_t hwLevel = hardwareMap.level( hwIndex );
        MachPhysGarrisonData* pItem = _NEW( MachPhysGarrisonData );
        pItem->constructionType( MachPhys::GARRISON );
        pItem->subType( 0 );
        pItem->hwLevel( hwLevel );
        pData_->garrisonData_[ hwLevel ] = pItem;
    }

    hardwareMap = MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::BEACON );

    for( size_t hwIndex = 0; hwIndex < hardwareMap.nLevels(); ++hwIndex )
    {
        size_t hwLevel = hardwareMap.level( hwIndex );
        MachPhysBeaconData* pItem = _NEW( MachPhysBeaconData );
        pItem->constructionType( MachPhys::BEACON );
        pItem->subType( 0 );
        pItem->hwLevel( hwLevel );
        pData_->beaconData_[ hwLevel ] = pItem;
    }

    hardwareMap = MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::POD );

    for( size_t hwIndex = 0; hwIndex < hardwareMap.nLevels(); ++hwIndex )
    {
        size_t hwLevel = hardwareMap.level( hwIndex );
        MachPhysPodData* pItem = _NEW( MachPhysPodData );
        pItem->constructionType( MachPhys::POD );
        pItem->subType( 0 );
        pItem->hwLevel( hwLevel );
        pData_->podData_[ hwLevel ] = pItem;
    }

    hardwareMap = MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::CIVILIAN );

    for( size_t hwIndex = 0; hwIndex < hardwareMap.nLevels(); ++hwIndex )
    {
        size_t hwLevel = hardwareMap.level( hwIndex );
        MachPhysFactoryData* pItem = _NEW( MachPhysFactoryData );
        pItem->constructionType( MachPhys::FACTORY );
        pItem->subType( MachPhys::CIVILIAN );
        pItem->hwLevel( hwLevel );
        pData_->factoryCivilianData_[ hwLevel ] = pItem;
    }

    hardwareMap = MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::MILITARY );

    for( size_t hwIndex = 0; hwIndex < hardwareMap.nLevels(); ++hwIndex )
    {
        size_t hwLevel = hardwareMap.level( hwIndex );
        MachPhysFactoryData* pItem = _NEW( MachPhysFactoryData );
        pItem->constructionType( MachPhys::FACTORY );
        pItem->subType( MachPhys::MILITARY );
        pItem->hwLevel( hwLevel );
        pData_->factoryMilitaryData_[ hwLevel ] = pItem;
    }

    hardwareMap = MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::TURRET );

    for( size_t hwIndex = 0; hwIndex < hardwareMap.nLevels(); ++hwIndex )
    {
        size_t hwLevel = hardwareMap.level( hwIndex );
        MachPhysMissileEmplacementData* pItem = _NEW( MachPhysMissileEmplacementData );
        pItem->constructionType( MachPhys::MISSILE_EMPLACEMENT );
        pItem->subType( MachPhys::TURRET );
        pItem->hwLevel( hwLevel );
        pData_->missileEmplacementTurretData_[ hwLevel ] = pItem;
    }

    hardwareMap = MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::SENTRY );

    for( size_t hwIndex = 0; hwIndex < hardwareMap.nLevels(); ++hwIndex )
    {
        size_t hwLevel = hardwareMap.level( hwIndex );
        MachPhysMissileEmplacementData* pItem = _NEW( MachPhysMissileEmplacementData );
        pItem->constructionType( MachPhys::MISSILE_EMPLACEMENT );
        pItem->subType( MachPhys::SENTRY );
        pItem->hwLevel( hwLevel );
        pData_->missileEmplacementSentryData_[ hwLevel ] = pItem;
    }

    hardwareMap = MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::LAUNCHER );

    for( size_t hwIndex = 0; hwIndex < hardwareMap.nLevels(); ++hwIndex )
    {
        size_t hwLevel = hardwareMap.level( hwIndex );
        MachPhysMissileEmplacementData* pItem = _NEW( MachPhysMissileEmplacementData );
        pItem->constructionType( MachPhys::MISSILE_EMPLACEMENT );
        pItem->subType( MachPhys::LAUNCHER );
        pItem->hwLevel( hwLevel );
        pData_->missileEmplacementLauncherData_[ hwLevel ] = pItem;
    }

    hardwareMap = MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::ICBM );

    for( size_t hwIndex = 0; hwIndex < hardwareMap.nLevels(); ++hwIndex )
    {
        size_t hwLevel = hardwareMap.level( hwIndex );
        MachPhysMissileEmplacementData* pItem = _NEW( MachPhysMissileEmplacementData );
        pItem->constructionType( MachPhys::MISSILE_EMPLACEMENT );
        pItem->subType( MachPhys::ICBM );
        pItem->hwLevel( hwLevel );
        pData_->missileEmplacementICBMData_[ hwLevel ] = pItem;
    }

    for( size_t i = 0; i != MachPhys::N_WEAPON_TYPES; ++i )
    {
        //(*MachPhysDataImplementation::instance().pWeaponDataStore())[i] =
        (*pData_->pWeaponDataStore())[i] =
            _NEW( MachPhysWeaponData );
    }

    //*(MachPhysDataImplementation::instance().pGeneralDataStore()) = _NEW( MachPhysGeneralData );
    *(pData_->pGeneralDataStore()) = _NEW( MachPhysGeneralData );
}



// **************************************************************************



// **************************************************************************


MachPhysDataParser::MachineDataStore    MachPhysDataParser::machineDataStore( const MachPhysDataImplementation::AggressorDataStore& store ) const
{
    MachineDataStore machineDStore( MachPhysDataImplementation::STORE_HARDWARE_SIZE, ctl_fixed_vector< MachPhysMachineData* >( MachPhysDataImplementation::STORE_SOFTWARE_SIZE, NULL ) );

    size_t  hwIndex = 0;

    for( size_t hwLevel = 0; hwLevel < store.size(); ++hwLevel )
    {
        size_t swIndex = 0;
        bool valueFound = false;
        for( size_t swLevel = 0; swLevel < store.size(); ++swLevel )
        {
            if( store[ hwLevel ][ swLevel ] != NULL )
            {
                machineDStore[ hwIndex ][ swIndex++ ] = store[ hwLevel ][ swLevel ];
                valueFound = true;
            }
        }

        if( valueFound )
            ++hwIndex;
    }

    return machineDStore;
}

MachPhysDataParser::MachineDataStore    MachPhysDataParser::machineDataStore( const AdministratorDataStore& store ) const
{
    MachineDataStore    machineDStore( MachPhysDataImplementation::STORE_HARDWARE_SIZE, ctl_fixed_vector< MachPhysMachineData* >( MachPhysDataImplementation::STORE_SOFTWARE_SIZE, NULL ) );

    size_t  hwIndex = 0;

    for( size_t hwLevel = 0; hwLevel < store.size(); ++hwLevel )
    {
        size_t swIndex = 0;
        bool valueFound = false;
        for( size_t swLevel = 0; swLevel < store.size(); ++swLevel )
        {
            if( store[ hwLevel ][ swLevel ] != NULL )
            {
                machineDStore[ hwIndex ][ swIndex++ ] = store[ hwLevel ][ swLevel ];
                valueFound = true;
            }
        }

        if( valueFound )
            ++hwIndex;
    }

    return machineDStore;
}

MachPhysDataParser::MachineDataStore    MachPhysDataParser::machineDataStore( const APCDataStore& store ) const
{
    MachineDataStore    machineDStore( MachPhysDataImplementation::STORE_HARDWARE_SIZE, ctl_fixed_vector< MachPhysMachineData* >( MachPhysDataImplementation::STORE_SOFTWARE_SIZE, NULL ) );

    size_t  hwIndex = 0;

    for( size_t hwLevel = 0; hwLevel < store.size(); ++hwLevel )
    {
        size_t swIndex = 0;
        bool valueFound = false;
        for( size_t swLevel = 0; swLevel < store.size(); ++swLevel )
        {
            if( store[ hwLevel ][ swLevel ] != NULL )
            {
                machineDStore[ hwIndex ][ swIndex++ ] = store[ hwLevel ][ swLevel ];
                valueFound = true;
            }
        }

        if( valueFound )
            ++hwIndex;
    }

    return machineDStore;
}

MachPhysDataParser::MachineDataStore    MachPhysDataParser::machineDataStore( const ResourceCarrierDataStore& store ) const
{
    MachineDataStore    machineDStore( MachPhysDataImplementation::STORE_HARDWARE_SIZE, ctl_fixed_vector< MachPhysMachineData* >( MachPhysDataImplementation::STORE_SOFTWARE_SIZE, NULL ) );

    size_t  hwIndex = 0;

    for( size_t hwLevel = 0; hwLevel < store.size(); ++hwLevel )
    {
        size_t swIndex = 0;
        bool valueFound = false;
        for( size_t swLevel = 0; swLevel < store.size(); ++swLevel )
        {
            if( store[ hwLevel ][ swLevel ] != NULL )
            {
                machineDStore[ hwIndex ][ swIndex++ ] = store[ hwLevel ][ swLevel ];
                valueFound = true;
            }
        }

        if( valueFound )
            ++hwIndex;
    }

    return machineDStore;
}

MachPhysDataParser::MachineDataStore    MachPhysDataParser::machineDataStore( const GeoLocatorDataStore& store ) const
{
    MachineDataStore    machineDStore( MachPhysDataImplementation::STORE_HARDWARE_SIZE, ctl_fixed_vector< MachPhysMachineData* >( MachPhysDataImplementation::STORE_SOFTWARE_SIZE, NULL ) );

    size_t  hwIndex = 0;

    for( size_t hwLevel = 0; hwLevel < store.size(); ++hwLevel )
    {
        size_t swIndex = 0;
        bool valueFound = false;
        for( size_t swLevel = 0; swLevel < store.size(); ++swLevel )
        {
            if( store[ hwLevel ][ swLevel ] != NULL )
            {
                machineDStore[ hwIndex ][ swIndex++ ] = store[ hwLevel ][ swLevel ];
                valueFound = true;
            }
        }

        if( valueFound )
            ++hwIndex;
    }

    return machineDStore;
}

MachPhysDataParser::MachineDataStore    MachPhysDataParser::machineDataStore( const SpyLocatorDataStore& store ) const
{
    MachineDataStore    machineDStore( MachPhysDataImplementation::STORE_HARDWARE_SIZE, ctl_fixed_vector< MachPhysMachineData* >( MachPhysDataImplementation::STORE_SOFTWARE_SIZE, NULL ) );

    size_t  hwIndex = 0;

    for( size_t hwLevel = 0; hwLevel < store.size(); ++hwLevel )
    {
        size_t swIndex = 0;
        bool valueFound = false;
        for( size_t swLevel = 0; swLevel < store.size(); ++swLevel )
        {
            if( store[ hwLevel ][ swLevel ] != NULL )
            {
                machineDStore[ hwIndex ][ swIndex++ ] = store[ hwLevel ][ swLevel ];
                valueFound = true;
            }
        }

        if( valueFound )
            ++hwIndex;
    }

    return machineDStore;
}

MachPhysDataParser::MachineDataStore    MachPhysDataParser::machineDataStore( const TechnicianDataStore& store ) const
{
    MachineDataStore    machineDStore( MachPhysDataImplementation::STORE_HARDWARE_SIZE, ctl_fixed_vector< MachPhysMachineData* >( MachPhysDataImplementation::STORE_SOFTWARE_SIZE, NULL ) );

    size_t  hwIndex = 0;

    for( size_t hwLevel = 0; hwLevel < store.size(); ++hwLevel )
    {
        size_t swIndex = 0;
        bool valueFound = false;
        for( size_t swLevel = 0; swLevel < store.size(); ++swLevel )
        {
            if( store[ hwLevel ][ swLevel ] != NULL )
            {
                machineDStore[ hwIndex ][ swIndex++ ] = store[ hwLevel ][ swLevel ];
                valueFound = true;
            }
        }

        if( valueFound )
            ++hwIndex;
    }

    return machineDStore;
}

MachPhysDataParser::MachineDataStore    MachPhysDataParser::machineDataStore( const ConstructorDataStore& store ) const
{
    MachineDataStore    machineDStore( MachPhysDataImplementation::STORE_HARDWARE_SIZE, ctl_fixed_vector< MachPhysMachineData* >( MachPhysDataImplementation::STORE_SOFTWARE_SIZE, NULL ) );

    size_t  hwIndex = 0;

    for( size_t hwLevel = 0; hwLevel < store.size(); ++hwLevel )
    {
        size_t swIndex = 0;
        bool valueFound = false;
        for( size_t swLevel = 0; swLevel < store.size(); ++swLevel )
        {
            if( store[ hwLevel ][ swLevel ] != NULL )
            {
                machineDStore[ hwIndex ][ swIndex++ ] = store[ hwLevel ][ swLevel ];
                valueFound = true;
            }
        }

        if( valueFound )
            ++hwIndex;
    }

    return machineDStore;
}

MachPhysDataParser::ConstructionDataStore   MachPhysDataParser::constructionDataStore( const HardwareLabDataStore& store ) const
{
    ConstructionDataStore    constructionDStore( MachPhysDataImplementation::STORE_HARDWARE_SIZE, NULL );

    size_t  hwIndex = 0;

    for( size_t hwLevel = 0; hwLevel < store.size(); ++hwLevel )
    {
        if( store[ hwLevel ] != NULL )
            constructionDStore[ hwIndex++ ] = store[ hwLevel ];
    }

    return constructionDStore;
}


MachPhysDataParser::ConstructionDataStore   MachPhysDataParser::constructionDataStore( const SmelterDataStore& store ) const
{
    ConstructionDataStore    constructionDStore( MachPhysDataImplementation::STORE_HARDWARE_SIZE, NULL );

    size_t  hwIndex = 0;

    for( size_t hwLevel = 0; hwLevel < store.size(); ++hwLevel )
    {
        if( store[ hwLevel ] != NULL )
            constructionDStore[ hwIndex++ ] = store[ hwLevel ];
    }

    return constructionDStore;
}

MachPhysDataParser::ConstructionDataStore   MachPhysDataParser::constructionDataStore( const MineDataStore& store ) const
{
    ConstructionDataStore    constructionDStore( MachPhysDataImplementation::STORE_HARDWARE_SIZE, NULL );

    size_t  hwIndex = 0;

    for( size_t hwLevel = 0; hwLevel < store.size(); ++hwLevel )
    {
        if( store[ hwLevel ] != NULL )
            constructionDStore[ hwIndex++ ] = store[ hwLevel ];
    }

    return constructionDStore;
}

MachPhysDataParser::ConstructionDataStore   MachPhysDataParser::constructionDataStore( const GarrisonDataStore& store ) const
{
    ConstructionDataStore    constructionDStore( MachPhysDataImplementation::STORE_HARDWARE_SIZE, NULL );

    size_t  hwIndex = 0;

    for( size_t hwLevel = 0; hwLevel < store.size(); ++hwLevel )
    {
        if( store[ hwLevel ] != NULL )
            constructionDStore[ hwIndex++ ] = store[ hwLevel ];
    }

    return constructionDStore;
}

MachPhysDataParser::ConstructionDataStore   MachPhysDataParser::constructionDataStore( const BeaconDataStore& store ) const
{
    ConstructionDataStore    constructionDStore( MachPhysDataImplementation::STORE_HARDWARE_SIZE, NULL );

    size_t  hwIndex = 0;

    for( size_t hwLevel = 0; hwLevel < store.size(); ++hwLevel )
    {
        if( store[ hwLevel ] != NULL )
            constructionDStore[ hwIndex++ ] = store[ hwLevel ];
    }

    return constructionDStore;
}

MachPhysDataParser::ConstructionDataStore   MachPhysDataParser::constructionDataStore( const PodDataStore& store ) const
{
    ConstructionDataStore    constructionDStore( MachPhysDataImplementation::STORE_HARDWARE_SIZE, NULL );

    size_t  hwIndex = 0;

    for( size_t hwLevel = 0; hwLevel < store.size(); ++hwLevel )
    {
        if( store[ hwLevel ] != NULL )
            constructionDStore[ hwIndex++ ] = store[ hwLevel ];
    }

    return constructionDStore;
}

MachPhysDataParser::ConstructionDataStore   MachPhysDataParser::constructionDataStore( const FactoryDataStore& store ) const
{
    ConstructionDataStore    constructionDStore( MachPhysDataImplementation::STORE_HARDWARE_SIZE, NULL );

    size_t  hwIndex = 0;

    for( size_t hwLevel = 0; hwLevel < store.size(); ++hwLevel )
    {
        if( store[ hwLevel ] != NULL )
            constructionDStore[ hwIndex++ ] = store[ hwLevel ];
    }

    return constructionDStore;
}

MachPhysDataParser::ConstructionDataStore   MachPhysDataParser::constructionDataStore( const MissileEmplacementDataStore& store ) const
{
    ConstructionDataStore    constructionDStore( MachPhysDataImplementation::STORE_HARDWARE_SIZE, NULL );

    size_t  hwIndex = 0;

    for( size_t hwLevel = 0; hwLevel < store.size(); ++hwLevel )
    {
        if( store[ hwLevel ] != NULL )
            constructionDStore[ hwIndex++ ] = store[ hwLevel ];
    }

    return constructionDStore;
}



// **************************************************************************

void MachPhysDataParser::readParameterisedDataFile( const SysPathName& pathname )
{
    SysMetaFile metaFile( "mach1.met" );

    std::auto_ptr< istream > pIstream;

    if( SysMetaFile::useMetaFile() )
    {
        //pIstream = _NEW( SysMetaFileIstream( metaFile, pathname, ios::text ) );
        pIstream = std::auto_ptr< istream >(
                _NEW( SysMetaFileIstream( metaFile, pathname, std::ios::in ) ));
    }
    else
    {
        ASSERT_FILE_EXISTS( pathname.c_str() );
        //pIstream = _NEW( ifstream( pathname.c_str(), std::ios::in | ios::in ) );
        pIstream = std::auto_ptr< istream >(
            _NEW( std::ifstream( pathname.c_str(), std::ios::in | std::ios::in ) ) );
    }

    UtlLineTokeniser   parser( *pIstream, pathname );
	bool machinesSection = false;
	bool constructionsSection = false;
	bool weaponsSection = false;
	bool EPPsSection = false;

    while( not parser.finished() )
    {
        ASSERT_INFO( parser.tokens()[ 0 ] );
//		for( int i = 0 ; i < parser.tokens().size() ; ++i )
//			cout << "token[ " << i << " ] :" << parser.tokens()[ i ] << ":" << std::endl;

        if( parser.tokens()[ 0 ] == "MACHINES" )
		{
			ASSERT( not constructionsSection, "Error in object data file -> no end of constructions section\n");
			ASSERT( not weaponsSection, "Error in object data file -> no end of weapons section\n");
			ASSERT( not EPPsSection, "Error in object data file -> no end of EPPs section\n");
			machinesSection = true;
		}

        if( parser.tokens()[ 0 ] == "CONSTRUCTIONS" )
		{
			ASSERT( not machinesSection, "Error in object data file -> no end of machines section\n");
			ASSERT( not weaponsSection, "Error in object data file -> no end of weapons section\n");
			ASSERT( not EPPsSection, "Error in object data file -> no end of EPPs section\n");
			constructionsSection = true;
		}

        if( parser.tokens()[ 0 ] == "WEAPONS" )
		{
			ASSERT( not machinesSection, "Error in object data file -> no end of machines section\n");
			ASSERT( not constructionsSection, "Error in object data file -> no end of constructions section\n");
			ASSERT( not EPPsSection, "Error in object data file -> no end of EPPs section\n");
			weaponsSection = true;
		}

		if( parser.tokens()[ 0 ] == "EVASION_STRATEGIES" )
		{
			ASSERT( not machinesSection, "Error in object data file -> no end of machines section\n");
			ASSERT( not constructionsSection, "Error in object data file -> no end of constructions section\n");
			ASSERT( not weaponsSection, "Error in object data file -> no end of weapons section\n");
			EPPsSection = true;
		}

		if( parser.tokens()[ 0 ] == "END" )
		{
			if(	parser.tokens().size() == 2 )
			{
				if( parser.tokens()[ 1 ] == "MACHINES" )
					machinesSection = false;
				else if( parser.tokens()[ 1 ] == "CONSTRUCTIONS" )
					constructionsSection = false;
				else if( parser.tokens()[ 1 ] == "WEAPONS" )
					weaponsSection = false;
				else if( parser.tokens()[ 1 ] == "EVASION_STRATEGIES" )
					EPPsSection = false;
			}
		}

		if( parser.tokens()[ 0 ] == "TYPE" )
		{
            ASSERT_INFO( parser.tokens()[ 1 ] );

			if( parser.tokens()[ 1 ] == "AGGRESSOR" )
				parseAggressorBlock( &parser );
			else if( parser.tokens()[ 1 ] == "ADMINISTRATOR" )
				parseAdministratorBlock( &parser );
			else if( parser.tokens()[ 1 ] == "APC" )
				parseAPCBlock( &parser );
			else if( parser.tokens()[ 1 ] == "RESOURCE_CARRIER" )
				parseResourceCarrierBlock( &parser );
			else if( parser.tokens()[ 1 ] == "GEO_LOCATOR" )
				parseGeoLocatorBlock( &parser );
			else if( parser.tokens()[ 1 ] == "SPY_LOCATOR" )
				parseSpyLocatorBlock( &parser );
			else if( parser.tokens()[ 1 ] == "TECHNICIAN" )
				parseTechnicianBlock( &parser );
			else if( parser.tokens()[ 1 ] == "CONSTRUCTOR" )
				parseConstructorBlock( &parser );
			else if( parser.tokens()[ 1 ] == "HARDWARE_LAB" )
				parseHardwareLabBlock( &parser );
			else if( parser.tokens()[ 1 ] == "SMELTER" )
				parseSmelterBlock( &parser );
			else if( parser.tokens()[ 1 ] == "FACTORY" )
				parseFactoryBlock( &parser );
			else if( parser.tokens()[ 1 ] == "MISSILE_EMPLACEMENT" )
				parseMissileEmplacementBlock( &parser );
			else if( parser.tokens()[ 1 ] == "GARRISON" )
				parseGarrisonBlock( &parser );
			else if( parser.tokens()[ 1 ] == "MINE" )
				parseMineBlock( &parser );
			else if( parser.tokens()[ 1 ] == "BEACON" )
				parseBeaconBlock( &parser );
			else if( parser.tokens()[ 1 ] == "POD" )
				parsePodBlock( &parser );
			else if( parser.tokens()[ 1 ] == "EPP" )
				parseEPPBlock( &parser );
			else if( parser.tokens()[ 1 ] == "ELECTRIC_CHARGE" )
				parseWeaponBlock( &parser, MachPhys::ELECTRIC_CHARGE );
			else if( parser.tokens()[ 1 ] == "FLAME_THROWER1" )
				parseWeaponBlock( &parser, MachPhys::FLAME_THROWER1 );
			else if( parser.tokens()[ 1 ] == "FLAME_THROWER2" )
				parseWeaponBlock( &parser, MachPhys::FLAME_THROWER2 );
			else if( parser.tokens()[ 1 ] == "BOLTER" )
				parseWeaponBlock( &parser, MachPhys::BOLTER );
			else if( parser.tokens()[ 1 ] == "AUTO_CANNON" )
				parseWeaponBlock( &parser, MachPhys::AUTO_CANNON );
			else if( parser.tokens()[ 1 ] == "GORILLA_PUNCH" )
				parseWeaponBlock( &parser, MachPhys::GORILLA_PUNCH );
			else if( parser.tokens()[ 1 ] == "PLASMA_RIFLE" )
				parseWeaponBlock( &parser, MachPhys::PLASMA_RIFLE );
			else if( parser.tokens()[ 1 ] == "PULSE_RIFLE" )
				parseWeaponBlock( &parser, MachPhys::PULSE_RIFLE );
			else if( parser.tokens()[ 1 ] == "PLASMA_CANNON1" )
				parseWeaponBlock( &parser, MachPhys::PLASMA_CANNON1 );
			else if( parser.tokens()[ 1 ] == "PLASMA_CANNON2" )
				parseWeaponBlock( &parser, MachPhys::PLASMA_CANNON2 );
			else if( parser.tokens()[ 1 ] == "PULSE_CANNON" )
				parseWeaponBlock( &parser, MachPhys::PULSE_CANNON );
			else if( parser.tokens()[ 1 ] == "HEAVY_BOLTER1" )
				parseWeaponBlock( &parser, MachPhys::HEAVY_BOLTER1 );
			else if( parser.tokens()[ 1 ] == "HEAVY_BOLTER2" )
				parseWeaponBlock( &parser, MachPhys::HEAVY_BOLTER2 );
			else if( parser.tokens()[ 1 ] == "HOMING_MISSILE" )
				parseWeaponBlock( &parser, MachPhys::HOMING_MISSILE );
			else if( parser.tokens()[ 1 ] == "LARGE_MISSILE" )
				parseWeaponBlock( &parser, MachPhys::LARGE_MISSILE );
			else if( parser.tokens()[ 1 ] == "MULTI_LAUNCHER1" )
				parseWeaponBlock( &parser, MachPhys::MULTI_LAUNCHER1 );
			else if( parser.tokens()[ 1 ] == "MULTI_LAUNCHER2" )
				parseWeaponBlock( &parser, MachPhys::MULTI_LAUNCHER2 );
			else if( parser.tokens()[ 1 ] == "MULTI_LAUNCHER3" )
				parseWeaponBlock( &parser, MachPhys::MULTI_LAUNCHER3 );
			else if( parser.tokens()[ 1 ] == "MULTI_LAUNCHER4" )
				parseWeaponBlock( &parser, MachPhys::MULTI_LAUNCHER4 );
			else if( parser.tokens()[ 1 ] == "MULTI_LAUNCHER5" )
				parseWeaponBlock( &parser, MachPhys::MULTI_LAUNCHER5 );
			else if( parser.tokens()[ 1 ] == "MULTI_LAUNCHER6" )
				parseWeaponBlock( &parser, MachPhys::MULTI_LAUNCHER6 );
			else if( parser.tokens()[ 1 ] == "MULTI_LAUNCHER7" )
				parseWeaponBlock( &parser, MachPhys::MULTI_LAUNCHER7 );
			else if( parser.tokens()[ 1 ] == "SUPERCHARGE_ADVANCED" )
				parseWeaponBlock( &parser, MachPhys::SUPERCHARGE_ADVANCED );
			else if( parser.tokens()[ 1 ] == "SUPERCHARGE_SUPER" )
				parseWeaponBlock( &parser, MachPhys::SUPERCHARGE_SUPER );
			else if( parser.tokens()[ 1 ] == "NUCLEAR_MISSILE" )
				parseWeaponBlock( &parser, MachPhys::NUCLEAR_MISSILE );
			else if( parser.tokens()[ 1 ] == "VORTEX" )
				parseWeaponBlock( &parser, MachPhys::VORTEX );
			else if( parser.tokens()[ 1 ] == "ION_ORBITAL_CANNON" )
				parseWeaponBlock( &parser, MachPhys::ION_ORBITAL_CANNON );
			else if( parser.tokens()[ 1 ] == "ORB_OF_TREACHERY" )
				parseWeaponBlock( &parser, MachPhys::ORB_OF_TREACHERY );
			else if( parser.tokens()[ 1 ] == "VIRUS" )
				parseWeaponBlock( &parser, MachPhys::VIRUS );
			else if( parser.tokens()[ 1 ] == "BEE_BOMB" )
				parseWeaponBlock( &parser, MachPhys::BEE_BOMB );
			else if( parser.tokens()[ 1 ] == "WASP_LIGHT_STING" )
				parseWeaponBlock( &parser, MachPhys::WASP_LIGHT_STING );
			else if( parser.tokens()[ 1 ] == "WASP_METAL_STING" )
				parseWeaponBlock( &parser, MachPhys::WASP_METAL_STING );
            else
                ASSERT_FAIL( "Illegal token" );
		}

        if( parser.tokens()[ 0 ] == "GENERAL_DATA" )
            parseGeneralData( &parser );

		parser.parseNextLine();
	}

}

void MachPhysDataParser::parseAggressorBlock( UtlLineTokeniser* pParser )
{

	PRE( pParser->tokens()[ 1 ] == "AGGRESSOR" );
    PRE( pParser->tokens().size() == 3 );

    MachPhys::AggressorSubType subType =  MachPhysSymbolParser::aggressorSubType( pParser->tokens()[ 2 ] );

    pParser->parseNextLine();

    while( pParser->tokens()[ 0 ] != "END" )
    {

        AggressorDataStore* pStore = pData_->pDataStore( subType );

		if( not	parseCommonMachineToken( pParser, machineDataStore( *pStore ),
          MachPhysLevels::instance().hardwareLevelIndexMap( subType ),
          MachPhysLevels::instance().softwareLevelIndexMap( subType ) ) )
        {
            size_t  nHardwareLevels = MachPhysLevels::instance().nHardwareLevels( subType );
            size_t  nSoftwareLevels = MachPhysLevels::instance().nSoftwareLevels( subType );

    		if( pParser->tokens()[ 0 ] == "ACCURACY")
            {
                ASSERT( pParser->tokens().size() == nHardwareLevels + 1, "" );

    			for( size_t hwIndex = 0; hwIndex < nHardwareLevels; ++hwIndex )
                {
                    MATHEX_SCALAR   accuracy = atol( pParser->tokens()[ hwIndex + 1 ].c_str() );
                    size_t  hardwareLevel = MachPhysLevels::instance().hardwareLevel( subType, hwIndex );

        			for( size_t swIndex = 0; swIndex < nSoftwareLevels; ++swIndex )
                    {
                        size_t  softwareLevel = MachPhysLevels::instance().softwareLevel( subType, swIndex );

        				(*pStore)[ hardwareLevel ][ softwareLevel ]->accuracy( accuracy );
                    }
                }
            }
            else if( pParser->tokens()[ 0 ] == "NUMBER_WEAPONS")
            {
                ASSERT( pParser->tokens().size() == nHardwareLevels + 1, "" );

    			for( size_t hwIndex = 0; hwIndex < nHardwareLevels; ++hwIndex )
                {
                    MATHEX_SCALAR   nWeapons = atol( pParser->tokens()[ hwIndex + 1 ].c_str() );
                    size_t  hardwareLevel = MachPhysLevels::instance().hardwareLevel( subType, hwIndex );

        			for( size_t swIndex = 0; swIndex < nSoftwareLevels; ++swIndex )
                    {
                        size_t  softwareLevel = MachPhysLevels::instance().softwareLevel( subType, swIndex );

        				(*pStore)[ hardwareLevel ][ softwareLevel ]->nWeapons( nWeapons );
                    }
                }
            }
            else
            {
                ASSERT_INFO( pParser->tokens()[ 0 ] );
                ASSERT_FAIL( "Illegal token" );
            }
        }

        pParser->parseNextLine();
	}

//    calculateMachineCost( aggressorBaseData_ );
}

void MachPhysDataParser::parseAdministratorBlock( UtlLineTokeniser* pParser )
{
	PRE( pParser->tokens()[ 1 ] == "ADMINISTRATOR" )
    PRE( pParser->tokens().size() == 3 );

   	MachPhys::AdministratorSubType subType =  MachPhysSymbolParser::administratorSubType( pParser->tokens()[ 2 ] );

	pParser->parseNextLine();

    while( pParser->tokens()[ 0 ] != "END" )
    {

        AdministratorDataStore* pStore = pData_->pDataStore( subType );

		if(	not parseCommonMachineToken( pParser, machineDataStore( *pStore ),
          MachPhysLevels::instance().hardwareLevelIndexMap( subType ),
          MachPhysLevels::instance().softwareLevelIndexMap( subType ) ) )
        {
            size_t  nHardwareLevels = MachPhysLevels::instance().nHardwareLevels( subType );
            size_t  nSoftwareLevels = MachPhysLevels::instance().nSoftwareLevels( subType );

    		if( pParser->tokens()[ 0 ] == "ACCURACY")
            {
                ASSERT( pParser->tokens().size() == nHardwareLevels + 1, "" );

    			for( size_t hwIndex = 0; hwIndex < nHardwareLevels; ++hwIndex )
                {
                    MATHEX_SCALAR   accuracy = atol( pParser->tokens()[ hwIndex + 1 ].c_str() );
                    size_t  hardwareLevel = MachPhysLevels::instance().hardwareLevel( subType, hwIndex );

        			for( size_t swIndex = 0; swIndex < nSoftwareLevels; ++swIndex )
                    {
                        size_t  softwareLevel = MachPhysLevels::instance().softwareLevel( subType, swIndex );

        				(*pStore)[ hardwareLevel ][ softwareLevel ]->accuracy( accuracy );
                    }
                }
            }
            else if( pParser->tokens()[ 0 ] == "NUMBER_WEAPONS")
            {
                ASSERT( pParser->tokens().size() == nHardwareLevels + 1, "" );

    			for( size_t hwIndex = 0; hwIndex < nHardwareLevels; ++hwIndex )
                {
                    MATHEX_SCALAR   nWeapons = atol( pParser->tokens()[ hwIndex + 1 ].c_str() );
                    size_t  hardwareLevel = MachPhysLevels::instance().hardwareLevel( subType, hwIndex );

        			for( size_t swIndex = 0; swIndex < nSoftwareLevels; ++swIndex )
                    {
                        size_t  softwareLevel = MachPhysLevels::instance().softwareLevel( subType, swIndex );

        				(*pStore)[ hardwareLevel ][ softwareLevel ]->nWeapons( nWeapons );
                    }
                }
            }
            else if( pParser->tokens()[ 0 ] == "MAX_SUBORDINATES")
            {
                ASSERT( pParser->tokens().size() == nHardwareLevels + 1, "" );

    			for( size_t hwIndex = 0; hwIndex < nHardwareLevels; ++hwIndex )
                {
                    MATHEX_SCALAR   maxSubordinates = atol( pParser->tokens()[ hwIndex + 1 ].c_str() );
                    size_t  hardwareLevel = MachPhysLevels::instance().hardwareLevel( subType, hwIndex );

        			for( size_t swIndex = 0; swIndex < nSoftwareLevels; ++swIndex )
                    {
                        size_t  softwareLevel = MachPhysLevels::instance().softwareLevel( subType, swIndex );

        				(*pStore)[ hardwareLevel ][ softwareLevel ]->maxSubordinates( maxSubordinates );
                    }
                }
            }
            else
            {
                ASSERT_INFO( pParser->tokens()[ 0 ] );
                ASSERT_FAIL( "Illegal token" );
            }
        }

		pParser->parseNextLine();
	}

//    calculateMachineCost( administratorBaseData_ );
}

void MachPhysDataParser::parseAPCBlock( UtlLineTokeniser* pParser )
{
	PRE( pParser->tokens()[ 1 ] == "APC" );
    PRE( pParser->tokens().size() == 2 );

	pParser->parseNextLine();

    while( pParser->tokens()[ 0 ] != "END" )
    {
		if( not	parseCommonMachineToken( pParser, machineDataStore( pData_->APCData_ ),
          MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::APC ),
          MachPhysLevels::instance().softwareLevelIndexMap( MachPhys::APC ) ) )
        {
            size_t  nHardwareLevels = MachPhysLevels::instance().nHardwareLevels( MachPhys::APC );
            size_t  nSoftwareLevels = MachPhysLevels::instance().nSoftwareLevels( MachPhys::APC );

    		if( pParser->tokens()[ 0 ] == "CAPACITY")
            {
                ASSERT( pParser->tokens().size() == nHardwareLevels + 1, "" );

    			for( size_t hwIndex = 0; hwIndex < nHardwareLevels; ++hwIndex )
                {
                    MATHEX_SCALAR   capacity = atol( pParser->tokens()[ hwIndex + 1 ].c_str() );
                    size_t  hardwareLevel = MachPhysLevels::instance().hardwareLevel( MachPhys::APC, hwIndex );

        			for( size_t swIndex = 0; swIndex < nSoftwareLevels; ++swIndex )
                    {
                        size_t  softwareLevel = MachPhysLevels::instance().softwareLevel( MachPhys::APC, swIndex );

        				pData_->APCData_[ hardwareLevel ][ softwareLevel ]->capacity( capacity );
                    }
                }
            }
            else
            {
                ASSERT_INFO( pParser->tokens()[ 0 ] );
                ASSERT_FAIL( "Illegal token" );
            }
        }
		pParser->parseNextLine();
	}
}

void MachPhysDataParser::parseResourceCarrierBlock( UtlLineTokeniser* pParser )
{
	PRE( pParser->tokens()[ 1 ] == "RESOURCE_CARRIER" );
    PRE( pParser->tokens().size() == 2 );

	pParser->parseNextLine();

    while( pParser->tokens()[ 0 ] != "END" )
    {
		if( not	parseCommonMachineToken( pParser, machineDataStore( pData_->resourceCarrierData_ ),
          MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::RESOURCE_CARRIER ),
          MachPhysLevels::instance().softwareLevelIndexMap( MachPhys::RESOURCE_CARRIER ) ) )
        {
            size_t  nHardwareLevels = MachPhysLevels::instance().nHardwareLevels( MachPhys::RESOURCE_CARRIER );
            size_t  nSoftwareLevels = MachPhysLevels::instance().nSoftwareLevels( MachPhys::RESOURCE_CARRIER );

    		if( pParser->tokens()[ 0 ] == "CAPACITY")
            {
                ASSERT( pParser->tokens().size() == nHardwareLevels + 1, "" );

    			for( size_t hwIndex = 0; hwIndex < nHardwareLevels; ++hwIndex )
                {
                    MATHEX_SCALAR   capacity = atol( pParser->tokens()[ hwIndex + 1 ].c_str() );
                    size_t  hardwareLevel = MachPhysLevels::instance().hardwareLevel( MachPhys::RESOURCE_CARRIER, hwIndex );

        			for( size_t swIndex = 0; swIndex < nSoftwareLevels; ++swIndex )
                    {
                        size_t  softwareLevel = MachPhysLevels::instance().softwareLevel( MachPhys::RESOURCE_CARRIER, swIndex );

        				pData_->resourceCarrierData_[ hardwareLevel ][ softwareLevel ]->capacity( capacity );
                    }
                }
            }
            else
            {
                ASSERT_INFO( pParser->tokens()[ 0 ] );
                ASSERT_FAIL( "Illegal token" );
            }
        }

		pParser->parseNextLine();
	}
}


void MachPhysDataParser::parseEPPBlock( UtlLineTokeniser* pParser )
{
	PRE( pParser->tokens()[ 1 ] == "EPP" )
    PRE( pParser->tokens().size() == 2 );

	pParser->parseNextLine();

	// first line MUST be the name of a new EPP

	ASSERT( pParser->tokens().size() == 2
			and pParser->tokens()[ 0 ] == "NAME",
			"First line of an EPP data block must be of the format NAME [EPPNAME]" );

	string EPPName = pParser->tokens()[ 1 ];
	MachPhysEvasionPriorityPlans::instance().addNewEPP( EPPName );

	pParser->parseNextLine();

    while( pParser->tokens()[ 0 ] != "END" )
    {
		ASSERT_INFO( pParser->tokens()[ 0 ] );
		ASSERT( pParser->tokens().size() == 2, "Illegal number of tokens in EPP block." );

		int priority = atoi( pParser->tokens()[ 1 ].c_str() );

		if( pParser->tokens()[ 0 ] == "GARRISONS_PRIORITY")
        {
         	MachPhysEvasionPriorityPlans::instance().garrisonPriority( EPPName, priority );
		}
		else if( pParser->tokens()[ 0 ] == "AGGRESSIVES_PRIORITY")
        {
         	MachPhysEvasionPriorityPlans::instance().aggressivesPriority( EPPName, priority );
		}
		else if( pParser->tokens()[ 0 ] == "POD_PRIORITY")
        {
         	MachPhysEvasionPriorityPlans::instance().podPriority( EPPName, priority );
		}
		else if( pParser->tokens()[ 0 ] == "TURRETS_PRIORITY")
        {
         	MachPhysEvasionPriorityPlans::instance().turretsPriority( EPPName, priority );
		}
        else
        {
            ASSERT_FAIL( "Illegal token" );
        }

		pParser->parseNextLine();
	}
}

void MachPhysDataParser::parseGeoLocatorBlock( UtlLineTokeniser* pParser )
{
	PRE( pParser->tokens()[ 1 ] == "GEO_LOCATOR" )
    PRE( pParser->tokens().size() == 2 );

	pParser->parseNextLine();

    while( pParser->tokens()[ 0 ] != "END" )
    {
		if( not	parseCommonMachineToken( pParser, machineDataStore( pData_->geoLocatorData_ ),
          MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::GEO_LOCATOR ),
          MachPhysLevels::instance().softwareLevelIndexMap( MachPhys::GEO_LOCATOR ) ) )
        {
            size_t  nHardwareLevels = MachPhysLevels::instance().nHardwareLevels( MachPhys::GEO_LOCATOR );
            size_t  nSoftwareLevels = MachPhysLevels::instance().nSoftwareLevels( MachPhys::GEO_LOCATOR );

    		if( pParser->tokens()[ 0 ] == "MINERAL_GRADE")
            {
                ASSERT( pParser->tokens().size() == nHardwareLevels + 1, "" );

    			for( size_t hwIndex = 0; hwIndex < nHardwareLevels; ++hwIndex )
                {
                    MATHEX_SCALAR   mineralGrade = atol( pParser->tokens()[ hwIndex + 1 ].c_str() );
                    size_t  hardwareLevel = MachPhysLevels::instance().hardwareLevel( MachPhys::GEO_LOCATOR, hwIndex );

        			for( size_t swIndex = 0; swIndex < nSoftwareLevels; ++swIndex )
                    {
                        size_t  softwareLevel = MachPhysLevels::instance().softwareLevel( MachPhys::GEO_LOCATOR, swIndex );

        				pData_->geoLocatorData_[ hardwareLevel ][ softwareLevel ]->mineralGrade( mineralGrade );
                    }
                }
            }
            else
            {
                ASSERT_INFO( pParser->tokens()[ 0 ] );
                ASSERT_FAIL( "Illegal token" );
            }
        }

		pParser->parseNextLine();
	}
}

void MachPhysDataParser::parseSpyLocatorBlock( UtlLineTokeniser* pParser )
{
	PRE( pParser->tokens()[ 1 ] == "SPY_LOCATOR" )
    PRE( pParser->tokens().size() == 2 );

	pParser->parseNextLine();

    while( pParser->tokens()[ 0 ] != "END" )
    {
		if( not	parseCommonMachineToken( pParser, machineDataStore( pData_->spyLocatorData_ ),
          MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::SPY_LOCATOR ),
          MachPhysLevels::instance().softwareLevelIndexMap( MachPhys::SPY_LOCATOR ) ) )
        {
            size_t  nHardwareLevels = MachPhysLevels::instance().nHardwareLevels( MachPhys::SPY_LOCATOR );
            size_t  nSoftwareLevels = MachPhysLevels::instance().nSoftwareLevels( MachPhys::SPY_LOCATOR );

    		if( pParser->tokens()[ 0 ] == "MINE_CAPACITY")
            {
                ASSERT( pParser->tokens().size() == nHardwareLevels + 1, "" );

    			for( size_t hwIndex = 0; hwIndex < nHardwareLevels; ++hwIndex )
                {
                    MATHEX_SCALAR   mineCapacity = atol( pParser->tokens()[ hwIndex + 1 ].c_str() );
                    size_t  hardwareLevel = MachPhysLevels::instance().hardwareLevel( MachPhys::SPY_LOCATOR, hwIndex );

        			for( size_t swIndex = 0; swIndex < nSoftwareLevels; ++swIndex )
                    {
                        size_t  softwareLevel = MachPhysLevels::instance().softwareLevel( MachPhys::SPY_LOCATOR, swIndex );

        				pData_->spyLocatorData_[ hardwareLevel ][ softwareLevel ]->mineCapacity( mineCapacity );
                    }
                }
            }
            else if( pParser->tokens()[ 0 ] == "DOWNLOAD_RATE")
            {
				ASSERT( pParser->tokens().size() == nHardwareLevels + 1, "" );
				for( size_t hwIndex = 0; hwIndex < nHardwareLevels; ++hwIndex )
                {
					MachPhys::ResearchUnits   downloadRate = atol( pParser->tokens()[ hwIndex + 1 ].c_str() );
					size_t  hardwareLevel = MachPhysLevels::instance().hardwareLevel( MachPhys::SPY_LOCATOR, hwIndex );
					for( size_t swIndex = 0; swIndex < nSoftwareLevels; ++swIndex )
					{
						size_t  softwareLevel = MachPhysLevels::instance().softwareLevel( MachPhys::SPY_LOCATOR, swIndex );
						pData_->spyLocatorData_[ hardwareLevel ][ softwareLevel ]->downloadRate( downloadRate );
					}
				}
			}
            else
            {
                ASSERT_INFO( pParser->tokens()[ 0 ] );
                ASSERT_FAIL( "Illegal token" );
            }
        }

		pParser->parseNextLine();
	}
}

void MachPhysDataParser::parseTechnicianBlock( UtlLineTokeniser* pParser )
{
	PRE( pParser->tokens()[ 1 ] == "TECHNICIAN" )
    PRE( pParser->tokens().size() == 3 );

  	MachPhys::TechnicianSubType subType = MachPhysSymbolParser::technicianSubType( pParser->tokens()[ 2 ] );

	pParser->parseNextLine();

    while( pParser->tokens()[ 0 ] != "END" )
    {
        TechnicianDataStore* pStore = pData_->pDataStore( subType );

		if(	not parseCommonMachineToken( pParser, machineDataStore( *pStore ),
          MachPhysLevels::instance().hardwareLevelIndexMap( subType ),
          MachPhysLevels::instance().softwareLevelIndexMap( subType ) ) )
        {
            size_t  nHardwareLevels = MachPhysLevels::instance().nHardwareLevels( subType );
            size_t  nSoftwareLevels = MachPhysLevels::instance().nSoftwareLevels( subType );

    		if( pParser->tokens()[ 0 ] == "RESEARCH_RATE")
            {
                ASSERT( pParser->tokens().size() == nHardwareLevels + 1, "" );

    			for( size_t hwIndex = 0; hwIndex < nHardwareLevels; ++hwIndex )
                {
                    MATHEX_SCALAR   researchRate = atol( pParser->tokens()[ hwIndex + 1 ].c_str() );
                    size_t  hardwareLevel = MachPhysLevels::instance().hardwareLevel( subType, hwIndex );

        			for( size_t swIndex = 0; swIndex < nSoftwareLevels; ++swIndex )
                    {
                        size_t  softwareLevel = MachPhysLevels::instance().softwareLevel( subType, swIndex );

        				(*pStore)[ hardwareLevel ][ softwareLevel ]->researchRate( researchRate );
                    }
                }
            }
            else
            {
                ASSERT_INFO( pParser->tokens()[ 0 ] );
                ASSERT_FAIL( "Illegal token" );
            }
        }

		pParser->parseNextLine();
	}
}

void MachPhysDataParser::parseConstructorBlock( UtlLineTokeniser* pParser )
{
	PRE( pParser->tokens()[ 1 ] == "CONSTRUCTOR" )
    PRE( pParser->tokens().size() == 3 );

    MachPhys::ConstructorSubType subType =  MachPhysSymbolParser::constructorSubType( pParser->tokens()[ 2 ] );

	pParser->parseNextLine();

    while( pParser->tokens()[ 0 ] != "END" )
    {
        ConstructorDataStore* pStore = pData_->pDataStore( subType );

		if(	not parseCommonMachineToken( pParser, machineDataStore( *pStore ),
          MachPhysLevels::instance().hardwareLevelIndexMap( subType ),
          MachPhysLevels::instance().softwareLevelIndexMap( subType ) ) )
        {
            size_t  nHardwareLevels = MachPhysLevels::instance().nHardwareLevels( subType );
            size_t  nSoftwareLevels = MachPhysLevels::instance().nSoftwareLevels( subType );

    		if( pParser->tokens()[ 0 ] == "CAPACITY")
            {
                ASSERT( pParser->tokens().size() == nHardwareLevels + 1, "" );

    			for( size_t hwIndex = 0; hwIndex < nHardwareLevels; ++hwIndex )
                {
                    MATHEX_SCALAR   capacity = atol( pParser->tokens()[ hwIndex + 1 ].c_str() );
                    size_t  hardwareLevel = MachPhysLevels::instance().hardwareLevel( subType, hwIndex );

        			for( size_t swIndex = 0; swIndex < nSoftwareLevels; ++swIndex )
                    {
                        size_t  softwareLevel = MachPhysLevels::instance().softwareLevel( subType, swIndex );

        				(*pStore)[ hardwareLevel ][ softwareLevel ]->capacity( capacity );
                    }
                }
            }
    		else if( pParser->tokens()[ 0 ] == "RATE_CONSTRUCTION")
            {
                ASSERT( pParser->tokens().size() == nHardwareLevels + 1, "" );

    			for( size_t hwIndex = 0; hwIndex < nHardwareLevels; ++hwIndex )
                {
                    MATHEX_SCALAR   constructionRate = atol( pParser->tokens()[ hwIndex + 1 ].c_str() );
                    size_t  hardwareLevel = MachPhysLevels::instance().hardwareLevel( subType, hwIndex );

        			for( size_t swIndex = 0; swIndex < nSoftwareLevels; ++swIndex )
                    {
                        size_t  softwareLevel = MachPhysLevels::instance().softwareLevel( subType, swIndex );

        				(*pStore)[ hardwareLevel ][ softwareLevel ]->constructionRate( constructionRate );
                    }
                }
            }
			else if( pParser->tokens()[ 0 ] == "MOST_ADVANCED_CONSTYPE")
            {
			    ASSERT( pParser->tokens().size() == nHardwareLevels + 1, "" );

    			for( size_t hwIndex = 0; hwIndex < nHardwareLevels; ++hwIndex )
                {
                    MachPhys::ConstructionType consType =  MachPhysSymbolParser::constructionType( pParser->tokens()[ hwIndex + 1 ] );
                    size_t  hardwareLevel = MachPhysLevels::instance().hardwareLevel( subType, hwIndex );

        			for( size_t swIndex = 0; swIndex < nSoftwareLevels; ++swIndex )
                    {
                        size_t  softwareLevel = MachPhysLevels::instance().softwareLevel( subType, swIndex );

         				(*pStore)[ hardwareLevel ][ softwareLevel ]->mostAdvancedConstructionType( consType );
                    }
                }
            }
			else if( pParser->tokens()[ 0 ] == "MOST_ADVANCED_CONSSUBTYPE")
            {
                ASSERT( pParser->tokens().size() == nHardwareLevels + 1, "" );

				int consSubType;
    			for( size_t hwIndex = 0; hwIndex < nHardwareLevels; ++hwIndex )
                {
					size_t  hardwareLevel = MachPhysLevels::instance().hardwareLevel( subType, hwIndex );
				   	size_t  firstSoftwareLevel = MachPhysLevels::instance().softwareLevel( subType, 0 );
					ASSERT ( (*pStore)[ hardwareLevel ][ firstSoftwareLevel ] != NULL , "MachPhysDataParser::parseConstructorBlock: the problem here is that the construction type MUST be specified in the parmdata file before the subtype." );
					MachPhys::ConstructionType CT = _CONST_CAST ( const MachPhysConstructorData*, (*pStore)[ hardwareLevel ][ firstSoftwareLevel ] )->mostAdvancedConstructionType();

					consSubType =  MachPhysSymbolParser::constructionSubType( CT, pParser->tokens()[ hwIndex + 1 ].c_str() );

        			for( size_t swIndex = 0; swIndex < nSoftwareLevels; ++swIndex )
                    {
                        size_t  softwareLevel = MachPhysLevels::instance().softwareLevel( subType, swIndex );

        				(*pStore)[ hardwareLevel ][ softwareLevel ]->mostAdvancedSubType( consSubType );
                    }
                }
            }

			else if( pParser->tokens()[ 0 ] == "MOST_ADVANCED_CONSLEVEL")
            {
                ASSERT( pParser->tokens().size() == nHardwareLevels + 1, "" );

    			for( size_t hwIndex = 0; hwIndex < nHardwareLevels; ++hwIndex )
                {
                    size_t constructionLevel =  MachPhysSymbolParser::parseConstructionLevel( pParser->tokens()[ hwIndex + 1 ].c_str() );
                    size_t  hardwareLevel = MachPhysLevels::instance().hardwareLevel( subType, hwIndex );

        			for( size_t swIndex = 0; swIndex < nSoftwareLevels; ++swIndex )
                    {
                        size_t  softwareLevel = MachPhysLevels::instance().softwareLevel( subType, swIndex );

        				(*pStore)[ hardwareLevel ][ softwareLevel ]->mostAdvancedConstructionLevel( constructionLevel );
                    }
                }
            }
            else
            {
                ASSERT_INFO( pParser->tokens()[ 0 ] );
                ASSERT_FAIL( "Illegal token" );
            }
        }
		pParser->parseNextLine();
	}
}

bool MachPhysDataParser::parseCommonMachineToken(
    UtlLineTokeniser* pParser,
    const MachineDataStore& store,
    const MachPhysLevelIndexMap& hardwareLevelIndexMap,
    const MachPhysLevelIndexMap& softwareLevelIndexMap )
{
    //  Note that store uses indices rather than levels

    if( pParser->tokens()[ 0 ] == "SW_COSTS" or pParser->tokens()[ 0 ] == "SW_RESEARCH_COSTS" )
    {
        ASSERT_INFO( pParser->tokens()[ 0 ] );
        ASSERT_INFO( pParser->tokens().size() );
        ASSERT_INFO( softwareLevelIndexMap.nLevels() );
        ASSERT( pParser->tokens().size() == softwareLevelIndexMap.nLevels() + 1, "" );
    }
    else
    {
        ASSERT_INFO( pParser->tokens()[ 0 ] );
        ASSERT_INFO( pParser->tokens().size() );
        ASSERT_INFO( hardwareLevelIndexMap.nLevels() );
        ASSERT( pParser->tokens().size() == hardwareLevelIndexMap.nLevels() + 1, "" );
    }

    bool    commonTokenFound = true;

	if( pParser->tokens()[ 0 ] == "SW_COSTS")
		for( size_t hwIndex = 0; hwIndex < hardwareLevelIndexMap.nLevels(); ++hwIndex )
    		for( size_t swIndex = 0; swIndex < softwareLevelIndexMap.nLevels(); ++swIndex )
	    		store[ hwIndex ][ swIndex ]->swCost( atol( pParser->tokens()[ swIndex + 1 ].c_str() ) );
	else if( pParser->tokens()[ 0 ] == "SW_RESEARCH_COSTS")
		for( size_t hwIndex = 0; hwIndex < hardwareLevelIndexMap.nLevels(); ++hwIndex )
    		for( size_t swIndex = 0; swIndex < softwareLevelIndexMap.nLevels(); ++swIndex )
	    		store[ hwIndex ][ swIndex ]->swResearchCost( atol( pParser->tokens()[ swIndex + 1 ].c_str() ) );

	else if( pParser->tokens()[ 0 ] == "HW_COSTS")
		for( size_t hwIndex = 0; hwIndex < hardwareLevelIndexMap.nLevels(); ++hwIndex )
    		for( size_t swIndex = 0; swIndex < softwareLevelIndexMap.nLevels(); ++swIndex )
	    		store[ hwIndex ][ swIndex ]->hwCost( atol( pParser->tokens()[ hwIndex + 1 ].c_str() ) );
	else if( pParser->tokens()[ 0 ] == "HW_RESEARCH_COSTS")
		for( size_t hwIndex = 0; hwIndex < hardwareLevelIndexMap.nLevels(); ++hwIndex )
    		for( size_t swIndex = 0; swIndex < softwareLevelIndexMap.nLevels(); ++swIndex )
	    		store[ hwIndex ][ swIndex ]->hwResearchCost( atol( pParser->tokens()[ hwIndex + 1 ].c_str() ) );
	else if( pParser->tokens()[ 0 ] == "VOLUME")
		for( size_t hwIndex = 0; hwIndex < hardwareLevelIndexMap.nLevels(); ++hwIndex )
    		for( size_t swIndex = 0; swIndex < softwareLevelIndexMap.nLevels(); ++swIndex )
	    		store[ hwIndex ][ swIndex ]->volume( atol( pParser->tokens()[ hwIndex + 1 ].c_str() ) );
	else if( pParser->tokens()[ 0 ] == "SPEED")
		for( size_t hwIndex = 0; hwIndex < hardwareLevelIndexMap.nLevels(); ++hwIndex )
    		for( size_t swIndex = 0; swIndex < softwareLevelIndexMap.nLevels(); ++swIndex )
	    		store[ hwIndex ][ swIndex ]->speed( atof( pParser->tokens()[ hwIndex + 1 ].c_str() ) );
	else if( pParser->tokens()[ 0 ] == "TASK_SPEED")
		for( size_t hwIndex = 0; hwIndex < hardwareLevelIndexMap.nLevels(); ++hwIndex )
    		for( size_t swIndex = 0; swIndex < softwareLevelIndexMap.nLevels(); ++swIndex )
	    		store[ hwIndex ][ swIndex ]->taskSpeed( atof( pParser->tokens()[ hwIndex + 1 ].c_str() ) );
	else if( pParser->tokens()[ 0 ] == "ROTATION_SPEED")
		for( size_t hwIndex = 0; hwIndex < hardwareLevelIndexMap.nLevels(); ++hwIndex )
    		for( size_t swIndex = 0; swIndex < softwareLevelIndexMap.nLevels(); ++swIndex )
	    		store[ hwIndex ][ swIndex ]->rotationSpeed( MexDegrees( atof( pParser->tokens()[ hwIndex + 1 ].c_str() )) );
	else if( pParser->tokens()[ 0 ] == "ROTATION_ACCELERATION")
		for( size_t hwIndex = 0; hwIndex < hardwareLevelIndexMap.nLevels(); ++hwIndex )
    		for( size_t swIndex = 0; swIndex < softwareLevelIndexMap.nLevels(); ++swIndex )
	    		store[ hwIndex ][ swIndex ]->rotationAcceleration( MexDegrees( atof( pParser->tokens()[ hwIndex + 1 ].c_str() ) ));
	else if( pParser->tokens()[ 0 ] == "REPAIR_RATE")
		for( size_t hwIndex = 0; hwIndex < hardwareLevelIndexMap.nLevels(); ++hwIndex )
    		for( size_t swIndex = 0; swIndex < softwareLevelIndexMap.nLevels(); ++swIndex )
	    		store[ hwIndex ][ swIndex ]->repairRate( atof( pParser->tokens()[ hwIndex + 1 ].c_str() ) );
	else if( pParser->tokens()[ 0 ] == "ACCELERATION")
		for( size_t hwIndex = 0; hwIndex < hardwareLevelIndexMap.nLevels(); ++hwIndex )
    		for( size_t swIndex = 0; swIndex < softwareLevelIndexMap.nLevels(); ++swIndex )
	    		store[ hwIndex ][ swIndex ]->acceleration( atof( pParser->tokens()[ hwIndex + 1 ].c_str() ) );
	else if( pParser->tokens()[ 0 ] == "ARMOUR")
		for( size_t hwIndex = 0; hwIndex < hardwareLevelIndexMap.nLevels(); ++hwIndex )
    		for( size_t swIndex = 0; swIndex < softwareLevelIndexMap.nLevels(); ++swIndex )
	    		store[ hwIndex ][ swIndex ]->armour( atof( pParser->tokens()[ hwIndex + 1 ].c_str() ) );
	else if( pParser->tokens()[ 0 ] == "HP")
		for( size_t hwIndex = 0; hwIndex < hardwareLevelIndexMap.nLevels(); ++hwIndex )
    		for( size_t swIndex = 0; swIndex < softwareLevelIndexMap.nLevels(); ++swIndex )
	    		store[ hwIndex ][ swIndex ]->hitPoints( atof( pParser->tokens()[ hwIndex + 1 ].c_str() ) );
	else if( pParser->tokens()[ 0 ] == "SCANNER_RANGE")
		for( size_t hwIndex = 0; hwIndex < hardwareLevelIndexMap.nLevels(); ++hwIndex )
    		for( size_t swIndex = 0; swIndex < softwareLevelIndexMap.nLevels(); ++swIndex )
	    		store[ hwIndex ][ swIndex ]->scannerRange( atof( pParser->tokens()[ hwIndex + 1 ].c_str() ) );
	else if( pParser->tokens()[ 0 ] == "LOCOMOTION")
		for( size_t hwIndex = 0; hwIndex < hardwareLevelIndexMap.nLevels(); ++hwIndex )
    		for( size_t swIndex = 0; swIndex < softwareLevelIndexMap.nLevels(); ++swIndex )
	    		store[ hwIndex ][ swIndex ]->locomotionType( parseLocomotionType( pParser->tokens()[ hwIndex + 1 ].c_str() ) );
	else if( pParser->tokens()[ 0 ] == "LOCOMOTION_PARAMETER")
		for( size_t hwIndex = 0; hwIndex < hardwareLevelIndexMap.nLevels(); ++hwIndex )
    		for( size_t swIndex = 0; swIndex < softwareLevelIndexMap.nLevels(); ++swIndex )
	    		store[ hwIndex ][ swIndex ]->locomotionParameter( atof( pParser->tokens()[ hwIndex + 1 ].c_str() ) );
	else if( pParser->tokens()[ 0 ] == "EPP")
		for( size_t hwIndex = 0; hwIndex < hardwareLevelIndexMap.nLevels(); ++hwIndex )
    		for( size_t swIndex = 0; swIndex < softwareLevelIndexMap.nLevels(); ++swIndex )
	    		store[ hwIndex ][ swIndex ]->setEPP( pParser->tokens()[ hwIndex + 1 ] );
	else if( pParser->tokens()[ 0 ] == "MIN_ACCESS_SIZE_INDEX")
		for( size_t hwIndex = 0; hwIndex < hardwareLevelIndexMap.nLevels(); ++hwIndex )
    		for( size_t swIndex = 0; swIndex < softwareLevelIndexMap.nLevels(); ++swIndex )
	    		store[ hwIndex ][ swIndex ]->minAccessSizeIndex( atoi( pParser->tokens()[ hwIndex + 1 ].c_str() ) );
	else if( pParser->tokens()[ 0 ] == "BODY_TURN_LINK")
		for( size_t hwIndex = 0; hwIndex < hardwareLevelIndexMap.nLevels(); ++hwIndex )
    		for( size_t swIndex = 0; swIndex < softwareLevelIndexMap.nLevels(); ++swIndex )
	    		store[ hwIndex ][ swIndex ]->upperBodyTurnLinkName( pParser->tokens()[ hwIndex + 1 ] );
	else if( pParser->tokens()[ 0 ] == "BODY_TURN_RATE")
		for( size_t hwIndex = 0; hwIndex < hardwareLevelIndexMap.nLevels(); ++hwIndex )
    		for( size_t swIndex = 0; swIndex < softwareLevelIndexMap.nLevels(); ++swIndex )
	    		store[ hwIndex ][ swIndex ]->upperBodyTurnRate(  MexDegrees( atof( pParser->tokens()[ hwIndex + 1 ].c_str() )) );
	else if( pParser->tokens()[ 0 ] == "HIGH_CLEARANCE")
		for( size_t hwIndex = 0; hwIndex < hardwareLevelIndexMap.nLevels(); ++hwIndex )
    		for( size_t swIndex = 0; swIndex < softwareLevelIndexMap.nLevels(); ++swIndex )
	    		store[ hwIndex ][ swIndex ]->highClearance(  atof( pParser->tokens()[ hwIndex + 1 ].c_str() ) );
	else if( pParser->tokens()[ 0 ] == "LOW_CLEARANCE")
		for( size_t hwIndex = 0; hwIndex < hardwareLevelIndexMap.nLevels(); ++hwIndex )
    		for( size_t swIndex = 0; swIndex < softwareLevelIndexMap.nLevels(); ++swIndex )
	    		store[ hwIndex ][ swIndex ]->lowClearance(  atof( pParser->tokens()[ hwIndex + 1 ].c_str() ) );
	else if( pParser->tokens()[ 0 ] == "NVG_CAPABILITY")
		for( size_t hwIndex = 0; hwIndex < hardwareLevelIndexMap.nLevels(); ++hwIndex )
    		for( size_t swIndex = 0; swIndex < softwareLevelIndexMap.nLevels(); ++swIndex )
	    		store[ hwIndex ][ swIndex ]->hasNVG( parseNVGStatus( pParser->tokens()[ hwIndex + 1 ].c_str() ) );
    else
        commonTokenFound = false;

    return commonTokenFound;
}

void MachPhysDataParser::parseHardwareLabBlock( UtlLineTokeniser* pParser )
{
	PRE( pParser->tokens()[ 1 ] == "HARDWARE_LAB" );
	PRE( pParser->tokens().size() == 3 );

    MachPhys::HardwareLabSubType subType = MachPhysSymbolParser::hardwareLabSubType( pParser->tokens()[ 2 ] );
    size_t  nHardwareLevels = MachPhysLevels::instance().nHardwareLevels( subType );

	pParser->parseNextLine();

    HardwareLabDataStore* pStore = pData_->pDataStore( subType );

    while( pParser->tokens()[ 0 ] != "END" )
    {

		if( parseCommonConstructionToken(
            pParser,
            constructionDataStore( *pStore ),
            MachPhysLevels::instance().hardwareLevelIndexMap( subType ) ) )
        {
            //  Success
        }
        else
        {
            ASSERT_INFO( pParser->tokens()[ 0 ] );
            ASSERT_FAIL( "Unknown token" );
        }

		pParser->parseNextLine();
	}

    POST( constructionDataValid( constructionDataStore( *pStore ) ) );
}


void MachPhysDataParser::parseSmelterBlock( UtlLineTokeniser* pParser )
{
	PRE( pParser->tokens()[ 1 ] == "SMELTER" )
	PRE( pParser->tokens().size() == 2 );

	pParser->parseNextLine();

    size_t  nHardwareLevels = MachPhysLevels::instance().nHardwareLevels( MachPhys::SMELTER );

    while( pParser->tokens()[ 0 ] != "END" )
    {
		if( parseCommonConstructionToken(
            pParser,
            constructionDataStore( pData_->smelterData_ ),
            MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::SMELTER ) ) )
        {
            //  Success
        }
        else if( pParser->tokens()[ 0 ] == "CAPACITY")
        {
            for( size_t hwIndex = 0; hwIndex < nHardwareLevels; ++hwIndex )
            {
                size_t  hardwareLevel = MachPhysLevels::instance().hardwareLevel( MachPhys::SMELTER, hwIndex );
   				pData_->smelterData_[ hardwareLevel ]->capacity( atol( pParser->tokens()[ hwIndex + 1 ].c_str() ) );
            }
        }
		else if( pParser->tokens()[ 0 ] == "MINE_DISTANCE")
		{
			for( size_t hwIndex = 0; hwIndex < nHardwareLevels; ++hwIndex )
			{
				size_t  hardwareLevel = MachPhysLevels::instance().hardwareLevel( MachPhys::SMELTER, hwIndex );
				pData_->smelterData_[ hardwareLevel ]->sqrMineDistance( atol( pParser->tokens()[ hwIndex + 1 ].c_str() ) );
			}
		}
	 	else if( pParser->tokens()[ 0 ] == "PUTDOWN")
		{
			MATHEX_SCALAR	fl[ 4 ];
			ASSERT( pParser->tokens().size() == 4, "Wrong number of tokens on put down point line.\n" )
			size_t hardwareLevel = MachPhysSymbolParser::parseConstructionLevel( pParser->tokens()[ 1 ].c_str() );
			for( int i = 2; i < 4 ;++i )
				sscanf( pParser->tokens()[ i ].c_str() , "%lf", &fl[ i - 2 ] );
			pData_->smelterData_[ hardwareLevel ]->addPutDownPoint( MexPoint2d( fl[ 0 ], fl[ 1 ] ) );
		}
        else
        {
            ASSERT_INFO( pParser->tokens()[ 0 ] );
            ASSERT_FAIL( "Unknown token" );
        }

		pParser->parseNextLine();
	}

    POST( constructionDataValid( constructionDataStore( pData_->smelterData_ ) ) );
}

void MachPhysDataParser::parseFactoryBlock( UtlLineTokeniser* pParser )
{
	PRE( pParser->tokens()[ 1 ] == "FACTORY" );
	PRE( pParser->tokens().size() == 3 );

    MachPhys::FactorySubType subType = MachPhysSymbolParser::factorySubType( pParser->tokens()[ 2 ] );
    size_t  nHardwareLevels = MachPhysLevels::instance().nHardwareLevels( subType );

	pParser->parseNextLine();

    while( pParser->tokens()[ 0 ] != "END" )
    {
        FactoryDataStore* pStore = pData_->pDataStore( subType );

		if( parseCommonConstructionToken(
            pParser,
            constructionDataStore( *pStore ),
            MachPhysLevels::instance().hardwareLevelIndexMap( subType ) ) )
        {
            //  Success
        }
        else if( pParser->tokens()[ 0 ] == "BUILD_RATES")
        {
            for( size_t hwIndex = 0; hwIndex < nHardwareLevels; ++hwIndex )
            {
                size_t  hardwareLevel = MachPhysLevels::instance().hardwareLevel( subType, hwIndex );
   				(*pStore)[ hardwareLevel ]->buildRate( atol( pParser->tokens()[ hwIndex + 1 ].c_str() ) );
            }
        }
        else
        {
            ASSERT_INFO( pParser->tokens()[ 0 ] );
            ASSERT_FAIL( "Unknown token" );
        }

		pParser->parseNextLine();
	}

}

void MachPhysDataParser::parseMissileEmplacementBlock( UtlLineTokeniser* pParser )
{
	PRE( pParser->tokens()[ 1 ] == "MISSILE_EMPLACEMENT" );
	PRE( pParser->tokens().size() == 3 );

    MachPhys::MissileEmplacementSubType subType = MachPhysSymbolParser::missileEmplacementSubType( pParser->tokens()[ 2 ] );
    size_t  nHardwareLevels = MachPhysLevels::instance().nHardwareLevels( subType );

	pParser->parseNextLine();

    while( pParser->tokens()[ 0 ] != "END" )
    {
        MissileEmplacementDataStore* pStore = pData_->pDataStore( subType );

		if( parseCommonConstructionToken(
            pParser,
            constructionDataStore( *pStore ),
            MachPhysLevels::instance().hardwareLevelIndexMap( subType ) ) )
        {
            //  Success
        }
        else if( pParser->tokens()[ 0 ] == "NUMBER_WEAPONS")
        {
            for( size_t hwIndex = 0; hwIndex < nHardwareLevels; ++hwIndex )
            {
                size_t  hardwareLevel = MachPhysLevels::instance().hardwareLevel( subType, hwIndex );
   				(*pStore)[ hardwareLevel ]->nWeapons( atol( pParser->tokens()[ hwIndex + 1 ].c_str() ) );
            }
        }
        else if( pParser->tokens()[ 0 ] == "ACCURACY")
        {
            for( size_t hwIndex = 0; hwIndex < nHardwareLevels; ++hwIndex )
            {
                size_t  hardwareLevel = MachPhysLevels::instance().hardwareLevel( subType, hwIndex );
   				(*pStore)[ hardwareLevel ]->accuracy( atol( pParser->tokens()[ hwIndex + 1 ].c_str() ) );
            }
        }
        else if( pParser->tokens()[ 0 ] == "MAX_ROTATION")
        {
            for( size_t hwIndex = 0; hwIndex < nHardwareLevels; ++hwIndex )
            {
                size_t  hardwareLevel = MachPhysLevels::instance().hardwareLevel( subType, hwIndex );
   				(*pStore)[ hardwareLevel ]->maxRotation( MexDegrees( atof( pParser->tokens()[ hwIndex + 1 ].c_str() ) ) );
            }
        }
        else
        {
            ASSERT_INFO( pParser->tokens()[ 0 ] );
            ASSERT_FAIL( "Unknown token" );
        }

		pParser->parseNextLine();
	}
}

void MachPhysDataParser::parseGarrisonBlock( UtlLineTokeniser* pParser )
{
	PRE( pParser->tokens()[ 1 ] == "GARRISON" )
	PRE( pParser->tokens().size() == 2 );

	size_t  nHardwareLevels = MachPhysLevels::instance().nHardwareLevels( MachPhys::GARRISON );

	pParser->parseNextLine();

    while( pParser->tokens()[ 0 ] != "END" )
    {
		if( parseCommonConstructionToken(
            pParser,
            constructionDataStore( pData_->garrisonData_ ),
            MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::GARRISON ) ) )
        {
            //  Success
        }
        else if( pParser->tokens()[ 0 ] == "REPAIR_RATE")
        {
            for( size_t hwIndex = 0; hwIndex < nHardwareLevels; ++hwIndex )
            {
                size_t  hardwareLevel = MachPhysLevels::instance().hardwareLevel( MachPhys::GARRISON, hwIndex );
   				pData_->garrisonData_[ hardwareLevel ]->repairRate( atol( pParser->tokens()[ hwIndex + 1 ].c_str() ) );
            }
        }
		else
		{
            ASSERT_INFO( pParser->tokens()[ 0 ] );
            ASSERT_FAIL( "Unknown token" );
        }

		pParser->parseNextLine();
	}

    POST( constructionDataValid( constructionDataStore( pData_->garrisonData_ ) ) );
}

void MachPhysDataParser::parseMineBlock( UtlLineTokeniser* pParser )
{
	PRE( pParser->tokens()[ 1 ] == "MINE" )
	PRE( pParser->tokens().size() == 2 );

    size_t  nHardwareLevels = MachPhysLevels::instance().nHardwareLevels( MachPhys::MINE );

	pParser->parseNextLine();

    while( pParser->tokens()[ 0 ] != "END" )
    {
		if( parseCommonConstructionToken(
            pParser,
            constructionDataStore( pData_->mineData_ ),
            MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::MINE ) ) )
        {
            //  Success
        }
        else if( pParser->tokens()[ 0 ] == "EXTRACTION_RATE")
        {
            for( size_t hwIndex = 0; hwIndex < nHardwareLevels; ++hwIndex )
            {
                size_t  hardwareLevel = MachPhysLevels::instance().hardwareLevel( MachPhys::MINE, hwIndex );
   				pData_->mineData_[ hardwareLevel ]->extractionRate( atol( pParser->tokens()[ hwIndex + 1 ].c_str() ) );
            }
        }
        else if( pParser->tokens()[ 0 ] == "CAPACITY")
        {
            for( size_t hwIndex = 0; hwIndex < nHardwareLevels; ++hwIndex )
            {
                size_t  hardwareLevel = MachPhysLevels::instance().hardwareLevel( MachPhys::MINE, hwIndex );
   				pData_->mineData_[ hardwareLevel ]->capacity( atol( pParser->tokens()[ hwIndex + 1 ].c_str() ) );
            }
        }
		else if( pParser->tokens()[ 0 ] == "MINERAL_DISTANCE")
		{
			for( size_t hwIndex = 0; hwIndex < nHardwareLevels; ++hwIndex )
			{
				size_t  hardwareLevel = MachPhysLevels::instance().hardwareLevel( MachPhys::MINE, hwIndex );
				pData_->mineData_[ hardwareLevel ]->sqrMineralDistance( atol( pParser->tokens()[ hwIndex + 1 ].c_str() ) );
			}
		}
        else if( pParser->tokens()[ 0 ] == "PICKUP")
        {
			ASSERT( pParser->tokens().size() == 4, "Wrong number of tokens on pick up point line.\n" )
			size_t level = MachPhysSymbolParser::parseConstructionLevel( pParser->tokens()[ 1 ].c_str() );

			pData_->mineData_[ level ]->addPickUpPoint(
              MexPoint2d( atof( pParser->tokens()[ 2 ].c_str() ),
                          atof( pParser->tokens()[ 3 ].c_str() ) ) );
        }
        else
        {
            ASSERT_INFO( pParser->tokens()[ 0 ] );
            ASSERT_FAIL( "Unknown token" );
        }

		pParser->parseNextLine();
	}
}

void MachPhysDataParser::parseBeaconBlock( UtlLineTokeniser* pParser )
{
	PRE( pParser->tokens()[ 1 ] == "BEACON" )
	PRE( pParser->tokens().size() == 2 );

	pParser->parseNextLine();

    while( pParser->tokens()[ 0 ] != "END" )
    {
		if( parseCommonConstructionToken(
            pParser,
            constructionDataStore( pData_->beaconData_ ),
            MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::BEACON ) ) )
        {
            //  Success
        }
        else
        {
            ASSERT_INFO( pParser->tokens()[ 0 ] );
            ASSERT_FAIL( "Unknown token" );
        }

		pParser->parseNextLine();
	}
}

void MachPhysDataParser::parsePodBlock( UtlLineTokeniser* pParser )
{
	PRE( pParser->tokens()[ 1 ] == "POD" )
	PRE( pParser->tokens().size() == 2 );

 	size_t  nHardwareLevels = MachPhysLevels::instance().nHardwareLevels( MachPhys::POD );

	pParser->parseNextLine();

    while( pParser->tokens()[ 0 ] != "END" )
    {
		if( parseCommonConstructionToken(
            pParser,
            constructionDataStore( pData_->podData_ ),
            MachPhysLevels::instance().hardwareLevelIndexMap( MachPhys::POD ) ) )
        {
            //  Success
        }
		else if( pParser->tokens()[ 0 ] == "EXTRACTION_RATE")
        {
            for( size_t hwIndex = 0; hwIndex < nHardwareLevels; ++hwIndex )
            {
                size_t  hardwareLevel = MachPhysLevels::instance().hardwareLevel( MachPhys::POD, hwIndex );
   				pData_->podData_[ hardwareLevel ]->extractionRate( atol( pParser->tokens()[ hwIndex + 1 ].c_str() ) );
            }
        }
        else if( pParser->tokens()[ 0 ] == "CAPACITY")
        {
            for( size_t hwIndex = 0; hwIndex < nHardwareLevels; ++hwIndex )
            {
			    size_t  hardwareLevel = MachPhysLevels::instance().hardwareLevel( MachPhys::POD, hwIndex );
   				pData_->podData_[ hardwareLevel ]->capacity( atol( pParser->tokens()[ hwIndex + 1 ].c_str() ) );
            }
        }
		else if( pParser->tokens()[ 0 ] == "PUTDOWN")
		{
			MATHEX_SCALAR	fl[ 4 ];
			ASSERT( pParser->tokens().size() == 4, "Wrong number of tokens on put down point line.\n" )
			size_t hardwareLevel = MachPhysSymbolParser::parseConstructionLevel( pParser->tokens()[ 1 ].c_str() );
			for( int i = 2; i < 4 ;++i )
				sscanf( pParser->tokens()[ i ].c_str() , "%lf", &fl[ i - 2 ] );
			pData_->podData_[ hardwareLevel ]->addPutDownPoint( MexPoint2d( fl[ 0 ], fl[ 1 ] ) );
		}
        else
        {
            ASSERT_INFO( pParser->tokens()[ 0 ] );
            ASSERT_FAIL( "Unknown token" );
        }

		pParser->parseNextLine();
	}

    POST( constructionDataValid( constructionDataStore( pData_->podData_ ) ) );
}

bool MachPhysDataParser::parseCommonConstructionToken(
    UtlLineTokeniser* pParser,
    const ConstructionDataStore& store,
    const MachPhysLevelIndexMap& levelIndexMap )
{
    //  Note that store uses indices rather than levels

    bool    commonTokenFound = true;

    ASSERT_INFO( pParser->tokens().size() );
    ASSERT_INFO( store.size() );

	if( pParser->tokens()[ 0 ] == "HW_COSTS")
    {
        ASSERT( pParser->tokens().size() == levelIndexMap.nLevels() + 1, "" );
		for( size_t hwIndex = 0; hwIndex < levelIndexMap.nLevels(); ++hwIndex )
			store[ hwIndex ]->cost( atol( pParser->tokens()[ hwIndex + 1 ].c_str() ) );
    }
	else if( pParser->tokens()[ 0 ] == "HW_RESEARCH_COSTS")
    {
        ASSERT( pParser->tokens().size() == levelIndexMap.nLevels() + 1, "" );
		for( size_t hwIndex = 0; hwIndex < levelIndexMap.nLevels(); ++hwIndex )
			store[ hwIndex ]->hwResearchCost( atol( pParser->tokens()[ hwIndex + 1 ].c_str() ) );
  }
	else if( pParser->tokens()[ 0 ] == "ARMOUR")
    {
        ASSERT( pParser->tokens().size() == levelIndexMap.nLevels() + 1, "" );
		for( size_t hwIndex = 0; hwIndex < levelIndexMap.nLevels(); ++hwIndex )
			store[ hwIndex ]->armour( atol( pParser->tokens()[ hwIndex + 1 ].c_str() ) );
    }
	else if( pParser->tokens()[ 0 ] == "HP")
    {
        ASSERT( pParser->tokens().size() == levelIndexMap.nLevels() + 1, "" );
		for( size_t hwIndex = 0; hwIndex < levelIndexMap.nLevels(); ++hwIndex )
			store[ hwIndex ]->hitPoints( atol( pParser->tokens()[ hwIndex + 1 ].c_str() ) );
    }
	else if( pParser->tokens()[ 0 ] == "ACCESS_SIZE_INDEX")
    {
        ASSERT( pParser->tokens().size() == levelIndexMap.nLevels() + 1, "" );
		for( size_t hwIndex = 0; hwIndex < levelIndexMap.nLevels(); ++hwIndex )
			store[ hwIndex ]->accessSizeIndex( atol( pParser->tokens()[ hwIndex + 1 ].c_str() ) );
    }
	else if( pParser->tokens()[ 0 ] == "BOUNDARY")
	{
		ASSERT( pParser->tokens().size() == 6, "Wrong number of tokens on boundary line.\n" )
		MATHEX_SCALAR	fl[ 4 ];
		size_t  hwLevel = MachPhysSymbolParser::parseConstructionLevel( pParser->tokens()[ 1 ] );
        size_t  hwIndex = levelIndexMap.index( hwLevel );

		for( size_t i = 2; i < 6 ;++i )
			fl[ i - 2] = atof( pParser->tokens()[ i ].c_str() );

		store[ hwIndex ]->defineLocalBoundary( MexAlignedBox2d( fl[ 0 ], fl[ 1 ], fl[ 2 ], fl[ 3 ] ) );
	}
	else if( pParser->tokens()[ 0 ] == "ENTRANCE")
	{
		ASSERT( pParser->tokens().size() == 10, "Wrong number of tokens on entrance line.\n" )
		size_t hwLevel = MachPhysSymbolParser::parseConstructionLevel( pParser->tokens()[ 1 ] );
        size_t  hwIndex = levelIndexMap.index( hwLevel );

        MexPoint2d externalPoint( atof( pParser->tokens()[ 2 ].c_str() ), atof( pParser->tokens()[ 3 ].c_str() ) );
        MexPoint2d internalPoint( atof( pParser->tokens()[ 4 ].c_str() ), atof( pParser->tokens()[ 5 ].c_str() ) );
        MexPoint2d doorPoint( atof( pParser->tokens()[ 6 ].c_str() ), atof( pParser->tokens()[ 7 ].c_str() ) );

        string  doorEntityName = pParser->tokens()[ 8 ];

		MachPhys::DoorType doorType = MachPhys::SMALL_DOOR;
        if( pParser->tokens()[ 9 ] == "SMALL_DOOR" )
    		doorType = MachPhys::SMALL_DOOR;
        else if( pParser->tokens()[ 9 ] == "LARGE_DOOR" )
    		doorType = MachPhys::LARGE_DOOR;
        else
        {
            ASSERT_FAIL( "Illegal door type" );
        }

		store[ hwIndex ]->addEntrance( externalPoint, internalPoint, doorPoint, doorEntityName, doorType );
	}
	else if( pParser->tokens()[ 0 ] == "INTERIOR_BOUNDARY")
    {
        parseInteriorBoundary( pParser, store, levelIndexMap );
    }
	else if( pParser->tokens()[ 0 ] == "INTERIOR_OBSTACLE")
    {
        parseInteriorObstacle( pParser, store, levelIndexMap );
    }
	else if( pParser->tokens()[ 0 ] == "STATION")
    {
        parseInteriorStation( pParser, store, levelIndexMap );
    }
    else
        commonTokenFound = false;

    return commonTokenFound;
}

void MachPhysDataParser::parseInteriorBoundary(
    UtlLineTokeniser* pParser,
    const ConstructionDataStore& store,
    const MachPhysLevelIndexMap& levelIndexMap )
{
    PRE( pParser->tokens().size() );
    PRE( pParser->tokens().size() == 6 );

    size_t hwLevel = MachPhysSymbolParser::parseConstructionLevel( pParser->tokens()[ 1 ] );
    size_t hwIndex = levelIndexMap.index( hwLevel );

    MexPoint2d  minCorner( atof( pParser->tokens()[ 2 ].c_str() ), atof( pParser->tokens()[ 3 ].c_str() ) );
    MexPoint2d  maxCorner( atof( pParser->tokens()[ 4 ].c_str() ), atof( pParser->tokens()[ 5 ].c_str() ) );

    store[ hwIndex ]->interiorBoundary( MexAlignedBox2d( minCorner, maxCorner ) );
}

void MachPhysDataParser::parseInteriorObstacle(
    UtlLineTokeniser* pParser,
    const ConstructionDataStore& store,
    const MachPhysLevelIndexMap& levelIndexMap )
{
    PRE( pParser->tokens().size() );
    PRE( pParser->tokens().size() % 2 == 0 );

    size_t hwLevel = MachPhysSymbolParser::parseConstructionLevel( pParser->tokens()[ 1 ] );
    size_t hwIndex = levelIndexMap.index( hwLevel );

    ctl_vector< MexPoint2d >    points;

    size_t nPoints = ( pParser->tokens().size() - 2 ) / 2;

    for( size_t i = 0; i < nPoints; ++i )
    {
        MexPoint2d  point( atof( pParser->tokens()[ i * 2 + 2 ].c_str() ), atof( pParser->tokens()[ i * 2 + 3 ].c_str() ) );

        points.push_back( point );
    }

    store[ hwIndex ]->addInteriorObstacle( points );
}

void MachPhysDataParser::parseInteriorStation(
    UtlLineTokeniser* pParser,
    const ConstructionDataStore& store,
    const MachPhysLevelIndexMap& levelIndexMap )
{
    PRE( pParser->tokens().size() );
    PRE( pParser->tokens().size() == 5 );

    size_t hwLevel = MachPhysSymbolParser::parseConstructionLevel( pParser->tokens()[ 1 ] );
    size_t hwIndex = levelIndexMap.index( hwLevel );

    MexPoint2d  position( atof( pParser->tokens()[ 2 ].c_str() ), atof( pParser->tokens()[ 3 ].c_str() ) );

    MachPhysStation::Type type = MachPhysStation::PARKING_BAY;

    if( pParser->tokens()[ 4 ] == "PARK" )
        type = MachPhysStation::PARKING_BAY;
    else if( pParser->tokens()[ 4 ] == "RESEARCH" )
        type = MachPhysStation::RESEARCH_BAY;
    else
    {
        ASSERT_INFO( pParser->tokens()[ 4 ] );
        ASSERT_FAIL( "Illegal station type" );
    }

    store[ hwIndex ]->pImpl_->stations_.addStation( position, type );
}

ostream& operator <<( ostream& o, const MachPhysDataParser& t )
{

    o << "MachPhysDataParser " << (void*)&t << " start" << std::endl;
    o << "MachPhysDataParser " << (void*)&t << " end" << std::endl;

    return o;
}

void MachPhysDataParser::parseWeaponBlock( UtlLineTokeniser* pParser, MachPhys::WeaponType type )
{
	PRE( pParser->tokens()[ 0 ] == "TYPE" );
    PRE( pParser->tokens().size() == 2 );

    //Get the weapon data item
    //MachPhysWeaponData& weaponData = *MachPhysDataImplementation::instance().pWeaponData( type );
    MachPhysWeaponData& weaponData = *pData_->pWeaponData( type );

    //Use a temporary vector of points for launch offsets
    MachPhysWeaponData::LaunchOffsets launchOffsets;
    launchOffsets.reserve( 1 );

    //Keep parsing data lines until hit END
    pParser->parseNextLine();
    while( pParser->tokens()[ 0 ] != "END" )
    {
        //See which data item is defined
        ASSERT_INFO( pParser->tokens()[0] );
        ASSERT_INFO( pParser->tokens()[1] );
        const string& firstToken = pParser->tokens()[0];

        if( firstToken == "DAMAGE" )
        {
            ASSERT( pParser->tokens().size() == 2, "" );
            int damagePoints = atol( pParser->tokens()[1].c_str() );
            weaponData.damagePoints( damagePoints );
        }
        else if( firstToken == "RANGE" )
        {
            ASSERT( pParser->tokens().size() == 2, "" );
            MATHEX_SCALAR range = atof( pParser->tokens()[1].c_str() );
            weaponData.range( range );
        }
        else if( firstToken == "SPEED" )
        {
            ASSERT( pParser->tokens().size() == 2, "" );
            MATHEX_SCALAR speed = atof( pParser->tokens()[1].c_str() );
            weaponData.projectileSpeed( speed );
        }
        else if( firstToken == "BURST" )
        {
            ASSERT( pParser->tokens().size() == 3
					or pParser->tokens().size() == 4, "" );
            ASSERT_INFO( pParser->tokens()[2] );
            uint nRounds = atol( pParser->tokens()[1].c_str() );
            PhysRelativeTime burstTime = atof( pParser->tokens()[2].c_str() );

			if( nRounds > 1 )
			{
				ASSERT( pParser->tokens().size() == 4, "Bursts of >1 round MUST have a FIXED or VARIABLE qualifier." );

				const string& burstTypeToken = pParser->tokens()[3];

				ASSERT( burstTypeToken == "FIXED" or burstTypeToken == "VARIABLE", "Unknown burst type qualifier - should be FIXED or VARIABLE" );

	   			if( burstTypeToken == "VARIABLE" )
					weaponData.fixedBurst( false );
			}

            weaponData.nRoundsPerBurst( nRounds );
            weaponData.burstDuration( burstTime );
        }
        else if( firstToken == "RELOAD_TIME" )
        {
            ASSERT( pParser->tokens().size() == 2, "" );
            PhysRelativeTime reloadTime = atof( pParser->tokens()[1].c_str() );
            ASSERT( reloadTime > 0.0, "" );
            weaponData.reloadTime( reloadTime );
        }
        else if( firstToken == "LAUNCH_OFFSET" )
        {
            ASSERT( pParser->tokens().size() == 4, "" );
            MATHEX_SCALAR x = atof( pParser->tokens()[1].c_str() );
            MATHEX_SCALAR y = atof( pParser->tokens()[2].c_str() );
            MATHEX_SCALAR z = atof( pParser->tokens()[3].c_str() );

            launchOffsets.push_back( MexPoint3d( x, y, z ) );
        }
        else if( firstToken == "RECOIL" )
        {
            ASSERT( pParser->tokens().size() == 4, "" );
            MATHEX_SCALAR distance = atof( pParser->tokens()[1].c_str() );
            MATHEX_SCALAR backTime = atof( pParser->tokens()[2].c_str() );
            MATHEX_SCALAR foreTime = atof( pParser->tokens()[3].c_str() );

            weaponData.recoilDistance ( distance );
            weaponData.recoilBackTime ( backTime );
            weaponData.recoilForeTime ( foreTime );
        }
        else if( firstToken == "TILT_ANGLES" )
        {
            ASSERT( pParser->tokens().size() == 3, "" );
            MexRadians minAngle = MexDegrees( atof( pParser->tokens()[1].c_str() ) );
            MexRadians maxAngle = MexDegrees( atof( pParser->tokens()[2].c_str() ) );
            weaponData.tiltAngles( minAngle, maxAngle );
        }
        else if( firstToken == "DEVIATION_ANGLES" )
        {
            ASSERT( pParser->tokens().size() == 3, "" );
            MexRadians horizontalAngle = MexDegrees( atof( pParser->tokens()[1].c_str() ) );
            MexRadians verticalAngle = MexDegrees( atof( pParser->tokens()[2].c_str() ) );
            weaponData.horizontalDeviationAngle( horizontalAngle );
            weaponData.verticalDeviationAngle( verticalAngle );
        }
        else if( firstToken == "EXTRAS" )
        {
            ASSERT( pParser->tokens().size() > 1, "" );
            MachPhysWeaponData::Extras extras;
            size_t nExtras = pParser->tokens().size() - 1;
            extras.reserve( nExtras );
            for( size_t i = 0; i != nExtras; ++i )
            {
                extras.push_back( atof( pParser->tokens()[i+1].c_str() ) );
            }

            weaponData.extras( extras );
        }
		else if( firstToken == "TRAIL" )
		{
            ASSERT( pParser->tokens().size() > 1, "" );

			if( pParser->tokens()[1] == "OFF" )
			{
            	ASSERT( pParser->tokens().size() == 2, "" );
				weaponData.trailOn( false );
			}
			else if( pParser->tokens()[1] == "ON" )
			{
				ASSERT(pParser->tokens().size() >= 6, "" );

				ASSERT_INFO(pParser->tokens()[2]);
				ASSERT(pParser->tokens()[2] == "COLOUR", "" );

				weaponData.trailOn( true );

				MATHEX_SCALAR r = atof( pParser->tokens()[ 3 ].c_str() );
				MATHEX_SCALAR g = atof( pParser->tokens()[ 4 ].c_str() );
				MATHEX_SCALAR b = atof( pParser->tokens()[ 5 ].c_str() );

				r /= 255.0;
				g /= 255.0;
				b /= 255.0;

				weaponData.trailColour( RenColour(r, g, b) );

				if( pParser->tokens().size() > 6 )
				{
					ASSERT(pParser->tokens().size() >= 8, "" );
					ASSERT(pParser->tokens()[6] == "TEXTURE", "" );

					if( pParser->tokens().size() == 8 )
					{

						weaponData.trailTextureKey( pParser->tokens()[ 7 ] );
						weaponData.trailStartTextureIndex( 0 );
						weaponData.trailEndTextureIndex( 0 );
					}
					else
					{
						ASSERT(pParser->tokens().size() == 10, "" );
						weaponData.trailTextureKey( pParser->tokens()[ 7 ] );
						weaponData.trailStartTextureIndex( atol( pParser->tokens()[8].c_str() ) );
						weaponData.trailEndTextureIndex( atol( pParser->tokens()[9].c_str() ));
					}
				}
			}
		}
		else if( firstToken == "FLAME" )
		{
            ASSERT( pParser->tokens().size() > 1, "" );

			if( pParser->tokens()[2] == "NOROTATE" )
			{
				weaponData.flameRotate( false );
			}
			else
			{
				weaponData.flameRotate( true );
			}
			if(	pParser->tokens().size() > 2 )
			{
				ASSERT(pParser->tokens().size() == 5 , " there should be 5 data" );
				ASSERT(pParser->tokens()[2] == "SCALING", "SCALING should be here" );

				weaponData.flameScale( true );
				weaponData.flameScaleFrom( atof( pParser->tokens()[ 3 ].c_str() ) );
				weaponData.flameScaleTo( atof( pParser->tokens()[ 4 ].c_str() ) );
			}
			else
			{
				weaponData.flameScale( false );
			}

		}
        //Parse the next line
        pParser->parseNextLine();
    }

    //Store the final launch offsets
    weaponData.launchOffsets( launchOffsets );
}

MachPhys::LocomotionType MachPhysDataParser::parseLocomotionType( const string& token )
{
    MachPhys::LocomotionType  result = MachPhys::WHEELS;

    if( token == "WHEELS" )
        result = MachPhys::WHEELS;
    else if( token == "TRACKS" )
        result = MachPhys::TRACKS;
    else if( token == "HOVER" )
        result = MachPhys::HOVER;
    else if( token == "SPIDER" )
        result = MachPhys::SPIDER;
    else if( token == "GLIDER" )
        result = MachPhys::GLIDER;
    else
    {
        ASSERT_INFO( token );
        ASSERT_FAIL( "Illegal locomotion token" );
    }

    return result;
}

bool MachPhysDataParser::parseNVGStatus( const string& token )
{
    bool result = false;

    if( token == "YES" )
        result = true;
    else if( token == "NO" )
        result = false;
    else
    {
        ASSERT_INFO( token );
        ASSERT_FAIL( "Illegal NVG status token" );
    }

    return result;
}

void MachPhysDataParser::parseGeneralData( UtlLineTokeniser* pParser )
{
    //Advance past the GENERAL_DATA line
    pParser->parseNextLine();

    //Get the data object to use
    MachPhysGeneralData& generalData = **(pData_->pGeneralDataStore());

    //read the data line by line
    while( pParser->tokens()[0] != "END" )
    {
        if( pParser->tokens()[0] == "1ST_PERSON_LOOK_UP_DOWN_RATE" )
        {
            generalData.firstPersonLookUpDownRate( MexDegrees( atof( pParser->tokens()[1].c_str() ) ) );
        }
        else if( pParser->tokens()[0] == "1ST_PERSON_LOOK_UP_DOWN_ANGLES" )
        {
            generalData.firstPersonLookUpDownMinAngle( MexDegrees( atof( pParser->tokens()[1].c_str() ) ) );
            generalData.firstPersonLookUpDownMaxAngle( MexDegrees( atof( pParser->tokens()[2].c_str() ) ) );
        }
        else if( pParser->tokens()[0] == "1ST_PERSON_MAX_TURN_RATES" )
        {
            generalData.firstPersonMaxFastTurnRate( MexDegrees( atof( pParser->tokens()[1].c_str() ) ) );
            generalData.firstPersonMaxSlowTurnRate( MexDegrees( atof( pParser->tokens()[2].c_str() ) ) );
        }
		else if( pParser->tokens()[0] == "START_ROTATION_DISTANCE" )
        {
            generalData.startRotationDistance( atof( pParser->tokens()[1].c_str() ) );
        }
		else if( pParser->tokens()[0] == "MAX_MOVE_GRADIENT" )
        {
            generalData.maxMoveGradient( MexDegrees( atof( pParser->tokens()[1].c_str() ) ) );
        }
		else if( pParser->tokens()[0] == "MAX_TERRAIN_UNEVENNESS" )
        {
            generalData.maxTerrainUnevenness( atof( pParser->tokens()[1].c_str() ) );
        }
		else if( pParser->tokens()[0] == "VIRTUAL_DEFCON_INTERVAL" )
		{
			generalData.virtualDefConInterval( atof( pParser->tokens()[1].c_str() ) );
		}
		else if( pParser->tokens()[0] == "PC_PATH_FINDING_PRIORITY" )
		{
			generalData.pcPathFindingPriority( atoi( pParser->tokens()[1].c_str() ) );
		}
		else if( pParser->tokens()[0] == "NUKE_LAUNCH_COST" )
		{
			generalData.nukeLaunchCost( atoi( pParser->tokens()[1].c_str() ) );
		}
		else if( pParser->tokens()[0] == "MAX_UNITS_PER_RACE" )
		{
			generalData.maxUnitsPerRace( atoi( pParser->tokens()[1].c_str() ) );
		}
		else if( pParser->tokens()[0] == "CAMPAIGN_EXTRA_UNITS_PER_RACE" )
		{
			generalData.campaignExtraUnitsPerRace( atoi( pParser->tokens()[1].c_str() ) );
		}
        else
        {
            ASSERT_INFO( pParser->tokens()[0] );
            ASSERT_FAIL( "Unrecognised general data keyword" );
        }

        pParser->parseNextLine();
    }
}

bool MachPhysDataParser::constructionDataValid(
    const ConstructionDataStore& store ) const
{
    bool    result = true;

    for( size_t i = 0; i < store.size(); ++i )
    {
		if( store[ i ] )
        {
    		if( not store[ i ]->dataValid() )
                result = false;
        }
    }

    return result;
}

/* End MPHYDATA.CPP *************************************************/
