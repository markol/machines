/*
 * M A C H D E S C . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/machdesc.hpp"

MachLogMachineDescriptor::MachLogMachineDescriptor()
{

    TEST_INVARIANT;
}

MachLogMachineDescriptor::~MachLogMachineDescriptor()
{
    TEST_INVARIANT;

}

void MachLogMachineDescriptor::CLASS_INVARIANT
{
}

ostream& operator <<( ostream& o, const MachLogMachineDescriptor& t )
{

    o << "MachLogMachineDescriptor " << (void*)&t << " start" << std::endl;
    o << "MachLogMachineDescriptor " << (void*)&t << " end" << std::endl;

    return o;
}

/* End MACHDESC.CPP *************************************************/
