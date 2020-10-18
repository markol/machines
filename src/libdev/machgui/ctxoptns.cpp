/*
 * C T X O P T N S . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/ctxoptns.hpp"
#include "stdlib/string.hpp"
#include "ctl/list.hpp"
#include "system/pathname.hpp"
#include "system/registry.hpp"
#include "system/memcaps.hpp"
#include "sound/soundmix.hpp"
#include "world4d/soundman.hpp"
#include "world4d/manager.hpp"
#include "world4d/scenemgr.hpp"
#include "render/device.hpp"
#include "render/display.hpp"
#include "render/capable.hpp"
#include "render/drivsel.hpp"
#include "gui/restring.hpp"
#include "gui/font.hpp"
#include "gui/image.hpp"
#include "machgui/ddrawdrop.hpp"
#include "machphys/compmgr.hpp"
#include "machphys/compitem.hpp"
#include "machgui/menubutn.hpp"
#include "machgui/menutext.hpp"
#include "machgui/msgbox.hpp"
#include "machgui/startup.hpp"
#include "machgui/strtdata.hpp"
#include "machgui/slidebar.hpp"
#include "machgui/musicvol.hpp"
#include "machgui/soundvol.hpp"
#include "machgui/optlayout.hpp"
#include "machgui/dropdwnc.hpp"
#include "machgui/chckbox.hpp"
#include "machgui/gammacor.hpp"
#include "machgui/internal/mgsndman.hpp"
#include "machgui/internal/strings.hpp"
#include "machlog/races.hpp"
#include "machlog/actor.hpp"

#define OPTIONS_AREA_MINX 95
#define OPTIONS_AREA_MINY 50
#define OPTIMISATIONS_AREA_MINX OPTIONS_AREA_MINX
#define OPTIMISATIONS_AREA_MINY 209

class MachGuiCtxOptions;

class MachGuiDDrawDropDownCallback : public MachGuiDropDownCallback
{
	public:
 		MachGuiDDrawDropDownCallback( MachGuiCtxOptions& optionsScreen )
		:	optionsScreen_( optionsScreen )
		{
		}
		virtual void callBack()
		{
			optionsScreen_.showDirect3DDrivers();
		}
	private:
		MachGuiCtxOptions& optionsScreen_;
};

class MachGuiOptionsExitMessageBoxResponder : public MachGuiMessageBoxResponder
{
public:
	MachGuiOptionsExitMessageBoxResponder( MachGuiCtxOptions* pOptionsCtx )
	:	pOptionsCtx_( pOptionsCtx )
	{}

	virtual bool okPressed()
	{
		pOptionsCtx_->exitFromOptions();
		return true;
	}

private:
	MachGuiCtxOptions* pOptionsCtx_;
};

MachGuiCtxOptions::MachGuiCtxOptions( MachGuiStartupScreens* pStartupScreens )
:	MachGuiStartupScreenContext( pStartupScreens ),
	animations_( pStartupScreens, SysPathName("gui/menu/sf_anims.anm") ),
	pMusicVolume_( NULL ),
	pSoundVolume_( NULL ),
	pSound3d_( NULL ),
	pTransitions_( NULL ),
	pScreenSize_( NULL ),
	pDirectDrawDrivers_( NULL ),
	pDirect3DDrivers_( NULL ),
	exitFromOptions_( false ),
	pGammaCorrection_( NULL )
{
	// Bitmap background
	pStartupScreens->changeBackdrop( "gui/menu/sf.bmp" );
    pStartupScreens->cursorOn( true );
    pStartupScreens->desiredCdTrack( MachGuiStartupScreens::MENU_MUSIC );

	// Get the display, required for things like gamma correction, display drivers etc
	RenDisplay* pDisplay_ = W4dManager::instance().sceneManager()->pDevice()->display();

	// Load the layout information
	MachGuiOptionsLayout screenLayout( SysPathName( "gui/layout/options.dat" ) );

 	// Create buttons
 	MachGuiMenuButton* pOkBtn = _NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 85, 377, 291, 423 ), IDS_MENUBTN_OK, MachGuiStartupScreens::BE_DUMMY_OK ) );
	MachGuiMenuButton* pCancelBtn = _NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 337,377, 543, 423 ), IDS_MENUBTN_CANCEL, MachGuiStartupScreens::BE_DUMMY_EXIT ) );
	pCancelBtn->escapeControl( true );
	pOkBtn->defaultControl( true );

	// Display large headings
	GuiBmpFont font = GuiBmpFont::getFont( SysPathName("gui/menu/largefnt.bmp") );
	GuiBmpFont smallFont = GuiBmpFont::getFont( SysPathName("gui/menu/smallfnt.bmp") );
	GuiResourceString optionsHeading( IDS_MENULB_OPTIONS );
	_NEW( MachGuiMenuText( pStartupScreens,
						   Gui::Box( Gui::Coord( OPTIONS_AREA_MINX, OPTIONS_AREA_MINY ),
						   			 font.textWidth( optionsHeading.asString() ),
						   			 font.charHeight() + 2 ), IDS_MENULB_OPTIONS, "gui/menu/largefnt.bmp" ) );

	GuiResourceString optimisationsHeading( IDS_MENULB_OPTIMISATIONS );
	_NEW( MachGuiMenuText( pStartupScreens,
						   Gui::Box( Gui::Coord( OPTIMISATIONS_AREA_MINX, OPTIMISATIONS_AREA_MINY ),
						   			 font.textWidth( optimisationsHeading.asString() ),
						   			 font.charHeight() + 2 ), IDS_MENULB_OPTIMISATIONS, "gui/menu/largefnt.bmp" ) );
	// Initialise gui items

	const MachGuiOptionsLayout::SlidebarInfo& musicVolSl = screenLayout.slidebarInfo( 0 );
	const MachGuiOptionsLayout::SlidebarInfo& soundVolSl = screenLayout.slidebarInfo( 1 );
	const MachGuiOptionsLayout::SlidebarInfo& gammaCorrectionSl = screenLayout.slidebarInfo( 2 );
	const MachGuiOptionsLayout::MenuTextInfo& musicVolTxt = screenLayout.menuTextInfo( 0 );
	const MachGuiOptionsLayout::MenuTextInfo& soundVolTxt = screenLayout.menuTextInfo( 1 );
	const MachGuiOptionsLayout::MenuTextInfo& gammaCorrectionTxt = screenLayout.menuTextInfo( 2 );
	const MachGuiOptionsLayout::MenuTextInfo& screenSizeTxt = screenLayout.menuTextInfo( 3 );
	const MachGuiOptionsLayout::MenuTextInfo& directDrawTxt = screenLayout.menuTextInfo( 4 );
	const MachGuiOptionsLayout::MenuTextInfo& direct3DTxt = screenLayout.menuTextInfo( 5 );
	const MachGuiOptionsLayout::CheckBoxInfo& sound3dCB = screenLayout.checkBoxInfo( 0 );
	const MachGuiOptionsLayout::CheckBoxInfo& transitionsCB = screenLayout.checkBoxInfo( 1 );
	const MachGuiOptionsLayout::CheckBoxInfo& screenResolutionLock = screenLayout.checkBoxInfo( 2 );
	const MachGuiOptionsLayout::CheckBoxInfo& cursorType = screenLayout.checkBoxInfo( 3 );
	const MachGuiOptionsLayout::CheckBoxInfo& reverseKeys = screenLayout.checkBoxInfo( 4 );
	const MachGuiOptionsLayout::CheckBoxInfo& reverseMouse = screenLayout.checkBoxInfo( 5 );

	// Create control labels
	_NEW( MachGuiMenuText( pStartupScreens, Gui::Box( musicVolTxt.topLeft, musicVolTxt.bottomRight ),
						   musicVolTxt.idsStringId, musicVolTxt.font, MachGuiMenuText::RIGHT_JUSTIFY ) );

	_NEW( MachGuiMenuText( pStartupScreens, Gui::Box( soundVolTxt.topLeft, soundVolTxt.bottomRight ),
						   soundVolTxt.idsStringId, soundVolTxt.font, MachGuiMenuText::RIGHT_JUSTIFY ) );

	_NEW( MachGuiMenuText( pStartupScreens, Gui::Box( screenSizeTxt.topLeft, screenSizeTxt.bottomRight ),
						   screenSizeTxt.idsStringId, screenSizeTxt.font, MachGuiMenuText::RIGHT_JUSTIFY ) );

	_NEW( MachGuiMenuText( pStartupScreens, Gui::Box( directDrawTxt.topLeft, directDrawTxt.bottomRight ),
						   directDrawTxt.idsStringId, directDrawTxt.font, MachGuiMenuText::RIGHT_JUSTIFY ) );

//	_NEW( MachGuiMenuText( pStartupScreens, Gui::Box( direct3DTxt.topLeft, direct3DTxt.bottomRight ),
//						   direct3DTxt.idsStringId, direct3DTxt.font, MachGuiMenuText::RIGHT_JUSTIFY ) );

	// Create check boxes
	pSound3d_ = _NEW( MachGuiCheckBox( pStartupScreens, pStartupScreens, sound3dCB.topLeft, sound3dCB.stringId ) );

	pTransitions_ = _NEW( MachGuiCheckBox( pStartupScreens, pStartupScreens, transitionsCB.topLeft, transitionsCB.stringId ) );

	pScreenResolutionLock_ = _NEW( MachGuiCheckBox( pStartupScreens, pStartupScreens, screenResolutionLock.topLeft, screenResolutionLock.stringId ) );

	pCursorType_ = _NEW( MachGuiCheckBox( pStartupScreens, pStartupScreens, cursorType.topLeft, cursorType.stringId ) );

	pReverseKeys_ = _NEW( MachGuiCheckBox( pStartupScreens, pStartupScreens, reverseKeys.topLeft, reverseKeys.stringId ) );

	pReverseMouse_ = _NEW( MachGuiCheckBox( pStartupScreens, pStartupScreens, reverseMouse.topLeft, reverseMouse.stringId ) );

	// Create volume sliders
	pMusicVolume_ = _NEW( MachGuiMusicVolumeSlideBar( pStartupScreens, pStartupScreens, musicVolSl.topLeft, musicVolSl.range ) );
	pSoundVolume_ = _NEW( MachGuiSoundVolumeSlideBar( pStartupScreens, pStartupScreens, soundVolSl.topLeft, soundVolSl.range ) );

	const RenCapabilities& caps = W4dManager::instance().sceneManager()->pDevice()->capabilities();

	// Get current resolution
	int inGameResolutionWidth = SysRegistry::instance().queryIntegerValue( "Screen Resolution", "Width", SysRegistry::CURRENT_USER );
	int inGameResolutionHeight = SysRegistry::instance().queryIntegerValue( "Screen Resolution", "Height", SysRegistry::CURRENT_USER );
	int inGameResolutionRefresh = SysRegistry::instance().queryIntegerValue( "Screen Resolution", "Refresh Rate", SysRegistry::CURRENT_USER );

	// Check that minimum resolution is specified
	if ( inGameResolutionWidth < 640 or inGameResolutionHeight < 480 )
	{
		inGameResolutionWidth = 640;
		inGameResolutionHeight = 480;
	}

	GuiStrings strings;
	MachGuiDropDownListBoxCreator::DropDownListBoxItems modeList;
	strings.reserve( 4 );
	modeList.reserve( 4 );

	// Iterate through screen modes selecting compatible ones ( i.e. conform to minimum size and colour bit depth ).
	const ctl_list<RenDisplay::Mode>& modes = pDisplay_->modeList();
	const RenDisplay::Mode& lowestMode = pDisplay_->lowestAllowedMode();
	const RenDisplay::Mode& highestMode = pDisplay_->highestAllowedMode();
	for ( ctl_list<RenDisplay::Mode>::const_iterator iter = modes.begin(); iter != modes.end(); ++iter )
	{
		const RenDisplay::Mode& mode = *iter;
		// I'd like to use the highest allowed mode to decide whether a mode is accepted or
		// not (instead of testing the memory required versus the available video memory)
		// unfortunately I can't since the modes are sorted by number of pixels and not by memory
		// requirement.
		//if ( mode >= lowestMode and mode.bitDepth() == 16 and
		if ( _CONST_CAST(RenDisplay::Mode&, mode) >= lowestMode and
			caps.maxAvailableDisplayMemoryAfterTextures() >= 3 * mode.memoryRequired() )
		{
			// Construct a string to go in the drop down list box ( e.g. "640x480" )
			char buffer[30];
//			string resolutionStr = itoa( mode.width(), buffer, 10 );
//			resolutionStr += itoa( mode.height(), buffer, 10 );
            sprintf(buffer, "%dx%d %d hz", mode.width(), mode.height(), mode.refreshRate());
            string resolutionStr = buffer;

			// If this mode is the current mode for ingame then make sure it appears first in the
			// drop down list box
			if ( inGameResolutionWidth == mode.width() and
				 inGameResolutionHeight == mode.height() and
				 ( inGameResolutionRefresh == 0 or inGameResolutionRefresh == mode.refreshRate()) )
			{
				strings.insert( strings.begin(), resolutionStr );
				modeList.insert( modeList.begin(), (void*)&mode );
			}
			else
			{
				strings.push_back( resolutionStr );
				modeList.push_back( (void*)&mode );
			}
		}
	}

	pScreenSize_ = _NEW( MachGuiDropDownListBoxCreator( pStartupScreens,
														pStartupScreens,
														Gui::Coord( 353, 119 ),
														153,
														strings,
														false,
														true ) );
   	pScreenSize_->items( modeList );

	// Create list of avaliable direct draw drivers/direct 3d drivers

	pDriverSelector_ = _NEW( RenDriverSelector( pDisplay_ ) );
	MachGuiDropDownListBoxCreator::DropDownListBoxItems dDrawDrivers;
	GuiStrings dDrawDriverNames;
	dDrawDriverNames.reserve( 4 );
	dDrawDrivers.reserve( 4 );

	const RenDriverSelector::RenDrivers& dDrawDriversList = pDriverSelector_->dDrawDrivers();

	for( RenDriverSelector::RenDrivers::const_iterator i = dDrawDriversList.begin(); i != dDrawDriversList.end(); ++i )
	{
		dDrawDrivers.push_back( ( MachGuiDropDownListBoxCreator::DropDownListBoxItem ) &(*i) );
		dDrawDriverNames.push_back( (*i)->name() );
	}
	dDrawDriverNames.push_back("default");

 	pDirectDrawDrivers_ = _NEW( MachGuiDDrawDropDownListBoxCreator( pStartupScreens,
														pStartupScreens,
														Gui::Coord( 353, 139 ),
														153,
														dDrawDriverNames,
														false,
														true,
														_NEW( MachGuiDDrawDropDownCallback( *this ) ) ) );

	pDirectDrawDrivers_->items( dDrawDrivers );

//	pDirectDrawDrivers_->text( pDriverSelector_->currentDDrawDriver()->name() );

//	showDirect3DDrivers();

	// Only display gamma correction slider if gamma correction is supported
	if ( pDisplay_->supportsGammaCorrection() )
	{
		_NEW( MachGuiMenuText( pStartupScreens, Gui::Box( gammaCorrectionTxt.topLeft, gammaCorrectionTxt.bottomRight ),
							   gammaCorrectionTxt.idsStringId, gammaCorrectionTxt.font, MachGuiMenuText::RIGHT_JUSTIFY ) );

	   	pGammaCorrection_ = _NEW( MachGuiGammaCorrectionSlideBar( pStartupScreens, pStartupScreens, gammaCorrectionSl.topLeft, gammaCorrectionSl.range, GAMMA_LOWER_LIMIT, GAMMA_UPPER_LIMIT ) );
		// Store initial value
	   	gammaCorrection_ = pGammaCorrection_->value();

		// Show gamma correction image (helps get gamma setting correct)
		_NEW( GuiImage( pStartupScreens, Gui::Coord(353, 198), Gui::bitmap("gui/menu/gammacal.bmp") ) );
	}

	const MachPhysComplexityManager::BooleanItems& boolItems =	MachPhysComplexityManager::instance().booleanItems();

	// Access boolean items
	uint index = 0;
	for( MachPhysComplexityManager::BooleanItems::const_iterator it = boolItems.begin(); it != boolItems.end(); ++it )
	{
		uint id = (*it)->id();

		booleanOptimisations_.push_back( _NEW( MachGuiCheckBox( pStartupScreens, pStartupScreens,
																Gui::Coord( 240, OPTIMISATIONS_AREA_MINY + 33 + ( 20*index ) ),
																id ) ) );
		++index;
	}

	// Access all the choices items, their id and # of choice per id
	const MachPhysComplexityManager::ChoiceItems& chItems = MachPhysComplexityManager::instance().choiceItems();
	index = 0;
	for( MachPhysComplexityManager::ChoiceItems::const_iterator it = chItems.begin(); it != chItems.end(); ++it )
	{
		uint id = (*it)->id();
		uint nch = (*it)->nChoices();

		GuiStrings choices;
		MachGuiDropDownListBoxCreator::DropDownListBoxItems choiceIds;
		choices.reserve( 4 );
		choiceIds.reserve( 4 );

		for( uint ch = 0; ch < nch; ++ch )
		{
			GuiResourceString choice( ch+id+1 );
			string choiceString = choice.asString();
			choices.push_back( choiceString );
			choiceIds.push_back( ( MachGuiDropDownListBoxCreator::DropDownListBoxItem ) (ch + 1) );
		}
		GuiResourceString choiceTitle( id );

	    _NEW( MachGuiMenuText( pStartupScreens, Gui::Box( 	Gui::Coord( 347 - smallFont.textWidth( choiceTitle.asString() ),
																		OPTIMISATIONS_AREA_MINY + 33 + ( 20*index ) ),
															smallFont.textWidth( choiceTitle.asString() ),
															smallFont.charHeight() + 8 ),
															id,
															"gui/menu/smallfnt.bmp" ) );

		choicesOptimisations_.push_back( _NEW( MachGuiDropDownListBoxCreator( pStartupScreens,
														pStartupScreens,
														Gui::Coord( 353, OPTIMISATIONS_AREA_MINY + 35 + ( 20*index ) ),
														153,
														choices,
														false,
														true ) ) );
		(*( choicesOptimisations_.end() - 1 ))->items( choiceIds );
		++index;
	}

	// Retain original settings in case user cancels subsequent settings
	musicVolume_ = pMusicVolume_->value();
	soundVolume_ = pSoundVolume_->value();

	initialDDrawDriver_ = pDriverSelector_->currentDDrawDriver();

	setOptions();

    TEST_INVARIANT;
}

MachGuiCtxOptions::~MachGuiCtxOptions()
{
    TEST_INVARIANT;

}

void MachGuiCtxOptions::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiCtxOptions& t )
{

    o << "MachGuiCtxOptions " << (void*)&t << " start" << std::endl;
    o << "MachGuiCtxOptions " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachGuiCtxOptions::update()
{
	if( exitFromOptions_ )
	{
		pStartupScreens_->buttonAction( MachGuiStartupScreens::BE_OK );
	}
	else
	{
		animations_.update();
	}
}

// virtual
bool MachGuiCtxOptions::okayToSwitchContext( void )
{
	return true;
}

// virtual
void MachGuiCtxOptions::buttonEvent( MachGuiStartupScreens::ButtonEvent buttonEvent )
{
	if ( buttonEvent == MachGuiStartupScreens::BE_DUMMY_OK )
	{
		getOptions();

		uint idsMessage;

		bool bDisplayMessageBox = false;

		// Determine if the video driver settings have changed
		/*if( ( pDriverSelector_->currentDDrawDriver()->name() != initialDDrawDriver_->name() ) or
			( pDriverSelector_->currentDDrawDriver()->description() != initialDDrawDriver_->description() ) )
		{
			idsMessage = IDS_MENUMSG_DISPLAYDRIVER;
			bDisplayMessageBox = true;
		}*/

		// Determine if the 2D selection boxes are enabled and screen resolution has changed
		const RenDisplay::Mode* pNewMode = (const RenDisplay::Mode*)pScreenSize_->item();
		const RenDisplay::Mode& pCurrentMode = W4dManager::instance().sceneManager()->pDevice()->display()->currentMode();

		if( pScreenResolutionLock_->checked() and ( ( pNewMode->width() != pCurrentMode.width() ) or ( pNewMode->height() != pCurrentMode.height() ) ) )
		{
			// If we already have a message to display from the previous settings change, then use a combined message
			if( bDisplayMessageBox )
			{
				idsMessage = IDS_MENUMESSAGE_DRIVER_RESOLUTION;
			}
			else
			{
				idsMessage = IDS_MENUMESSAGE_RESOLUTION;
			}
			bDisplayMessageBox = true;
		}

		if( bDisplayMessageBox )
		{
			// Inform user that changes will not take effect until the machine is rebooted
			pStartupScreens_->displayOKMsgBox( idsMessage, _NEW( MachGuiOptionsExitMessageBoxResponder(this) ) );
		}
		else
		{
			pStartupScreens_->buttonAction( MachGuiStartupScreens::BE_OK );
		}
	}
	else if ( buttonEvent == MachGuiStartupScreens::BE_DUMMY_EXIT )
	{
		pMusicVolume_->value( musicVolume_ );
		pSoundVolume_->value( soundVolume_ );
		pDriverSelector_->useDDrawDriver( initialDDrawDriver_ );
		// Only restore gamma correction if gamma correction is supported
		if ( pGammaCorrection_ )
		{
			pGammaCorrection_->value( gammaCorrection_ );
		}
		pStartupScreens_->buttonAction( MachGuiStartupScreens::EXIT );
	}
}

