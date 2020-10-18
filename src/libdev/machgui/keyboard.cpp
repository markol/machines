#include "base/memwatch.hpp"
#include "stdlib/string.hpp"
#include "machgui/ingame.hpp"
#include "machgui/prodbank.hpp"
#include "machgui/hwrebank.hpp"
#include "machgui/cameras.hpp"
#include "machgui/squadron.hpp"
#include "machgui/map.hpp"
#include "machgui/machnav.hpp"
#include "machgui/worldvie.hpp"
#include "machgui/command.hpp"
#include "machgui/fstpersn.hpp"
#include "machgui/promtext.hpp"
#include "world4d/scenemgr.hpp"
#include "phys/mcmotion.hpp"
#include "phys/motchunk.hpp"
#include "mathex/point3d.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/poly2d.hpp"
#include "profiler/profiler.hpp"
#include "sim/manager.hpp"
#include "gui/event.hpp"
#include "gui/gui.hpp"
#include "machgui/internal/igameimp.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/vmman.hpp"
#include "machphys/mcmovinf.hpp"
#include "render/surfmgr.hpp"
#include "machlog/network.hpp"

bool MachInGameScreen::doHandleKeyEvent( const GuiKeyEvent& e )
{
	CB_DEPIMPL( Commands, allCommands_ );
	CB_DEPIMPL( MachGuiFirstPerson*, pFirstPerson_ );
	CB_DEPIMPL( PhysAbsoluteTime, gameStateTimer_ );

	NEIL_STREAM("InGame button event : " << e.key() <<
				" ctrl " << e.isCtrlPressed() <<
				" shift " << e.isShiftPressed() <<
				" alt " << e.isAltPressed() << std::endl );

	bool processed = false;
	bool finished = false;
	unsigned int count = 0;

	// If the game has been won/lost then only allow escape key to work
	if ( actualGameState() != MachInGameScreen::PLAYING )
	{
		processed = true;

		if ( ( e.key() == DevKey::ESCAPE or e.key() == DevKey::SPACE ) and e.state() == Gui::PRESSED )
		{
			gameStateTimer_ = 0;
		}
	}

	while ( not finished and not processed )
	{
		switch ( count )
		{
			case 0:
				processed = pImpl_->pPromptText_->doHandleKeyEvent( e );
				break;
			case 1:
				ASSERT(	pImpl_->pCameras_, "pCameras_ is NULL" );
				processed = pImpl_->pCameras_->processButtonEvent( e.buttonEvent() );

				// Dismiss control panel?
				if ( e.key() == DevKey::LEFT_ARROW and e.isAltPressed() and e.state() == Gui::PRESSED )
				{
					pImpl_->controlPanelOn_ = false;
					processed = true;
				}
				if ( e.key() == DevKey::RIGHT_ARROW and e.isAltPressed() and e.state() == Gui::PRESSED )
				{
					pImpl_->controlPanelOn_ = true;
					processed = true;
				}
				break;
			case 2:
				ASSERT(	pImpl_->pSquadronBank_, "pSquadronBank_ is NULL" );
				processed = pImpl_->pSquadronBank_->processButtonEvent( e.buttonEvent() );
				break;
			case 3:
				ASSERT(	pImpl_->pMachineNavigation_, "pMachineNavigation_ is NULL" );
				processed = pImpl_->pMachineNavigation_->processButtonEvent( e.buttonEvent() );
				break;
			case 4:
				ASSERT(	pImpl_->pConstructionNavigation_, "pConstructionNavigation_ is NULL" );
				processed = pImpl_->pConstructionNavigation_->processButtonEvent( e.buttonEvent() );
				break;
			case 5:
				ASSERT(	pImpl_->pWorldViewWindow_, "pWorldViewWindow_ is NULL" );
				processed = pImpl_->pWorldViewWindow_->processButtonEvent( e.buttonEvent() );
				break;
			case 6:
				if ( pImpl_->pProductionBank_ )		// (May not be a production bank in existence)
					processed = pImpl_->pProductionBank_->processButtonEvent( e.buttonEvent() );
				break;
			case 7:
				if ( pImpl_->pHWResearchBank_ )		// (May not be a research bank in existence)
					processed = pImpl_->pHWResearchBank_->processButtonEvent( e.buttonEvent() );
				break;
			case 8:
				if ( e.key() == DevKey::ESCAPE and e.state() == Gui::PRESSED )
				{
					if ( isMachineNavigationContext() or isConstructionNavigationContext() )
					{
						mainMenuOrSingleFactoryContext();
					}
					else
					{
						pImpl_->switchToMenus_ = true;
					}

					processed = true;
				}
				break;
			case 9:	// F10 gets you into menus
				if ( e.key() == DevKey::F10 and e.state() == Gui::PRESSED )
				{
					pImpl_->switchToMenus_ = true;
					processed = true;
				}
				break;
			case 10:	// Screen shot
			    if( e.key() == DevKey::F12 and e.isShiftPressed() and e.isCtrlPressed() and e.state() == Gui::PRESSED )
				{
    		        initiateScreenShot();
					processed = true;
				}
				break;
			case 11: // Command hot keys
				{
					for ( Commands::iterator iter = allCommands_.begin(); iter != allCommands_.end() and not processed; ++iter )
					{
						processed = (*iter)->processButtonEvent( e.buttonEvent() );
					}
				}
				break;
			case 12: // Go inhead
			    if( e.key() == DevKey::PAD_1 and e.state() == Gui::PRESSED and pFirstPerson_->okayToSwitchTo1stPerson() )
				{
    		        switchToInHead();
					processed = true;
				}
				break;
			case 13: // Pause game
	        	if( e.key() == DevKey::BREAK and e.state() == Gui::PRESSED )
				{
					// Can't pause game in multiplayer games
					if ( not MachLogNetwork::instance().isNetworkGame() and
						 actualGameState() == PLAYING )
					{
						if ( SimManager::instance().isSuspended() )
						{
						    SimManager::instance().resume();
							MachLogVoiceMailManager::instance().acceptMailPostings( true );
						}
						else
						{
							SimManager::instance().suspend();
							MachLogVoiceMailManager::instance().acceptMailPostings( false );
						}

						processed = true;
					}
				}
			default:
				finished = true;
				break;
		}

		NEIL_STREAM( "Keyboard event " << count << " processed " << ( processed ? "true" : "false" ) << std::endl );

		++count;
	}

	NEIL_STREAM( "COUNT " << count << std::endl );

	#ifndef PRODUCTION
	if ( ( not getenv("cb_nokeyboardhacks") ) and
		 ( not ( count == 1 ) ) )
	{
		doHandleKeyEventHacks( e );
	}
	#endif

	return processed;
}

