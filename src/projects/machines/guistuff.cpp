#include <cstdio>
#include "machines/sdlapp.hpp"
#include "world4d/scenemgr.hpp"
#include "render/device.hpp"
#include "gui/manager.hpp"
#include "machgui/startup.hpp"
#include "machgui/messbrok.hpp"
#include "machgui/strtdata.hpp"
#include "machgui/database.hpp"
#include "machgui/dbplanet.hpp"
#include "machgui/dbscenar.hpp"
#include "machgui/dbsystem.hpp"
#include "machlog/network.hpp"
#include "network/netnet.hpp"
#include "recorder/recorder.hpp"
#include "device/timer.hpp"
#include "machines/scrndump.hpp"
#include "sim/manager.hpp"
#include "system/winapi.hpp"


void debugTiming( const char*, bool );

void SDLApp::cleanUpGui()
{
	_DELETE( pStartupScreens_ );
	pStartupScreens_ = NULL;
}

void SDLApp::initialiseGui( StartedFromLobby startedFromLobby, BaseProgressReporter* pReporter )
{
	HAL_STREAM("SDLApp::initialiseGui\n" );
	RenSurface backBuf = manager_->pDevice()->backSurface();

	HAL_STREAM("SDLApp::initialiseGui set backbuffer\n" );
	Gui::backBuffer( backBuf );

	HAL_STREAM("SDLApp::initialiseGui new MachGuiStartupScreens\n" );
	pStartupScreens_ = _NEW( MachGuiStartupScreens( manager_, pRoot_, pReporter ) );
	if( startedFromLobby == LOBBY_START )
	{
		//have to verify that the system really is in a lobbied state and doesn't just think it is.
		if( NetNetwork::instance().isLobbiedGame() )
		{
			pStartupScreens_->startupData()->playerName( NetNetwork::instance().localPlayerName() );
			if( MachLogNetwork::instance().isNodeLogicalHost() )
			{
				pStartupScreens_->startupData()->hostGame( true );
			}
			else
			{
				pStartupScreens_->startupData()->hostGame( false );
				// Send join message so that host can update availablePlayers list etc.
				pStartupScreens_->messageBroker().sendJoinMessage( pStartupScreens_->startupData()->playerName(), pStartupScreens_->startupData()->uniqueMachineNumber() );
			}
			// Initialise multiplayer settings ( assumes at least 1 planet and 1 scenario in Small map size ).
			MachGuiDbSystem& system = MachGuiDatabase::instance().multiPlayerSystem( MachGuiDatabase::SMALL );
			MachGuiDbPlanet& planet = system.planet( 0 );
			MachGuiDbScenario& scenario = planet.scenario( 0 );
			pStartupScreens_->startupData()->scenario( &scenario );
			pStartupScreens_->switchContext( MachGuiStartupScreens::CTX_IMREADY );
		}
	}

	HAL_STREAM("SDLApp::initialiseGui complete\n" );
}

void SDLApp::updateGui()
{
	debugTiming("D3Dapp:: update gui", true );

	pStartupScreens_->updateGui();

	debugTiming("D3Dapp:: update gui complete", false );
}

void SDLApp::displayGui()
{
	debugTiming("D3Dapp:: display gui", true );

	pStartupScreens_->displayGui();

	debugTiming("D3Dapp:: display gui complete", false );
}

void SDLApp::checkFinishApp()
{
	if ( pStartupScreens_->finishApp() )
		finish();
}

//  TBD: The display of recorder info should be on a keypush somewhere.
static bool displayRecorderInfo()
{
    static bool result = not( getenv( "cb_recorder_display" ) and
//      ( stricmp( getenv( "cb_recorder_display" ), "off" ) == 0 ) );
      ( strcasecmp( getenv( "cb_recorder_display" ), "off" ) == 0 ) );

    return result;
}

// Update the display.
void SDLApp::loopCycle()
{
    if( displayRecorderInfo() )
    {
        if( RecRecorder::instance().state() != RecRecorder::INACTIVE )
            RenDevice::current()->out() << RecRecorder::instance() << std::endl;

        if( RecRecorder::instance().state() == RecRecorder::PLAYING )
            RenDevice::current()->out() << SimManager::instance().currentTime() << std::endl;
    }

    //Prevent processing before clientStartup() call
    if( not initialised_ )
        return;

	checkFinishApp();

	MachScreenDumper::instance().update();

	pStartupScreens_->loopCycle();

	MachScreenDumper::instance().dump();
}

void SDLApp::activateGui()
{
	if ( pStartupScreens_ )
		pStartupScreens_->activate();
}