void MachGuiCtxOptions::getOptions( void )
{
	// Used to set current game settings to be that of gui items

	if(pSound3d_->checked() != SysRegistry::instance().queryIntegerValue( "Options\\3DSound", "on", SysRegistry::CURRENT_USER ) )
	{
		SOUND_STREAM("Updating sounds" << std::endl);
//		//It is necessary to clear all sounds from the system
//		//before loading a new set.
		MachGuiSoundManager::instance().clearAll();
		W4dSoundManager::instance().stopAll();
		load3dSoundFiles( pSound3d_->checked() );
		//We now have to change all the in game sounds to the correct
		//number of dimensions
		if(pSound3d_->checked())
		{
			W4dSoundManager::instance().convertSoundDimensions(Snd::THREE_D);
		}
		else
		{
			W4dSoundManager::instance().convertSoundDimensions(Snd::TWO_D);
		}
		SysRegistry::instance().setIntegerValue( "Options\\3DSound", "on", pSound3d_->checked(), SysRegistry::CURRENT_USER );
	}

	pStartupScreens_->startupData()->transitionFlicsOn( pTransitions_->checked() );
	SysRegistry::instance().setIntegerValue( "Options\\transitions", "on", pStartupScreens_->startupData()->transitionFlicsOn(), SysRegistry::CURRENT_USER );

	// Store the new screen size in the registry
    const RenDisplay::Mode* pNewMode = (const RenDisplay::Mode*)pScreenSize_->item();
	SysRegistry::instance().setIntegerValue( "Screen Resolution", "Width", pNewMode->width(), SysRegistry::CURRENT_USER );
	SysRegistry::instance().setIntegerValue( "Screen Resolution", "Height", pNewMode->height(), SysRegistry::CURRENT_USER );
	SysRegistry::instance().setIntegerValue( "Screen Resolution", "Refresh Rate", pNewMode->refreshRate(), SysRegistry::CURRENT_USER );

	// Store sound and CD settings
	SysRegistry::instance().setIntegerValue( "Options\\CD", "Volume", pMusicVolume_->value() + 0.5 /*stop rounding errors*/, SysRegistry::CURRENT_USER );
	SysRegistry::instance().setIntegerValue( "Options\\Sound", "Volume", pSoundVolume_->value() + 0.5 /*stop rounding errors*/, SysRegistry::CURRENT_USER );

	// Store gamma correction value
	if ( pGammaCorrection_ )
	{
		SysRegistry::instance().setIntegerValue( "Options\\Gamma Correction", "Value", pGammaCorrection_->value() * GAMMA_REG_MULTIPLIER, SysRegistry::CURRENT_USER );
	}

	// Store option to maintain screen res of in-game menus	in menus
	SysRegistry::instance().setIntegerValue( "Screen Resolution", "Lock Resolution", pScreenResolutionLock_->checked(), SysRegistry::CURRENT_USER );

	// Store cursor type (2D/3D)
	SysRegistry::instance().setIntegerValue( "Options\\Cursor Type", "2D", pCursorType_->checked(), SysRegistry::CURRENT_USER );

	// If cursor type has changed then refresh all selection boxes
	if ( cursorType2d_ != pCursorType_->checked() )
	{
	   	MachLogRaces::Objects& allObjects = MachLogRaces::instance().objects();

		for ( MachLogRaces::Objects::iterator iter = allObjects.begin(); iter != allObjects.end(); ++iter )
		{
			MachActor* pActor = *iter;
			if ( pActor->selectionState() == MachLog::SELECTED )
			{
				// Deselect, then reselect to refresh bounding box
				pActor->selectionState( MachLog::NOT_SELECTED );
				pActor->selectionState( MachLog::SELECTED );
			}
			else if ( pActor->selectionState() == MachLog::HIGHLIGHTED )
			{
				// Deselect, then reselect to refresh bounding box
				pActor->selectionState( MachLog::NOT_SELECTED );
				pActor->selectionState( MachLog::HIGHLIGHTED );
			}
		}
	}

	//Store reverse direction of up/down keys/mouse
	SysRegistry::instance().setIntegerValue( "Options\\Reverse UpDown Keys", "on", pReverseKeys_->checked(), SysRegistry::CURRENT_USER );
	SysRegistry::instance().setIntegerValue( "Options\\Reverse BackForward Mouse", "on", pReverseMouse_->checked(), SysRegistry::CURRENT_USER );

	// Access all the boolean optimisations
	const MachPhysComplexityManager::BooleanItems& boolItems =	MachPhysComplexityManager::instance().booleanItems();
	uint index = 0;
	for( MachPhysComplexityManager::BooleanItems::const_iterator it = boolItems.begin(); it != boolItems.end(); ++it )
	{
		uint id = (*it)->id();
		MachPhysComplexityManager::instance().changeBooleanItem( id, booleanOptimisations_[index]->checked() );
		++index;
	}
	index = 0;

	// Access all the choices items, their id and # of choice per id
	const MachPhysComplexityManager::ChoiceItems& chItems = MachPhysComplexityManager::instance().choiceItems();
	for( MachPhysComplexityManager::ChoiceItems::const_iterator it = chItems.begin(); it != chItems.end(); ++it )
	{
		uint id = (*it)->id();
        // TODO: void* to uint??, this ptr value is stored simply as number?
		//MachPhysComplexityManager::instance().changeChoiceItem( id, ( uint ) choicesOptimisations_[index]->item() - 1 );
		MachPhysComplexityManager::instance().changeChoiceItem( id, ( size_t ) choicesOptimisations_[index]->item() - 1 );
		++index;
	}

	// Set Direct3DDriver to be the one selected in the list of available Direct3DDrivers
	//CtlCountedPtr<RenDriver> const & newDriver = ( CtlCountedPtr<RenDriver> const & ) *(pDirect3DDrivers_->item());
//	CtlCountedPtr<RenDriver> const & newDriver =
//        ( CtlCountedPtr<RenDriver> const & ) *((CtlCountedPtr<RenDriver>*)pDirect3DDrivers_->item());

//	pDriverSelector_->useD3dDriver( newDriver );

	//Save display driver info in registry
//	pDriverSelector_->updateDriverRegistries();
}

