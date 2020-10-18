/*
 * S W L A B . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "system/pathname.hpp"
#include "world4d/link.hpp"
#include "world4d/soundman.hpp"
#include "mathex/transf3d.hpp"

#include "machphys/swlab.hpp"
#include "machphys/entrance.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/softdata.hpp"
#include "machphys/levels.hpp"

MachPhysSoftwareLab::MachPhysSoftwareLab(
    W4dEntity* pParent,
    const W4dTransform3d& localTransform,
    size_t level,
    MachPhys::Race race )
: MachPhysConstruction( factory( level ), pParent, localTransform, level, race ),
  pData_( _NEW( MachPhysSoftwareLabData( factory( level ).data(), globalTransform() ) ) )
{
	W4dSoundManager::instance().play( this, SID_LABS, PhysAbsoluteTime( 0 ), 0 );
//    W4dSoundManager::instance().play( this, SysPathName( "sounds/labs.wav" ),
//                                      PhysAbsoluteTime( 0 ), 100.0, 8.0,
//                                      W4dSoundManager::LOOP_CONTINUOUSLY );

    TEST_INVARIANT;
}

//  This is the constructor that is used by the factory. It is the
//  only constructor that actually builds a software lab from scratch

MachPhysSoftwareLab::MachPhysSoftwareLab( W4dEntity* pParent, size_t level )
: MachPhysConstruction( pParent, W4dTransform3d(), compositeFileName( level ), wireframeFileName( level ),
   interiorCompositeFileName( level ), 10.0, level,
   MachPhysData::instance().softwareLabData( level ) ),
  pData_( _NEW( MachPhysSoftwareLabData( MachPhysData::instance().softwareLabData( level ), W4dTransform3d() ) ) )
{
    TEST_INVARIANT;
}

MachPhysSoftwareLab::~MachPhysSoftwareLab()
{
    _DELETE( pData_ );

    TEST_INVARIANT;
}

// static
MachPhysSoftwareLab& MachPhysSoftwareLab::factory( size_t level )
{
    static  Factory   factory_( MachPhysLevels::instance().nHardwareIndices( MachPhys::SOFTWARE_LAB ) );

    return factory_.part(
        level,
        MachPhysLevels::instance().uniqueHardwareIndex( MachPhys::SOFTWARE_LAB, level ) );
}

//virtual
const MachPhysConstructionData& MachPhysSoftwareLab::constructionData() const
{
	return data();
}

const MachPhysSoftwareLabData& MachPhysSoftwareLab::data() const
{
	return *pData_;
}

SysPathName MachPhysSoftwareLab::compositeFileName( size_t level ) const
{
    SysPathName result;

    switch( level )
    {
        case 1:
			//Altered path SJA 19/11
            result = "models/softlab/level1/exterior/sw1e.cdf";
            break;

        default:
            ASSERT_BAD_CASE_INFO( level );
            break;
    }

    return result;
}

SysPathName MachPhysSoftwareLab::wireframeFileName( size_t level ) const
{
    SysPathName result;

    switch( level )
    {
        case 1:
            result = "models/softlab/level1/wirefram/sw1w.cdf";
            break;

        default:
            ASSERT_BAD_CASE_INFO( level );
            break;
    }

    return result;
}

SysPathName MachPhysSoftwareLab::interiorCompositeFileName( size_t level ) const
{
    SysPathName result;

    switch( level )
    {
        case 1:
			//Altered path SJA 19/11
            result = "models/softlab/level1/interior/sw1i.cdf";
            break;

        default:
            ASSERT_BAD_CASE_INFO( level );
            break;
    }

    return result;
}

void MachPhysSoftwareLab::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

/* End SWLAB.CPP *****************************************************/
