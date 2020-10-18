/*
 * L S G E X T R A . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/lsgextra.hpp"

MachLogLoadSaveGameExtras::MachLogLoadSaveGameExtras()
{

    TEST_INVARIANT;
}

MachLogLoadSaveGameExtras::~MachLogLoadSaveGameExtras()
{
    TEST_INVARIANT;

}

void MachLogLoadSaveGameExtras::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogLoadSaveGameExtras& t )
{

    o << "MachLogLoadSaveGameExtras " << (void*)&t << " start" << std::endl;
    o << "MachLogLoadSaveGameExtras " << (void*)&t << " end" << std::endl;

    return o;
}

/* End LSGEXTRA.CPP *************************************************/