void MachGuiCtxOptions::setOptions( void )
{
	// Used to set gui items to reflect current game settings
	SysRegistry::KeyHandle handle;
	if( SysRegistry::instance().onlyOpenKey( "Options\\3DSound", &handle, SysRegistry::CURRENT_USER ) == SysRegistry::SUCCESS )
	{
		pSound3d_->check( SysRegistry::instance().queryIntegerValue( "Options\\3DSound", "on", SysRegistry::CURRENT_USER ) );
	}
	else
	{
		pSound3d_->check( SndMixer::instance().is3dMixer() );
	}

	// Set resolution lock on if it the first time the game is being run
	SysRegistry::KeyHandle resolutionHandle;

	if( SysRegistry::instance().onlyOpenKey( "Screen Resolution\\Lock Resolution", &resolutionHandle, SysRegistry::CURRENT_USER ) != SysRegistry::SUCCESS )
	{
		SysRegistry::instance().setIntegerValue( "Screen Resolution", "Lock Resolution", 1, SysRegistry::CURRENT_USER );
	}

	pScreenResolutionLock_->check( SysRegistry::instance().queryIntegerValue( "Screen Resolution", "Lock Resolution", SysRegistry::CURRENT_USER ) );
 	pCursorType_->check( SysRegistry::instance().queryIntegerValue( "Options\\Cursor Type", "2D", SysRegistry::CURRENT_USER ) );
	pReverseKeys_->check( SysRegistry::instance().queryIntegerValue( "Options\\Reverse UpDown Keys", "on", SysRegistry::CURRENT_USER ) );
	pReverseMouse_->check( SysRegistry::instance().queryIntegerValue( "Options\\Reverse BackForward Mouse", "on", SysRegistry::CURRENT_USER ) );
	pTransitions_->check( pStartupScreens_->startupData()->transitionFlicsOn() );

	// Access all the boolean optimisations
	const MachPhysComplexityManager::BooleanItems& boolItems =	MachPhysComplexityManager::instance().booleanItems();
	uint index = 0;
	for( MachPhysComplexityManager::BooleanItems::const_iterator it = boolItems.begin(); it != boolItems.end(); ++it )
	{
		uint id = (*it)->id();
		booleanOptimisations_[index]->check( (*it)->enabled() );
		++index;
	}

	index = 0;
	// Access all the choices items, their id and # of choice per id
	const MachPhysComplexityManager::ChoiceItems& chItems = MachPhysComplexityManager::instance().choiceItems();
	for( MachPhysComplexityManager::ChoiceItems::const_iterator it = chItems.begin(); it != chItems.end(); ++it )
	{
		uint id = (*it)->id();

		GuiResourceString choice( id + (*it)->choice() + 1 );

		choicesOptimisations_[index]->text( choice.asString() );

		++index;
	}

	cursorType2d_ = pCursorType_->checked();
}