bool MachInGameScreen::doHandleKeyEventHacks( const GuiKeyEvent& e )
{
	#ifndef PRODUCTION

	NEIL_STREAM( "MachInGameScreen::doHandleKeyEventHacks " << std::endl );

	// Quick hack to switch "fog of war" on/off
	if ( e.key() == DevKey::KEY_M and e.isShiftPressed() and e.isCtrlPressed() and e.state() == Gui::PRESSED )
	{
		static bool fogOfWarOn = true;
		fogOfWarOn = not fogOfWarOn;
		pImpl_->pContinentMap_->fogOfWarOn( fogOfWarOn );
	}

	// *** Todo : This stuff needs sorting out. It's been taken from the Machines app and needs a serious tidy...
	// mouse events are handled seperately...
	PRE( e.key() != DevKey::LEFT_MOUSE );
	PRE( e.key() != DevKey::RIGHT_MOUSE );

	PhysMotionControl::processButtonEvent( e.buttonEvent() );

	if( e.state() == Gui::PRESSED )
	{
		if( e.key() == DevKey::KEY_U )
		{
        	if( e.isShiftPressed())
				pImpl_->pSceneManager_->autoAdjustFrameRate( false );
        	else
				pImpl_->pSceneManager_->autoAdjustFrameRate( true );
		}

        if( e.key() == DevKey::F4 and e.isShiftPressed() )
        {
            if( e.isCtrlPressed() )
                ProProfiler::instance().disableProfiling();
            else
                ProProfiler::instance().enableProfiling();
        }

        if( e.key() == DevKey::F5 and e.isShiftPressed() )
        {
            if( e.isCtrlPressed() )
            {
                MemChkMemoryWatchingOn( false );
                BaseMemWatcher::instance().traceOustandingAllocations( ProProfiler::instance().outputStream() );
                BaseMemWatcher::instance().clear();
            }
            else
                MemChkMemoryWatchingOn( true );
        }

        if( e.key() == DevKey::F7 and e.isShiftPressed() and e.isCtrlPressed() )
        {
            static bool    showStats = false;

            if( showStats )
                pImpl_->pSceneManager_->hideStats();
            else
                pImpl_->pSceneManager_->showStats(0.333);

            showStats = not showStats;
        }

   		if( e.key() == DevKey::F8 and e.isShiftPressed() and e.isCtrlPressed() )
		{
			for( MachInGameScreenImpl::Actors::iterator i = pImpl_->selectedActors_.begin();
				 i != pImpl_->selectedActors_.end(); ++i )
			{
				MachActor* pActor = *i;
				ASSERT( pActor, "" );
				if( pActor->objectIsMachine() )
				{
					MachLogMachine& machine = pActor->asMachine();
					machine.motionSeq().toggleLogging();
				}
			}
		}

        if( e.key() == DevKey::F9 and e.isShiftPressed() and e.isCtrlPressed() )
        {
            pImpl_->showCurrentMachine_ = not pImpl_->showCurrentMachine_;
        }

		if( e.key() == DevKey::F11 )
        {
        	RENDER_STREAM( RenSurfaceManager::instance() );
        }

		if ( e.key() == DevKey::KEY_N and e.isShiftPressed() )
		{
			pImpl_->showNetworkPing_ = e.isCtrlPressed();
 		}
		if( e.key() == DevKey::ASTERISK_PAD )
		{
			if( e.isShiftPressed() )
			{
				if( e.isCtrlPressed() )
				{
					NETWORK_STREAM_DISABLE;
				}
				else
				{
					NETWORK_STREAM_ENABLE;
				}
			}
		}
	}
	#endif

	return false;
}

/* End D3DAPP.CPP ***************************************************/
