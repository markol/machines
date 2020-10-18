/*
 * T R A N D A T A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/rcardata.hpp"

MachPhysResourceCarrierData::MachPhysResourceCarrierData()
{

    TEST_INVARIANT;
}

MachPhysResourceCarrierData::~MachPhysResourceCarrierData()
{
    TEST_INVARIANT;

}

MachPhys::SizeUnits MachPhysResourceCarrierData::capacity() const
{
	return capacity_;
}

void MachPhysResourceCarrierData::capacity( const MachPhys::SizeUnits& newCapacity )
{
	capacity_ = newCapacity;
}

void MachPhysResourceCarrierData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysResourceCarrierData& t )
{
    //o << (MachPhysObjectData)t;
	//o << (MachPhysMachineData)t;
	o << "Capacity " << t.capacity() << std::endl;

    return o;
}

/* End tranDATA.CPP *************************************************/
