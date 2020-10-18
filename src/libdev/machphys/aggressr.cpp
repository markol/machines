/*
 * A G G R E S S R . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "machphys/ofactory.hpp"
#include "machphys/subid.hpp"

//This file must be included before mexpdata.hpp to avoid
//instantiation errors.  No idea why.
#include "stdlib/string.hpp"
#include "system/pathname.hpp"
#include "mathex/point3d.hpp"

#include "machphys/aggressr.hpp"
#include "machphys/mexpdata.hpp"

#include "machphys/mphydata.hpp"
#include "machphys/aggrdata.hpp"
#include "machphys/levels.hpp"
#include "machphys/armourer.hpp"

PER_DEFINE_PERSISTENT( MachPhysAggressor );

MachPhysAggressor::MachPhysAggressor(
    W4dEntity* pParent,
    const W4dTransform3d& localTransform,
    MachPhys::AggressorSubType subType,
    size_t bodyLevel,
    size_t brainLevel,
    MachPhys::Race race,
    MachPhys::WeaponCombo combo )
: MachPhysMachine( part( subType, bodyLevel ), pParent, localTransform, bodyLevel, brainLevel, race,
  MachPhysData::instance().aggressorData( subType, bodyLevel, brainLevel ) ),
  MachPhysCanAttack( part( subType, bodyLevel ), this ),
  subType_( subType )
{
    //Mount the appropriate weapons
    MachPhysArmourer::fitWeapons( this, this, combo );

    TEST_INVARIANT;
}

//  This is the constructor that is used by the factory. It is the
//  only constructor that actually builds an aggressor from scratch

MachPhysAggressor::MachPhysAggressor( W4dEntity* pParent, Id id )
: MachPhysMachine( pParent, W4dTransform3d(), compositeFileName( id.subType_, id.level_ ),
                   MachPhysData::instance().aggressorData( id.subType_, id.level_, 1 ) ),
  subType_( id.subType_ )
{
    //Set up the mounting link info for attaching weapons
    initialiseMountingLinks();

	createExplosionData();

    TEST_INVARIANT;
}

MachPhysAggressor::MachPhysAggressor( PerConstructor con )
: MachPhysMachine( con )
{
}

MachPhysAggressor::~MachPhysAggressor()
{
    TEST_INVARIANT;

}

MachPhys::AggressorSubType MachPhysAggressor::subType( void ) const
{
    return subType_;
}

SysPathName MachPhysAggressor::compositeFileName( MachPhys::AggressorSubType subType, size_t bodyLevel ) const
{
    SysPathName result;

    switch( subType )
    {
        case MachPhys::GRUNT:
        {
            switch( bodyLevel )
            {
                case 1:
                    result = "models/aggresso/grunt/level1/agg1.cdf";
                    break;

                case 2:
                    result = "models/aggresso/grunt/level2/agg2.cdf";
                    break;


                case 3:
                    result = "models/aggresso/grunt/level3/agg3.cdf";
                    break;

                default:
                    ASSERT_BAD_CASE_INFO( bodyLevel );
            }
            break;
        }

        case MachPhys::ASSASSIN:
        {
            switch( bodyLevel )
            {
                case 2:
                    result = "models/aggresso/assassin/level2/aga2.cdf";
                    break;

                case 3:
                    result = "models/aggresso/assassin/level3/aga3.cdf";
                    break;
            }
            break;
        }

        case MachPhys::BALLISTA:
        {
            switch( bodyLevel )
            {
                case 3:
                    result = "models/aggresso/ballista/level3/agb3.cdf";
                    break;

                case 4:
                    result = "models/aggresso/ballista/level4/agb4.cdf";
                    break;
            }
            break;
        }

        case MachPhys::KNIGHT:
        {
            switch( bodyLevel )
            {
                case 3:
                    result = "models/aggresso/knight/level3/agk3.cdf";
                    break;

                case 4:
                    result = "models/aggresso/knight/level4/agk4.cdf";
                    break;

                case 5:
                    result = "models/aggresso/knight/level5/agk5.cdf";
                    break;
            }
            break;
        }

        case MachPhys::NINJA:
        {
            switch( bodyLevel )
            {
                case 5:
                    result = "models/aggresso/ninja/level5/agm5.cdf";
                    break;
            }
            break;
        }

        default:
            ASSERT_BAD_CASE_INFO( subType );
            break;
    }

    return result;
}

// static
MachPhysAggressor& MachPhysAggressor::part( MachPhys::AggressorSubType subType, size_t hardwareLevel )
{
    return factory().part(
        Id( subType, hardwareLevel ),
        MachPhysLevels::instance().uniqueHardwareIndex( subType, hardwareLevel ) );
}

// static
MachPhysAggressor::Factory& MachPhysAggressor::factory()
{
    static  Factory   factory_( MachPhysLevels::instance().nHardwareIndices( MachPhys::AGGRESSOR ) );

    return factory_;
}

//virtual
const MachPhysMachineData& MachPhysAggressor::machineData( void ) const
{
	return data();
}

const MachPhysAggressorData& MachPhysAggressor::data( void ) const
{
	return MachPhysData::instance().aggressorData( subType_, bodyLevel(), brainLevel() );
}

void MachPhysAggressor::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysAggressor& t )
{

    o << "MachPhysAggressor " << (void*)&t << " start" << std::endl;
    o << "MachPhysAggressor " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
W4dComposite& MachPhysAggressor::asComposite()
{
    return _STATIC_CAST( W4dComposite&, *this );
}

//virtual
const W4dComposite& MachPhysAggressor::asComposite() const
{
    return _STATIC_CAST( const W4dComposite&, *this );
}

// should only be called by one time constructor
// to overide default settings of MachPhysMachine::defaultExplosionData()
void MachPhysAggressor::createExplosionData( void )
{

    switch( subType() )
    {
        case MachPhys::GRUNT:
			break;
        case MachPhys::ASSASSIN:
        {
            MachPhysMachineExplosionData& expData = explosionDataForEdit();
           	// just update the position of the explosion center
	        MexPoint3d explosionCenter( expData.explosionCenter() );
	        explosionCenter.z( explosionCenter.z() + 1.5 );
 	        expData.explosionCenter( explosionCenter );
            break;
        }
        case MachPhys::BALLISTA:
			break;
        case MachPhys::KNIGHT:
        {
            switch( bodyLevel() )
            {
                case 3:
                {
		            MachPhysMachineExplosionData& expData = explosionDataForEdit();
	                MexPoint3d explosionCenter( expData.explosionCenter() );
	                explosionCenter.z( explosionCenter.z() + 0.4 );
 	                expData.explosionCenter( explosionCenter );
                    break;
                }
                case 4:
                {
		            MachPhysMachineExplosionData& expData = explosionDataForEdit();
	                MexPoint3d explosionCenter( expData.explosionCenter() );
	                explosionCenter.z( explosionCenter.z() + 2.0 );
 	                expData.explosionCenter( explosionCenter );
                    break;
                }
                case 5:
                {
		            MachPhysMachineExplosionData& expData = explosionDataForEdit();
	                MexPoint3d explosionCenter( expData.explosionCenter() );
	                explosionCenter.z( explosionCenter.z() + 1.8 );
 	                expData.explosionCenter( explosionCenter );
                    break;
                }
                default:
                    break;
			 }
			break;
        }
        case MachPhys::NINJA:
        {
            MachPhysMachineExplosionData& expData = explosionDataForEdit();
           	// just update the position of the explosion center
	        MexPoint3d explosionCenter( expData.explosionCenter() );
	        explosionCenter.z( explosionCenter.z() + 1.0 );
 	        expData.explosionCenter( explosionCenter );
			break;
        }
        default:
            ASSERT_BAD_CASE_INFO( subType() );
    }
}

void perWrite( PerOstream& ostr, const MachPhysAggressor& aggressor )
{
    const MachPhysMachine& base1 = aggressor;
    const MachPhysCanAttack& base2 = aggressor;

    ostr << base1;
    ostr << base2;
    ostr << aggressor.subType_;
}

void perRead( PerIstream& istr, MachPhysAggressor& aggressor )
{
    MachPhysMachine& base1 = aggressor;
    MachPhysCanAttack& base2 = aggressor;

    istr >> base1;
    istr >> base2;
    istr >> aggressor.subType_;
}

bool MachPhysAggressor::canPunch() const
{
	return subType_ == MachPhys::NINJA;
}

//virtual
bool MachPhysAggressor::canTrackWeaponBase() const
{
    return canTurnUpperBody();
}

//virtual
void MachPhysAggressor::doWeaponBaseTrackTarget( const W4dEntity& targetObject )
{
    PRE( canTurnUpperBody() );
    //upperBodyTrackTarget( targetObject );
    upperBodyTrackTarget( _CONST_CAST(W4dEntity&, targetObject) );
}

//virtual
void MachPhysAggressor::doStopWeaponBaseTrackingTarget()
{
    upperBodyStopTrackingTarget();
}

/* End AGGRESSR.CPP *************************************************/
