/*
 * D B P L Y S C I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/internal/dbplysci.hpp"

#include "machphys/machphys.hpp"
#include "machgui/dbscenar.hpp"
#include "machgui/dbplayer.hpp"

PER_DEFINE_PERSISTENT( MachGuiDbIPlayerScenario );

MachGuiDbIPlayerScenario::MachGuiDbIPlayerScenario()
:   pDbScenario_( NULL ),
    pDbPlayer_( NULL ),
    isWon_( false ),
    lastScore_( 0 ),
    nAttempts_( 0 ),
    lastUpdateId_( 0 )
{
    //Initialise the lists of machines left over
    raceUnits_.reserve( MachPhys::N_RACES );
    for( size_t i = 0; i != MachPhys::N_RACES; ++i )
    {
        raceUnits_.push_back( Units() );
        raceUnits_.back().reserve( 2 );
    }

    setFlags_.reserve( 4 );

    TEST_INVARIANT;
}

MachGuiDbIPlayerScenario::~MachGuiDbIPlayerScenario()
{
    TEST_INVARIANT;

}

void MachGuiDbIPlayerScenario::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiDbIPlayerScenario& t )
{

    o << "MachGuiDbIPlayerScenario " << (void*)&t << " start" << std::endl;
    o << "MachGuiDbIPlayerScenario " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachGuiDbIPlayerScenario& ob )
{
    ostr << ob.pDbScenario_;
    ostr << ob.pDbPlayer_;
    ostr << ob.isWon_;
    ostr << ob.lastScore_;
    ostr << ob.nAttempts_;
    ostr << ob.lastUpdateId_;
    ostr << ob.raceUnits_;
    ostr << ob.setFlags_;
}

void perRead( PerIstream& istr, MachGuiDbIPlayerScenario& ob )
{
    istr >> ob.pDbScenario_;
    istr >> ob.pDbPlayer_;
    istr >> ob.isWon_;
    istr >> ob.lastScore_;
    istr >> ob.nAttempts_;
    istr >> ob.lastUpdateId_;
    istr >> ob.raceUnits_;
    istr >> ob.setFlags_;
}

/* End DBPLYSCI.CPP *************************************************/
