/*
 * M S G B O X E I . C P P
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/msgboxei.hpp"
#include "machgui/startup.hpp"
#include "machgui/strtdata.hpp"
#include "machgui/messbrok.hpp"
#include "machlog/network.hpp"
#include "world4d/soundman.hpp"
#include "system/winapi.hpp"

MachGuiExitToInternetMessageBoxResponder::MachGuiExitToInternetMessageBoxResponder
(
	MachGuiStartupScreens* pStartupScreens,
	MachGuiExitToInternetMessageBoxResponder::UnloadGame unloadGame
)
: pStartupScreens_( pStartupScreens ),
  unloadGame_( unloadGame )
{
}

MachGuiExitToInternetMessageBoxResponder::~MachGuiExitToInternetMessageBoxResponder()
{
    TEST_INVARIANT;

}

void MachGuiExitToInternetMessageBoxResponder::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiExitToInternetMessageBoxResponder& t )
{

    o << "MachGuiExitToInternetMessageBoxResponder " << (void*)&t << " start" << std::endl;
    o << "MachGuiExitToInternetMessageBoxResponder " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachGuiExitToInternetMessageBoxResponder::okPressed()
{
	if( unloadGame_ == UNLOAD_GAME )
	{
		pStartupScreens_->unloadGame();
		W4dSoundManager::instance().unloadAll();
	}
	else
	{
		if ( pStartupScreens_->startupData()->isHost() )
		{
			pStartupScreens_->messageBroker().sendHostCancelMessage();
		}
		else
		{
			pStartupScreens_->messageBroker().sendClientCancelMessage( pStartupScreens_->startupData()->playerName() );
		}
		// Sleep to make sure that message gets sent
		for ( int i = 0; i < 10; ++i )
		{
			//Transmit the termination messages!
			MachLogNetwork::instance().update();
			SysWindowsAPI::sleep( 100 );
			SysWindowsAPI::peekMessage();
		}
	}

	MachLogNetwork::instance().terminateAndReset();
// Disabled for now
//	pStartupScreens_->contextFinishFromLobby();
	return true;
}

/* End MSGBOXEI.CPP *************************************************/
