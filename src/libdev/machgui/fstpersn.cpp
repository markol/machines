/*
 * F S T P E R S N . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/fstpersn.hpp"
#include "mathex/point2d.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/random.hpp"
#include "world4d/scenemgr.hpp"
#include "world4d/domain.hpp"
#include "world4d/envirnmt.hpp"
#include "world4d/soundman.hpp"
#include "world4d/entity.hpp"
#include "world4d/manager.hpp"
#include "envirnmt/planet.hpp"
#include "render/device.hpp"
#include "render/camera.hpp"
#include "gui/painter.hpp"
#include "gui/event.hpp"
#include "gui/image.hpp"
#include "device/keyboard.hpp"
#include "device/keytrans.hpp"
#include "machlog/actor.hpp"
#include "machlog/aggressr.hpp"
#include "machlog/camera.hpp"
#include "machlog/canattac.hpp"
#include "machlog/machine.hpp"
#include "machlog/p1mchndl.hpp"
#include "machlog/MachLog1stPersonActiveSquad.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/planet.hpp"
#include "machlog/weapon.hpp"
#include "machlog/cntrl_pc.hpp"
#include "machlog/races.hpp"
#include "machlog/network.hpp"
#include "machlog/vmman.hpp"
#include "machgui/ingame.hpp"
#include "machgui/cameras.hpp"
#include "machgui/animatio.hpp"
#include "machgui/radar.hpp"
#include "machgui/chatmsgd.hpp"
#include "machgui/chatmsgs.hpp"
#include "machgui/database.hpp"
#include "machgui/dbscenar.hpp"
#include "machgui/MachGuiFPCommand.hpp"
#include "machphys/objdata.hpp"
#include "machphys/snddata.hpp"
#include "machphys/machine.hpp"
#include "system/pathname.hpp"
#include "system/registry.hpp"
#include "device/time.hpp"
#include "machgui/internal/mgsndman.hpp"
#include "sim/manager.hpp"

#define WEAPON_DROPDOWN_TIME 0.7
#define WEAPON_DROPDOWN_DELAYTIME 0.4
#define WEAPON_DROPDOWN_FRAMES 10
#define STARTUP_SEQUENCE_TIME 0.5
#define STARTUP_SEQUENCE_FRAMES 10

class MachGuiPausedImage : public GuiDisplayable
{
public:
	MachGuiPausedImage( GuiDisplayable* pParent, MachInGameScreen* pInGameScreen )
	:	GuiDisplayable( pParent, Gui::Box(0,0,0,0), GuiDisplayable::LAYER5 ),
		pInGameScreen_( pInGameScreen )
	{}

	virtual void doDisplay()
	{
	 	// Display "Paused" in centre of screen if game is paused
		if ( SimManager::instance().isSuspended() )
		{
			// Get screen dimensions
		    RenDevice& device = *W4dManager::instance().sceneManager()->pDevice();
		    const int w = device.windowWidth();
			const int h = device.windowHeight();

			GuiBitmap pausedBmp;

			switch ( pInGameScreen_->actualGameState() )
			{
				case MachInGameScreen::WON:
					if ( MachGuiDatabase::instance().currentScenario().isTrainingScenario() )
					{
						pausedBmp = Gui::bitmap( "gui/misc/complete.bmp" );
					}
					else
					{
						pausedBmp = Gui::bitmap( "gui/misc/victory.bmp" );
					}
					break;
				case MachInGameScreen::LOST:
					if ( MachGuiDatabase::instance().currentScenario().isTrainingScenario() )
					{
						pausedBmp = Gui::bitmap( "gui/misc/failed.bmp" );
					}
					else
					{
						pausedBmp = Gui::bitmap( "gui/misc/defeat.bmp" );
					}
					break;
				default:
					pausedBmp = Gui::bitmap( "gui/misc/paused.bmp" );
					break;
			}

			pausedBmp.enableColourKeying();
			int xPos = ( w / 2 ) - ( pausedBmp.width() / 2 );
			int yPos = ( h / 2 ) - ( pausedBmp.height() / 2 );

			GuiPainter::instance().blit( pausedBmp, Gui::Coord(xPos,yPos) );
		}
		else if ( pInGameScreen_->isNetworkStuffed() )
		{
			// Get screen dimensions
		    RenDevice& device = *W4dManager::instance().sceneManager()->pDevice();
		    const int w = device.windowWidth();
			const int h = device.windowHeight();

			int borderHeight = h * 0.05;

			// Display "Network Busy" message ( top right )
			GuiBitmap pausedBmp = Gui::bitmap( "gui/misc/netbusy.bmp" );
			pausedBmp.enableColourKeying();
			int xPos = w - pausedBmp.width();

			GuiPainter::instance().blit( pausedBmp, Gui::Coord(xPos,borderHeight + 2) );
		}
	}

private:
	MachInGameScreen* pInGameScreen_;
};

class MachGuiFirstPersonImpl
{
public:
	MachGuiFirstPersonImpl();

	bool switchToMenus_;
	bool switchToInGame_;
	MachActor* pActor_;
	MachInGameScreen* pInGameScreen_;
	bool inFirstPerson_;
	W4dSceneManager* pSceneManager_;
	int borderHeight_;
	int lastBorderHeight_;
	DevKeyToCommandTranslator* pKeyTranslator_;
	DevKeyToCommandTranslator::CommandList commandList_;
    MachLog1stPersonHandler* pLogHandler_; 	// Handles 1st person commands - makes things happen in game
    MexPoint3d targetPoint_; 				// The point currently to be aimed at (global coords)
    MachGuiAnimation* pAttackCursor_; 		// Attack cross-hair
    MachGuiAnimation* pNormalCursor_; 		// Nothing to target cursor
	MachGuiAnimation* pMissCursor_; 		// Targeted on actor but weapons cannot tilt
	MachGuiAnimation* pStartCursor_;		// When machine is first embodied, cursor expands.
	bool switchBackToGroundCamera_; 		// Camera to switch back to when leaving 1st person
	MachActor* pTargetActor_;
	GuiBitmap compassBmp_;
	GuiBitmap healthBmp_;
	GuiBitmap armourBmp_;
	MachGuiRadar* pRadar_;
	int borderDrawCount_;
	GuiBitmap leftWeaponBmp_;
	GuiBitmap rightWeaponBmp_;
	GuiBitmap topWeaponBmp_;
	GuiBitmap weaponChargeBmp_;
	GuiBitmap weaponBackgroundBmp_;
	int weaponSelectIndex_;
	bool justEnteredFirstPerson_;
	GuiBitmap weaponStartupFrames_[10];
	double leftWeaponChangeEndTime_;
	double rightWeaponChangeEndTime_;
	double topWeaponChangeEndTime_;
	int leftWeaponPos_;
	int rightWeaponPos_;
	int topWeaponPos_;
	bool resolutionChanged_;
	bool isDead_;
	PhysAbsoluteTime timeOfDeath_;
	MachGuiInGameChatMessagesDisplay* pChatMessageDisplay_;
	bool rightMouseButtonHeadTurningUsed_;
	double lastRightClickTime_; // Used for checking for right mouse button double click
	double timeWeaponsFired_;
	MachGuiPausedImage* pPausedImage_;
	bool reverseUpDownKeys_;
	bool reverseUpDownMouse_;
	MexBasicRandom hitInterferenceRandom_;
	bool machineNVGOn_;
	double startupTimer_;
	bool finishedStartupSequence_;
	bool isHitInterferenceOn_;
	double hitInterferenceEndTime_;
	int frameNumber_;

    // FP Command
    MachGuiFPCommand* pCommandWidget_;
    int64_t commandSquadIndex_;
    double timeSquadIndexChanged_;
};

MachGuiFirstPersonImpl::MachGuiFirstPersonImpl()
:	switchToMenus_( false ),
	switchToInGame_( false ),
	pActor_( NULL ),
	inFirstPerson_( false ),
	pInGameScreen_( NULL ),
	pSceneManager_( NULL ),
    pLogHandler_( NULL ),
	switchBackToGroundCamera_( true ),
	compassBmp_( Gui::bitmap( "gui/fstpersn/cursor/compass.bmp" ) ),
	healthBmp_( Gui::bitmap( "gui/fstpersn/cursor/health.bmp" ) ),
	armourBmp_( Gui::bitmap( "gui/fstpersn/cursor/armour.bmp" ) ),
	weaponChargeBmp_( Gui::bitmap( "gui/fstpersn/weapon/chrgey.bmp" ) ),
	weaponBackgroundBmp_( Gui::bitmap( "gui/fstpersn/weapon/weapon.bmp" ) ),
	pAttackCursor_( NULL ),
    pNormalCursor_( NULL ),
	pMissCursor_( NULL ),
	pStartCursor_( NULL ),
	pRadar_( NULL ),
	resolutionChanged_( true ),
	pChatMessageDisplay_( NULL ),
	rightMouseButtonHeadTurningUsed_( false ),
	lastRightClickTime_( 0 ),
	pPausedImage_( NULL ),
	reverseUpDownKeys_( SysRegistry::instance().queryIntegerValue( "Options\\Reverse UpDown Keys", "on", SysRegistry::CURRENT_USER ) ),
	reverseUpDownMouse_( SysRegistry::instance().queryIntegerValue( "Options\\Reverse BackForward Mouse", "on", SysRegistry::CURRENT_USER ) ),
	hitInterferenceRandom_( MexBasicRandom::constructSeededFromTime() ),
	machineNVGOn_( false ),
    finishedStartupSequence_( false ),
    pCommandWidget_( nullptr ),
    commandSquadIndex_(-1L),
    timeSquadIndexChanged_(0.0)
{
	compassBmp_.enableColourKeying();
	weaponChargeBmp_.enableColourKeying();
	weaponBackgroundBmp_.enableColourKeying();
}

MachGuiFirstPerson::MachGuiFirstPerson( W4dSceneManager* pSceneManager, W4dRoot*, MachInGameScreen* pInGameScreen )
:	GuiRoot( Gui::Box( 0, 0, 10000, 10000 ) )
{
	// Create implementation
	pImpl_ = _NEW( MachGuiFirstPersonImpl() );

	// De-pImpl_ variables used within this function.
	CB_DEPIMPL( MachInGameScreen*, pInGameScreen_ );
	CB_DEPIMPL( W4dSceneManager*, pSceneManager_ );
	CB_DEPIMPL( DevKeyToCommandTranslator*, pKeyTranslator_ );
	CB_DEPIMPL( DevKeyToCommandTranslator::CommandList, commandList_ );
	CB_DEPIMPL( MachGuiPausedImage*, pPausedImage_ );

    commandList_.reserve( NUM_COMMANDS + 1 );

	pInGameScreen_ = pInGameScreen;
	pSceneManager_ = pSceneManager;

	// Setup keyboard translator
 	pKeyTranslator_ = _NEW( DevKeyToCommandTranslator() );
	// Keyboard shoot handler
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::SPACE, FIRE ) );
	// Keboard centre head handlers
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::PAD_5, CENTREHEAD ) );
	// Keboard weapon select handlers
 	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::TAB, WEAPONSELECT ) );
	// Keboard move foward/backward handlers
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::UP_ARROW, FOWARD, DevKeyToCommand::CTRLKEY_EITHER, DevKeyToCommand::SHIFTKEY_RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::DOWN_ARROW, BACKWARD, DevKeyToCommand::CTRLKEY_EITHER, DevKeyToCommand::SHIFTKEY_RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::UP_ARROW_PAD, FOWARD, DevKeyToCommand::CTRLKEY_EITHER, DevKeyToCommand::SHIFTKEY_RELEASED ) );
	// Removed by NA 3/2/99 so that switch to zenith works. obviously you can no longer reverse the machine using the pad down arrow key.
	//pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::DOWN_ARROW_PAD, BACKWARD, DevKeyToCommand::CTRLKEY_EITHER, DevKeyToCommand::SHIFTKEY_RELEASED ) );

	// Keyboard head up/down handlers
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::UP_ARROW, LOOKDOWNFAST, DevKeyToCommand::CTRLKEY_RELEASED, DevKeyToCommand::SHIFTKEY_PRESSED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::DOWN_ARROW, LOOKUPFAST, DevKeyToCommand::CTRLKEY_RELEASED, DevKeyToCommand::SHIFTKEY_PRESSED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::UP_ARROW, LOOKDOWN, DevKeyToCommand::CTRLKEY_PRESSED, DevKeyToCommand::SHIFTKEY_PRESSED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::DOWN_ARROW, LOOKUP, DevKeyToCommand::CTRLKEY_PRESSED, DevKeyToCommand::SHIFTKEY_PRESSED ) );
	// Keyboard turn left/right handlers
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::LEFT_ARROW, TURNLEFT, DevKeyToCommand::CTRLKEY_PRESSED, DevKeyToCommand::SHIFTKEY_RELEASED ) );
 	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::RIGHT_ARROW, TURNRIGHT, DevKeyToCommand::CTRLKEY_PRESSED, DevKeyToCommand::SHIFTKEY_RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::LEFT_ARROW, TURNLEFTFAST, DevKeyToCommand::CTRLKEY_RELEASED, DevKeyToCommand::SHIFTKEY_RELEASED ) );
 	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::RIGHT_ARROW, TURNRIGHTFAST, DevKeyToCommand::CTRLKEY_RELEASED, DevKeyToCommand::SHIFTKEY_RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::LEFT_ARROW_PAD, TURNLEFT, DevKeyToCommand::CTRLKEY_PRESSED, DevKeyToCommand::SHIFTKEY_RELEASED ) );
 	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::RIGHT_ARROW_PAD, TURNRIGHT, DevKeyToCommand::CTRLKEY_PRESSED, DevKeyToCommand::SHIFTKEY_RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::LEFT_ARROW_PAD, TURNLEFTFAST, DevKeyToCommand::CTRLKEY_RELEASED, DevKeyToCommand::SHIFTKEY_RELEASED ) );
 	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::RIGHT_ARROW_PAD, TURNRIGHTFAST, DevKeyToCommand::CTRLKEY_RELEASED, DevKeyToCommand::SHIFTKEY_RELEASED ) );
	// Keyboard turn head left/right handlers
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::LEFT_ARROW, TURNHEADLEFT, DevKeyToCommand::CTRLKEY_PRESSED, DevKeyToCommand::SHIFTKEY_PRESSED ) );
 	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::RIGHT_ARROW, TURNHEADRIGHT, DevKeyToCommand::CTRLKEY_PRESSED, DevKeyToCommand::SHIFTKEY_PRESSED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::LEFT_ARROW, TURNHEADLEFTFAST, DevKeyToCommand::CTRLKEY_RELEASED, DevKeyToCommand::SHIFTKEY_PRESSED ) );
 	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::RIGHT_ARROW, TURNHEADRIGHTFAST, DevKeyToCommand::CTRLKEY_RELEASED, DevKeyToCommand::SHIFTKEY_PRESSED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::LEFT_ARROW_PAD, TURNHEADLEFT, DevKeyToCommand::CTRLKEY_PRESSED, DevKeyToCommand::SHIFTKEY_PRESSED ) );
 	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::RIGHT_ARROW_PAD, TURNHEADRIGHT, DevKeyToCommand::CTRLKEY_PRESSED, DevKeyToCommand::SHIFTKEY_PRESSED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::LEFT_ARROW_PAD, TURNHEADLEFTFAST, DevKeyToCommand::CTRLKEY_RELEASED, DevKeyToCommand::SHIFTKEY_PRESSED ) );
 	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::RIGHT_ARROW_PAD, TURNHEADRIGHTFAST, DevKeyToCommand::CTRLKEY_RELEASED, DevKeyToCommand::SHIFTKEY_PRESSED ) );
    // Keyboard commands for first person command
    pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::HOME, COMMAND_SELECT_NEXT, DevKeyToCommand::CTRLKEY_RELEASED, DevKeyToCommand::SHIFTKEY_RELEASED ));
    pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::END, COMMAND_SELECT_PREV, DevKeyToCommand::CTRLKEY_RELEASED, DevKeyToCommand::SHIFTKEY_RELEASED ));
    pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::KEY_DELETE, COMMAND_ORDER_ATTACK, DevKeyToCommand::CTRLKEY_RELEASED, DevKeyToCommand::SHIFTKEY_RELEASED ) );
    pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::PAGE_DOWN, COMMAND_ORDER_MOVE, DevKeyToCommand::CTRLKEY_RELEASED, DevKeyToCommand::SHIFTKEY_RELEASED ) );
    pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::INSERT, COMMAND_ORDER_FOLLOW, DevKeyToCommand::CTRLKEY_RELEASED, DevKeyToCommand::SHIFTKEY_RELEASED ) );


	pKeyTranslator_->initEventQueue();

	// Setup command list that stores foward, backward, etc motion status.
	for ( size_t i = 0; i < NUM_COMMANDS; ++i )
	{
		commandList_.push_back( DevKeyToCommandTranslator::Command() );
	}

	pPausedImage_ = _NEW( MachGuiPausedImage( this, pInGameScreen_ ) );

    TEST_INVARIANT;
}

MachGuiFirstPerson::~MachGuiFirstPerson()
{
    TEST_INVARIANT;

	// De-pImpl_ variables used within this function.
	CB_DEPIMPL( DevKeyToCommandTranslator*, pKeyTranslator_ );
	CB_DEPIMPL( MachLog1stPersonHandler*, pLogHandler_ );
	CB_DEPIMPL( MachGuiPausedImage*, pPausedImage_ );
    CB_DEPIMPL( MachGuiFPCommand*, pCommandWidget_);

    if (pCommandWidget_ != nullptr)
    {
        delete pCommandWidget_;
    }

	_DELETE( pKeyTranslator_ );
	_DELETE( pLogHandler_ );
	_DELETE( pPausedImage_ );
	_DELETE( pImpl_ );
}

void MachGuiFirstPerson::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiFirstPerson& t )
{

    o << "MachGuiFirstPerson " << (void*)&t << " start" << std::endl;
    o << "MachGuiFirstPerson " << (void*)&t << " end" << std::endl;

    return o;
}

// virtual
void MachGuiFirstPerson::doDisplay()
{
	// De-pImpl_ variables used within this function.
	CB_DEPIMPL( W4dSceneManager*, pSceneManager_ );
	CB_DEPIMPL( int, lastBorderHeight_ );
	CB_DEPIMPL( int, borderDrawCount_ );
	CB_DEPIMPL( bool, justEnteredFirstPerson_ );
	CB_DEPIMPL( MachGuiAnimation*, pStartCursor_ );
	CB_DEPIMPL( MachLog1stPersonHandler*, pLogHandler_ );
	CB_DEPIMPL( bool, finishedStartupSequence_ );
	CB_DEPIMPL( int, frameNumber_ );

	if ( borderDrawCount_ or not finishedStartupSequence_ )
	{
		// Get screen dimensions
	    RenDevice& device = *pSceneManager_->pDevice();
	    const int w = device.windowWidth();
		const int h = device.windowHeight();

		// Borders at top and bottom of screen.
		GuiPainter::instance().filledRectangle( Gui::Box( 0, h - lastBorderHeight_, w, h ), Gui::BLACK() );
		GuiPainter::instance().filledRectangle( Gui::Box( 0, 0, w, lastBorderHeight_), Gui::BLACK() );

		if ( borderDrawCount_ )
		{
			--borderDrawCount_;
		}
	}

	displayCompass();
	displayHealthArmour();
	displayWeapons();

	if ( justEnteredFirstPerson_ == true )
	{
		MachGuiSoundManager::instance().playSound( "gui/sounds/radaron.wav" );

		// If there are any weapons then play the weapon selection sound.
		if ( pLogHandler_ and pLogHandler_->nWeapons() )
			MachGuiSoundManager::instance().playSound( "gui/sounds/weapchng.wav" );

		justEnteredFirstPerson_ = false;
	}

	++frameNumber_;
}

//virtual
void MachGuiFirstPerson::update()
{
	// De-pImpl_ variables used within this function.
	CB_DEPIMPL( MachLog1stPersonHandler*, pLogHandler_ );
	CB_DEPIMPL( DevKeyToCommandTranslator::CommandList, commandList_ );
	CB_DEPIMPL( DevKeyToCommandTranslator*, pKeyTranslator_ );
	CB_DEPIMPL( MachGuiAnimation*, pAttackCursor_ );
    CB_DEPIMPL( MachGuiAnimation*, pNormalCursor_ );
	CB_DEPIMPL( MachGuiAnimation*, pMissCursor_ );
	CB_DEPIMPL( MachGuiAnimation*, pStartCursor_ );
	CB_DEPIMPL( MexPoint3d, targetPoint_ );
	CB_DEPIMPL( MachActor*, pTargetActor_ );
	CB_DEPIMPL( MachActor*, pActor_ );
	CB_DEPIMPL( W4dSceneManager*, pSceneManager_ );
	CB_DEPIMPL( MachInGameScreen*, pInGameScreen_ );
	CB_DEPIMPL( bool, justEnteredFirstPerson_ );
	CB_DEPIMPL( bool, isDead_ );
	CB_DEPIMPL( PhysAbsoluteTime, timeOfDeath_ );
	CB_DEPIMPL( bool, switchToInGame_ );
	CB_DEPIMPL( bool, rightMouseButtonHeadTurningUsed_ );
	CB_DEPIMPL( double, timeWeaponsFired_ );
	CB_DEPIMPL( bool, reverseUpDownKeys_ );
	CB_DEPIMPL( bool, reverseUpDownMouse_ );
	CB_DEPIMPL( MexBasicRandom, hitInterferenceRandom_ );
	CB_DEPIMPL( bool, machineNVGOn_ );
	CB_DEPIMPL( double, hitInterferenceEndTime_ );
	CB_DEPIMPL( double, startupTimer_ );
	CB_DEPIMPL( bool, finishedStartupSequence_ )
	CB_DEPIMPL( bool, isHitInterferenceOn_ );
	CB_DEPIMPL( int, frameNumber_ );
    CB_DEPIMPL( int64_t, commandSquadIndex_ );
    CB_DEPIMPL( MachGuiFPCommand*, pCommandWidget_ );
    CB_DEPIMPL( double, timeSquadIndexChanged_ );

	double now = DevTime::instance().time();

	if ( justEnteredFirstPerson_ == true )
	{
		// Controls when weapon not recharged sound is played.
		timeWeaponsFired_ = now;

        // Keeps the squad changing from going too fast
        timeSquadIndexChanged_ = now;
	}

	// Check to see if we should automatically leave 1st person
	if ( not isDead_ )
	{
		MachPhys::Race playerRace = MachLogRaces::instance().pcController().race();

		// If the actor has been deleted or has changed race then exit the actor
		if ( not pActor_ or
			 pActor_->isDead() or
			 pActor_->race() != playerRace )
		{
			isDead_ = true;
			timeOfDeath_ = Phys::time();
		}
	}

	if ( isDead_ )
	{
		pInGameScreen_->sceneManager().pDevice()->interferenceOn( std::min( ( Phys::time() - timeOfDeath_ ) + 0.1, 1.0 ) );
		if ( Phys::time() - timeOfDeath_ > 0.5 )
		{
			switchToInGame_ = true;
		}
		return;
	}

	// Reset startup cursor animation
	if ( justEnteredFirstPerson_ == true )
		pStartCursor_->jumpToCell( 0 );

	pKeyTranslator_->resetCommands( &commandList_ );

    //Check have a handler
    if( pLogHandler_ )
    {
        MachLog1stPersonHandler& logHandler = *pLogHandler_;

        if ( commandSquadIndex_ == -1L and logHandler.getActiveSquadron().hasActiveSquadron() )
        {
            commandSquadIndex_ = logHandler.getActiveSquadron().getActiveSquadronId() - 1;
        }

		if( pActor_ and not ( pActor_->objectType() == MachLog::AGGRESSOR
							  and pActor_->asAggressor().subType() == MachPhys::NINJA
							  and pActor_->busy() ) ) // prevents major movement when gorilla is doing its ground punch
 		{

	    	if( commandList_[FOWARD].on() )
	    	{
	    		logHandler.moveForwards();
	    	}

	    	if( commandList_[BACKWARD].on() )
	    	{
	    		logHandler.moveBackwards();
	    	}

	    	if( commandList_[TURNLEFT].on() )
	    	{
				logHandler.turnAtFastRate( false );
	    		logHandler.turnLeft();
	    	}

	    	if( commandList_[TURNRIGHT].on() )
	    	{
				logHandler.turnAtFastRate( false );
	    		logHandler.turnRight();
	    	}

	    	if( commandList_[TURNLEFTFAST].on() )
	    	{
				logHandler.turnAtFastRate( true );
	    		logHandler.turnLeft();
	    	}

	    	if( commandList_[TURNRIGHTFAST].on() )
	    	{
				logHandler.turnAtFastRate( true );
	    		logHandler.turnRight();
	    	}
		}

        // Cycle squadrons for Command
        if( commandList_[COMMAND_SELECT_NEXT].on() )
        {
            // prevent super-fast cycling
            if (now - timeSquadIndexChanged_ >= 0.15)
            {
                // start over if need be
                if (++commandSquadIndex_ > 9)
                {
                    commandSquadIndex_ = 0;
                }

                const_cast<MachLog1stPersonActiveSquadron&>(logHandler.getActiveSquadron()).setActiveSquadron(commandSquadIndex_);
                pCommandWidget_->updateSquadIcon();
                pCommandWidget_->updateSquadNumber();

                // If commandSquadIndex didn't point to an empty squad, update time variable.
                timeSquadIndexChanged_ = ((logHandler.getActiveSquadron().getActiveSquadronId()-1^commandSquadIndex_) == 0) ? now : timeSquadIndexChanged_;
            }
        }
        if( commandList_[COMMAND_SELECT_PREV].on() )
        {
            // prevent super-fast cycling
            if (now - timeSquadIndexChanged_ >= 0.15)
            {
                // roll back over to end
                if (--commandSquadIndex_ < 0)
                {
                    commandSquadIndex_ = 9;
                }

                const_cast<MachLog1stPersonActiveSquadron&>(logHandler.getActiveSquadron()).setActiveSquadron(commandSquadIndex_);
                pCommandWidget_->updateSquadIcon();
                pCommandWidget_->updateSquadNumber();

                // If commandSquadIndex didn't point to an empty squad, update time variable.
                timeSquadIndexChanged_ = ((logHandler.getActiveSquadron().getActiveSquadronId()-1^commandSquadIndex_) == 0) ? now : timeSquadIndexChanged_;
            }
        }

        // The command icons should not light up if there's nobody selected. This value will be used more further down...
        bool canIssueCommands = logHandler.getActiveSquadron().hasActiveSquadron();
        if (not canIssueCommands)
        {
            pCommandWidget_->setAttackIconState(MachGuiFPCommand::CommandIconState::INVALID);
            pCommandWidget_->setFollowIconState(MachGuiFPCommand::CommandIconState::INVALID);
            pCommandWidget_->setMoveIconState(MachGuiFPCommand::CommandIconState::INVALID);
        }

		// Turn head...
    	if( commandList_[TURNHEADLEFT].on() and logHandler.canTurnHead() )
    	{
			logHandler.turnHeadBy( - MexDegrees( 1.0 ) );
    	}
    	if( commandList_[TURNHEADRIGHT].on() and logHandler.canTurnHead() )
    	{
			logHandler.turnHeadBy( MexDegrees( 1.0 ) );
		}
    	if( commandList_[TURNHEADLEFTFAST].on() and logHandler.canTurnHead() )
    	{
			logHandler.turnHeadBy( - MexDegrees( 10.0 ) );
    	}
    	if( commandList_[TURNHEADRIGHTFAST].on() and logHandler.canTurnHead() )
    	{
			logHandler.turnHeadBy( MexDegrees( 10.0 ) );
    	}

		// Look up/down...
		if( commandList_[LOOKUP].on() )
    	{
			if ( reverseUpDownKeys_ )
				logHandler.lookDown( MexDegrees( 0.5  ) );
			else
				logHandler.lookDown( - MexDegrees( 0.5  ) );
    	}
		if( commandList_[LOOKDOWN].on() )
    	{
			if ( reverseUpDownKeys_ )
				logHandler.lookDown( - MexDegrees( 0.5  ) );
			else
				logHandler.lookDown( MexDegrees( 0.5 ) );
    	}
		if( commandList_[LOOKUPFAST].on() )
    	{
			if ( reverseUpDownKeys_ )
				logHandler.lookDown( MexDegrees( 5.0 ) );
			else
				logHandler.lookDown( - MexDegrees( 5.0 ) );
    	}
		if( commandList_[LOOKDOWNFAST].on() )
    	{
			if ( reverseUpDownKeys_ )
				logHandler.lookDown( - MexDegrees( 5.0 ) );
			else
				logHandler.lookDown( MexDegrees( 5.0 )  );
    	}

		// Centre head...
		if ( commandList_[CENTREHEAD].on() or
			 ( rightMouseButtonHeadTurningUsed_ and not DevMouse::instance().rightButton() ) )
		{
			rightMouseButtonHeadTurningUsed_ = false;

			if ( logHandler.canTurnHead() )
			{
				logHandler.turnHeadTo( MexRadians(0) );
			}
			logHandler.lookAhead();
		}

		// Turn machine head or whole body...

		// Get screen size.
	    RenDevice& device = *pSceneManager_->pDevice();
	    const double halfScreenWidth = (double)device.windowWidth() / 2.0;
		const double halfScreenHeight = (double)device.windowHeight() / 2.0;

		// Get new mouse position
		DevMouse::Position mousePos = DevMouse::instance().position();

		double deltaX = (double)mousePos.first - halfScreenWidth;
		double deltaY = (double)mousePos.second - halfScreenHeight;
		double origDeltaX = deltaX;

		// If head can turn independant of body then do so ( as long as Shift is pressed or Right mouse button pressed )...
		if ( logHandler.canTurnHead() and ( DevKeyboard::instance().shiftPressed() or DevMouse::instance().rightButton() ) )
		{
			if ( DevMouse::instance().rightButton() )
				rightMouseButtonHeadTurningUsed_ = true;

			// in fast mode movement is progressivly faster based on distance mouse has moved
			// half screen width = 10 deg rotation ( in slow mode )
			if  ( DevKeyboard::instance().ctrlPressed() )
			{
				deltaX = ( deltaX / halfScreenWidth ) * ( Mathex::PI / 18.0 );
			}
			else
			{
				// Move ten times faster when mouse has moved a long way ( 60 pixels )
				if ( not ( deltaX < 60.0 and deltaX > -60.0 ) )
				{
					deltaX *= 7.0;
				}
				deltaX = ( deltaX / halfScreenHeight ) * ( Mathex::PI / 7.0 );
			}

			if ( deltaX != 0.0 )
				logHandler.turnHeadBy( deltaX );
		}
		else // Move whole body
		{
			// Work out if body should be turning at fast rate
			bool turnFast = false;
			if ( not DevKeyboard::instance().ctrlPressed() and
				 ( ( deltaX < -10.0 or deltaX > 10.0 ) or deltaX == 0.0 ) )
			{
				turnFast = true;
			}

			logHandler.turnAtFastRate( turnFast );

			if ( deltaX < 0 )
			{
				logHandler.turnLeft();
			}
			else if ( deltaX > 0 )
			{
				logHandler.turnRight();
			}
		}

		// Look up/down. All machines can do this.
		// in fast mode movement is progressivly faster based on distance mouse has moved
		// half screen height = 10 deg rotation ( in slow mode )
		if ( DevKeyboard::instance().ctrlPressed() )
		{
			deltaY =  ( deltaY / halfScreenHeight ) * ( Mathex::PI / 18.0 );
		}
		else
		{
			if ( abs( origDeltaX ) < abs( deltaY * 3.0 ) )
			{
				// Move two times faster when mouse has moved a long way ( 50 pixels )
				if ( not ( deltaY < 50.0 and deltaY > -50.0 ) )
				{
					deltaY *= 4.0;
				}

				deltaY =  ( deltaY / halfScreenHeight ) * ( Mathex::PI / 12.0 );
			}
			else
			{
				deltaY = 0.0;
			}
		}

		if ( deltaY != 0.0 )
		{
			if ( reverseUpDownMouse_ )
				logHandler.lookDown( -deltaY );
			else
				logHandler.lookDown( deltaY );
		}

		// Reset mouse position to centre of screen
		DevMouse::instance().changePosition( halfScreenWidth, halfScreenHeight );

	    //Update the handler
	    logHandler.update();

        //Check the current aim
        MachLog1stPersonTargetInfo targetingInfo;
        logHandler.acquireTargetingInfo(targetingInfo);

        // This guy is used for the health indicator stuff elsewhere in this file. Will either be someone you can shoot at or NULL
        pTargetActor_ = targetingInfo.shootingTarget;

        bool viableTarget = targetingInfo.strikeType == MachPhys::ON_OBJECT;
        bool viableShootingTarget = (targetingInfo.shootingTarget != nullptr) and viableTarget;
        bool viableCommandTarget = (targetingInfo.getCommandTarget() != nullptr) and viableTarget;
        bool viableMoveToTarget = logHandler.isPointingTowardsGround() and logHandler.isViableMoveToTarget(targetingInfo);
        bool moveIndicatorPresent = logHandler.isMoveIndicatorPresent();

        // Don't allow the target of ore holos or debris!
        if (viableShootingTarget)
        {
            if ( targetingInfo.shootingTarget->objectIsOreHolograph() or
                 targetingInfo.shootingTarget->objectIsDebris() )
            {
                viableTarget = false;
            }
        }
        else if (viableCommandTarget)
        {
            if ( targetingInfo.getCommandTarget()->objectIsOreHolograph() or
                 targetingInfo.getCommandTarget()->objectIsDebris() )
            {
                viableTarget = false;
            }
        }

        // FOR SHOOTING: Are we targeting a machine?
        if ( viableShootingTarget and viableTarget )
        {
            // Enemy machine?
            if ( targetingInfo.shootingTarget->race() != pActor_->race() )
            {
                bool anglesValid = logHandler.targetAnglesValid();

                // Display attack cursor or miss cursor
                if ( not pAttackCursor_->isVisible() and anglesValid )
                {
                    // Attack cursor has just come on line. play sound
                    MachGuiSoundManager::instance().playSound( "gui/sounds/attackon.wav" );
                }
                pAttackCursor_->isVisible( anglesValid );
                pMissCursor_->isVisible( not anglesValid );
            }
            else
            {
                if ( not pMissCursor_->isVisible() )
                {
                    // Attack cursor has just come on line. play sound
                    MachGuiSoundManager::instance().playSound( "gui/sounds/friendon.wav" );
                }

                // Display miss cursor ( friendly machine )
                pAttackCursor_->isVisible( false );
                pMissCursor_->isVisible( true );
            }
        }
        else
        {
            // Hide attack cursors
            pAttackCursor_->isVisible( false );
            pMissCursor_->isVisible( false );
        }

        // FOR COMMAND: Are we targeting a machine?
        if ( canIssueCommands and viableCommandTarget and viableTarget )
        {
            // Enemy machine?
            if ( targetingInfo.getCommandTarget()->race() != pActor_->race() )
            {
                // Light up Attack Icon
                pCommandWidget_->setAttackIconState(MachGuiFPCommand::CommandIconState::VALID);

                // Dim the Follow Icon
                pCommandWidget_->setFollowIconState(MachGuiFPCommand::CommandIconState::INVALID);
            }
            else
            {
                // He's a friend bruv!
                // Don't light up Attack Icon
                pCommandWidget_->setAttackIconState(MachGuiFPCommand::CommandIconState::INVALID);

                // Light up Follow Icon (FOLLOW FRIENDLY)
                pCommandWidget_->setFollowIconState(MachGuiFPCommand::CommandIconState::VALID);
            }
        }
        else
        {
            if (canIssueCommands)
            {
                // Don't light up attack icon
                pCommandWidget_->setAttackIconState(MachGuiFPCommand::CommandIconState::INVALID);

                // Light up Follow Icon (FOLLOW SELF)
                pCommandWidget_->setFollowIconState(MachGuiFPCommand::CommandIconState::VALID);

                // Only light up Move Icon when pointing at navigable ground AND when the indicator disappears
                if (viableMoveToTarget and not moveIndicatorPresent)
                {
                    pCommandWidget_->setMoveIconState(MachGuiFPCommand::CommandIconState::VALID);
                }
                else
                {
                    pCommandWidget_->setMoveIconState(MachGuiFPCommand::CommandIconState::INVALID);
                }
            }
        }

        // Display normal or startup cursor - This is the CROSSHAIRS btw
        if ( pStartCursor_->cellIndex() == pStartCursor_->numCells() - 1 )
        {
            pNormalCursor_->isVisible( not viableTarget or not viableShootingTarget );
            pStartCursor_->isVisible( false );
        }
        else
        {
            pNormalCursor_->isVisible( false );
            pStartCursor_->isVisible( not viableTarget );

            // End startup cursor animation if there is a valid target
            if ( viableTarget )
            {
                pStartCursor_->jumpToCell( pStartCursor_->numCells() - 1 );
            }
        }

        //Fire enabled weapons if requested
        if( commandList_[FIRE].on() or DevMouse::instance().leftButton() )
        {
            bool canFire = false;
            // Check that at least one weapon can fire
            for ( uint weaponIndex = 0; weaponIndex < logHandler.nWeapons() and not canFire; ++weaponIndex )
            {
                if ( not targetingInfo.shootingTarget and logHandler.weaponCanOnlyFireAtActor( weaponIndex ) )
                {
                    // canFire remains as false
                }
                else
                {
                    if ( logHandler.weapon( weaponIndex ).percentageRecharge() >= 100 and
                         logHandler.isWeaponEnabled( weaponIndex ) )
                    {
                        // We've found a weapon that can fire
                        canFire = true;
                    }
                }
            }

            if ( not canFire )
            {
                // timeWeaponsFired_ stops sound from being triggered over and over
                if ( now - timeWeaponsFired_ > 0.8 )
                {
                    MachGuiSoundManager::instance().playSound( "gui/sounds/nowefire.wav" );
                    timeWeaponsFired_ = now;
                }
            }
            else
            {
                timeWeaponsFired_ = now;

                //Fire the weapons that can fire at points
                logHandler.fire( targetingInfo.shootingPoint );

                //Fire the weapons that can only fire at actors, if we have one
                if( targetingInfo.shootingTarget )
                    logHandler.fire( targetingInfo.shootingTarget );
            }
        }

        if (commandList_[COMMAND_ORDER_ATTACK].on() and canIssueCommands and viableTarget)
        {
            MachActor* pTarget = targetingInfo.getCommandTarget();
            if (pTarget)
            {
                pCommandWidget_->setAttackIconState(MachGuiFPCommand::CommandIconState::ACTIVATED);
                logHandler.getActiveSquadron().issueAttackCommand(pTarget);
            }
        }

        if (commandList_[COMMAND_ORDER_FOLLOW].on() and canIssueCommands)
        {
            if (not viableTarget) // FOLLOW SELF
            {
                pCommandWidget_->setFollowIconState(MachGuiFPCommand::CommandIconState::ACTIVATED);
                logHandler.getActiveSquadron().issueFollowCommand(pActor_);
            }
            else                  // FOLLOW FRIENDLY
            {
                MachActor* pTarget = targetingInfo.getCommandTarget();
                if (pTarget and pTarget->race() == pActor_->race())
                {
                    pCommandWidget_->setFollowIconState(MachGuiFPCommand::CommandIconState::ACTIVATED);
                    logHandler.getActiveSquadron().issueFollowCommand(pTarget);
                }
            }
        }

        if (commandList_[COMMAND_ORDER_MOVE].on() and canIssueCommands and viableMoveToTarget and not viableTarget and not moveIndicatorPresent)
        {
            const auto& point = targetingInfo.getCommandPoint();
            pCommandWidget_->setMoveIconState(MachGuiFPCommand::CommandIconState::ACTIVATED);
            logHandler.getActiveSquadron().issueMoveCommand(point);
            logHandler.displayMoveIndicator(point);
        }
	}

	pNormalCursor_->update();
	pAttackCursor_->update();
	pMissCursor_->update();
	pStartCursor_->update();

	// Make sure the FOW stuff carries on being unfogged.
	pInGameScreen_->updateWhilstInFirstPerson();

	changed();

	if ( justEnteredFirstPerson_ == true )
	{
		startupTimer_ = now;
	}

	if ( not finishedStartupSequence_ )
	{
		double timeDiff = now - startupTimer_;
		timeDiff *= (STARTUP_SEQUENCE_FRAMES / STARTUP_SEQUENCE_TIME);

		// Cause static effect when machine is first entered (switch on/off intermittently)
		if ( timeDiff < STARTUP_SEQUENCE_FRAMES or frameNumber_ < STARTUP_SEQUENCE_FRAMES )
		{
			// Check to see if we are already playing hit interference. Check to see if its ended
			if ( isHitInterferenceOn_ )
			{
				if ( now > hitInterferenceEndTime_ )
				{
					isHitInterferenceOn_ = false;
					pInGameScreen_->sceneManager().pDevice()->interferenceOff();
				}
			}

			if ( not isHitInterferenceOn_ )
			{
				isHitInterferenceOn_ = true;
				int randomInt = mexRandomInt( &hitInterferenceRandom_, 0, 2 );

				if ( randomInt == 0 ) // Turn interference off for a time period
				{
					hitInterferenceEndTime_ = now + mexRandomScalar( &hitInterferenceRandom_, 0.05, 0.15 );
				}
				else // Turn interference on
				{
					// Give a slight random element to the intensity of the interference
					int randomIntensityMod = mexRandomInt( &hitInterferenceRandom_, -2, 3 );

					double timeDiff = now - startupTimer_;
					timeDiff *= 20.0;

					double interferenceLevel = 10.0 - ( timeDiff + randomIntensityMod );
					interferenceLevel /= 10.0;

					// Make sure interference level falls between certain values
					if ( interferenceLevel < 0.1 )
					{
						interferenceLevel = 0.1;
					}
					else if ( interferenceLevel > 1.0 )
					{
						interferenceLevel = 1.0;
					}

					pInGameScreen_->sceneManager().pDevice()->interferenceOn( interferenceLevel );
					hitInterferenceEndTime_ = now + mexRandomScalar( &hitInterferenceRandom_, 0.05, 0.4 );
				}
			}
		}
		else
		{
			finishedStartupSequence_ = true;
			pInGameScreen_->sceneManager().pDevice()->interferenceOff();
		}
	}
	else
	{
        //Get the environment safely from the logical planet
        W4dEnvironment& env = MachLogPlanet::instance().surface()->environment();

		// Turn interference on/off when machine is hit (don't do it every time though cus it might be annoying)
		if ( pActor_ and isHitInterferenceOn_ )
		{
			if ( now > hitInterferenceEndTime_ )
			{
				isHitInterferenceOn_ = false;
				pInGameScreen_->sceneManager().pDevice()->interferenceOff();

				// Turn NVG on/off
				if ( not env.isNvgOn() and machineNVGOn_ )
				{
				 	env.nvgOn(true);
				}
				else if ( env.isNvgOn() and not machineNVGOn_ )
				{
				 	env.nvgOn(false);
				}
			}
		}
		else
		{
			// Machine hit? play static effect... ( 1 in 20 chance of effect being played )
			if ( pActor_ and W4dManager::instance().frameNumber() == pActor_->lastBeHitFrame() )
			{
				int randomInt = mexRandomInt( &hitInterferenceRandom_, 0, 20 );

				if ( randomInt == 0 )
				{
					// Turn interference on
					isHitInterferenceOn_ = true;
					// Randomise the time the static lasts for
			 		hitInterferenceEndTime_ = now + mexRandomScalar( &hitInterferenceRandom_, 0.1, 0.5 );
					// Randomise the intensity of the static
					double hitStaticIntensity = mexRandomInt( &hitInterferenceRandom_, 4, 8 );
					pInGameScreen_->sceneManager().pDevice()->interferenceOn( hitStaticIntensity / 10.0 );

					// NVG capable machine? Turn NVG on/off... ( 1 in 5 chance of effect being played )
					if ( pActor_->objectIsMachine() and pActor_->asMachine().hasNVG() )
					{
						randomInt = mexRandomInt( &hitInterferenceRandom_, 0, 5 );

						if ( randomInt == 0 )
						{
							if ( not env.isNvgOn() )
							{
								pInGameScreen_->sceneManager().pDevice()->interferenceOff();
								env.nvgOn( true );
							}
							else
							{
								env.nvgOn( false );
							}
						}
					}

					// Play hit sound effect
					MachGuiSoundManager::instance().playSound( "gui/sounds/machhit.wav" );
				}
			}
		}
	}

	// Make sure new gui sounds are played
	MachGuiSoundManager::instance().update();
}

//virtual
void MachGuiFirstPerson::doBecomeRoot()
{
	// De-pImpl_ variables used within this function.
   	CB_DEPIMPL( MachActor*, pActor_ );
	CB_DEPIMPL( bool, inFirstPerson_ );
	CB_DEPIMPL( bool, switchBackToGroundCamera_ );
	CB_DEPIMPL( MachInGameScreen*, pInGameScreen_ );
	CB_DEPIMPL( W4dSceneManager*, pSceneManager_ );
	CB_DEPIMPL( MachLog1stPersonHandler*, pLogHandler_ );
	CB_DEPIMPL( int, borderDrawCount_ );
	CB_DEPIMPL( int, weaponSelectIndex_ );
	CB_DEPIMPL( MachGuiRadar*, pRadar_ );
	CB_DEPIMPL( bool, justEnteredFirstPerson_ );
	CB_DEPIMPL( MachGuiAnimation*, pAttackCursor_ );
    CB_DEPIMPL( MachGuiAnimation*, pNormalCursor_ );
	CB_DEPIMPL( MachGuiAnimation*, pMissCursor_ );
	CB_DEPIMPL( MachGuiAnimation*, pStartCursor_ );
	CB_DEPIMPL( int, borderHeight_ );
	CB_DEPIMPL( bool, resolutionChanged_ );
	CB_DEPIMPL( bool, isDead_ );
	CB_DEPIMPL( GuiBitmap, weaponBackgroundBmp_ );
	CB_DEPIMPL( MachGuiInGameChatMessagesDisplay*, pChatMessageDisplay_ );
	CB_DEPIMPL( double, timeWeaponsFired_ );
	CB_DEPIMPL( MachActor*, pTargetActor_ );
	CB_DEPIMPL( bool, reverseUpDownKeys_ );
	CB_DEPIMPL( bool, reverseUpDownMouse_ );
	CB_DEPIMPL( bool, finishedStartupSequence_ );
	CB_DEPIMPL( bool, isHitInterferenceOn_ );
	CB_DEPIMPL( int, frameNumber_ );
	CB_DEPIMPL( bool, machineNVGOn_ );
    CB_DEPIMPL( MachGuiFPCommand*, pCommandWidget_);

	// Just about to enter machine so initialise the startup sequence
	finishedStartupSequence_ = false;
	isHitInterferenceOn_ = false;
	frameNumber_ = 0;
	machineNVGOn_ = false;

	RenDevice& device = *pSceneManager_->pDevice();
	const int h = device.windowHeight();
	const int w = device.windowWidth();

	if ( resolutionChanged_ )
	{
		// Work out border height ( black bars at top and bottom of screen ).
	    borderHeight_ = h * 0.05;

		// Clear up old cursors
		_DELETE( pStartCursor_ );
		_DELETE( pNormalCursor_ );
		_DELETE( pAttackCursor_ );
		_DELETE( pMissCursor_ );

		//Load cursors...
		GuiBitmap attachCursorBmp 	= Gui::bitmap( "gui/fstpersn/cursor/targv1.bmp" );
		GuiBitmap normalCursorBmp 	= Gui::bitmap( "gui/fstpersn/cursor/targ1.bmp" );
		GuiBitmap missCursorBmp 	= Gui::bitmap( "gui/fstpersn/cursor/targnv1.bmp" );
		GuiBitmap startCursorBmp 	= Gui::bitmap( "gui/fstpersn/cursor/ping1.bmp" );

		pStartCursor_ = MachGuiAnimation::createAnimation( this, SysPathName("gui/fstpersn/cursor/ping.anm" ), true, (w/2) - (startCursorBmp.width() / 2), (h/2) - (startCursorBmp.height() / 2) );
		pNormalCursor_ = MachGuiAnimation::createAnimation( this, SysPathName("gui/fstpersn/cursor/targ.anm" ), true, (w/2) - (normalCursorBmp.width() / 2), (h/2) - (normalCursorBmp.height() / 2) );
		pAttackCursor_ = MachGuiAnimation::createAnimation( this, SysPathName("gui/fstpersn/cursor/targv.anm" ), true, (w/2) - (attachCursorBmp.width() / 2), (h/2) - (attachCursorBmp.height() / 2) );
		pMissCursor_ = MachGuiAnimation::createAnimation( this, SysPathName("gui/fstpersn/cursor/targnv.anm" ), true, (w/2) - (missCursorBmp.width() / 2), (h/2) - (missCursorBmp.height() / 2) );

		// Create radar
		GuiBitmap rcmMapBmp = Gui::bitmap("gui/fstpersn/radar/rmmap.bmp");
        if (pRadar_ != nullptr)
        {
            delete pRadar_;
        }

        pRadar_ = _NEW( MachGuiRadar( this, Gui::Coord( w-rcmMapBmp.width(), h-borderHeight_-rcmMapBmp.height() ) ) );
		pRadar_->actor( pActor_ );

		// Reset resolution changed flag
		resolutionChanged_ = false;
	}

	// Reposition debug text
	pSceneManager_->pDevice()->debugTextCoords(0, borderHeight_ + 1);

	inFirstPerson_ = true;

	// Store camera type use in game
	switchBackToGroundCamera_ = pInGameScreen_->cameras()->isGroundCameraActive();

    // Create FP Command Widget
    if (pCommandWidget_ == nullptr)
    {
        pCommandWidget_ = new MachGuiFPCommand(this, Gui::Coord(21, h-187));
    }

    //Embody the actor
    embodyActor();

    //Attach the camera to the machine
    attachCamera();

	// Set mouse pointer to middle of screen...
	// Get screen size.
    const double halfScreenWidth = (double)device.windowWidth() / 2.0;
	const double halfScreenHeight = (double)device.windowHeight() / 2.0;

	DevMouse::instance().changePosition( halfScreenWidth, halfScreenHeight );

	// Initialise the log handler
	if ( pLogHandler_ )
	{
		pLogHandler_->update();
		weaponSelectIndex_ = pLogHandler_->nWeapons();
	}

	// Create chat message display
	if ( MachLogNetwork::instance().isNetworkGame() )
	{
		const int chatMessagesX = 0;
		int chatMessagesY = borderHeight_;

		if ( pLogHandler_ and pLogHandler_->nWeapons() != 0 )
		{
			chatMessagesY = borderHeight_ + weaponBackgroundBmp_.height() + 4;
		}

		pChatMessageDisplay_ = _NEW( MachGuiInGameChatMessagesDisplay( 	this,
																		Gui::Box( Gui::Coord( chatMessagesX, chatMessagesY ),
																				   MachGuiInGameChatMessages::reqWidth(),
																				   MachGuiInGameChatMessages::reqHeight() ) ) );
	}

	// Initialise black border drawing count ( black borders only get drawn for first two frames )
	borderDrawCount_ = 2;

	// Load up icons for weapons, animation etc
	loadBitmaps();

	pRadar_->initialise();

	justEnteredFirstPerson_ = true;

	// Switch startup cursor on, all others off.
	pAttackCursor_->isVisible( false );
	pNormalCursor_->isVisible( false );
	pMissCursor_->isVisible( false );
	pStartCursor_->isVisible( true );

	// Remove FOW interference
	pSceneManager_->pDevice()->interferenceOff();

	// Reset isDead flag, stops us from being throw out of first person.
	isDead_ = false;

	pTargetActor_ = NULL;

	reverseUpDownKeys_ = SysRegistry::instance().queryIntegerValue( "Options\\Reverse UpDown Keys", "on", SysRegistry::CURRENT_USER );
	reverseUpDownMouse_ = SysRegistry::instance().queryIntegerValue( "Options\\Reverse BackForward Mouse", "on", SysRegistry::CURRENT_USER );
}

//virtual
void MachGuiFirstPerson::doBecomeNotRoot()
{
	// De-pImpl_ variables used within this function.
	CB_DEPIMPL( MachActor*, pActor_ );
	CB_DEPIMPL( bool, inFirstPerson_ );
	CB_DEPIMPL( MachInGameScreen*, pInGameScreen_ );
	CB_DEPIMPL( bool, switchBackToGroundCamera_ );
	CB_DEPIMPL( DevKeyToCommandTranslator::CommandList, commandList_ );
	CB_DEPIMPL( DevKeyToCommandTranslator*, pKeyTranslator_ );
	CB_DEPIMPL( MachGuiAnimation*, pAttackCursor_ );
    CB_DEPIMPL( MachGuiAnimation*, pNormalCursor_ );
	CB_DEPIMPL( MachGuiAnimation*, pMissCursor_ );
	CB_DEPIMPL( MachGuiAnimation*, pStartCursor_ );
	CB_DEPIMPL( W4dSceneManager*, pSceneManager_ );
	CB_DEPIMPL( MachGuiInGameChatMessagesDisplay*, pChatMessageDisplay_ );
	CB_DEPIMPL( bool, switchToMenus_ );

    //Detach the camera and exit the actor
    detachCamera();
    exitActor();

	inFirstPerson_ = false;

	// Reset all the commands.
	pKeyTranslator_->resetCommands( &commandList_, true );

	// Free up some memory
	unloadBitmaps();

	// Switch startup cursor on, all others off.
	pAttackCursor_->isVisible( false );
	pNormalCursor_->isVisible( false );
	pMissCursor_->isVisible( false );
	pStartCursor_->isVisible( true );

	// Clean up chat message display
	_DELETE( pChatMessageDisplay_ );
	pChatMessageDisplay_ = NULL;

	//Stop all playing sounds if we are going into menus
	if ( switchToMenus_ )
	{
		W4dSoundManager::instance().stopAll();
	}

	// Clean up any gui sounds that are currently playing
	MachGuiSoundManager::instance().clearAll();
}

//virtual
bool MachGuiFirstPerson::doHandleRightClickEvent( const GuiMouseEvent& event )
{
	// De-pImpl_ variables used within this function.
	CB_DEPIMPL( double, lastRightClickTime_ );
	CB_DEPIMPL( bool, switchToInGame_ );

	// Right button used to exit 1st person	( when double clicked )
	if ( event.rightButton() == Gui::RELEASED )
	{
		if ( DevTime::instance().time() - lastRightClickTime_ < 0.5 )
		{
			switchToInGame_ = true;
		}
		else
		{
			lastRightClickTime_ = DevTime::instance().time();
		}
	}

    return true;
}

// virtual
bool MachGuiFirstPerson::doHandleKeyEvent( const GuiKeyEvent& event )
{
	// De-pImpl_ variables used within this function.
	CB_DEPIMPL( DevKeyToCommandTranslator::CommandList, commandList_ );
	CB_DEPIMPL( DevKeyToCommandTranslator*, pKeyTranslator_ );
	CB_DEPIMPL( bool, switchToInGame_ );
	CB_DEPIMPL( bool, switchToMenus_ );
	CB_DEPIMPL( MachLog1stPersonHandler*, pLogHandler_ );
	CB_DEPIMPL( W4dSceneManager*, pSceneManager_ );
	CB_DEPIMPL( MachInGameScreen*, pInGameScreen_ );
	CB_DEPIMPL( MachActor*, pActor_ );
	CB_DEPIMPL( bool, switchBackToGroundCamera_ );
	CB_DEPIMPL( bool, machineNVGOn_ );

	bool processed = false;
	bool finished = false;
	unsigned int count = 0;

	// If the game has been won/lost then only allow escape or space key to work
	if ( pInGameScreen_->actualGameState() != MachInGameScreen::PLAYING )
	{
		processed = true;

		if ( ( event.key() == DevKey::ESCAPE or event.key() == DevKey::SPACE ) and event.state() == Gui::PRESSED )
		{
			switchToInGame_ = true;
		}
	}

	while ( not finished and not processed )
	{
		switch ( count )
		{
			case 0:	 // Weapon selection
				{
					DevKeyToCommandTranslator::CommandId commandId;
					bool found = pKeyTranslator_->translate( event.buttonEvent(), &commandId );
					if ( found and
						 commandId == WEAPONSELECT and
						 event.state() == Gui::RELEASED and
						 pLogHandler_ and
						 pActor_ )
					{
						processed = true;
						doWeaponSelect();
					}
				}
				break;
			case 1: // Weapon fire, general machine motion
				processed = pKeyTranslator_->translate( event.buttonEvent(), &commandList_ );
				break;
			case 2: // Exit first person
				if ( event.key() == DevKey::ESCAPE and event.state() == Gui::PRESSED )
				{
					switchToInGame_ = true;
					processed = true;
				}
				else if ( event.key() == DevKey::PAD_0 and event.state() == Gui::PRESSED )
				{
					switchBackToGroundCamera_ = true;
					switchToInGame_ = true;
					processed = true;
				}
				else if ( event.key() == DevKey::PAD_2 and event.state() == Gui::PRESSED )
				{
					switchBackToGroundCamera_ = false;
					switchToInGame_ = true;
					processed = true;
				}
				break;
			case 3: // Switch to ingame menus
				if ( event.key() == DevKey::F10 and event.state() == Gui::PRESSED )
				{
					switchToMenus_ = true;
					processed = true;
				}
			case 4: // Screen shot
				if( event.key() == DevKey::F12 and event.isShiftPressed() and event.isCtrlPressed() and event.state() == Gui::PRESSED )
				{
    		        pInGameScreen_->initiateScreenShot();
					processed = true;
				}
		   	case 5:
				if ( event.key() == DevKey::KEY_N and event.state() == Gui::PRESSED and pActor_ )
				{
					if( pActor_->objectIsMachine() and not pActor_->asMachine().hasNVG() )
					{
						// do nothing
					}
					else
					{
                        //Get the environment safely from the logical planet
                        W4dEnvironment& env = MachLogPlanet::instance().surface()->environment();

						if ( env.isNvgOn() )
						{
							env.nvgOn(false);
							machineNVGOn_ = false;
						}
						else
						{
							env.nvgOn(true);
							machineNVGOn_ = true;
						}
					}
					processed = true;
				}
			case 6: // Pause game
	        	if( event.key() == DevKey::BREAK and event.state() == Gui::PRESSED )
				{
					// Can't pause game in multiplayer games
					if ( not MachLogNetwork::instance().isNetworkGame() )
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
		++count;
	}

	return processed;
}

bool MachGuiFirstPerson::switchToMenus() const
{
	// De-pImpl_ variables used within this function.
	CB_DEPIMPL( bool, switchToMenus_ );

	return switchToMenus_;
}

bool MachGuiFirstPerson::switchToInGame() const
{
	// De-pImpl_ variables used within this function.
	CB_DEPIMPL( bool, switchToInGame_ );

	return switchToInGame_;
}

void MachGuiFirstPerson::resetSwitchToMenus()
{
	// De-pImpl_ variables used within this function.
	CB_DEPIMPL( bool, switchToMenus_ );

	switchToMenus_ = false;
}

void MachGuiFirstPerson::resetSwitchToInGame()
{
	// De-pImpl_ variables used within this function.
	CB_DEPIMPL( bool, switchToInGame_ );

	switchToInGame_ = false;
}

void MachGuiFirstPerson::setActor( MachActor* pActor )
{
	// De-pImpl_ variables used within this function.
	CB_DEPIMPL( MachActor*, pActor_ );
	CB_DEPIMPL( MachGuiRadar*, pRadar_ );

	if ( pActor->objectIsMachine() )
	{
		pActor_ = pActor;
		if ( pRadar_ )
			pRadar_->actor( pActor_ );

	    //Become an observer, in case it gets deleted etc
	    pActor_->attach( this );
	}
}

void MachGuiFirstPerson::resetActor()
{
	// De-pImpl_ variables used within this function.
	CB_DEPIMPL( MachActor*, pActor_ );
	CB_DEPIMPL( MachGuiRadar*, pRadar_ );

	// Stop observing
	if ( pActor_ )
		pActor_->detach( this );

	pActor_ = NULL;
	if ( pRadar_ )
		pRadar_->resetActor();
}

//virtual
bool MachGuiFirstPerson::beNotified( W4dSubject* pSubject, W4dSubject::NotificationEvent event, int clientData )
{
	// De-pImpl_ variables used within this function.
	CB_DEPIMPL( MachActor*, pActor_ );
	CB_DEPIMPL( bool, isDead_ );
	CB_DEPIMPL( PhysAbsoluteTime, timeOfDeath_ );
	CB_DEPIMPL( MachInGameScreen*, pInGameScreen_ );
	CB_DEPIMPL( MachGuiRadar*, pRadar_ );
	CB_DEPIMPL( bool, inFirstPerson_ );

	ASSERT( ( not pActor_ ) or pSubject == pActor_, "MachGuiFirstPerson::beNotified notified about wrong actor" );

    bool exitFromActor = false;

    //We want to deselect the actor if being deleted
    switch( event )
    {
        case W4dSubject::DELETED:
            exitFromActor = true;
            break;
		case W4dSubject::CLIENT_SPECIFIC:
			if ( clientData == MachLog::RACE_CHANGED )
			{
				exitFromActor = true;
			}
			break;
    }

    //Cancel the selection if required
    if( exitFromActor )
    {
		MexTransform3d newPos;
		if ( inFirstPerson_ )
		{
			// Get position of camera
			newPos = pInGameScreen_->cameras()->currentCamera()->globalTransform();

			// Detach first person camera
			detachCamera();
		}

        pActor_ = NULL;

		if ( pRadar_ )
			pRadar_->resetActor();

		if ( inFirstPerson_ )
		{
			// Note : exitActor causes beNotified to get called again, this is undesirable but there
			// ain't much we can do about it. Calling detach won't work during a beNotified. The ASSERT
			// above seems odd but it is written like this to cope with the second beNotified event.
			exitActor();

			if ( not isDead_ )
			{
				isDead_ = true;
				timeOfDeath_ = Phys::time();
			}

			// Use ground camera at the position of the face plate for up to the next 0.5 second
			// whilst we show a static inteference effect showing that the machine has blown up.
			pInGameScreen_->cameras()->useGroundCamera();
			pInGameScreen_->cameras()->lookAt( newPos.position() );
		}
	}

    return not exitFromActor;
}

//virtual
void MachGuiFirstPerson::domainDeleted( W4dDomain* )
{
    //Do nothing
}

bool MachGuiFirstPerson::okayToSwitchTo1stPerson()
{
	// De-pImpl_ variables used within this function.
	CB_DEPIMPL( MachActor*, pActor_ );
	CB_DEPIMPL( MachInGameScreen*, pInGameScreen_ );

	// Can only enter 1st person in a multiplayer game if it hasn't been disabled.
	if ( pInGameScreen_->isFirstPersonDisabled() )
	{
		return false;
	}

	MachPhys::Race playerRace = MachLogRaces::instance().pcController().race();

	// Check that the actor can be entered.
	if ( pActor_ and
		 pActor_->objectIsMachine() and
		 pActor_->race() == playerRace and
		 not pActor_->asMachine().insideAPC() and
		 not pActor_->isDead() )
	{
		return true;
	}

	return false;
}

void MachGuiFirstPerson::setFirstPerson3DViewport()
{
	// De-pImpl_ variables used within this function.
	CB_DEPIMPL( W4dSceneManager*, pSceneManager_ );
	CB_DEPIMPL( int, borderHeight_ );
	CB_DEPIMPL( int, lastBorderHeight_ );
	CB_DEPIMPL( bool, justEnteredFirstPerson_ );
	CB_DEPIMPL( bool, finishedStartupSequence_ );
	CB_DEPIMPL( double, startupTimer_ );
	CB_DEPIMPL( int, frameNumber_ );

	if ( justEnteredFirstPerson_ == true )
	{
		startupTimer_ = DevTime::instance().time();
	}

     //Set the viewport boundary for the first person window
    RenDevice& device = *pSceneManager_->pDevice();
    const int w = device.windowWidth();
	const int h = device.windowHeight();

	lastBorderHeight_ = borderHeight_;

	if ( not finishedStartupSequence_ )
	{
		double now = DevTime::instance().time();
		double timeDiff = now - startupTimer_;
		timeDiff *= (STARTUP_SEQUENCE_FRAMES / STARTUP_SEQUENCE_TIME);;

		lastBorderHeight_ = ( h / 2 ) - 10;
		double diffHeight = ( h / 2 ) - borderHeight_;
		diffHeight /= STARTUP_SEQUENCE_FRAMES;

		int borderHeight1 = diffHeight * timeDiff;
		int borderHeight2 = diffHeight * frameNumber_;

		lastBorderHeight_ -= std::min( borderHeight1, borderHeight2 );

		if ( lastBorderHeight_ < borderHeight_ )
		{
			lastBorderHeight_ = borderHeight_;
		}

		WAYNE_STREAM( "calculating viewport timeDiff " << timeDiff << " lastBorderHeight_ " << lastBorderHeight_ << std::endl );
	}

	device.setViewport( 0, lastBorderHeight_, w, h - ( 2 * lastBorderHeight_ ) );
}

void MachGuiFirstPerson::embodyActor()
{
	// De-pImpl_ variables used within this function.
	CB_DEPIMPL( MachActor*, pActor_ );
	CB_DEPIMPL( MachLog1stPersonHandler*, pLogHandler_ );
	CB_DEPIMPL( MachGuiRadar*, pRadar_ );
	CB_DEPIMPL( MachInGameScreen*, pInGameScreen_ );
	CB_DEPIMPL( bool, rightMouseButtonHeadTurningUsed_ );
    CB_DEPIMPL( MachGuiFPCommand*, pCommandWidget_);

    PRE( pActor_ != NULL );
    PRE( pLogHandler_ == NULL );

    //Construct the appropriate type of handler depending on actor type
    MachActor& actor = *pActor_;

	// Remove selection box from around actor.
	actor.selectionState( MachLog::NOT_SELECTED );

    if( actor.objectIsMachine() )
    {
        pLogHandler_ = _NEW( MachLog1stPersonMachineHandler( &actor.asMachine(), MachLog1stPersonHandler::LOCAL ) );
		pRadar_->logHandler( pLogHandler_ );
        pCommandWidget_->logHandler(pLogHandler_);

        // If the embodied machine has a squadron, it shall be selected. Let's show that.
        if (pLogHandler_->getActiveSquadron().hasActiveSquadron())
        {
            pCommandWidget_->updateSquadIcon();
            pCommandWidget_->updateSquadNumber();
        }

		W4dEntity* thisEntity = _STATIC_CAST(W4dEntity*, &(pActor_->asMachine().physMachine()));
		W4dSoundManager::instance().stop(thisEntity);
		W4dSoundManager::instance().play(thisEntity, SID_INHEAD, 0, 0);

		rightMouseButtonHeadTurningUsed_ = false;
    }
    else
    {
        ASSERT_FAIL("Attempt to embody non-machine" );
    }

    //Enable all the weapons
    for( uint index = pLogHandler_->nWeapons(); index--; )
        pLogHandler_->enableWeapon( index, true );
}

void MachGuiFirstPerson::exitActor()
{
	// De-pImpl_ variables used within this function.
	CB_DEPIMPL( MachLog1stPersonHandler*, pLogHandler_ );
	CB_DEPIMPL( MachActor*, pActor_ );
	CB_DEPIMPL( MachGuiRadar*, pRadar_ );
    CB_DEPIMPL( MachGuiFPCommand*, pCommandWidget_);
    CB_DEPIMPL( int64_t, commandSquadIndex_);

    //Delete the handler
    _DELETE( pLogHandler_ );
    pLogHandler_ = NULL;

    // RESET RADAR (sorta...)
	pRadar_->resetLogHandler();

    // RESET COMMAND WIDGET
    pCommandWidget_->resetLogHandler();
    pCommandWidget_->clearSquadIcon();
    pCommandWidget_->resetSquadNumber();
    commandSquadIndex_ = -1L;

	// Put selection box back round actor.
	if ( pActor_ )
	{
		pActor_->selectionState( MachLog::SELECTED );

		//If the actor is a machine then we have to stop the
		//in head sound and play the idle sound
	    if ( pActor_->objectIsMachine() )
	    {
			MachLogMachine& logicalMachine = pActor_->asMachine();
			MachPhysMachine& thisMachine = logicalMachine.physMachine();
			W4dEntity* thisEntity = _STATIC_CAST(W4dEntity*, &(pActor_->asMachine().physMachine()));
			W4dSoundManager::instance().stop(thisEntity);
			SoundId moveSound;
			SoundId idleSound;
			thisMachine.getLocomotionSounds(&idleSound, &moveSound, thisMachine.machineData());
			W4dSoundManager::instance().play(thisEntity, idleSound, 0, 0);

			if( logicalMachine.objectIsCanAttack() )
			{
				// ensure all machines from which you manually exit first person are left in
				//a state of high alertness
				logicalMachine.asCanAttack().setMinimumAlertness( 125 );
			}

		}
	}
}

void MachGuiFirstPerson::detachCamera()
{
    // De-pImpl_ variables used within this function.
	CB_DEPIMPL( MachInGameScreen*, pInGameScreen_ );
	CB_DEPIMPL( bool, switchBackToGroundCamera_ );
	CB_DEPIMPL( MachActor*, pActor_ );
	CB_DEPIMPL( W4dSceneManager*, pSceneManager_ );

	// Store 1st person camera
	MachLogCamera* p1stPersonCamera = NULL;
    if ( pInGameScreen_->cameras()->is1stPersonCameraActive() )
	{
		p1stPersonCamera = pInGameScreen_->cameras()->currentCamera();
	}

	// Switch to ground camera or zenith camera. 1st person camera is useless in the normal in game screen.
	if ( switchBackToGroundCamera_ )
	{
		pInGameScreen_->cameras()->useGroundCamera();

		if ( pActor_ )
		{
			pInGameScreen_->cameras()->lookAt( *pActor_ );
		}
		else
		{
			MexTransform3d newPos;
		   	pInGameScreen_->cameras()->currentCamera()->globalTransform().transform( MexPoint3d( -10.0, 0.0, 0.0 ), &newPos );

			pInGameScreen_->cameras()->lookAt( newPos.position() );
		}
	}
	else
	{
		pInGameScreen_->cameras()->useZenithCamera();
	}

    //Attach first person camera directly to any planet domain
	if ( p1stPersonCamera )
	{
    	p1stPersonCamera->attachTo( MachLogPlanetDomains::pDomainAt( MexPoint2d(0,0) ), MexTransform3d() );
	}

	// Night Vision Goggles off.
	W4dEnvironment& env = MachLogPlanet::instance().surface()->environment();
	env.nvgOn(false);
}

void MachGuiFirstPerson::attachCamera()
{
    // De-pImpl_ variables used within this function.
	CB_DEPIMPL( MachLog1stPersonHandler*, pLogHandler_ );
	CB_DEPIMPL( MachInGameScreen*, pInGameScreen_ );
	CB_DEPIMPL( W4dSceneManager*, pSceneManager_ );

    PRE( pLogHandler_ != NULL );

    //Select the appropriate camera
	pInGameScreen_->cameras()->use1stPersonCamera();
	MachLogCamera* pCamera = pInGameScreen_->cameras()->currentCamera();

    //Attach it to the actor
    MexTransform3d offsetTransform;
    W4dEntity& cameraParent = pLogHandler_->cameraAttachment( &offsetTransform );

    pCamera->attachTo( &cameraParent, offsetTransform );

    //Tell the handler to do its camera related initialisation
    pLogHandler_->initialiseCamera( pCamera );

	// Night Vision Goggles off.
	W4dEnvironment& env = MachLogPlanet::instance().surface()->environment();
	env.nvgOn(false);

	pCamera->update();
}

void MachGuiFirstPerson::displayCompass()
{
    // De-pImpl_ variables used within this function.
	CB_DEPIMPL( MachActor*, pActor_ );
	CB_DEPIMPL( GuiBitmap, compassBmp_ );
	CB_DEPIMPL( W4dSceneManager*, pSceneManager_ );
	CB_DEPIMPL( MachLog1stPersonHandler*, pLogHandler_ );
	CB_DEPIMPL( MachGuiAnimation*, pStartCursor_ );

	// Only display compass if start cursor has finished animating
	if ( pActor_ and not pStartCursor_->isVisible() )
	{
		// Get screen size.
	    RenDevice& device = *pSceneManager_->pDevice();
	    const int halfScreenWidth = device.windowWidth() / 2;
		const int halfScreenHeight = device.windowHeight() / 2;

		MexTransform3d trans = pActor_->globalTransform();
		MexEulerAngles angles = trans.rotationAsEulerAngles();
		MexDegrees azimuth = angles.azimuth();

		// Add head turn ( if machine can turn head! )
		if ( pLogHandler_ and pLogHandler_->canTurnHead() )
		{
			azimuth += pLogHandler_->currentHeadAngle();
		}

		MATHEX_SCALAR heading = azimuth.asScalar() + 90.0;

		// Force degrees to lie between 0 - 360.
		while ( heading < 0.0 )
			heading += 360.0;

		while ( heading >= 360.0 )
			heading -= 360.0;

		const int compassZeroOffset = 38;
		const int compass360Width = 160;
		const int compassDisplayWidth = 48;

		int compassOffset = heading / 360.0 * compass360Width;

		int startBmpPos = compassZeroOffset + compassOffset - ( compassDisplayWidth / 2 );

		const int compassXOffset = -24;
		const int compassYOffset = 26;

		GuiPainter::instance().blit(compassBmp_,
									Gui::Box( startBmpPos, 0, startBmpPos+compassDisplayWidth, compassBmp_.height() ),
									Gui::Coord( halfScreenWidth+compassXOffset, halfScreenHeight+compassYOffset ) );
	}
}

void MachGuiFirstPerson::displayHealthArmour()
{
    // De-pImpl_ variables used within this function.
	CB_DEPIMPL( MachActor*, pTargetActor_ );
	CB_DEPIMPL( GuiBitmap, healthBmp_ );
	CB_DEPIMPL( GuiBitmap, armourBmp_ );
	CB_DEPIMPL( W4dSceneManager*, pSceneManager_ );

	if ( pTargetActor_ )
	{
		// Get screen size.
	    RenDevice& device = *pSceneManager_->pDevice();
	    const int halfScreenWidth = device.windowWidth() / 2;
		const int halfScreenHeight = device.windowHeight() / 2;

		const MachPhysObjectData& objData = pTargetActor_->objectData();

		MachPhys::ArmourUnits maxAp = objData.armour();
		MachPhys::HitPointUnits maxHp = objData.hitPoints();

		MachPhys::ArmourUnits ap = pTargetActor_->armour();
		MachPhys::HitPointUnits hp = pTargetActor_->hp();

		const double healthHeight = healthBmp_.height();
		const double armourHeight = armourBmp_.height();

		double displayHealthHeight = ( (double)hp / (double)maxHp ) * healthHeight;
		double displayArmourHeight = ( (double)ap / (double)maxAp ) * armourHeight;

		const int healthBarXOffset = -33;
		const int armourBarXOffset = 29;
		const int barYOffset = -9;

		GuiPainter::instance().blit(healthBmp_,
									Gui::Box( 0, healthHeight-displayHealthHeight, healthBmp_.width(), healthHeight ),
									Gui::Coord( halfScreenWidth+healthBarXOffset, halfScreenHeight+barYOffset+healthHeight-displayHealthHeight ) );
		GuiPainter::instance().blit(armourBmp_,
									Gui::Box( 0, armourHeight-displayArmourHeight, armourBmp_.width(), armourHeight),
									Gui::Coord( halfScreenWidth+armourBarXOffset, halfScreenHeight+barYOffset+armourHeight-displayArmourHeight ) );
	}
}

//virtual
void MachGuiFirstPerson::doHandleMouseClickEvent( const GuiMouseEvent& /*rel*/ )
{}

