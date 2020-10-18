/*
 * P R O D U N I T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/produnit.hpp"

const int MachLogProductionUnit::UNCHECKED_PRIORITY = -1;
PER_DEFINE_PERSISTENT( MachLogProductionUnit );

EXISTS( MachLogProductionUnit );

MachLogProductionUnit::MachLogProductionUnit()
{
	LOG_CONSTRUCTION;
    TEST_INVARIANT;
}

MachLogProductionUnit::MachLogProductionUnit( MachLog::ObjectType type, int subType, int hwLevel, int swLevel, int priority )
:	type_( type ),
	subType_( subType ),
	hwLevel_( hwLevel ),
	swLevel_( swLevel ),
	priority_( priority ),
	amountBuilt_( 0 ),
	weaponCombo_( MachPhys::N_WEAPON_COMBOS ),
	needRebuild_( false ),
	constructionId_( -1 )
{
	LOG_CONSTRUCTION;
    TEST_INVARIANT;
}

MachLogProductionUnit::MachLogProductionUnit( const MachLogProductionUnit& copyMe )
:	type_( copyMe.type_ ),
	subType_( copyMe.subType_ ),
	hwLevel_( copyMe.hwLevel_ ),
	swLevel_( copyMe.swLevel_ ),
	priority_( copyMe.priority_ ),
	amountBuilt_( 0 ),
	weaponCombo_( copyMe.weaponCombo_ ),
	needRebuild_( copyMe.needRebuild_ ),
	constructionId_( copyMe.constructionId_ )
{
	LOG_CONSTRUCTION;
    TEST_INVARIANT;
}

MachLogProductionUnit::~MachLogProductionUnit()
{
    TEST_INVARIANT;
	LOG_DESTRUCTION;

}

void MachLogProductionUnit::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

MachLog::ObjectType MachLogProductionUnit::type() const
{
	TEST_INVARIANT;
	return type_;
}

void MachLogProductionUnit::type( MachLog::ObjectType newType )
{
	TEST_INVARIANT;
	type_ = newType;
}

int MachLogProductionUnit::subType() const
{
	TEST_INVARIANT;
	return subType_;
}

void MachLogProductionUnit::subType( int newSubType )
{
	TEST_INVARIANT;
	subType_ = newSubType;
}

int MachLogProductionUnit::hwLevel() const
{
	TEST_INVARIANT;
	return hwLevel_;
}

void MachLogProductionUnit::hwLevel( int newHwLevel )
{
	TEST_INVARIANT;
	hwLevel_ = newHwLevel;
}

int MachLogProductionUnit::swLevel() const
{
	TEST_INVARIANT;
	return swLevel_;
}

void MachLogProductionUnit::swLevel( int newSwLevel )
{
	TEST_INVARIANT;
	swLevel_ = newSwLevel;
}

int MachLogProductionUnit::priority() const
{
	TEST_INVARIANT;
	return priority_;
}

void MachLogProductionUnit::priority( int newPriority )
{
	TEST_INVARIANT;
	priority_ = newPriority;
}

const MexTransform3d& MachLogProductionUnit::globalTransform() const
{
	TEST_INVARIANT;
	return globalTransform_;
}

void MachLogProductionUnit::globalTransform( const MexTransform3d& newTransform )
{
	TEST_INVARIANT;
	globalTransform_ = newTransform;
}

MachPhys::WeaponCombo MachLogProductionUnit::weaponCombo() const
{
	TEST_INVARIANT;
	return weaponCombo_;
}
void MachLogProductionUnit::weaponCombo( MachPhys::WeaponCombo newWeaponCombo )
{
	TEST_INVARIANT;
	weaponCombo_ = newWeaponCombo;
}

MachPhys::BuildingMaterialUnits MachLogProductionUnit::amountBuilt() const
{
	TEST_INVARIANT;
	return amountBuilt_;
}

void MachLogProductionUnit::amountBuilt( MachPhys::BuildingMaterialUnits newAmount )
{
	TEST_INVARIANT;
	amountBuilt_ = newAmount;
}

int MachLogProductionUnit::constructionId() const
{
	TEST_INVARIANT;
	return constructionId_;
}

void MachLogProductionUnit::constructionId( int ID )
{
	TEST_INVARIANT;
	constructionId_ = ID;
}

bool MachLogProductionUnit::needRebuild() const
{
	TEST_INVARIANT;
	return needRebuild_;
}

void MachLogProductionUnit::needRebuild( bool rebuildSetting )
{
	TEST_INVARIANT;
	needRebuild_ = rebuildSetting;
}

bool MachLogProductionUnit::operator==( const MachLogProductionUnit& t)
{
	TEST_INVARIANT;
	bool result = type_ == t.type_ and
			subType_ == t.subType_ and
			hwLevel_ == t.hwLevel_ and
			swLevel_ == t.swLevel_ and
			weaponCombo_ == t.weaponCombo_;

	result &= ( priority_ == UNCHECKED_PRIORITY or
		t.priority_ == UNCHECKED_PRIORITY or
		priority_ == t.priority_ );

	return result;
}

ostream& operator <<( ostream& o, const MachLogProductionUnit& t )
{

    o << "MachLogProductionUnit " << (void*)&t << " start" << std::endl;
	o << " Type: " << t.type_ << std::endl;
	o << " subType: " << t.subType_ << std::endl;
	o << " hwLevel: " << t.hwLevel_ << std::endl;
	o << " swLevel: " << t.swLevel_ << std::endl;
	o << " amount: " << t.amountBuilt_ << std::endl;
	o << " priority: " << t.priority_ << std::endl;
	o << " Transform: " << t.globalTransform_ << std::endl;
	o << " Weapon Combo " << t.weaponCombo_ << std::endl;
    o << "MachLogProductionUnit " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogProductionUnit& actor )
{
	ostr << actor.type_;
	ostr << actor.subType_;
	ostr << actor.hwLevel_;
	ostr << actor.swLevel_;
	ostr << actor.priority_;
	ostr << actor.amountBuilt_;
	ostr << actor.globalTransform_;
	ostr << actor.weaponCombo_;
	ostr << actor.constructionId_;
    ostr << actor.needRebuild_;
}

void perRead( PerIstream& istr, MachLogProductionUnit& actor )
{
	istr >> actor.type_;
	istr >> actor.subType_;
	istr >> actor.hwLevel_;
	istr >> actor.swLevel_;
	istr >> actor.priority_;
	istr >> actor.amountBuilt_;
	istr >> actor.globalTransform_;
	istr >> actor.weaponCombo_;
	istr >> actor.constructionId_;
    istr >> actor.needRebuild_;
}

bool operator< ( const MachLogProductionUnit& lhs, const MachLogProductionUnit& rhs )
{
    //Check against relative place in the hierarchy
    return (lhs.type_ < rhs.type_ ) or
             ((lhs.type_ == rhs.type_) and
               ((lhs.subType_ < rhs.subType_) or
                 ((lhs.subType_ == rhs.subType_) and
                   ((lhs.hwLevel_ < rhs.hwLevel_) or
                     ((lhs.hwLevel_ == rhs.hwLevel_) and
                       (lhs.swLevel_ < rhs.swLevel_))))));
}

bool operator> ( const MachLogProductionUnit& lhs, const MachLogProductionUnit& rhs )
{
    //Check against relative place in the hierarchy
    return (lhs.type_ > rhs.type_ ) or
             ((lhs.type_ == rhs.type_) and
               ((lhs.subType_ > rhs.subType_) or
                 ((lhs.subType_ == rhs.subType_) and
                   ((lhs.hwLevel_ > rhs.hwLevel_) or
                     ((lhs.hwLevel_ == rhs.hwLevel_) and
                       (lhs.swLevel_ > rhs.swLevel_))))));
}

/* End PRODUNIT.CPP *************************************************/
