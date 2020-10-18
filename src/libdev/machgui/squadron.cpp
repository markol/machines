/*
 * S Q U A D R O N . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "mathex/point2d.hpp"
#include "mathex/random.hpp"
#include "machlog/cntrl_pc.hpp"
#include "machlog/machine.hpp"
#include "machlog/races.hpp"
#include "machlog/squad.hpp"
#include "machlog/administ.hpp"

#include "machgui/ingame.hpp"
#include "machgui/squadron.hpp"
#include "machgui/guictrl.hpp"
#include "gui/painter.hpp"
#include "device/butevent.hpp"
#include "device/keytrans.hpp"
#include "machgui/internal/mgsndman.hpp"

SysPathName MachGuiCorralResource_hack( MachActor * pActor ); // Defined in corral.cpp

class MachGuiSquadronAdminIcon : public GuiDisplayable
{
public:
	MachGuiSquadronAdminIcon( GuiDisplayable* pParent, const Gui::Coord& rel, size_t squadNum )
	:  GuiDisplayable( pParent, Gui::Box( 	rel.x(), rel.y(), 
											rel.x() + MachGuiSquadronAdminIcon::reqWidth(), 
											rel.y() + MachGuiSquadronAdminIcon::reqHeight() ) ),
		squadNum_( squadNum ),
        numInSquad_( 0 )
	{
		staticImageRandom_ = MexBasicRandom::constructSeededFromTime();
	}

	virtual ~MachGuiSquadronAdminIcon()
	{}

	static size_t reqWidth()	{ return 40; }
	static size_t reqHeight()   { return 40; }

	void update()
	{
	   	MachLogRaces& races = MachLogRaces::instance();
   		MachPhys::Race race = races.pcController().race();
        MachLogSquadron* pSquadron = races.squadrons( race )[ squadNum_ ];
        numInSquad_ = pSquadron->machines().size();
		noCommander_ = true;

		if ( numInSquad_ > 0 )
        {
			if ( pSquadron->hasCommander() )
	        {
				bitmap_ = Gui::bitmap( MachGuiCorralResource_hack( &pSquadron->commander() ) );
				noCommander_ = false;
				adminSw_ = pSquadron->commander().swLevel();
	        }	
		}

		redrawEveryFrame( noCommander_ and ( numInSquad_ > 0 ) );

		changed();
	}

	virtual void doDisplay()
	{
		// TV static bitmaps
		static GuiBitmap nosquadBmp[5];
		static bool initialised = false;
		if ( not initialised )
		{
			nosquadBmp[0] = Gui::bitmap( SysPathName( "gui/misc/nosquad1.bmp" ) );
			nosquadBmp[1] = Gui::bitmap( SysPathName( "gui/misc/nosquad2.bmp" ) );
			nosquadBmp[2] = Gui::bitmap( SysPathName( "gui/misc/nosquad3.bmp" ) );
			nosquadBmp[3] = Gui::bitmap( SysPathName( "gui/misc/nosquad4.bmp" ) );
			nosquadBmp[4] = Gui::bitmap( SysPathName( "gui/misc/nosquad5.bmp" ) );
			initialised = true;
		}
		
		Gui::Coord absCoordInOne( absoluteCoord() );
		absCoordInOne.x( absCoordInOne.x() + 1 );
		absCoordInOne.y( absCoordInOne.y() + 1 );

   		if ( numInSquad_ > 0 )
		{
			GuiPainter::instance().hollowRectangle( absoluteBoundary(), Gui::BLACK(), 1 );

			// Squad with no commander ( show static )
			if ( noCommander_ )
			{
				GuiPainter::instance().blit(nosquadBmp[ mexRandomInt( &staticImageRandom_, 5 ) ], absCoordInOne );

		   	} 
			else // Squad with commander, show commander
			{
				GuiPainter::instance().blit(bitmap_, absCoordInOne );
		 	}
		}
		else // Empty squad
		{
			GuiPainter::instance().filledRectangle( absoluteBoundary(), Gui::BLACK() );
		}	 
	}

private:
	GuiBitmap bitmap_;
	bool empty_;
	bool noCommander_;
	MexBasicRandom staticImageRandom_;
	size_t squadNum_;
	size_t adminSw_;
	size_t numInSquad_;
};

class MachGuiSquadron : public GuiDisplayable
{
public:
	MachGuiSquadron( MachGuiSquadronBank* pParent, size_t squadNum, MachInGameScreen* pInGameScreen )
	:	GuiDisplayable( pParent, Gui::Box(0,0, MachGuiSquadron::reqWidth(), MachGuiSquadron::reqHeight() ) )
	{
		pAdminIcon_ = _NEW( MachGuiSquadronAdminIcon( this, Gui::Coord( MachGuiSquadronBankIcon::reqWidth(), 0 ), squadNum ) );
		pIcon_ = _NEW( MachGuiSquadronBankIcon( this, Gui::Coord(0,0), squadNum, pInGameScreen, pParent ) );
	}
	virtual ~MachGuiSquadron()
	{}

	static size_t reqWidth()
	{
		return ( MachGuiSquadronBankIcon::reqWidth() + MachGuiSquadronAdminIcon::reqWidth() ); 
	}
	static size_t reqHeight()
	{
		return ( MachGuiSquadronBankIcon::reqHeight() ); 
	}

	virtual void doDisplay()
	{}

	void update()
	{
		pAdminIcon_->update();
		pIcon_->update();	
	}

	void loadGame()
	{
		pIcon_->loadGame();
	}

	void unloadGame()
	{
		pIcon_->unloadGame();
	}

private:
	MachGuiSquadronAdminIcon* pAdminIcon_;
	MachGuiSquadronBankIcon* pIcon_;
};

MachGuiSquadronBank::MachGuiSquadronBank( GuiDisplayable* pParent, 
										  const Gui::Coord& rel,
										  MachSquadronIcon* pNavButton,
										  MachInGameScreen* pInGameScreen )
: GuiSimpleScrollableList( 	pParent, 
							Gui::Box( rel.x(), rel.y(), rel.x() + MachGuiSquadronBank::reqWidth(), rel.y() + MachGuiSquadronBank::reqHeight() ), 
 							MachGuiSquadron::reqWidth() + 22, MachGuiSquadronBankIcon::reqHeight() - 1, 1 ),
  pInGameScreen_( pInGameScreen ),
  pNavButton_( pNavButton )
{
	for( size_t i = 1; i < N_SQUADRON_ICONS; ++i )
	{
		_NEW( MachGuiSquadron( this, i, pInGameScreen ) );
	}
	// Squad zero comes last ( makes more sense because of keyboard layout ).
	_NEW( MachGuiSquadron( this, 0, pInGameScreen ) );

	childrenUpdated();

 	pKeyTranslator_ = _NEW( DevKeyToCommandTranslator() );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::KEY_1, CREATE_SQUAD1, DevKeyToCommand::PRESSED, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::KEY_2, CREATE_SQUAD2, DevKeyToCommand::PRESSED, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::KEY_3, CREATE_SQUAD3, DevKeyToCommand::PRESSED, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::KEY_4, CREATE_SQUAD4, DevKeyToCommand::PRESSED, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
 	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::KEY_5, CREATE_SQUAD5, DevKeyToCommand::PRESSED, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::KEY_6, CREATE_SQUAD6, DevKeyToCommand::PRESSED, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::KEY_7, CREATE_SQUAD7, DevKeyToCommand::PRESSED, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::KEY_8, CREATE_SQUAD8, DevKeyToCommand::PRESSED, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::KEY_9, CREATE_SQUAD9, DevKeyToCommand::PRESSED, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::KEY_0, CREATE_SQUAD0, DevKeyToCommand::PRESSED, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::KEY_1, SELECT_SQUAD1, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::KEY_2, SELECT_SQUAD2, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::KEY_3, SELECT_SQUAD3, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::KEY_4, SELECT_SQUAD4, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
 	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::KEY_5, SELECT_SQUAD5, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::KEY_6, SELECT_SQUAD6, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::KEY_7, SELECT_SQUAD7, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::KEY_8, SELECT_SQUAD8, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::KEY_9, SELECT_SQUAD9, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::KEY_0, SELECT_SQUAD0, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->initEventQueue();
}

bool MachGuiSquadronBank::processButtonEvent( const DevButtonEvent& buttonEvent )
{
	if ( buttonEvent.action() == DevButtonEvent::RELEASE )
		return false;

	typedef DevKeyToCommand::CommandId CommandId;

	CommandId commandId;
	bool processed = pKeyTranslator_->translate( buttonEvent, &commandId );
		
	if ( processed )
	{
		switch ( commandId )
		{
			case CREATE_SQUAD1:
			case CREATE_SQUAD2:
			case CREATE_SQUAD3:
			case CREATE_SQUAD4:
			case CREATE_SQUAD5:
			case CREATE_SQUAD6:
			case CREATE_SQUAD7:
			case CREATE_SQUAD8:
			case CREATE_SQUAD9:
			case CREATE_SQUAD0:
				createSquad( commandId - CREATE_SQUAD0 );
				break;
			case SELECT_SQUAD1:
			case SELECT_SQUAD2:
			case SELECT_SQUAD3:
			case SELECT_SQUAD4:
			case SELECT_SQUAD5:
			case SELECT_SQUAD6:
			case SELECT_SQUAD7:
			case SELECT_SQUAD8:
			case SELECT_SQUAD9:
			case SELECT_SQUAD0:
				selectSquad( commandId - SELECT_SQUAD0 );
				break;
		}
	}

	return processed;
}

void MachGuiSquadronBank::createSquad( size_t squadIndex )
{
   	MachLogRaces& races = MachLogRaces::instance();
   	MachPhys::Race race = races.pcController().race();

    //Form the selected items into a squadron.
    //First clear the existing squadron.
    MachLogSquadron* pSquadron = races.squadrons( race )[ squadIndex ];
    MachLogSquadron::Machines& existingMembers = pSquadron->machines();
    while( existingMembers.size() != 0 )
	{
        existingMembers.back()->squadron( NULL );
	}

    //Now add each selected actor
    const MachInGameScreen::Actors& members = pInGameScreen_->selectedActors();
    for( MachInGameScreen::Actors::const_iterator it = members.begin();
         it != members.end(); ++it )
    {
		if ( (*it)->objectIsMachine() and (*it)->race() == race )
		{
			(*it)->asMachine().squadron( pSquadron );
		}
    }

    //Cancel the current command
    pInGameScreen_->cancelActiveCommand();

	// Dismiss squad menu
    //pInGameScreen_->currentContext( MachGui::MAIN_MENU );
	
	// Indicate that any commands should now be applied to this squad
	pInGameScreen_->applyCommandToSquadron( true );

	update();
}

void MachGuiSquadronBank::selectSquad( size_t squadIndex )
{
   	MachLogRaces& races = MachLogRaces::instance();
   	MachPhys::Race race = races.pcController().race();

    //Select the squadron members
	pInGameScreen_->deselectAll();

	MachLogSquadron::Machines& machines = races.squadrons( race )[ squadIndex ]->machines();
	for( MachLogSquadron::Machines::iterator i = machines.begin(); i != machines.end(); ++i )
	{
        pInGameScreen_->select( *i );
	}

	// Dismiss squad menu
    //pInGameScreen_->currentContext( MachGui::MAIN_MENU );

	// Indicate that any commands should now be applied to this squad
	pInGameScreen_->applyCommandToSquadron( true );
}

MachGuiSquadronBank::~MachGuiSquadronBank()
{
	_DELETE( pKeyTranslator_ );
}

void MachGuiSquadronBank::loadGame()
{
	for( Children::iterator iter = children().begin(); iter != children().end(); ++iter )
	{
		MachGuiSquadron* pSquad = _REINTERPRET_CAST( MachGuiSquadron*, *iter );
		pSquad->loadGame();	
	}

    update();
}

void MachGuiSquadronBank::unloadGame()
{
	for( Children::iterator iter = children().begin(); iter != children().end(); ++iter )
	{
		MachGuiSquadron* pSquad = _REINTERPRET_CAST( MachGuiSquadron*, *iter );
		pSquad->unloadGame();	
	}
}

//static 
size_t MachGuiSquadronBank::reqWidth()
{
	const size_t spacer = 22;
	return ( 2 * MachGuiSquadron::reqWidth() + spacer );
}

//static 
size_t MachGuiSquadronBank::reqHeight()
{
	return ( 5 * MachGuiSquadronBankIcon::reqHeight() - 4 /* buttons overlap by one pixel */ );
}

