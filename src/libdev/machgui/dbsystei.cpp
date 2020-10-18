/*
 * D B S Y S T E I . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/internal/dbsystei.hpp"
#include "machgui/dbplanet.hpp"

PER_DEFINE_PERSISTENT( MachGuiDbISystem );

MachGuiDbISystem::MachGuiDbISystem()
{
    planets_.reserve( 4 );

    TEST_INVARIANT;
}

MachGuiDbISystem::~MachGuiDbISystem()
{
    TEST_INVARIANT;

}

void MachGuiDbISystem::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiDbISystem& t )
{

    o << "MachGuiDbISystem " << (void*)&t << " start" << std::endl;
    o << "MachGuiDbISystem " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachGuiDbISystem& ob )
{
    ostr << ob.campaignPicture_;
    //ostr << ob.planets_;
    // Write a copy without user scenarios
    ctl_vector< MachGuiDbPlanet* > *planets = _NEW( ctl_vector< MachGuiDbPlanet* > );
    for ( MachGuiDbPlanet* planet : ob.planets_ )
        if ( !planet->isCustom() ) planets->push_back( planet );
    ostr << *planets;
    _DELETE( planets );
}

void perRead( PerIstream& istr, MachGuiDbISystem& ob )
{
    istr >> ob.campaignPicture_;
    istr >> ob.planets_;
}
/* End DBSYSTEI.CPP *************************************************/
