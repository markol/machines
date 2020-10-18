/*
 * O R E H D A T A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/orehdata.hpp"

MachPhysOreHolographData::MachPhysOreHolographData()
{
    TEST_INVARIANT;
}

MachPhysOreHolographData::~MachPhysOreHolographData()
{
    TEST_INVARIANT;

}

void MachPhysOreHolographData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}


ostream& operator <<( ostream& o, const MachPhysOreHolographData& t )
{

    o << "OreHolograph is empty " << std::endl;

    return o;
}

/* End OREHDATA.CPP *************************************************/