void MachGuiSquadronBank::update()
{
	pNavButton_->update();

	for( Children::iterator itter = children().begin(); itter != children().end(); ++itter )
	{
		MachGuiSquadron* pSquad = _REINTERPRET_CAST( MachGuiSquadron*, *itter );
		pSquad->update();	
	}

	changed();
}


MachGuiSquadronBankIcon::MachGuiSquadronBankIcon( MachGuiSquadron *pParent, 
									const Gui::Coord& rel,
									unsigned squadronIndex,
									MachInGameScreen * pInGameScreen,
									MachGuiSquadronBank* pBank )
: GuiIcon( pParent, rel, iconBitmaps( squadronIndex ) ),
  squadronIndex_( squadronIndex ),
  pInGameScreen_( pInGameScreen ),
  pBank_( pBank ),
  numInSquad_( 0 ),
  inDoBeReleased_( false )
{
	PRE( squadronIndex < MachGuiSquadronBank::N_SQUADRON_ICONS );
}

MachGuiSquadronBankIcon::~MachGuiSquadronBankIcon()
{
	// Intentionally Empty
}

void MachGuiSquadronBankIcon::loadGame()
{
	// Start observing squadron X (1 - 10).
   	MachLogRaces& races = MachLogRaces::instance();
   	MachPhys::Race race = races.pcController().race();
    MachLogSquadron* pSquadron = races.squadrons( race )[ squadronIndex_ ];
	pSquadron->attach( this );
}

