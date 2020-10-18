/*
 * D B P L Y S C N . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/dbplyscn.hpp"
#include <algorithm>
#include "machgui/internal/dbplysci.hpp"
#include "machgui/dbplayer.hpp"

PER_DEFINE_PERSISTENT( MachGuiDbPlayerScenario );

MachGuiDbPlayerScenario::MachGuiDbPlayerScenario( MachGuiDbPlayer* pDbPlayer, MachGuiDbScenario* pDbScenario )
{
    pData_ = _NEW( MachGuiDbIPlayerScenario );
    pData_->pDbPlayer_ = pDbPlayer;
    pData_->pDbScenario_ = pDbScenario;

    TEST_INVARIANT;
}

MachGuiDbPlayerScenario::~MachGuiDbPlayerScenario()
{
    TEST_INVARIANT;

    _DELETE( pData_ );
}

void MachGuiDbPlayerScenario::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiDbPlayerScenario& t )
{

    o << "MachGuiDbPlayerScenario " << (void*)&t << " start" << std::endl;
    o << "MachGuiDbPlayerScenario " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachGuiDbPlayerScenario& ob )
{
    ostr << ob.pData_;
}

void perRead( PerIstream& istr, MachGuiDbPlayerScenario& ob )
{
    istr >> ob.pData_;
}

MachGuiDbPlayerScenario::MachGuiDbPlayerScenario( PerConstructor )
:   pData_( NULL )
{
}

MachGuiDbScenario& MachGuiDbPlayerScenario::scenario() const
{
    return *(pData_->pDbScenario_);
}

MachGuiDbPlayer& MachGuiDbPlayerScenario::player() const
{
    return *(pData_->pDbPlayer_);
}

bool MachGuiDbPlayerScenario::hasWon() const
{
    return pData_->isWon_;
}

int MachGuiDbPlayerScenario::lastScore() const
{
    return pData_->lastScore_;
}

uint MachGuiDbPlayerScenario::nAttempts() const
{
    return pData_->nAttempts_;
}

void MachGuiDbPlayerScenario::upDate( bool didWin, int score )
{
    //Record if ever won
    if( didWin)
        pData_->isWon_ = true;

    //Save highest score ( note : reason why lastScore_ is a badly
	//named variable is because the usage has changed from lastScore to highestScore )
	if ( nAttempts() == 0 or score > pData_->lastScore_ )
	{
    	pData_->lastScore_ = score;
	}

    //We have another attempt
    pData_->nAttempts_++;

    //Update the sequence number
    pData_->lastUpdateId_ = pData_->pDbPlayer_->nextUpdateId();
}

uint MachGuiDbPlayerScenario::updateId() const
{
    return pData_->lastUpdateId_;
}

void MachGuiDbPlayerScenario::raceSurvivingUnits( MachPhys::Race race, const Units& units )
{
    PRE( race < MachPhys::N_RACES );
    PRE( hasWon() );

    //Find the collection for the race
    MachGuiDbIPlayerScenario::Units& myUnits = pData_->raceUnits_[ race ];

    //erase any existing entries
    myUnits.erase( myUnits.begin(), myUnits.end() );

    //Use the new collection
    myUnits = units;
}

const MachGuiDbPlayerScenario::Units& MachGuiDbPlayerScenario::raceSurvivingUnits( MachPhys::Race race ) const
{
    PRE( race < MachPhys::N_RACES );
    PRE( hasWon() );

    return pData_->raceUnits_[ race ];
}

void MachGuiDbPlayerScenario::setFlag( const string& flag, bool doSet )
{
    //See if the flag is already entered
    MachGuiDbIPlayerScenario::Strings& setFlags = pData_->setFlags_;
    MachGuiDbIPlayerScenario::Strings::iterator it = find( setFlags.begin(), setFlags.end(), flag );
    if( it == setFlags.end() )
    {
        //Not currently entered. If adding, push the string onto the collection
        if( doSet )
            setFlags.push_back( flag );
    }
    else
    {
        //Is already entered. If clearing, erase it
        if( not doSet )
            setFlags.erase( it );
    }
}

bool MachGuiDbPlayerScenario::isFlagSet( const string& flag )
{
    MachGuiDbIPlayerScenario::Strings& setFlags = pData_->setFlags_;

    return find( setFlags.begin(), setFlags.end(), flag ) != setFlags.end();
}

void MachGuiDbPlayerScenario::clearSetFlags()
{
    MachGuiDbIPlayerScenario::Strings& setFlags = pData_->setFlags_;
    setFlags.erase( setFlags.begin(), setFlags.end() );
}

/* End DBPLYSCN.CPP *************************************************/
