/*
 * M A P A R E A I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/internal/mapareai.hpp"

MachGuiMapAreaImpl::MachGuiMapAreaImpl()
:	pMapAreaMagic_( NULL )
{

    TEST_INVARIANT;
}

MachGuiMapAreaImpl::~MachGuiMapAreaImpl()
{
    TEST_INVARIANT;

}

void MachGuiMapAreaImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiMapAreaImpl& t )
{

    o << "MachGuiMapAreaImpl " << (void*)&t << " start" << std::endl;
    o << "MachGuiMapAreaImpl " << (void*)&t << " end" << std::endl;

    return o;
}

/* End MAPAREAI.CPP *************************************************/
