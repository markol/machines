/*
 * T E C H D A T A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/techdata.hpp"

MachPhysTechnicianData::MachPhysTechnicianData()
{

    TEST_INVARIANT;
}

MachPhysTechnicianData::~MachPhysTechnicianData()
{
    TEST_INVARIANT;

}

void MachPhysTechnicianData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

MachPhys::ResearchUnits MachPhysTechnicianData::researchRate() const
{
	return researchRate_;
}

void MachPhysTechnicianData::researchRate( const MachPhys::ResearchUnits& newResearchRate )
{
	researchRate_ = newResearchRate;
}

ostream& operator <<( ostream& o, const MachPhysTechnicianData& t )
{

    //o << (MachPhysObjectData)t;
	//o << (MachPhysMachineData)t;
	o << "Research Rate " << t.researchRate() << std::endl;

    return o;
}

/* End TECHDATA.CPP *************************************************/
