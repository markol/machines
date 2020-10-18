/*
 * D A T A B A S I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/internal/databasi.hpp"
#include "machgui/dbsystem.hpp"
#include "machgui/dbplayer.hpp"
#include "machgui/dbsavgam.hpp"
#include "machgui/dbhandlr.hpp"

PER_DEFINE_PERSISTENT( MachGuiIDatabase );

MachGuiIDatabase::MachGuiIDatabase()
:   pElementMap_( NULL ),
    nextPlayerId_( 0 ),
    pDbCurrentPlayer_( NULL ),
    nextUseSequenceId_( 0 ),
    pDbHandler_( _NEW( MachGuiDatabaseHandler ) ),
    pCurrentDbScenario_( NULL )
{
    campaignSystems_.reserve( 20 );
    skirmishSystems_.reserve( 3 );
    multiPlayerSystems_.reserve( 3 );
    players_.reserve( 20 );
    savedGames_.reserve( 64 );

    TEST_INVARIANT;
}

MachGuiIDatabase::~MachGuiIDatabase()
{
    TEST_INVARIANT;

    _DELETE( pElementMap_ );
    _DELETE( pDbHandler_ );
}

void MachGuiIDatabase::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiIDatabase& t )
{

    o << "MachGuiIDatabase " << (void*)&t << " start" << std::endl;
    o << "MachGuiIDatabase " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachGuiIDatabase& ob )
{
    ostr << ob.campaignSystems_;
    ostr << ob.skirmishSystems_;
    ostr << ob.multiPlayerSystems_;
    ostr << ob.players_;
    ostr << ob.savedGames_;
    ostr << ob.nextPlayerId_;
    ostr << ob.nextUseSequenceId_;
}

void perRead( PerIstream& istr, MachGuiIDatabase& ob )
{
    istr >> ob.campaignSystems_;
    istr >> ob.skirmishSystems_;
    istr >> ob.multiPlayerSystems_;
    istr >> ob.players_;
    istr >> ob.savedGames_;
    istr >> ob.nextPlayerId_;
    istr >> ob.nextUseSequenceId_;
    ob.pElementMap_ = NULL;
    ob.pDbCurrentPlayer_ = NULL;
}

bool MachGuiIDatabase::PlayerNameComparator::operator ()
(
    const MachGuiDbPlayer_ptr& pLhs, const MachGuiDbPlayer_ptr& pRhs
) const
{
	return pLhs->name() < pRhs->name();
}
/* End DATABASI.CPP *************************************************/