//static
void MachGuiCtxOptions::load3dSoundFiles( bool enabled )
{
	// Load the sound definition file depending on 3d sound enabled and the bit rate currently being used

	unsigned int totalMem =	SysMemoryCaps::totalPhysicalMem();

	//World4d needs to know what sounds we are operating with.
	enabled ? W4dSoundManager::instance().currentSoundDimensions(Snd::THREE_D) : W4dSoundManager::instance().currentSoundDimensions(Snd::TWO_D);

	if( enabled )
	{
		if(totalMem > 64000000)
			W4dSoundManager::instance().readSoundDefinitionFile("sounds/snddef64.dat");
		else if(totalMem > 32000000)
			W4dSoundManager::instance().readSoundDefinitionFile("sounds/snddef32.dat");
		else
			W4dSoundManager::instance().readSoundDefinitionFile("sounds/snddef16.dat");
	}
	else
	{
		if(totalMem > 64000000)
			W4dSoundManager::instance().readSoundDefinitionFile("sounds/sd2d64.dat");
		else if(totalMem > 32000000)
			W4dSoundManager::instance().readSoundDefinitionFile("sounds/sd2d32.dat");
		else
			W4dSoundManager::instance().readSoundDefinitionFile("sounds/sd2d16.dat");
	}
}

