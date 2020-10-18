/*
 * D B P L A Y E R . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/dbplayer.hpp"
#include "machgui/internal/dbplayei.hpp"
#include "machgui/dbplyscn.hpp"

PER_DEFINE_PERSISTENT( MachGuiDbPlayer );

MachGuiDbPlayer::MachGuiDbPlayer( uint id, const string& name )
{
    pData_ = _NEW( MachGuiDbIPlayer );

    pData_->id_= id;
    pData_->name_ = name;

    TEST_INVARIANT;
}

MachGuiDbPlayer::~MachGuiDbPlayer()
{
    TEST_INVARIANT;

    //Delete all the player scenario object
    uint nPlayerScenarios = pData_->playerScenarios_.size();
    for( uint i = nPlayerScenarios; i--; )
    {
        _DELETE( pData_->playerScenarios_[i] );
    }

    _DELETE( pData_ );
}

void MachGuiDbPlayer::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiDbPlayer& t )
{

    o << "MachGuiDbPlayer " << (void*)&t << " start" << std::endl;
    o << "MachGuiDbPlayer " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachGuiDbPlayer& ob )
{
    ostr << ob.pData_;
}

void perRead( PerIstream& istr, MachGuiDbPlayer& ob )
{
    istr >> ob.pData_;
}

MachGuiDbPlayer::MachGuiDbPlayer( PerConstructor )
:   pData_( NULL )
{
}

uint MachGuiDbPlayer::id() const
{
    return pData_->id_;
}

const string& MachGuiDbPlayer::name() const
{
    return pData_->name_;
}

void MachGuiDbPlayer::name( const string& newName )
{
    pData_->name_ = newName;
}

uint MachGuiDbPlayer::nextUpdateId()
{
    return pData_->nextUpdateId_++;
}

bool MachGuiDbPlayer::hasPlayed( const MachGuiDbScenario& scenario, MachGuiDbPlayerScenario** ppPlayerScenario ) const
{
    bool result = false;

    //Find the entry in the collection
    uint nPlayed = pData_->playerScenarios_.size();
    for( uint i = 0; not result  and  i != nPlayed; ++i )
    {
        MachGuiDbPlayerScenario* pPlayerScenario = pData_->playerScenarios_[i];
        if( &(pPlayerScenario->scenario()) == &scenario )
        {
            result = true;
            *ppPlayerScenario = pPlayerScenario;
        }
    }

    return result;
}

MachGuiDbPlayerScenario& MachGuiDbPlayer::playerScenario( MachGuiDbScenario* pDbScenario )
{
    MachGuiDbPlayerScenario* pPlayerScenario;
    if( not hasPlayed( *pDbScenario, &pPlayerScenario ) )
    {
        //This is a new one, so create it
        pPlayerScenario = _NEW( MachGuiDbPlayerScenario( this, pDbScenario ) );
        pData_->playerScenarios_.push_back( pPlayerScenario );
    }

    return *pPlayerScenario;
}

uint MachGuiDbPlayer::nPlayerScenarios() const
{
    return  pData_->playerScenarios_.size();
}

MachGuiDbPlayerScenario& MachGuiDbPlayer::playerScenario( uint index ) const
{
    PRE( index < nPlayerScenarios() );
    return  *(pData_->playerScenarios_[ index ]);
}

MachGuiDbPlayerScenario& MachGuiDbPlayer::mostRecentPlayerScenario()
{
    PRE( nPlayerScenarios() != 0 );

    //Find the entry in the collection with highest update id
    MachGuiDbPlayerScenario* mostRecentPlayerScenario = pData_->playerScenarios_.front();
    uint nPlayed = pData_->playerScenarios_.size();

    for( uint i = 1; i != nPlayed; ++i )
    {
        MachGuiDbPlayerScenario* pPlayerScenario = pData_->playerScenarios_[i];
        if( pPlayerScenario->updateId() > mostRecentPlayerScenario->updateId() )
            mostRecentPlayerScenario = pPlayerScenario;
    }

    return *mostRecentPlayerScenario;
}

uint MachGuiDbPlayer::useSequenceId() const
{
    return pData_->useSequenceId_;
}

void MachGuiDbPlayer::useSequenceId( uint id )
{
    pData_->useSequenceId_ = id;
}

void MachGuiDbPlayer::lastSelectedScenario( MachGuiDbScenario* pScenario )
{
	pData_->pLastSelectedScenario_ = pScenario;
}

MachGuiDbScenario* MachGuiDbPlayer::lastSelectedScenario() const
{
	return pData_->pLastSelectedScenario_;
}

/* End DBPLAYER.CPP *************************************************/
