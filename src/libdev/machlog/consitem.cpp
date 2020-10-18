/*
 * C O N S I T E M . C P P
 * (c) Charybdis Limited, 1998. All Rights Conserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/consitem.hpp"
#include "machlog/internal/consitei.hpp"
#include "machlog/races.hpp"
#include "machlog/constree.hpp"

#include "machphys/machphys.hpp"

#include "machphys/mphydata.hpp"
#include "machphys/beacdata.hpp"
#include "machphys/harddata.hpp"
#include "machphys/minedata.hpp"
#include "machphys/poddata.hpp"
#include "machphys/smeldata.hpp"
#include "machphys/msemdata.hpp"
#include "machphys/garrdata.hpp"
#include "machphys/factdata.hpp"

PER_DEFINE_PERSISTENT( MachLogConstructionItem );

/* //////////////////////////////////////////////////////////////////////////// */

MachLogConstructionItem::MachLogConstructionItem( 	MachLog::ObjectType consType,
													int subType ,
													size_t hwLevel,
													MachPhys::WeaponCombo wc )
:	pImpl_( _NEW( MachLogConstructionItemImpl() ) )
{
	CB_MachLogConstructionItem_DEPIMPL();

	constructionType_ = consType;
	subType_ = subType;
	hwLevel_ = hwLevel;
	weaponCombo_ = wc;

	for(int i = 0; i < MachPhys::N_RACES ; ++i)
	{
		activated_[ i ] = false;
		activationLocked_[ i ] = false;
	}

	if( wc != MachPhys::N_WEAPON_COMBOS )
		hasWeaponCombo_ = true;


	switch( consType )
	{
		case MachLog::BEACON :
			buildingCost_ = MachPhysData::instance().beaconData( hwLevel ).cost();
			break;
		case MachLog::FACTORY :
			buildingCost_ = MachPhysData::instance().factoryData( (MachPhys::FactorySubType)subType, hwLevel ).cost();
			break;
		case MachLog::GARRISON :
			buildingCost_ = MachPhysData::instance().garrisonData( hwLevel ).cost();
			break;
		case MachLog::HARDWARE_LAB :
			buildingCost_ = MachPhysData::instance().hardwareLabData( (MachPhys::HardwareLabSubType)subType, hwLevel ).cost();
			break;
		case MachLog::POD :
			buildingCost_ = MachPhysData::instance().podData( hwLevel ).cost();
			break;
		case MachLog::MINE :
			buildingCost_ = MachPhysData::instance().mineData( hwLevel ).cost();
			break;
		case MachLog::MISSILE_EMPLACEMENT :
			buildingCost_ = MachPhysData::instance().missileEmplacementData( (MachPhys::MissileEmplacementSubType)subType, hwLevel ).cost();
			break;
		case MachLog::SMELTER :
			buildingCost_ = MachPhysData::instance().smelterData( hwLevel ).cost();
			break;
		DEFAULT_ASSERT_BAD_CASE( consType );
	}

	TEST_INVARIANT;
}

/* //////////////////////////////////////////////////////////////////////////// */

MachLogConstructionItem::~MachLogConstructionItem( void )
{
	TEST_INVARIANT;

	_DELETE( pImpl_ );
}

/* //////////////////////////////////////////////////////////////////////////// */

const MachLog::ObjectType&
MachLogConstructionItem::constructionType() const
{
	CB_DEPIMPL( MachLog::ObjectType, constructionType_ );

	return constructionType_;
}

//////////////////////////////////////////////////////////////////////////////

int
MachLogConstructionItem::subType() const
{
	CB_DEPIMPL( int, subType_ );

	return subType_;
}

//////////////////////////////////////////////////////////////////////////////

size_t
MachLogConstructionItem::hwLevel() const
{
	CB_DEPIMPL( size_t, hwLevel_ );

	return hwLevel_;
}

//////////////////////////////////////////////////////////////////////////////

MachPhys::BuildingMaterialUnits
MachLogConstructionItem::buildingCost() const
{
	CB_DEPIMPL( MachPhys::BuildingMaterialUnits, buildingCost_ );

	return buildingCost_;
}

//////////////////////////////////////////////////////////////////////////////

const MachPhys::WeaponCombo
MachLogConstructionItem::weaponCombo() const
{
	CB_DEPIMPL( MachPhys::WeaponCombo, weaponCombo_ );

	return weaponCombo_;
}

//////////////////////////////////////////////////////////////////////////////

bool
MachLogConstructionItem::hasWeaponCombo() const
{
	CB_DEPIMPL( bool, hasWeaponCombo_ );

	return hasWeaponCombo_;
}

//////////////////////////////////////////////////////////////////////////////

bool
MachLogConstructionItem::activated( MachPhys::Race race ) const
{
	CB_DEPIMPL_ARRAY( bool, activated_ );

	return activated_[ race ];
}

//////////////////////////////////////////////////////////////////////////////

void MachLogConstructionItem::activate( MachPhys::Race race )
{
	CB_DEPIMPL_ARRAY( bool, activated_ );
	CB_DEPIMPL_ARRAY( bool, activationLocked_ );
	if( not activationLocked_[ race ] )
		activated_[ race ] = true;
}

//////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const MachLogConstructionItem& t )
{

    o << "MachLogConstructionItem " << (void*)&t << " start" << std::endl;
	o << " ConstructionType " << t.constructionType() << std::endl;
	o << " subType " << t.subType() << std::endl;
	o << " hwLevel " << t.hwLevel() << std::endl;
	o << " weapon combo " << t.weaponCombo() << " hasWC defined " << t.hasWeaponCombo() << std::endl;
   	//for( MachPhys::Race i = 0; i != MachPhys::N_RACES; ++((int&)i) )
   	for( MachPhys::Race i = MachPhys::RED; i != MachPhys::N_RACES; ++i )
	{
	   	o << " activated[" << i << "] " << t.activated( i ) << " activation locked " << t.activationLocked( i ) << std::endl;
	}
    o << "MachLogConstructionItem " << (void*)&t << " end" << std::endl;

    return o;
}

//////////////////////////////////////////////////////////////////////////////

void MachLogConstructionItem::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

void MachLogConstructionItem::activationLocked( MachPhys::Race race, bool newValue )
{
	CB_DEPIMPL_ARRAY( bool, activationLocked_ );
	CB_DEPIMPL_ARRAY( bool, activated_ );
	activationLocked_[ race ] = newValue;
	if( newValue and activated_[ race ] )
		activated_[ race ] = false;
}

bool MachLogConstructionItem::activationLocked( MachPhys::Race race ) const
{
	CB_DEPIMPL_ARRAY( bool, activationLocked_ );
	return activationLocked_[ race ];
}

void perWrite( PerOstream& ostr, const MachLogConstructionItem& consItem )
{
	ostr << consItem.pImpl_;
}

void perRead( PerIstream& istr, MachLogConstructionItem& consItem )
{
	istr >> consItem.pImpl_;
}

MachLogConstructionItem::MachLogConstructionItem( PerConstructor )
{
}

//////////////////////////////////////////////////////////////////////////////

/* End CONSITEM.CPP **************************************************/
