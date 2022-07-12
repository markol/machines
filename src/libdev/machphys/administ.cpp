/*
 * A D M I N I S T . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//These 3 files need to be included before administ.hpp to avoid instantiation errors.
//Don't know why
#include "machphys/ofactory.hpp"
#include "machphys/subid.hpp"
#include "stdlib/string.hpp"

#include "machphys/administ.hpp"

#include "machphys/mexpdata.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/admndata.hpp"
#include "machphys/levels.hpp"
#include "machphys/armourer.hpp"

#include "system/pathname.hpp"

#include "mathex/point3d.hpp"

PER_DEFINE_PERSISTENT( MachPhysAdministrator );

MachPhysAdministrator::MachPhysAdministrator(
    W4dEntity* pParent,
    const W4dTransform3d& localTransform,
    MachPhys::AdministratorSubType subType,
    size_t bodyLevel,
    size_t brainLevel,
    MachPhys::Race race,
    MachPhys::WeaponCombo combo )
:	MachPhysMachine( part( subType, bodyLevel ), pParent,
					localTransform, bodyLevel, brainLevel, race,
   					MachPhysData::instance().administratorData( subType, bodyLevel, brainLevel ) ),
    MachPhysCanAttack( part( subType, bodyLevel ), this ),
	subType_( subType )
{
    //Mount the appropriate weapons
    MachPhysArmourer::fitWeapons( this, this, combo );

    TEST_INVARIANT;
}


//  This is the constructor that is used by the factory. It is the
//  only constructor that actually builds an administrator from scratch

MachPhysAdministrator::MachPhysAdministrator( W4dEntity* pParent, Id id )
:	MachPhysMachine( pParent, W4dTransform3d(), compositeFileName( id.subType_, id.level_ ),
                     MachPhysData::instance().administratorData( id.subType_, id.level_, 1 ) ),
	subType_( id.subType_ )
{
    //Set up the mounting link info for attaching weapons
    initialiseMountingLinks();

	createExplosionData();

    TEST_INVARIANT;

}

MachPhysAdministrator::MachPhysAdministrator( PerConstructor con )
: MachPhysMachine( con )
{
}

MachPhysAdministrator::~MachPhysAdministrator()
{
    TEST_INVARIANT;

}

MachPhys::AdministratorSubType MachPhysAdministrator::subType() const
{
    return subType_;
}

SysPathName MachPhysAdministrator::compositeFileName( MachPhys::AdministratorSubType subType, size_t bodyLevel ) const
{
    SysPathName result;

    switch( subType )
    {
        case MachPhys::BOSS:
        {
            switch( bodyLevel )
            {
                case 1:
                    result = "models/administ/boss/level1/adb1.cdf";
                    break;

                case 2:
                    result = "models/administ/boss/level2/adb2.cdf";
                    break;

                default:
                    ASSERT_BAD_CASE_INFO( bodyLevel );
            }
            break;
        }

        case MachPhys::OVERSEER:
        {
            switch( bodyLevel )
            {
                case 2:
                    result = "models/administ/overseer/level2/ado2.cdf";
                    break;

                case 3:
                    result = "models/administ/overseer/level3/ado3.cdf";
                    break;

                default:
                    ASSERT_BAD_CASE_INFO( bodyLevel );
            }
            break;
        }

        case MachPhys::COMMANDER:
        {
            switch( bodyLevel )
            {
                case 4:
                    result = "models/administ/command/level4/adc4.cdf";
                    break;

                case 5:
                    result = "models/administ/command/level5/adc5.cdf";
                    break;

                default:
                    ASSERT_BAD_CASE_INFO( bodyLevel );
            }
            break;
        }

        default:
            ASSERT_BAD_CASE_INFO( subType );
    }

    return result;
}

// static
MachPhysAdministrator& MachPhysAdministrator::part(
    MachPhys::AdministratorSubType subType,
    size_t hardwareLevel )
{
    return factory().part(
        Id( subType, hardwareLevel ),
        MachPhysLevels::instance().uniqueHardwareIndex( subType, hardwareLevel ) );
}

// static
MachPhysAdministrator::Factory& MachPhysAdministrator::factory()
{
    static  Factory   factory_( MachPhysLevels::instance().nHardwareIndices( MachPhys::ADMINISTRATOR ) );

    return factory_;
}

void MachPhysAdministrator::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysAdministrator& t )
{

    o << "MachPhysAdministrator " << (void*)&t << " start" << std::endl;
    o << "MachPhysAdministrator " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
const MachPhysMachineData& MachPhysAdministrator::machineData() const
{
	return data();
}

const MachPhysAdministratorData& MachPhysAdministrator::data() const
{
	return MachPhysData::instance().administratorData( subType_, bodyLevel() , brainLevel() );
}

//virtual
W4dComposite& MachPhysAdministrator::asComposite()
{
    return _STATIC_CAST( W4dComposite&, *this );
}

//virtual
const W4dComposite& MachPhysAdministrator::asComposite() const
{
    return _STATIC_CAST( const W4dComposite&, *this );
}

// should only be called by one time constructor
// to overide default settings of MachPhysMachine::defaultExplosionData()
void MachPhysAdministrator::createExplosionData()
{

    switch( subType() )
    {
        case MachPhys::BOSS:
            break;
        case MachPhys::OVERSEER:
        {
            switch( bodyLevel() )
            {
                case 2:
                    break;
                case 3:
				{
                    MachPhysMachineExplosionData& expData = explosionDataForEdit();
           	        // just update the position of the explosion center
	                MexPoint3d explosionCenter( expData.explosionCenter() );
	                explosionCenter.z( explosionCenter.z() + 1 );
 	                expData.explosionCenter( explosionCenter );
                    break;
            	}
                default:
	            	ASSERT_BAD_CASE_INFO( bodyLevel() );
                    break;
            }
            break;
        }
        case MachPhys::COMMANDER:
        {
            MachPhysMachineExplosionData& expData = explosionDataForEdit();
           	// just update the position of the explosion center
	        MexPoint3d explosionCenter( expData.explosionCenter() );
	        explosionCenter.z( explosionCenter.z() + 1.3 );
 	        expData.explosionCenter( explosionCenter );
            break;
        }
        default:
            ASSERT_BAD_CASE_INFO( subType() );
    }
}

void perWrite( PerOstream& ostr, const MachPhysAdministrator& machine )
{
    const MachPhysMachine& base1 = machine;
    const MachPhysCanAttack& base2 = machine;
    ostr << base1;
    ostr << base2;

    ostr << machine.subType_;
}

void perRead( PerIstream& istr, MachPhysAdministrator& machine )
{
    MachPhysMachine& base1 = machine;
    MachPhysCanAttack& base2 = machine;
    istr >> base1;
    istr >> base2;

    istr >> machine.subType_;
}

//virtual
bool MachPhysAdministrator::canTrackWeaponBase() const
{
    return canTurnUpperBody();
}

//virtual
void MachPhysAdministrator::doWeaponBaseTrackTarget( const W4dEntity& targetObject )
{
    PRE( canTurnUpperBody() );
    //upperBodyTrackTarget( targetObject );
    upperBodyTrackTarget( _CONST_CAST(W4dEntity&, targetObject) );
}

//virtual
void MachPhysAdministrator::doStopWeaponBaseTrackingTarget()
{
    upperBodyStopTrackingTarget();
}
/* End ADMINIST.CPP *************************************************/
