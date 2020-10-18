/*
 * C T O R D A T A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/ctordata.hpp"

MachPhysConstructorData::MachPhysConstructorData()
{

    TEST_INVARIANT;
}

MachPhysConstructorData::~MachPhysConstructorData()
{
    TEST_INVARIANT;

}

MachPhys::SizeUnits MachPhysConstructorData::capacity( void ) const
{
	return capacity_;
}

void MachPhysConstructorData::capacity( const MachPhys::SizeUnits& newCapacity)
{
	capacity_ = newCapacity;
}

MachPhys::BuildingMaterialUnits MachPhysConstructorData::constructionRate( void ) const
{
	return constructionRate_;
}

void MachPhysConstructorData::constructionRate( const MachPhys::BuildingMaterialUnits& newConstructionRate )
{
	constructionRate_ = newConstructionRate;
}

void MachPhysConstructorData::mostAdvancedConstructionType(MachPhys::ConstructionType constructionType)
{
	mostAdvancedConstructionType_ = constructionType;
}

MachPhys::ConstructionType MachPhysConstructorData::mostAdvancedConstructionType() const
{
	return mostAdvancedConstructionType_;
}

void MachPhysConstructorData::mostAdvancedSubType(int subType)
{
	 mostAdvancedSubType_ = subType;
}

int MachPhysConstructorData::mostAdvancedSubType() const
{
	 return mostAdvancedSubType_;
}

void MachPhysConstructorData::mostAdvancedConstructionLevel(size_t constructionLevel)
{
	 mostAdvancedConstructionLevel_ = constructionLevel;
}

size_t MachPhysConstructorData::mostAdvancedConstructionLevel() const
{
	 return mostAdvancedConstructionLevel_;
}


void MachPhysConstructorData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysConstructorData& t )
{

    //o << (MachPhysObjectData)t;
	//o << (MachPhysMachineData)t;
	o << "Capacity " << t.capacity() << std::endl;
	o << "Construct Rate " << t.constructionRate() << std::endl;

    return o;
}

/* End CTORDATA.CPP *************************************************/
