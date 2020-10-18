/*
 * S L O C D A T A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/slocdata.hpp"

MachPhysSpyLocatorData::MachPhysSpyLocatorData()
{

    TEST_INVARIANT;
}

MachPhysSpyLocatorData::~MachPhysSpyLocatorData()
{
    TEST_INVARIANT;

}

int MachPhysSpyLocatorData::mineCapacity( void ) const
{
	return mineCapacity_;
}

void MachPhysSpyLocatorData::mineCapacity( int newMineCapacity )
{
	mineCapacity_ = newMineCapacity;
}

MachPhys::ResearchUnits MachPhysSpyLocatorData::downloadRate( void ) const
{
	return downloadRate_;
}

void MachPhysSpyLocatorData::downloadRate (MachPhys::ResearchUnits newDownloadRate )
{
	downloadRate_ = newDownloadRate;
}


void MachPhysSpyLocatorData::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysSpyLocatorData& t )
{

    //o << (MachPhysObjectData)t;
	//o << (MachPhysMachineData)t;
	o << " Mine capacity " << t.mineCapacity_ << std::endl;

    return o;
}

/* End LOCRDATA.CPP *************************************************/