void MachGuiSquadronBankIcon::unloadGame()
{}

void MachGuiSquadronBankIcon::doBeDepressed( const GuiMouseEvent& )
{
	MachGuiSoundManager::instance().playSound( "gui/sounds/igclick.wav" );
}

//static 
size_t MachGuiSquadronBankIcon::reqWidth()
{
	return 29; // TODO : remove hard coding.
}


//static 
size_t MachGuiSquadronBankIcon::reqHeight()
{
	return 40; // TODO : remove hard coding.
}

// virtual
bool MachGuiSquadronBankIcon::beNotified(  	W4dSubject*,
											W4dSubject::NotificationEvent event,
											int clientData )
{
    switch( event )
    {
		case W4dSubject::CLIENT_SPECIFIC:
			if ( clientData == MachLog::SQUADRON_CHANGED )
			{
				DEBUG_STREAM( DIAG_NEIL, "MachGuiSquadronBankIcon::beNotified squad member deleted/added" << std::endl );

				// Only update whole squad menu if we are not already doing so in 'doBeReleased'.
				if ( not inDoBeReleased_ )
					pBank_->update();
			}
		default:
			break;
    }

	return true; // True indicates that we still want to observe the squadron
}

//virtual 
void MachGuiSquadronBankIcon::domainDeleted( W4dDomain* )
{}