void MachGuiFirstPerson::displayWeapons()
{
	// De-pImpl_ variables used within this function.
	CB_DEPIMPL( GuiBitmap, leftWeaponBmp_ );
	CB_DEPIMPL( GuiBitmap, rightWeaponBmp_ );
	CB_DEPIMPL( GuiBitmap, topWeaponBmp_ );
	CB_DEPIMPL( GuiBitmap, weaponBackgroundBmp_ );
	CB_DEPIMPL( int, borderHeight_ );
	CB_DEPIMPL( bool, justEnteredFirstPerson_ );
	CB_DEPIMPL( double, leftWeaponChangeEndTime_ );
	CB_DEPIMPL( double, topWeaponChangeEndTime_ );
	CB_DEPIMPL( double, rightWeaponChangeEndTime_ );
	CB_DEPIMPL( int, leftWeaponPos_ );
	CB_DEPIMPL( int, topWeaponPos_ );
	CB_DEPIMPL( int, rightWeaponPos_ );

	if ( justEnteredFirstPerson_ == true )
	{
		double now = DevTime::instance().time();

		leftWeaponChangeEndTime_ = now + WEAPON_DROPDOWN_TIME;
		topWeaponChangeEndTime_ = now + WEAPON_DROPDOWN_TIME + WEAPON_DROPDOWN_DELAYTIME;
		rightWeaponChangeEndTime_ = now + WEAPON_DROPDOWN_TIME + (2 * WEAPON_DROPDOWN_DELAYTIME);

		double timePerFrame = WEAPON_DROPDOWN_TIME / WEAPON_DROPDOWN_FRAMES;
		double framesBehind = WEAPON_DROPDOWN_DELAYTIME / timePerFrame;

		leftWeaponPos_ = -1;
		topWeaponPos_ = framesBehind - 1;
		rightWeaponPos_ = ( 2 * framesBehind ) - 1;
	}

	const int weaponIconWidthSpacer = 2;

	Gui::Coord topLeft(1,borderHeight_);

	// Display LEFT weapon first
	if ( displayWeapon( MachPhys::LEFT, topLeft, leftWeaponBmp_, leftWeaponChangeEndTime_, leftWeaponPos_ ) )
	{
		topLeft.x( topLeft.x() + weaponBackgroundBmp_.width() + weaponIconWidthSpacer );
	}
	// Display TOP weapon
	if ( displayWeapon( MachPhys::TOP, topLeft, topWeaponBmp_, topWeaponChangeEndTime_, topWeaponPos_ ) )
	{
		topLeft.x( topLeft.x() + weaponBackgroundBmp_.width() + weaponIconWidthSpacer );
	}
	// Display RIGHT weapon
	displayWeapon( MachPhys::RIGHT, topLeft, rightWeaponBmp_, rightWeaponChangeEndTime_, rightWeaponPos_ );
}

