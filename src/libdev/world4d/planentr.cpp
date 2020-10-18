/*
 * P L A N E N T R . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/planentr.hpp"

PER_DEFINE_PERSISTENT( W4dCompositePlanEntry );

W4dCompositePlanEntry::W4dCompositePlanEntry( PerConstructor )
{

    TEST_INVARIANT;
}

W4dCompositePlanEntry::~W4dCompositePlanEntry()
{
    TEST_INVARIANT;

}

void perWrite( PerOstream& ostr, const W4dCompositePlanEntry& entry )
{
    ostr << entry.id_;
    ostr << entry.plan_;
}

void perRead( PerIstream& istr, W4dCompositePlanEntry& entry )
{
    istr >> entry.id_;
    istr >> entry.plan_;
}

void W4dCompositePlanEntry::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const W4dCompositePlanEntry& t )
{

    o << "W4dCompositePlanEntry " << (void*)&t << " start" << std::endl;
    o << "W4dCompositePlanEntry " << (void*)&t << " end" << std::endl;

    return o;
}

/* End PLANENTR.CPP *************************************************/
