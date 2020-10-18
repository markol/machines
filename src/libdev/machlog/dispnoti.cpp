/*
 * D I S P N O T I . C P P 
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/dispnoti.hpp"

MachLogDispositionChangeNotifiable::MachLogDispositionChangeNotifiable()
{

    TEST_INVARIANT;
}

MachLogDispositionChangeNotifiable::~MachLogDispositionChangeNotifiable()
{
    TEST_INVARIANT;

}

void MachLogDispositionChangeNotifiable::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogDispositionChangeNotifiable& t )
{

    o << "MachLogDispositionChangeNotifiable " << (void*)&t << " start" << std::endl;
    o << "MachLogDispositionChangeNotifiable " << (void*)&t << " end" << std::endl;

    return o;
}

/* End DISPNOTI.CPP *************************************************/