bool MachGuiFirstPerson::displayWeapon( MachPhys::Mounting mounting, const Gui::Coord& topLeft, const GuiBitmap& weaponBmp, double changeEndTime, int& weaponFrame )
{
	// De-pImpl_ variables used within this function.
	CB_DEPIMPL( MachLog1stPersonHandler*, pLogHandler_ );
	CB_DEPIMPL( GuiBitmap, weaponBackgroundBmp_ );
	CB_DEPIMPL( GuiBitmap, weaponChargeBmp_ );
	CB_DEPIMPL_ARRAY( GuiBitmap, weaponStartupFrames_ );

	bool foundWeapon = false;

	if ( pLogHandler_ )
	{
		for ( int index = 0; index < pLogHandler_->nWeapons() and not foundWeapon; ++index )
		{
			MachLogWeapon& weapon = pLogHandler_->weapon( index );
			if ( weapon.mounting() == mounting )
			{
				foundWeapon = true;

				double now = DevTime::instance().time();
				double timeDiff = changeEndTime - now;
				timeDiff *= ( WEAPON_DROPDOWN_FRAMES / WEAPON_DROPDOWN_TIME );
				int frame = 0;

				// Check if weapon is enabled, change frame shown based on this
				if ( pLogHandler_->isWeaponEnabled( index ) )
				{
					frame = WEAPON_DROPDOWN_FRAMES - timeDiff;

					if ( weaponFrame != WEAPON_DROPDOWN_FRAMES )
					{
						++weaponFrame;
					}

					frame = std::min( weaponFrame, frame );
				}
				else
				{
					frame = timeDiff;

					if ( weaponFrame != 0 )
					{
						--weaponFrame;
					}

					frame = std::max( weaponFrame, frame );
				}

				if ( frame >= WEAPON_DROPDOWN_FRAMES )
				{
					GuiPainter::instance().blit( weaponBackgroundBmp_, topLeft );

					// Weapon bitmap offset
					Gui::Coord wrcTopLeft( topLeft );
					wrcTopLeft.x( wrcTopLeft.x() + 8 );
					wrcTopLeft.y( wrcTopLeft.y() + 33 );

					GuiPainter::instance().blit( weaponBmp, wrcTopLeft );

					wrcTopLeft.x( topLeft.x() + 9 );
					wrcTopLeft.y( topLeft.y() + 19 );

					double chargeDisplayWidth = ( (double)weaponChargeBmp_.width() / 100.0 ) * weapon.percentageRecharge();

					GuiPainter::instance().blit(weaponChargeBmp_,
												Gui::Box( 0, 0, chargeDisplayWidth, weaponChargeBmp_.height() ),
												wrcTopLeft );
				}
				else
				{
					ASSERT( frame < WEAPON_DROPDOWN_FRAMES, "No more than 10 frames allowed for animated weapon drop down." );

					int displayFrame = std::max( frame, 0 );
				 	// Blit to screen.
					GuiPainter::instance().blit( weaponStartupFrames_[displayFrame], topLeft );
				}
			}
		}
	}

	return foundWeapon;
}

