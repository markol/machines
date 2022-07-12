/*
 * T E C H N I C I . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/ofactory.hpp"
#include "machphys/subid.hpp"
#include "machphys/technici.hpp"

#include "stdlib/string.hpp"
#include "mathex/point3d.hpp"
#include "world4d/soundman.hpp"
#include "world4d/compplan.hpp"
#include "sim/manager.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/techdata.hpp"
#include "machphys/levels.hpp"
#include "machphys/mexpdata.hpp"

#include "system/pathname.hpp"

PER_DEFINE_PERSISTENT( MachPhysTechnician );

MachPhysTechnician::MachPhysTechnician(
    W4dEntity* pParent,
    const W4dTransform3d& localTransform,
    MachPhys::TechnicianSubType subType,
    size_t bodyLevel,
    size_t brainLevel,
    MachPhys::Race race )
: MachPhysMachine( part( subType, bodyLevel ), pParent, localTransform, bodyLevel, brainLevel, race,
   MachPhysData::instance().technicianData( subType, bodyLevel, brainLevel ) ),
  subType_( subType )
{
	TEST_INVARIANT;
}

//  This is the constructor that is used by the factory. It is the
//  only constructor that actually builds a technician from scratch

MachPhysTechnician::MachPhysTechnician( W4dEntity* pParent, Id id )
: MachPhysMachine( pParent, W4dTransform3d(), compositeFileName( id.subType_, id.level_ ),
                   MachPhysData::instance().technicianData( id.subType_, id.level_, id.level_ ) ),
  subType_( id.subType_ )
{
	createExplosionData();
    TEST_INVARIANT;
}

MachPhysTechnician::MachPhysTechnician( PerConstructor con )
: MachPhysMachine( con )
{
}

MachPhysTechnician::~MachPhysTechnician()
{
    TEST_INVARIANT;

}

MachPhys::TechnicianSubType MachPhysTechnician::subType() const
{
    return subType_;
}

SysPathName MachPhysTechnician::compositeFileName( MachPhys::TechnicianSubType subType, size_t bodyLevel ) const
{
    SysPathName result;

    switch( subType )
    {
        case MachPhys::LAB_TECH:

            switch( bodyLevel )
            {
                case 2:
                    result = "models/technici/labtech/level2/tel2.cdf";
                    break;

                default:
                    ASSERT_BAD_CASE_INFO( bodyLevel );
                    break;
            }

        	break;

        case MachPhys::TECH_BOY:

            switch( bodyLevel )
            {
                case 3:
                    result = "models/technici/techboy/level3/tet3.cdf";
                    break;

                default:
                    ASSERT_BAD_CASE_INFO( bodyLevel );
                    break;
            }

        	break;

        case MachPhys::BRAIN_BOX:

            switch( bodyLevel )
            {
                case 5:
                    result = "models/technici/brainbox/level5/teb5.cdf";
                    break;

                default:
                    ASSERT_BAD_CASE_INFO( bodyLevel );
                    break;
            }

        	break;

        default:
            ASSERT_BAD_CASE_INFO( subType );
            break;
    }

    return result;
}

// static
MachPhysTechnician& MachPhysTechnician::part( MachPhys::TechnicianSubType subType, size_t hardwareLevel )
{
    return factory().part(
        Id( subType, hardwareLevel ),
        MachPhysLevels::instance().uniqueHardwareIndex( subType, hardwareLevel ) );
}

// static
MachPhysTechnician::Factory& MachPhysTechnician::factory()
{
    static  Factory   factory_( MachPhysLevels::instance().nHardwareIndices( MachPhys::TECHNICIAN ) );

    return factory_;
}

//virtual
const MachPhysMachineData& MachPhysTechnician::machineData() const
{
	return data();
}

const MachPhysTechnicianData& MachPhysTechnician::data() const
{
	return MachPhysData::instance().technicianData( subType_, bodyLevel(), brainLevel() );
}

void MachPhysTechnician::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysTechnician& t )
{

    o << "MachPhysTechnician " << (void*)&t << " start" << std::endl;
    o << "MachPhysTechnician " << (void*)&t << " end" << std::endl;

    return o;
}

PhysRelativeTime MachPhysTechnician::research()
{
    PhysRelativeTime interval;
	static int animation = 0;

	W4dCompositePlanPtr thePlan;
	if(  not cycleAnims( &thePlan ) )
        interval = 1;
	else
	{
        const uint nTimes = 4;
		if(animation)
		{
			cycleAnims( &thePlan );
			--animation;
		}
		else
			++animation;

        plan( *thePlan, SimManager::instance().currentTime(), nTimes - 1 );
        interval = nTimes * thePlan->finishTime();
	}

    return interval;
}

void MachPhysTechnician::createExplosionData()
{
    MachPhysMachineExplosionData& expData = explosionDataForEdit();
    switch( subType() )
    {
        case MachPhys::LAB_TECH:
            break;
        case MachPhys::BRAIN_BOX:
        {
            switch( bodyLevel() )
            {
                case 5:
				{
        	  		// just update the position of the explosion center
	        		MexPoint3d explosionCenter( expData.explosionCenter() );
	        		explosionCenter.z( explosionCenter.z() + 0.20 );
 	        		expData.explosionCenter( explosionCenter );
                    break;
				}
			    default:
            		ASSERT_BAD_CASE_INFO( bodyLevel() );
			}
        }
        case MachPhys::TECH_BOY:
        {
           	// just update the position of the explosion center
	        MexPoint3d explosionCenter( expData.explosionCenter() );
	        explosionCenter.z( explosionCenter.z() + 0.25 );
 	        expData.explosionCenter( explosionCenter );
            break;
        }
        default:
            ASSERT_BAD_CASE_INFO( subType() );
    }
}

void perWrite( PerOstream& ostr, const MachPhysTechnician& machine )
{
    const MachPhysMachine& base = machine;
    ostr << base;

    ostr << machine.subType_;
}

void perRead( PerIstream& istr, MachPhysTechnician& machine )
{
    MachPhysMachine& base = machine;
    istr >> base;

    istr >> machine.subType_;
}

/* End TECHNICI.CPP *************************************************/
