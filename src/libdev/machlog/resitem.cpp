/*
 * R E S I T E M . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/resitem.hpp"
#include "machlog/internal/resitemi.hpp"
#include "mathex/point2d.hpp"
#include "machlog/races.hpp"
#include "machlog/restree.hpp"
#include "machlog/network.hpp"
#include "machlog/messbrok.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/levels.hpp"
#include "machphys/aggrdata.hpp"
#include "machphys/admndata.hpp"
#include "machphys/ctordata.hpp"
#include "machphys/glocdata.hpp"
#include "machphys/slocdata.hpp"
#include "machphys/rcardata.hpp"
#include "machphys/apcdata.hpp"
#include "machphys/techdata.hpp"

PER_DEFINE_PERSISTENT( MachLogResearchItem );

MachLogResearchItem::MachLogResearchItem( MachLog::ObjectType objectType,
					int subType ,
					MachLogMachine::Level hwLevel,
					MachLogMachine::Level swLevel,
					int techLevel,
					MachPhys::ResearchUnits researchCost ,
					MachPhys::BuildingMaterialUnits buildingCost ,
					MachLogResearchItem* pParentDependancy,
					MachPhys::HardwareLabSubType hardwareLabSubType,
					MachPhys::WeaponCombo wc )
 :	pImpl_( _NEW( MachLogResearchItemImpl() ) )
 {
	CB_MachLogResearchItem_DEPIMPL();
	
	// Initialise data
	objectType_ = objectType;
	subType_ = subType;
	hwLevel_ = hwLevel;
	technologyLevel_ = techLevel;
	researchCost_ = researchCost;
	buildingCost_ = buildingCost;
	pParentDependancy_ = pParentDependancy;
	hardwareLabSubType_ = hardwareLabSubType;
	weaponCombo_ = wc;
	factoryInstanceCost_ = 0;

	ASSERT_INFO( hardwareLabSubType_ );
	ASSERT( hardwareLabSubType_ == MachPhys::LAB_CIVILIAN or
			hardwareLabSubType_ == MachPhys::LAB_MILITARY,
			"Invalid lab type\n" );
	HAL_STREAM("MachLogResearchItem  " << (void*)this << " CTOR [" << objectType << "," << subType << "," << hwLevel << "," << swLevel << "]" << std::endl );
	for(int i = 0; i < N_RESEARCH_RACES; ++i)
	{
		swLevel_[ i ] = swLevel;
		available_[ i ] = false;
		researched_[ i ] = false;
		amountResearched_[ i ] = 0;
		amountBuilt_[ i ] = 0;
		for( int j = 0; j < MAX_SW_LEVELS; ++j )
		{
			swAvailable_[ i][ j ] = false;
			swResearched_[ i][ j ] = false;
			swAmountResearched_[ i][ j ] = 0;
			swAmountBuilt_[ i][ j ] = 0;
		}
	}
	if( wc != MachPhys::N_WEAPON_COMBOS )
		hasWeaponCombo_ = true;

	switch( objectType )
	{
		case MachLog::AGGRESSOR :
//			if( MachPhysLevels::instance().levelValid( (MachPhys::AggressorSubType)subType, hwLevel, swLevel ) )
				factoryInstanceCost_ = MachPhysData::instance().aggressorData( (MachPhys::AggressorSubType)subType, hwLevel, swLevel ).cost();
		break;
		case MachLog::ADMINISTRATOR :
//			if( MachPhysLevels::instance().levelValid( (MachPhys::AdministratorSubType)subType, hwLevel, swLevel ) )
				factoryInstanceCost_ = MachPhysData::instance().administratorData( (MachPhys::AdministratorSubType)subType, hwLevel, swLevel ).cost();
		break;
		case MachLog::RESOURCE_CARRIER :
//			if( MachPhysLevels::instance().levelValid( mapToMachPhysMachine( objectType ), hwLevel, swLevel ) )
				factoryInstanceCost_ = MachPhysData::instance().resourceCarrierData( hwLevel, swLevel ).cost();
		break;
		case MachLog::APC :
//			if( MachPhysLevels::instance().levelValid( mapToMachPhysMachine( objectType ), hwLevel, swLevel ) )
				factoryInstanceCost_ = MachPhysData::instance().APCData( hwLevel, swLevel ).cost();
		break;
		case MachLog::SPY_LOCATOR :
//			if( MachPhysLevels::instance().levelValid( mapToMachPhysMachine( objectType ), hwLevel, swLevel ) )
				factoryInstanceCost_ = MachPhysData::instance().spyLocatorData( hwLevel, swLevel ).cost();
		break;
		case MachLog::CONSTRUCTOR :
//			if( MachPhysLevels::instance().levelValid( (MachPhys::ConstructorSubType)subType, hwLevel, swLevel ) )
				factoryInstanceCost_ = MachPhysData::instance().constructorData( (MachPhys::ConstructorSubType)subType, hwLevel, swLevel ).cost();
		break;
		case MachLog::GEO_LOCATOR :
//			if( MachPhysLevels::instance().levelValid( mapToMachPhysMachine( objectType ), hwLevel, swLevel ) )
				factoryInstanceCost_ = MachPhysData::instance().geoLocatorData( hwLevel, swLevel ).cost();
		break;
		case MachLog::TECHNICIAN :
//			if( MachPhysLevels::instance().levelValid( (MachPhys::TechnicianSubType)subType, hwLevel, swLevel ) )
				factoryInstanceCost_ = MachPhysData::instance().technicianData( (MachPhys::TechnicianSubType)subType, hwLevel, swLevel ).cost();
		break;
		default:
			;	//we don't need to do anything for things not machines
	}
}

MachLogResearchItem::~MachLogResearchItem()
{
	_DELETE( pImpl_ );
}

const MachLog::ObjectType&
MachLogResearchItem::objectType() const
{
	CB_MachLogResearchItem_DEPIMPL();

	return objectType_;
}

int
MachLogResearchItem::subType() const
{
	CB_MachLogResearchItem_DEPIMPL();

	return subType_;
}

const MachLogMachine::Level&
MachLogResearchItem::hwLevel() const
{
	CB_MachLogResearchItem_DEPIMPL();

	return hwLevel_;
}

const MachPhys::WeaponCombo			
MachLogResearchItem::weaponCombo() const
{
	CB_MachLogResearchItem_DEPIMPL();

	return weaponCombo_;
}

bool								
MachLogResearchItem::hasWeaponCombo() const
{
	CB_MachLogResearchItem_DEPIMPL();

	return hasWeaponCombo_;
}


const MachLogMachine::Level&
MachLogResearchItem::swLevel( MachPhys::Race race ) const
{
	CB_MachLogResearchItem_DEPIMPL();

	HAL_STREAM("MachLogResearchItem " << (void*)this << " [" << objectType_ << "," << subType_ << "," << hwLevel_ << "] swLevel for " << race << " is " << swLevel_[ race ] << std::endl );

	return swLevel_[ race ];
}

void MachLogResearchItem::swLevel( MachPhys::Race race, int level )
{
	CB_MachLogResearchItem_DEPIMPL();

	HAL_STREAM("MachLogResearchItem [" << objectType_ << "," << subType_ << "," << hwLevel_ << "] swLevel for " << race << " SET TO " << level << std::endl );
	swLevel_[ race ] = level;
}

bool MachLogResearchItem::hasParentDependancy() const
{
	CB_MachLogResearchItem_DEPIMPL();

	return pParentDependancy_ != NULL;
}

MachLogResearchItem&
MachLogResearchItem::parentDependancy() const
{
	CB_MachLogResearchItem_DEPIMPL();

	ASSERT( hasParentDependancy() ,"Oops no parent dependacy available.\n" );
	return *pParentDependancy_;
}

int
MachLogResearchItem::technologyLevel() const
{
	CB_MachLogResearchItem_DEPIMPL();

	return technologyLevel_;
}

const MachPhys::ResearchUnits&
MachLogResearchItem::researchCost() const
{
	CB_MachLogResearchItem_DEPIMPL();

	return researchCost_;
}


void MachLogResearchItem::researchCost(const MachPhys::ResearchUnits& newCost )
{
	CB_MachLogResearchItem_DEPIMPL();

	researchCost_ = newCost;
}

const MachPhys::BuildingMaterialUnits&
MachLogResearchItem::buildingCost() const
{
	CB_MachLogResearchItem_DEPIMPL();

	return buildingCost_;
}


void MachLogResearchItem::buildingCost( const MachPhys::BuildingMaterialUnits& newCost )
{
	CB_MachLogResearchItem_DEPIMPL();

	buildingCost_ = newCost;
}

bool
MachLogResearchItem::available( MachPhys::Race race ) const
{
	CB_MachLogResearchItem_DEPIMPL();

	return available_[ race ];
}

bool
MachLogResearchItem::researched( MachPhys::Race race ) const
{
	CB_MachLogResearchItem_DEPIMPL();

	return researched_[ race ];
}


void MachLogResearchItem::available( MachPhys::Race race, bool value )
{
	CB_MachLogResearchItem_DEPIMPL();

//	HAL_STREAM("MLRI::available " << objectType_ << " sub " << subType_ << " hw " << hwLevel_ << " value " << value << std::endl );
	available_[ race ] = value;
}
void MachLogResearchItem::researched( MachPhys::Race race, bool value )
{
	CB_MachLogResearchItem_DEPIMPL();

//	HAL_STREAM("MLRI::available " << objectType_ << " sub " << subType_ << " hw " << hwLevel_ << " value " << value << std::endl );
	researched_[ race ] = value;
}
MachPhys::ResearchUnits MachLogResearchItem::amountResearched( MachPhys::Race race )
{
	CB_MachLogResearchItem_DEPIMPL();

	return amountResearched_[ race ];
}

void MachLogResearchItem::advanceResearch( MachPhys::Race race , MachPhys::ResearchUnits rAmount )
{
	CB_MachLogResearchItem_DEPIMPL();

	amountResearched_[ race ] += rAmount;
	amountBuilt_[ race ] += costToAddResearch( rAmount );

	if( amountResearched_[ race ] >= researchCost_ )
	{
		amountResearched_[ race ] = researchCost_;
		researched_[race] = true;

		if( MachLogNetwork::instance().isNetworkGame() )
			MachLogNetwork::instance().messageBroker().sendNewResearchMessage( objectType_, subType_, hwLevel_, weaponCombo_, race );
		//I have just researched a new obtyoe/subtype/hwlevel combo
		//ensure that at least the first level of software becomes active for free.
		//unfortunately the software level is only cached in the first occ..so return first hwlevel for type and subtype
		MachLogRaces::instance().cascadeUpdateForResearch( race );
	}
}


MachPhys::BuildingMaterialUnits MachLogResearchItem::amountBuilt( MachPhys::Race race )
{
	CB_MachLogResearchItem_DEPIMPL();

	return amountBuilt_[ race ];
}

//////////////////////// SW FUNCTIONS ///////////////////////////////
int	MachLogResearchItem::swTechnologyLevel( int level ) const
{
	CB_MachLogResearchItem_DEPIMPL();

	ASSERT_INFO( level );
	ASSERT( level > 0 and level < MAX_SW_LEVELS + 1,"Unknown level MLResearchItem::sw TechnoloyLevel\n" );
	return swTechnologyLevel_[ level -1 ];
}

void MachLogResearchItem::swTechnologyLevel( int level, int newValue )
{
	CB_MachLogResearchItem_DEPIMPL();

	ASSERT_INFO( level );
	ASSERT( level > 0 and level < MAX_SW_LEVELS + 1,"Unknown level in SET MLResearchItem::sw TechnoloyLevel\n" );
	swTechnologyLevel_[ level -1 ] = newValue;
}

const MachPhys::ResearchUnits&
MachLogResearchItem::swResearchCost( int level ) const
{
	CB_MachLogResearchItem_DEPIMPL();

	ASSERT_INFO( level );
	ASSERT( level > 0 and level < MAX_SW_LEVELS + 1,"Unknown level MLResearchItem::sw ResearchCost\n" );
	return swResearchCost_[ level - 1];
}

void MachLogResearchItem::swResearchCost( int level, MachPhys::ResearchUnits value )
{
	CB_MachLogResearchItem_DEPIMPL();

	ASSERT_INFO( level );
	ASSERT( level > 0 and level < MAX_SW_LEVELS + 1,"Unknown level SET MLResearchItem::sw ResearchCost\n" );
	swResearchCost_[ level - 1] = value;
}

const MachPhys::BuildingMaterialUnits& MachLogResearchItem::swBuildingCost( int level ) const
{
	CB_MachLogResearchItem_DEPIMPL();

	ASSERT_INFO( level );
	ASSERT( level > 0 and level < MAX_SW_LEVELS + 1,"Unknown level MLResearchItem::sw builidngcost\n" );
	return swBuildingCost_[ level - 1 ];
}

void MachLogResearchItem::swBuildingCost( int level, MachPhys::BuildingMaterialUnits value )
{
	CB_MachLogResearchItem_DEPIMPL();

	ASSERT_INFO( level );
	ASSERT( level > 0 and level < MAX_SW_LEVELS + 1,"Unknown level SET MLResearchItem::sw builidngcost\n" );
	swBuildingCost_[ level - 1 ] = value;
}

bool
MachLogResearchItem::swAvailable( MachPhys::Race race, int level ) const
{
	CB_MachLogResearchItem_DEPIMPL();

	ASSERT_INFO( level );
	ASSERT( level > 0 and level < MAX_SW_LEVELS + 1,"Unknown level MLResearchItem::sw Avavailae\n" );
	return swAvailable_[ race ][ level -1 ];
}

bool
MachLogResearchItem::swResearched( MachPhys::Race race, int level ) const
{
	CB_MachLogResearchItem_DEPIMPL();

	ASSERT_INFO( level );
	ASSERT( level > 0 and level < MAX_SW_LEVELS + 1,"Unknown level MLResearchItem::sw Researched\n" );
	return swResearched_[ race ][ level - 1 ];
}


void MachLogResearchItem::swAvailable( MachPhys::Race race, int level, bool value )
{
	CB_MachLogResearchItem_DEPIMPL();

	ASSERT_INFO( level );
	ASSERT( level > 0 and level < MAX_SW_LEVELS + 1,"Unknown level MLResearchItem::sw Avaialble\n" );
	swAvailable_[ race ][ level -1 ] = value;
}

void MachLogResearchItem::swResearched( MachPhys::Race race, int level, bool value )
{
	CB_MachLogResearchItem_DEPIMPL();

	ASSERT_INFO( level );
	ASSERT( level > 0 and level < MAX_SW_LEVELS + 1,"Unknown level MLResearchItem::sw Researched\n" );
	swResearched_[ race ][ level - 1] = value;
	if( level > swLevel( race ) )
		swLevel( race, level );
}

MachPhys::ResearchUnits MachLogResearchItem::swAmountResearched( MachPhys::Race race, int level )
{
	CB_MachLogResearchItem_DEPIMPL();

	ASSERT_INFO( level );
	ASSERT( level > 0 and level < MAX_SW_LEVELS + 1,"Unknown level MLResearchItem::sw AmountResearched\n" );
	return swAmountResearched_[ race ][level-1];
}

void MachLogResearchItem::swAdvanceResearch( MachPhys::Race race , int level, MachPhys::ResearchUnits rAmount, MachPhys::BuildingMaterialUnits bAmount)
{
	CB_MachLogResearchItem_DEPIMPL();

	ASSERT_INFO( level );
	ASSERT( level > 0 and level < MAX_SW_LEVELS + 1,"Unknown level MLResearchItem::sw AmountBuilt\n" );
	swAmountResearched_[ race ][ level -1 ] += rAmount;
	swAmountBuilt_[ race ][ level - 1 ] += bAmount;
	if( swAmountResearched_[ race ][ level -1 ] >= swResearchCost_[ level -1] and
		swAmountBuilt_[ race ][ level - 1 ] >= swBuildingCost_[ level - 1] )
		{
			swResearched_[race][level-1] = true;
			//update currently highest sw level available
			swLevel_[race] = level - 1;
			MachLogRaces::instance().cascadeUpdateForResearch( race );
		}
}


MachPhys::BuildingMaterialUnits MachLogResearchItem::swAmountBuilt( MachPhys::Race race, int level )
{
	CB_MachLogResearchItem_DEPIMPL();

	ASSERT_INFO( level );
	ASSERT( level > 0 and level < MAX_SW_LEVELS + 1,"Unknown level MLResearchItem::sw AmountBuilt\n" );
	return swAmountBuilt_[ race][level - 1 ];
}

void MachLogResearchItem::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

MachPhys::HardwareLabSubType MachLogResearchItem::hardwareLabSubType() const
{
	CB_MachLogResearchItem_DEPIMPL();

	return hardwareLabSubType_;
}

MachLogResearchItem* MachLogResearchItem::pParentDependancy() const
{
	CB_MachLogResearchItem_DEPIMPL();

	return pParentDependancy_;
}
	
MachPhys::ResearchUnits MachLogResearchItem::amountResearched( MachPhys::Race race ) const
{
	CB_MachLogResearchItem_DEPIMPL();

	return amountResearched_[ race ];
}

MachPhys::BuildingMaterialUnits MachLogResearchItem::amountBuilt( MachPhys::Race race ) const
{
	CB_MachLogResearchItem_DEPIMPL();

	return amountBuilt_[ race ];
}

MachPhys::ResearchUnits MachLogResearchItem::swAmountResearched( MachPhys::Race race, int level ) const
{
	CB_MachLogResearchItem_DEPIMPL();

	ASSERT_INFO( level );
	ASSERT( level > 0 and level < MAX_SW_LEVELS + 1,"Unknown level MLResearchItem::sw AmountResearched\n" );
	return swAmountResearched_[ race ][level-1];
}

MachPhys::BuildingMaterialUnits MachLogResearchItem::swAmountBuilt( MachPhys::Race race, int level ) const
{
	CB_MachLogResearchItem_DEPIMPL();

	ASSERT_INFO( level );
	ASSERT( level > 0 and level < MAX_SW_LEVELS + 1,"Unknown level MLResearchItem::sw AmountBuilt\n" );
	return swAmountBuilt_[ race][level - 1 ];
}

ostream& operator <<( ostream& o, const MachLogResearchItem& t )
{

    o << "MachLogResearchItem " << (void*)&t << " start" << std::endl;
	o << " ObjectType " << t.objectType() << std::endl;
	o << " subType " << t.subType() << std::endl;
	o << " hwLevel " << t.hwLevel() << std::endl;
	o << " weapon combo " << t.weaponCombo() << " hasWC defined " << t.hasWeaponCombo() << std::endl;
	o << " technology level " << t.technologyLevel() << std::endl;
	o << " research cost " << t.researchCost() << std::endl;
	o << " building cost " << t.buildingCost() << std::endl;
	o << " lab type " << t.hardwareLabSubType() << std::endl;
	o << " pParentDependancy ";
	if( t.pParentDependancy() )
		o << t.parentDependancy().objectType() << " " << t.parentDependancy().subType() << " " << t.parentDependancy().hwLevel() << std::endl;
	else
		o << "NULL\n";
	for( int i = 0; i < MachLogResearchItem::N_RESEARCH_RACES; ++i )
	{
		o << " swLevel[" << i << "] " << t.swLevel( (MachPhys::Race) i ) << std::endl;
		o << " researched[" << i << "] " << t.researched( (MachPhys::Race) i ) << std::endl;
		o << " available[" << i << "] " << t.available( (MachPhys::Race) i ) << std::endl;
		o << " amount researched[" << i << "] " << t.amountResearched( (MachPhys::Race) i ) << std::endl;
		o << " amount built[" << i << "] " << t.amountBuilt( (MachPhys::Race) i ) << std::endl;
	}
	for( int i = 0; i < MachLogResearchItem::N_RESEARCH_RACES; ++i )
	{
		for( int j = 0; j < MachLogResearchItem::MAX_SW_LEVELS; ++j )
		{
			o << " sw research cost " << t.swResearchCost( j ) << std::endl;
			o << " sw building cost " << t.swBuildingCost( j ) << std::endl;
			o << " swResearched[" << i << "," << j << "] " << t.swResearched( (MachPhys::Race) i, j ) << std::endl;
			o << " swAvailable[" << i << "," << j << "] " << t.swAvailable( (MachPhys::Race)i, j ) << std::endl;
			o << " swAmount researched[" << i << "," << j << "] " << t.swAmountResearched( (MachPhys::Race)i, j ) << std::endl;
			o << " swAmount built[" << i << "," << j << "] " << t.swAmountBuilt( (MachPhys::Race)i, j ) << std::endl;
		}
	}
    o << "MachLogResearchItem " << (void*)&t << " end" << std::endl;

    return o;
}

MachPhys::BuildingMaterialUnits MachLogResearchItem::factoryInstanceCost() const
{
	CB_MachLogResearchItem_DEPIMPL();

	return factoryInstanceCost_;
}

MachPhys::MachineType MachLogResearchItem::mapToMachPhysMachine( MachLog::ObjectType ot ) const
{
	CB_MachLogResearchItem_DEPIMPL();

	switch( ot )
	{
		case MachLog::SPY_LOCATOR:
			return MachPhys::SPY_LOCATOR;
		case MachLog::RESOURCE_CARRIER:
			return MachPhys::RESOURCE_CARRIER;
		case MachLog::APC:
			return MachPhys::APC;
		case MachLog::GEO_LOCATOR:
			return MachPhys::GEO_LOCATOR;
		DEFAULT_ASSERT_BAD_CASE( ot );
	}
	return MachPhys::AGGRESSOR;
}

MachPhys::BuildingMaterialUnits MachLogResearchItem::costToAddResearch( MachPhys::ResearchUnits researchToAdd ) const
{
	CB_MachLogResearchItem_DEPIMPL();

	ASSERT_INFO( objectType_ );
	ASSERT_INFO( subType_ );
	ASSERT_INFO( hwLevel_ );
	ASSERT_INFO( buildingCost_ );
	ASSERT_INFO( researchToAdd );
	ASSERT_INFO( researchCost_ );

	return _STATIC_CAST( MachPhys::BuildingMaterialUnits, ( buildingCost_ * _STATIC_CAST( float, researchToAdd ) / _STATIC_CAST( float, researchCost_ ) ) );
}

void perWrite( PerOstream& ostr, const MachLogResearchItem& resItem )
{	
	ostr << resItem.pImpl_;
}

void perRead( PerIstream& istr, MachLogResearchItem& resItem )
{
	istr >> resItem.pImpl_;
}

MachLogResearchItem::MachLogResearchItem( PerConstructor )
{
}

void MachLogResearchItem::factoryInstanceCost( MachPhys::BuildingMaterialUnits newCost )
{
	CB_MachLogResearchItem_DEPIMPL();
	factoryInstanceCost_ = newCost;
}

/* End RESITEM.CPP **************************************************/