void MachGuiFirstPerson::activate()
{
    // De-pImpl_ variables used within this function.
	CB_DEPIMPL( int, borderDrawCount_ );

	borderDrawCount_ = 2;
}

void MachGuiFirstPerson::loadWeaponBmps()
{
	// De-pImpl_ variables used within this function.
	CB_DEPIMPL( GuiBitmap, leftWeaponBmp_ );
	CB_DEPIMPL( GuiBitmap, rightWeaponBmp_ );
	CB_DEPIMPL( GuiBitmap, topWeaponBmp_ );
	CB_DEPIMPL( MachLog1stPersonHandler*, pLogHandler_ );

	for ( int index = 0; index < pLogHandler_->nWeapons(); ++index )
	{
		MachLogWeapon& weapon = pLogHandler_->weapon( index );
		switch ( weapon.mounting() )
		{
			case MachPhys::LEFT:
				leftWeaponBmp_ = getWeaponBmp( weapon.type() );
				leftWeaponBmp_.enableColourKeying();
				break;
			case MachPhys::RIGHT:
				rightWeaponBmp_ = getWeaponBmp( weapon.type() );
			   	rightWeaponBmp_.enableColourKeying();
				break;
			case MachPhys::TOP:
				topWeaponBmp_ = getWeaponBmp( weapon.type() );
				topWeaponBmp_.enableColourKeying();
				break;
		}
	}
}

