/*
 * G A R R I S O N . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/garrison.hpp"
#include "machphys/ofactory.hpp"

#include "stdlib/string.hpp"
#include "system/pathname.hpp"

#include "mathex/transf3d.hpp"
#include "sim/manager.hpp"
#include "machphys/garrdata.hpp"
#include "machphys/entrance.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/levels.hpp"
#include "machphys/snddata.hpp"

#include "world4d/soundman.hpp"

PER_DEFINE_PERSISTENT( MachPhysGarrison );

MachPhysGarrison::MachPhysGarrison(
    W4dEntity* pParent,
    const W4dTransform3d& localTransform,
    size_t level,
    MachPhys::Race race )
: MachPhysConstruction( part( level ), pParent, localTransform, level, race ),
  pData_( _NEW( MachPhysGarrisonData( part( level ).data(), globalTransform() ) ) )
{
    TEST_INVARIANT;
}

//  This is the constructor that is used by the factory. It is the
//  only constructor that actually builds a garrison from scratch

MachPhysGarrison::MachPhysGarrison( W4dEntity* pParent, size_t level )
: MachPhysConstruction( pParent, W4dTransform3d(), compositeFileName( level ), wireframeFileName( level ),
    interiorCompositeFileName( level ), 10.0, level,
    MachPhysData::instance().garrisonData( level ) ),
  pData_( _NEW( MachPhysGarrisonData( MachPhysData::instance().garrisonData( level ), W4dTransform3d() ) ) )
{
    TEST_INVARIANT;
}

MachPhysGarrison::MachPhysGarrison( PerConstructor con )
: MachPhysConstruction( con ),
  pData_( NULL )
{
}

MachPhysGarrison::~MachPhysGarrison()
{
    TEST_INVARIANT;

    _DELETE( pData_ );
}

// static
MachPhysGarrison& MachPhysGarrison::part( size_t hardwareLevel )
{
    return factory().part(
        hardwareLevel,
        MachPhysLevels::instance().uniqueHardwareIndex( MachPhys::GARRISON, hardwareLevel ) );
}

// static
MachPhysGarrison::Factory& MachPhysGarrison::factory()
{
    static  Factory   factory_( MachPhysLevels::instance().nHardwareIndices( MachPhys::GARRISON ) );
    
    return factory_;
}

//virtual
const MachPhysConstructionData& MachPhysGarrison::constructionData() const
{
	return data();
}

const MachPhysGarrisonData& MachPhysGarrison::data() const
{
	return *pData_;
}

SysPathName MachPhysGarrison::compositeFileName( size_t level ) const
{
    SysPathName result;
    
    switch( level )
    {
        case 1:
			//Altered path SJA 19/11
            result = "models/garrison/level1/exterior/ga1e.cdf";
            break;
            
        default:
            ASSERT_BAD_CASE_INFO( level );
            break;
    }
    
    return result;
}

SysPathName MachPhysGarrison::wireframeFileName( size_t level ) const
{
    SysPathName result;
    
    switch( level )
    {
        case 1:
            result = "models/garrison/level1/wirefram/ga1w.cdf";
            break;
            
        default:
            ASSERT_BAD_CASE_INFO( level );
            break;
    }
    
    return result;
}

SysPathName MachPhysGarrison::interiorCompositeFileName( size_t level ) const
{
    SysPathName result;
    
    switch( level )
    {
        case 1:
			//Altered path SJA 19/11
            result = "models/garrison/level1/interior/ga1i.cdf";
            break;
            
        default:
            ASSERT_BAD_CASE_INFO( level );
            break;
    }
    
    return result;
}

void MachPhysGarrison::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void MachPhysGarrison::persistenceInitialiseData()
{
    pData_ = _NEW( MachPhysGarrisonData( 
      MachPhysData::instance().garrisonData( level() ), W4dTransform3d() ) );

    persistenceConstructionData( *pData_ );
}

void MachPhysGarrison::doPercentageComplete( double newPercentage )
{
	MachPhysConstruction::doPercentageComplete(newPercentage);
	if(newPercentage > 99)
	{
		PhysAbsoluteTime now = SimManager::instance().currentTime();
		W4dSoundManager::instance().play(this, SID_GARRISON, now, 0);
	}
}

void perWrite( PerOstream& ostr, const MachPhysGarrison& construction )
{
    const MachPhysConstruction& base = construction;
    ostr << base;
}

void perRead( PerIstream& istr, MachPhysGarrison& construction )
{
    MachPhysConstruction& base = construction;
    istr >> base;

    construction.persistenceInitialiseData();
}

void MachPhysGarrison::damageLevel( const double& percent )
{
	MachPhysConstruction::damageLevel( percent );
	damageSmoke1Type( GARRISON_GREEN );
	damageSmoke2Type( GARRISON_YELLOW );
}

/* End GARRISON.CPP *****************************************************/