void MachGuiCtxOptions::showDirect3DDrivers()
{
	//CtlCountedPtr<RenDriver> const & newDriver = ( CtlCountedPtr<RenDriver> const & ) *(pDirectDrawDrivers_->item());
	CtlCountedPtr<RenDriver> const & newDriver =
        ( CtlCountedPtr<RenDriver> const & ) *((CtlCountedPtr<RenDriver>*)pDirectDrawDrivers_->item());
	pDriverSelector_->useDDrawDriver( newDriver );

	_DELETE( pDirect3DDrivers_ );

    const RenDriverSelector::RenDrivers& d3dDriversList = pDriverSelector_->d3dDrivers();
	MachGuiDropDownListBoxCreator::DropDownListBoxItems d3dDrivers;
	GuiStrings d3dDriverNames;
	d3dDriverNames.reserve( 4 );
	d3dDrivers.reserve( 4 );

	for( RenDriverSelector::RenDrivers::const_iterator i = d3dDriversList.begin(); i != d3dDriversList.end(); ++i )
	{
		d3dDrivers.push_back( ( MachGuiDropDownListBoxCreator::DropDownListBoxItem ) &(*i) );
		d3dDriverNames.push_back( (*i)->name() );
	}
 	pDirect3DDrivers_ = _NEW( MachGuiDropDownListBoxCreator( pStartupScreens_,
														pStartupScreens_,
														Gui::Coord( 353, 159 ),
														153,
														d3dDriverNames,
														false,
														true ) );
   	pDirect3DDrivers_->items( d3dDrivers );

	pDirect3DDrivers_->text( pDriverSelector_->currentD3dDriver()->name() );
}

void MachGuiCtxOptions::exitFromOptions()
{
	exitFromOptions_ = true;
}

/* End CTXOPTNS.CPP *************************************************/