GuiBitmap MachGuiFirstPerson::getWeaponBmp( MachPhys::WeaponType wt )
{
	GuiBitmap retVal;

	switch ( wt )
	{
		case MachPhys::ELECTRIC_CHARGE:
			retVal = Gui::bitmap( "gui/fstpersn/weapon/echarge.bmp" );
			break;
		case MachPhys::BOLTER:
			retVal = Gui::bitmap( "gui/fstpersn/weapon/bolter.bmp" );
			break;
		case MachPhys::HEAVY_BOLTER1:
			retVal = Gui::bitmap( "gui/fstpersn/weapon/hbolter.bmp" );
			break;
		case MachPhys::HEAVY_BOLTER2:
			retVal = Gui::bitmap( "gui/fstpersn/weapon/hbolter2.bmp" );
			break;
		case MachPhys::FLAME_THROWER1:
			retVal = Gui::bitmap( "gui/fstpersn/weapon/flame1.bmp" );
			break;
		case MachPhys::FLAME_THROWER2:
			retVal = Gui::bitmap( "gui/fstpersn/weapon/flame2.bmp" );
			break;
		case MachPhys::AUTO_CANNON:
			retVal = Gui::bitmap( "gui/fstpersn/weapon/autoc.bmp" );
			break;
		case MachPhys::SUPERCHARGE_ADVANCED:
			retVal = Gui::bitmap( "gui/fstpersn/weapon/heala.bmp" );
			break;
		case MachPhys::SUPERCHARGE_SUPER:
			retVal = Gui::bitmap( "gui/fstpersn/weapon/heals.bmp" );
			break;
		case MachPhys::PLASMA_CANNON1:
			retVal = Gui::bitmap( "gui/fstpersn/weapon/plscan1.bmp" );
			break;
		case MachPhys::PLASMA_CANNON2:
			retVal = Gui::bitmap( "gui/fstpersn/weapon/plscan2.bmp" );
			break;
		case MachPhys::PLASMA_RIFLE:
			retVal = Gui::bitmap( "gui/fstpersn/weapon/plsrif.bmp" );
			break;
		case MachPhys::PULSE_CANNON:
			retVal = Gui::bitmap( "gui/fstpersn/weapon/pulscan.bmp" );
			break;
		case MachPhys::PULSE_RIFLE:
			retVal = Gui::bitmap( "gui/fstpersn/weapon/pulsrif.bmp" );
			break;
		case MachPhys::LARGE_MISSILE:
			retVal = Gui::bitmap( "gui/fstpersn/weapon/misslrg.bmp" );
			break;
		case MachPhys::MULTI_LAUNCHER1:
			retVal = Gui::bitmap( "gui/fstpersn/weapon/miss6.bmp" );	// Multi launcher 1 releases missiles of type 6
			break;
		case MachPhys::MULTI_LAUNCHER2:
			retVal = Gui::bitmap( "gui/fstpersn/weapon/miss5.bmp" );	// Multi launcher 2 releases missiles of type 5
			break;
		case MachPhys::MULTI_LAUNCHER3:
			retVal = Gui::bitmap( "gui/fstpersn/weapon/miss1.bmp" );  	// Multi launcher 3 releases missiles of type 1
			break;
		case MachPhys::MULTI_LAUNCHER4:
			retVal = Gui::bitmap( "gui/fstpersn/weapon/miss4.bmp" );	// Multi launcher 4 releases missiles of type 4
			break;
		case MachPhys::MULTI_LAUNCHER5:
			retVal = Gui::bitmap( "gui/fstpersn/weapon/miss2.bmp" );	// Multi launcher 5 releases missiles of type 2
			break;
		case MachPhys::MULTI_LAUNCHER6:
			retVal = Gui::bitmap( "gui/fstpersn/weapon/miss3.bmp" );	// Multi launcher 6 releases missiles of type 3
			break;
		case MachPhys::MULTI_LAUNCHER7:
			retVal = Gui::bitmap( "gui/fstpersn/weapon/miss7.bmp" );	// Multi launcher 7 releases missiles of type 7
			break;
		case MachPhys::VORTEX:
			retVal = Gui::bitmap( "gui/fstpersn/weapon/vortex.bmp" );
			break;
		case MachPhys::ORB_OF_TREACHERY:
			retVal = Gui::bitmap( "gui/fstpersn/weapon/treach.bmp" );
			break;
		case MachPhys::GORILLA_PUNCH:
			retVal = Gui::bitmap( "gui/fstpersn/weapon/punch.bmp" );
			break;
		case MachPhys::VIRUS:
			retVal = Gui::bitmap( "gui/fstpersn/weapon/virus.bmp" );
			break;
		case MachPhys::BEE_BOMB:
			retVal = Gui::bitmap( "gui/fstpersn/weapon/beebomb.bmp" );
			break;
		case MachPhys::WASP_LIGHT_STING:
			retVal = Gui::bitmap( "gui/fstpersn/weapon/wasplite.bmp" );
			break;
		case MachPhys::WASP_METAL_STING:
			retVal = Gui::bitmap( "gui/fstpersn/weapon/waspmis.bmp" );
			break;
		case MachPhys::NUCLEAR_MISSILE:
		case MachPhys::ION_ORBITAL_CANNON:
		case MachPhys::N_WEAPON_TYPES:
			ASSERT_BAD_CASE;
			break;
		DEFAULT_ASSERT_BAD_CASE( wt );
	}

	return retVal;
}