void MachGuiSquadronBankIcon::doBeReleased( const GuiMouseEvent& )
{
	// Safe guard against 'update' being called multiple times due to observing
	// squadron.
	inDoBeReleased_ = true;

    //Check context for action
    MachGui::ControlPanelContext context = pInGameScreen_->currentContext();
   	MachLogRaces& races = MachLogRaces::instance();
   	MachPhys::Race race = races.pcController().race();

    switch( context )
    {
        case MachGui::FORM_SQUADRON_COMMAND:
        {
            //Form the selected items into a squadron.
            //First clear the existing squadron.
            MachLogSquadron* pSquadron = races.squadrons( race )[ squadronIndex_ ];
            MachLogSquadron::Machines& existingMembers = pSquadron->machines();
            while( existingMembers.size() != 0 )
			{
                existingMembers.back()->squadron( NULL );
			}

            //Now add each selected actor
            const MachInGameScreen::Actors& members = pInGameScreen_->selectedActors();
            for( MachInGameScreen::Actors::const_iterator it = members.begin();
                 it != members.end(); ++it )
            {
				if ( (*it)->objectIsMachine() )
				{
					(*it)->asMachine().squadron( pSquadron );
				}
            }

            //Cancel the current command
            pInGameScreen_->cancelActiveCommand();

			// Dismiss squad menu
            pInGameScreen_->currentContext( MachGui::MAIN_MENU );
			
			// Indicate that any commands should now be applied to this squad
			pInGameScreen_->applyCommandToSquadron( true );

			pBank_->update();

            break;
        }

        case MachGui::SQUADRON_MENU:
        {
            //Select the squadron members
        	pInGameScreen_->deselectAll();

        	MachLogSquadron::Machines& machines = races.squadrons( race )[ squadronIndex_ ]->machines();
        	for( MachLogSquadron::Machines::iterator i = machines.begin(); i != machines.end(); ++i )
        	{
		        pInGameScreen_->select( *i );
        	}

			// Dismiss squad menu
            pInGameScreen_->currentContext( MachGui::MAIN_MENU );

			// Indicate that any commands should now be applied to this squad
			pInGameScreen_->applyCommandToSquadron( true );

            break;
        }
    }

	inDoBeReleased_ = false;
}

