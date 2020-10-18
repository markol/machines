/*
 * D B P L A Y E I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/internal/dbplayei.hpp"
#include "machgui/dbplyscn.hpp"
#include "machgui/dbscenar.hpp"

PER_DEFINE_PERSISTENT( MachGuiDbIPlayer );

MachGuiDbIPlayer::MachGuiDbIPlayer()
:   id_( 0 ),
    nextUpdateId_( 0 ),
    useSequenceId_( 0 ),
	pLastSelectedScenario_( NULL )
{
    playerScenarios_.reserve( 10 );

    TEST_INVARIANT;
}

MachGuiDbIPlayer::~MachGuiDbIPlayer()
{
    TEST_INVARIANT;

}

void MachGuiDbIPlayer::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiDbIPlayer& t )
{

    o << "MachGuiDbIPlayer " << (void*)&t << " start" << std::endl;
    o << "MachGuiDbIPlayer " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachGuiDbIPlayer& ob )
{
    ostr << ob.id_;
    ostr << ob.name_;
    ostr << ob.nextUpdateId_;
    ostr << ob.playerScenarios_;
    ostr << ob.useSequenceId_;
	ostr << ob.pLastSelectedScenario_;
}

void perRead( PerIstream& istr, MachGuiDbIPlayer& ob )
{
    istr >> ob.id_;
    istr >> ob.name_;
    istr >> ob.nextUpdateId_;
    istr >> ob.playerScenarios_;
    istr >> ob.useSequenceId_;
	istr >> ob.pLastSelectedScenario_;
}
/* End DBPLAYEI.CPP *************************************************/