void MachGuiFirstPerson::doWeaponSelect()
{
    // De-pImpl_ variables used within this function.
	CB_DEPIMPL( int, weaponSelectIndex_ );
	CB_DEPIMPL( MachLog1stPersonHandler*, pLogHandler_ );
	CB_DEPIMPL( MachActor*, pActor_ );
	CB_DEPIMPL( double, leftWeaponChangeEndTime_ );
	CB_DEPIMPL( double, topWeaponChangeEndTime_ );
	CB_DEPIMPL( double, rightWeaponChangeEndTime_ );

	PRE( pLogHandler_ );

	if ( pLogHandler_->nWeapons() < 2 )
		return; // Return early if this machine has no weapons or just one weapon

	MachGuiSoundManager::instance().playSound( "gui/sounds/weapchng.wav" );

	++weaponSelectIndex_;

	if ( weaponSelectIndex_ > pLogHandler_->nWeapons() )
		weaponSelectIndex_ = 0;

	// Special case for gorilla weapon select
	if ( pActor_->objectType() == MachLog::AGGRESSOR and
		 pActor_->subType() == MachPhys::NINJA and
		 weaponSelectIndex_ == pLogHandler_->nWeapons() - 1 )
	{
		weaponSelectIndex_ = pLogHandler_->nWeapons();
	}

	// Select all weapons
	if ( weaponSelectIndex_ == pLogHandler_->nWeapons() )
	{
		for ( int weaponIndex = 0; weaponIndex < pLogHandler_->nWeapons(); ++weaponIndex )
		{
			if ( not pLogHandler_->isWeaponEnabled( weaponIndex ) )
			{
				pLogHandler_->enableWeapon( weaponIndex, true );

				MachLogWeapon& weapon = pLogHandler_->weapon( weaponIndex );
				switch ( weapon.mounting() )
				{
					case MachPhys::LEFT:
						updateWeaponAnimEndTime( leftWeaponChangeEndTime_ );
						break;
					case MachPhys::RIGHT:
						updateWeaponAnimEndTime( rightWeaponChangeEndTime_ );
						break;
					case MachPhys::TOP:
						updateWeaponAnimEndTime( topWeaponChangeEndTime_ );
						break;
				}
			}
		}
	}
	else // Select one weapon
	{
		// Special case for gorilla weapon select. Select both missiles at same time
		if ( pActor_->objectType() == MachLog::AGGRESSOR and pActor_->subType() == MachPhys::NINJA )
		{
			if ( weaponSelectIndex_ == 0 )
			{
				if ( not pLogHandler_->isWeaponEnabled( 0 ) )
				{
			 		pLogHandler_->enableWeapon( 0, true ); // Missile 1
					updateWeaponAnimEndTime( leftWeaponChangeEndTime_ );
				}
				if ( not pLogHandler_->isWeaponEnabled( 1 ) )
				{
					pLogHandler_->enableWeapon( 1, true ); // Missile 2
					updateWeaponAnimEndTime( rightWeaponChangeEndTime_ );
				}
				if ( pLogHandler_->isWeaponEnabled( 2 ) )
				{
					pLogHandler_->enableWeapon( 2, false );// Punch
					updateWeaponAnimEndTime( topWeaponChangeEndTime_ );
				}
			}
			else
			{
			 	if ( pLogHandler_->isWeaponEnabled( 0 ) )
				{
			 		pLogHandler_->enableWeapon( 0, false ); // Missile 1
					updateWeaponAnimEndTime( leftWeaponChangeEndTime_ );
				}
				if ( pLogHandler_->isWeaponEnabled( 1 ) )
				{
					pLogHandler_->enableWeapon( 1, false ); // Missile 2
					updateWeaponAnimEndTime( rightWeaponChangeEndTime_ );
				}
				if ( not pLogHandler_->isWeaponEnabled( 2 ) )
				{
					pLogHandler_->enableWeapon( 2, true );// Punch
					updateWeaponAnimEndTime( topWeaponChangeEndTime_ );
				}
			}
		}
		else
		{
			for ( int weaponIndex = 0; weaponIndex < pLogHandler_->nWeapons(); ++weaponIndex )
			{
				bool enable = weaponIndex == weaponSelectIndex_;

				if ( pLogHandler_->isWeaponEnabled( weaponIndex ) != enable )
				{
		 			pLogHandler_->enableWeapon( weaponIndex, enable );

					MachLogWeapon& weapon = pLogHandler_->weapon( weaponIndex );
					switch ( weapon.mounting() )
					{
						case MachPhys::LEFT:
							updateWeaponAnimEndTime( leftWeaponChangeEndTime_ );
							break;
						case MachPhys::RIGHT:
							updateWeaponAnimEndTime( rightWeaponChangeEndTime_ );
							break;
						case MachPhys::TOP:
							updateWeaponAnimEndTime( topWeaponChangeEndTime_ );
							break;
					}
				}
		 	}
		}
	}
}

