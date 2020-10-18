/*
 * R E S I T E M I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/internal/resitemi.hpp"

PER_DEFINE_PERSISTENT( MachLogResearchItemImpl )

MachLogResearchItemImpl::MachLogResearchItemImpl()
{

    TEST_INVARIANT;
}

MachLogResearchItemImpl::~MachLogResearchItemImpl()
{
    TEST_INVARIANT;

}

void MachLogResearchItemImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogResearchItemImpl& t )
{

    o << "MachLogResearchItemImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogResearchItemImpl " << (void*)&t << " end" << std::endl;

    return o;
}


void perWrite( PerOstream& ostr, const MachLogResearchItemImpl& resItem )
{	
	ostr << resItem.objectType_;
	ostr << resItem.subType_;
	ostr << resItem.hwLevel_;
	ostr << resItem.technologyLevel_;
	ostr << resItem.researchCost_;
	ostr << resItem.buildingCost_;
	ostr << resItem.factoryInstanceCost_;
	ostr << resItem.pParentDependancy_;
	ostr << resItem.hardwareLabSubType_;
	ostr << resItem.weaponCombo_;
	ostr << resItem.hasWeaponCombo_;
	for( int i = MachPhys::RED; i < MachLogResearchItem::N_RESEARCH_RACES; ++i )
	{
		ostr << resItem.swLevel_[ i ];
		ostr << resItem.available_[ i ];
		ostr << resItem.researched_[ i ];
		ostr << resItem.amountResearched_[ i ];
		ostr << resItem.amountBuilt_[ i ];
		for( int j = 0; j < MachLogResearchItem::MAX_SW_LEVELS; ++j )
		{
			ostr << resItem.swAvailable_[ i ][ j ];
			ostr << resItem.swResearched_[ i ][ j ];
			ostr << resItem.swAmountResearched_[ i ][ j ];
			ostr << resItem.swAmountBuilt_[ i ][ j ];
		}
	}
	for( int j = 0; j < MachLogResearchItem::MAX_SW_LEVELS; ++j )
	{
		ostr << resItem.swTechnologyLevel_[ j ];
		ostr << resItem.swResearchCost_[ j ];
		ostr << resItem.swBuildingCost_[ j ];
	}
}

void perRead( PerIstream& istr, MachLogResearchItemImpl& resItem )
{
	istr >> resItem.objectType_;
	istr >> resItem.subType_;
	istr >> resItem.hwLevel_;
	istr >> resItem.technologyLevel_;
	istr >> resItem.researchCost_;
	istr >> resItem.buildingCost_;
	istr >> resItem.factoryInstanceCost_;
	istr >> resItem.pParentDependancy_;
	istr >> resItem.hardwareLabSubType_;
	istr >> resItem.weaponCombo_;
	istr >> resItem.hasWeaponCombo_;
	for( int i = MachPhys::RED; i < MachLogResearchItem::N_RESEARCH_RACES; ++i )
	{
		istr >> resItem.swLevel_[ i ];
		istr >> resItem.available_[ i ];
		istr >> resItem.researched_[ i ];
		istr >> resItem.amountResearched_[ i ];
		istr >> resItem.amountBuilt_[ i ];
		for( int j = 0; j < MachLogResearchItem::MAX_SW_LEVELS; ++j )
		{
			istr >> resItem.swAvailable_[ i ][ j ];
			istr >> resItem.swResearched_[ i ][ j ];
			istr >> resItem.swAmountResearched_[ i ][ j ];
			istr >> resItem.swAmountBuilt_[ i ][ j ];
		}
	}
	for( int j = 0; j < MachLogResearchItem::MAX_SW_LEVELS; ++j )
	{
		istr >> resItem.swTechnologyLevel_[ j ];
		istr >> resItem.swResearchCost_[ j ];
		istr >> resItem.swBuildingCost_[ j ];
	}
	HAL_STREAM("perRead::MachLogResearchItemImpl " << resItem.objectType_ << " " << resItem.hwLevel_ << " " << resItem.weaponCombo_ << std::endl );
}

MachLogResearchItemImpl::MachLogResearchItemImpl( PerConstructor )
{
}

/* End RESITEMI.CPP *************************************************/
