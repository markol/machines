/*
 * A P C . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/apcdata.hpp"

MachPhysAPCData::MachPhysAPCData()
{

    TEST_INVARIANT;
}

MachPhysAPCData::~MachPhysAPCData()
{
    TEST_INVARIANT;

}

MachPhys::SizeUnits MachPhysAPCData::capacity() const
{
	return capacity_;
}

void MachPhysAPCData::capacity( const MachPhys::SizeUnits& newCapacity )
{
	capacity_ = newCapacity;
}

void MachPhysAPCData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysAPCData& t )
{
    //o << (MachPhysObjectData)t;
	//o << (MachPhysMachineData)t;
	o << "Capacity " << t.capacity() << std::endl;

    return o;
}

/* End APCDATA.CPP *************************************************/
