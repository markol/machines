/*
 * M A C H D A T I . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/internal/machdati.hpp"

#ifndef _INLINE
    #include "machphys/internal/machdati.ipp"
#endif

MachPhysIMachineData::MachPhysIMachineData()
{

    TEST_INVARIANT;
}

MachPhysIMachineData::~MachPhysIMachineData()
{
    TEST_INVARIANT;

}

void MachPhysIMachineData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysIMachineData& t )
{

    o << "MachPhysIMachineData " << (void*)&t << " start" << std::endl;
    o << "MachPhysIMachineData " << (void*)&t << " end" << std::endl;

    return o;
}

/* End MACHDATI.CPP *************************************************/
