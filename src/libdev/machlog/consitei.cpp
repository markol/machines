/*
 * C O N S I T E I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/internal/consitei.hpp"

PER_DEFINE_PERSISTENT( MachLogConstructionItemImpl )

MachLogConstructionItemImpl::MachLogConstructionItemImpl()
{

    TEST_INVARIANT;
}

MachLogConstructionItemImpl::~MachLogConstructionItemImpl()
{
    TEST_INVARIANT;

}

void MachLogConstructionItemImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogConstructionItemImpl& t )
{

    o << "MachLogConstructionItemImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogConstructionItemImpl " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogConstructionItemImpl& consItem )
{	
	ostr << consItem.constructionType_;
	ostr << consItem.subType_;
	ostr << consItem.hwLevel_;
	ostr << consItem.weaponCombo_;
	ostr << consItem.hasWeaponCombo_;
	for( int i = MachPhys::RED; i < MachPhys::N_RACES; ++i )
	{
		ostr << consItem.activated_[ i ];
		ostr << consItem.activationLocked_[ i ];
	}
	ostr << consItem.buildingCost_;
}

void perRead( PerIstream& istr, MachLogConstructionItemImpl& consItem )
{

	istr >> consItem.constructionType_;
	istr >> consItem.subType_;
	istr >> consItem.hwLevel_;
	istr >> consItem.weaponCombo_;
	istr >> consItem.hasWeaponCombo_;
	for( int i = MachPhys::RED; i < MachPhys::N_RACES; ++i )
	{
		istr >> consItem.activated_[ i ];
		istr >> consItem.activationLocked_[ i ];
	}
	istr >> consItem.buildingCost_;

}

MachLogConstructionItemImpl::MachLogConstructionItemImpl( PerConstructor )
{
}

/* End CONSITEI.CPP *************************************************/