//////////////////////////////////////////////////////////////////////

// static 
const SysPathName& 
MachGuiSquadronBankIcon::iconBitmaps( unsigned squadronIndex )
{
	PRE( squadronIndex < MachGuiSquadronBank::N_SQUADRON_ICONS );

//	SysPathNames p = make_pair( SysPathName( "gui/squadbnk/num0.bmp" ), SysPathName( "gui/squadbnk/num0.bmp" ) );

	static SysPathName bitmaps_[ MachGuiSquadronBank::N_SQUADRON_ICONS ] =
	{
		SysPathName( "gui/misc/squad0.bmp" ),
		SysPathName( "gui/misc/squad1.bmp" ),
		SysPathName( "gui/misc/squad2.bmp" ),
		SysPathName( "gui/misc/squad3.bmp" ),
		SysPathName( "gui/misc/squad4.bmp" ),
		SysPathName( "gui/misc/squad5.bmp" ),
		SysPathName( "gui/misc/squad6.bmp" ),
		SysPathName( "gui/misc/squad7.bmp" ),
		SysPathName( "gui/misc/squad8.bmp" ),
		SysPathName( "gui/misc/squad9.bmp" )
	};

	POST( bitmaps_[ squadronIndex ].existsAsFile() );
	return bitmaps_[ squadronIndex ];
}

//virtual 
void MachGuiSquadronBankIcon::doDisplayInteriorEnabled( const Gui::Coord& abs )
{
	static GuiBitmap numbers[10] = { Gui::bitmap( SysPathName( "gui/misc/numsqd0.bmp" ) ),
									 Gui::bitmap( SysPathName( "gui/misc/numsqd1.bmp" ) ),
									 Gui::bitmap( SysPathName( "gui/misc/numsqd2.bmp" ) ),
									 Gui::bitmap( SysPathName( "gui/misc/numsqd3.bmp" ) ),
									 Gui::bitmap( SysPathName( "gui/misc/numsqd4.bmp" ) ),
									 Gui::bitmap( SysPathName( "gui/misc/numsqd5.bmp" ) ),
									 Gui::bitmap( SysPathName( "gui/misc/numsqd6.bmp" ) ),
									 Gui::bitmap( SysPathName( "gui/misc/numsqd7.bmp" ) ),
									 Gui::bitmap( SysPathName( "gui/misc/numsqd8.bmp" ) ),
									 Gui::bitmap( SysPathName( "gui/misc/numsqd9.bmp" ) ) };

	GuiIcon::doDisplayInteriorEnabled( abs );

	if ( numInSquad_ > 0 )
	{
		Gui::Coord absCopy( abs );
		absCopy.y( absCopy.y() + reqHeight() - 4 /* border height*/ - 9 /* number height*/); //TODO: remove hard coding
		absCopy.x( absCopy.x() + reqWidth() - 4 /*border width*/ );

		MachGui::drawNumber( numbers, numInSquad_, &absCopy );
	}
}

void MachGuiSquadronBankIcon::update()
{
   	MachLogRaces& races = MachLogRaces::instance();
	MachPhys::Race race = races.pcController().race();
    MachLogSquadron* pSquadron = races.squadrons( race )[ squadronIndex_ ];
    numInSquad_ = pSquadron->machines().size();

	changed();
}

/* End SQUADRON.CPP *************************************************/