void MachGuiFirstPerson::updateWeaponAnimEndTime( double& time )
{
	double now = DevTime::instance().time();
	time = time - now;
	if ( time < 0.0 )
	{
		time = 0.0;
	}

	time = now + ( WEAPON_DROPDOWN_TIME - time );
}

void MachGuiFirstPerson::loadBitmaps()
{
	CB_DEPIMPL_ARRAY( GuiBitmap, weaponStartupFrames_ );
	CB_DEPIMPL( MachGuiRadar*, pRadar_ );

	loadWeaponBmps();

	for ( int loop = 0; loop < WEAPON_DROPDOWN_FRAMES; ++loop )
	{
		string framePath( "gui/fstpersn/weapon/weap");
		char buffer[3];

//		framePath += itoa( loop, buffer, 10 );
        sprintf(buffer, "%d", loop);
        framePath += buffer;
		framePath += ".bmp";

		weaponStartupFrames_[loop] = Gui::bitmap( framePath );
		weaponStartupFrames_[loop].enableColourKeying();
	}

	pRadar_->loadBitmaps();
}

void MachGuiFirstPerson::unloadBitmaps()
{
	CB_DEPIMPL_ARRAY( GuiBitmap, weaponStartupFrames_ );
	CB_DEPIMPL( GuiBitmap, leftWeaponBmp_ );
	CB_DEPIMPL( GuiBitmap, rightWeaponBmp_ );
	CB_DEPIMPL( GuiBitmap, topWeaponBmp_ );
	CB_DEPIMPL( MachGuiRadar*, pRadar_ );

	for ( int loop = 0; loop < WEAPON_DROPDOWN_FRAMES; ++loop )
	{
		weaponStartupFrames_[loop] = GuiBitmap();
	}

	leftWeaponBmp_ = GuiBitmap();
	rightWeaponBmp_ = GuiBitmap();
	topWeaponBmp_ = GuiBitmap();

	pRadar_->unloadBitmaps();
}

void MachGuiFirstPerson::resolutionChange()
{
	CB_DEPIMPL( bool, resolutionChanged_ );

	resolutionChanged_ = true;
}

/* End FSTPERSN.CPP *************************************************/
