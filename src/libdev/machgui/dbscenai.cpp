/*
 * D B S C E N A I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/internal/dbscenai.hpp"
#include "machgui/dbplanet.hpp"
#include "machgui/startup.hpp"

PER_DEFINE_PERSISTENT( MachGuiDbIScenario );

MachGuiDbIScenario::MachGuiDbIScenario()
: pPlanet_( NULL ), 
  musicTrack_( MachGuiStartupScreens::DEFAULT_INGAME_MUSIC ),
  hasBeenWon_( false )
{
    saveMachineRaces_.reserve( MachPhys::N_RACES );

    TEST_INVARIANT;
}

MachGuiDbIScenario::~MachGuiDbIScenario()
{
    TEST_INVARIANT;

}

void MachGuiDbIScenario::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiDbIScenario& t )
{

    o << "MachGuiDbIScenario " << (void*)&t << " start" << std::endl;
    o << "MachGuiDbIScenario " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachGuiDbIScenario& ob )
{
    ostr << ob.pPlanet_;
    ostr << ob.planetFile_;
    ostr << ob.campaignPicture_;
    ostr << ob.briefingPicture_;
    ostr << ob.debriefingPicture_;
	ostr << ob.debriefingLosePicture_;
    ostr << ob.entryFlic_;
    ostr << ob.winFlic_;
    ostr << ob.loseFlic_;
	ostr << ob.maxPlayers_;
	ostr << ob.musicTrack_;
	ostr << ob.saveMachineRaces_;
	ostr << ob.hasBeenWon_;
}

void perRead( PerIstream& istr, MachGuiDbIScenario& ob )
{
    istr >> ob.pPlanet_;
    istr >> ob.planetFile_;
    istr >> ob.campaignPicture_;
    istr >> ob.briefingPicture_;
    istr >> ob.debriefingPicture_;
	istr >> ob.debriefingLosePicture_;
    istr >> ob.entryFlic_;
    istr >> ob.winFlic_;
    istr >> ob.loseFlic_;
	istr >> ob.maxPlayers_;
	istr >> ob.musicTrack_;
	istr >> ob.saveMachineRaces_;
	istr >> ob.hasBeenWon_;
}

/* End DBSCENAI.CPP *************************************************/
