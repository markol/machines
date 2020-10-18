/*
 * D B P L A N E I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/internal/dbplanei.hpp"
#include "machgui/dbsystem.hpp"
#include "machgui/dbscenar.hpp"

PER_DEFINE_PERSISTENT( MachGuiDbIPlanet );

MachGuiDbIPlanet::MachGuiDbIPlanet()
:   pSystem_( NULL )
{

    scenarios_.reserve( 4 );

    TEST_INVARIANT;
}

MachGuiDbIPlanet::~MachGuiDbIPlanet()
{
    TEST_INVARIANT;

}

void MachGuiDbIPlanet::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiDbIPlanet& t )
{

    o << "MachGuiDbIPlanet " << (void*)&t << " start" << std::endl;
    o << "MachGuiDbIPlanet " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachGuiDbIPlanet& ob )
{
    ostr << ob.pSystem_;
    ostr << ob.campaignPicture_;
    ostr << ob.scenarios_;
}

void perRead( PerIstream& istr, MachGuiDbIPlanet& ob )
{
    istr >> ob.pSystem_;
    istr >> ob.campaignPicture_;
    istr >> ob.scenarios_;
}
/* End DBPLANEI.CPP *************************************************/
