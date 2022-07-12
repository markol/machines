/*
 * A P C . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/ofactory.hpp"
#include "machphys/apc.hpp"

//This file must be included before mexpdata.hpp to avoid
//instantiation errors.  No idea why.
#include "mathex/point3d.hpp"

#include "stdlib/string.hpp"

#include "ctl/list.hpp"

#include "system/pathname.hpp"
#include "machphys/mexpdata.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/apcdata.hpp"
#include "machphys/levels.hpp"
#include "machphys/linkdata.hpp"


#include "world4d/link.hpp"

PER_DEFINE_PERSISTENT( MachPhysAPC );

MachPhysAPC::MachPhysAPC(
    W4dEntity* pParent,
    const W4dTransform3d& localTransform,
    size_t bodyLevel,
    size_t brainLevel,
    MachPhys::Race race )
: MachPhysMachine( part( bodyLevel ), pParent, localTransform, bodyLevel, brainLevel, race,
   MachPhysData::instance().APCData( bodyLevel, brainLevel ) )
{
    TEST_INVARIANT;

}

//  This is the constructor that is used by the factory. It is the
//  only constructor that actually builds a transporter from scratch

MachPhysAPC::MachPhysAPC( W4dEntity* pParent, Id bodyLevel )
: MachPhysMachine( pParent, W4dTransform3d(), compositeFileName( bodyLevel ),
                   MachPhysData::instance().APCData( bodyLevel, 1 ) )
{
    createExplosionData();

    TEST_INVARIANT;

}

MachPhysAPC::MachPhysAPC( PerConstructor con )
: MachPhysMachine( con )
{
}

SysPathName MachPhysAPC::compositeFileName( size_t bodyLevel ) const
{
    SysPathName result;

    switch( bodyLevel )
    {
        case 2:
            result = "models/transpor/person/level2/trp2.cdf";
            break;

        case 4:
            result = "models/transpor/person/level4/trp4.cdf";
            break;

        case 5:
            result = "models/transpor/person/level5/trp5.cdf";
            break;


        default:
            ASSERT_BAD_CASE_INFO( bodyLevel );
            break;
    }

    return result;
}

// static
MachPhysAPC& MachPhysAPC::part( size_t level )
{
    return factory().part(
        level,
        MachPhysLevels::instance().uniqueHardwareIndex( MachPhys::APC, level ) );
}

// static
MachPhysAPC::Factory& MachPhysAPC::factory()
{
    static  Factory   factory_( MachPhysLevels::instance().nHardwareIndices( MachPhys::APC ) );

    return factory_;
}

//virtual
const MachPhysMachineData& MachPhysAPC::machineData() const
{
	return data();
}

const MachPhysAPCData& MachPhysAPC::data() const
{
	return MachPhysData::instance().APCData( bodyLevel(), brainLevel() );
}

MachPhysAPC::~MachPhysAPC()
{
    TEST_INVARIANT;

}

void MachPhysAPC::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysAPC& t )
{

    o << "MachPhysAPC " << (void*)&t << " start" << std::endl;
    o << "MachPhysAPC " << (void*)&t << " end" << std::endl;

    return o;
}

void MachPhysAPC::createExplosionData()
{
    MachPhysMachineExplosionData& dataForEdit = explosionDataForEdit();


    dataForEdit.minToShootOff( 3 );

	// For the two "flying" apcs, move the explosion center
	// to the machine center
    switch( bodyLevel() )
    {
        case 2:
			break;
        case 4:
		{
    		MexPoint3d expCenter( dataForEdit.explosionCenter() );
	        expCenter.z( expCenter.z() + 3.5 );
 	        dataForEdit.explosionCenter( expCenter );
			break;
        }
        case 5:
    	{
    		MexPoint3d expCenter( dataForEdit.explosionCenter() );
	        expCenter.z( expCenter.z() + 4 );
 	        dataForEdit.explosionCenter( expCenter );
			break;
        }
        default:
            ASSERT_BAD_CASE_INFO( bodyLevel() );
	}
}

void perWrite( PerOstream& ostr, const MachPhysAPC& machine )
{
    const MachPhysMachine& base = machine;
    ostr << base;
}

void perRead( PerIstream& istr, MachPhysAPC& machine )
{
    MachPhysMachine& base = machine;
    istr >> base;
}

/* End APC.CPP *************************************************/
