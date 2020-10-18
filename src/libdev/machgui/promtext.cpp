/*
 * P R O M T E X T . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "ctl/list.hpp"
#include "ctl/pvector.hpp"
#include "machgui/gui.hpp"
#include "machgui/promtext.hpp"
#include "machgui/cameras.hpp"
#include "machgui/menutext.hpp"
#include "machgui/strtdata.hpp"
#include "machgui/chatmsgs.hpp"
#include "machgui/ingame.hpp"
#include "machlog/network.hpp"
#include "machlog/races.hpp"
#include "machlog/cntrl_pc.hpp"
#include "machlog/pod.hpp"
#include "machlog/canattac.hpp"
#include "machlog/weapon.hpp"
#include "machlog/nuclbomb.hpp"
#include "machlog/missilem.hpp"
#include "machlog/restree.hpp"
#include "machlog/resitem.hpp"
#include "machphys/objdata.hpp"
#include "machphys/wepdata.hpp"
#include "machphys/mphydata.hpp"
#include "gui/manager.hpp"
#include "gui/event.hpp"
#include "gui/font.hpp"
#include "gui/painter.hpp"
#include "gui/restring.hpp"
#include "device/butevent.hpp"
#include "machgui/internal/strings.hpp"
#include "world4d/manager.hpp"
#include "world4d/scenemgr.hpp"
#include "render/device.hpp"
#include "render/surfmgr.hpp"
#include "device/cd.hpp"
//#include <mbstring.h>

#define SYSTEM_MESSAGE 100

typedef ctl_vector<string> strings;

extern unsigned char pPromptText1[];
extern unsigned char pPromptTextKey1[];
extern unsigned char pPromptText2[];
extern unsigned char pPromptTextKey2[];
extern unsigned char pPromptText3[];
extern unsigned char pPromptTextKey3[];
extern unsigned char pPromptText4[];
extern unsigned char pPromptTextKey4[];
extern unsigned char pPromptText5[];
extern unsigned char pPromptTextKey5[];
extern unsigned char pPromptText6[];
extern unsigned char pPromptTextKey6[];
extern unsigned char pPromptText7[];
extern unsigned char pPromptTextKey7[];
extern unsigned char pPromptText8[];
extern unsigned char pPromptTextKey8[];
extern unsigned char pPromptText9[];
extern unsigned char pPromptTextKey9[];


bool checkEnteredText( const unsigned char* pEncryptedText, const unsigned char* pDecryptKey, const string& enteredText )
{
//	int sizeOfEncryptedText = _mbslen( pEncryptedText );
	int sizeOfEncryptedText = 10; // Magic codes, have a fun decrypting them - martin.

	string decryptedText;

	if ( sizeOfEncryptedText != enteredText.length() )
	{
		return false;
	}

	for ( int i = 0; i < sizeOfEncryptedText; ++i )
	{
		decryptedText += pEncryptedText[i] ^ pDecryptKey[i];
	}

	return enteredText == decryptedText;
}

class MachPromptTextImpl
{
public:
	MachPromptTextImpl();

	GuiBitmap promptBmp_;
    string cursorPromptText_; //The prompt displayed for mouse moves
    string commandPromptText_; //The prompt displayed for current command
	strings cursorPromptTextLines_;
	strings commandPromptTextLines_;
	MachCameras* pCameras_;
	bool displayCursorPromptText_;
	MachPromptText::PromptDisplayed promptDisplayed_;
	GuiBmpFont font_;
	GuiBmpFont shadowFont_;
	int blitToX_;
	bool refresh_;
	bool restartScroll_;
	GuiBitmap lightOn_;
	GuiBitmap lightOff_;
	bool enteringChatMessage_;
	MachPhys::Race chatMessageIntendedForRace_;
	string chatMessageIntendedForStr_;
	string lastDisplayedChatMessage_;
	int beginningTextWidth_;
	GuiDisplayable* pPassEventsTo_;
	int standardMessageIndex_;
	int opponentIndex_;
	MachInGameScreen* pInGameScreen_;
};

MachPromptTextImpl::MachPromptTextImpl()
:	font_( GuiBmpFont::getFont( "gui/menu/promtfnt.bmp" ) ),
	shadowFont_( GuiBmpFont::getFont( "gui/menu/promdfnt.bmp" ) ),
	refresh_( true ),
	lightOn_( Gui::bitmap( "gui/misc/tplight2.bmp" ) ),
	lightOff_( Gui::bitmap( "gui/misc/tplight1.bmp" ) ),
	enteringChatMessage_( false ),
	chatMessageIntendedForRace_( MachPhys::N_RACES ),
	beginningTextWidth_( 0 ),
	pPassEventsTo_( NULL ),
	standardMessageIndex_( 0 ),
	pInGameScreen_( NULL )
{
	lightOn_.enableColourKeying();
	lightOff_.enableColourKeying();
}

MachPromptText::MachPromptText( MachInGameScreen* pParent, const Gui::Boundary& relativeBoundary, MachCameras* pCameras, GuiDisplayable* pPassEventsTo )
: 	GuiSingleLineEditBox( pParent, relativeBoundary, GuiBmpFont::getFont( "gui/menu/promtfnt.bmp" ) )
{
	pImpl_ = _NEW( MachPromptTextImpl() );

	CB_DEPIMPL( GuiBitmap, promptBmp_ );
	CB_DEPIMPL( MachCameras*, pCameras_ );
	CB_DEPIMPL( MachPromptText::PromptDisplayed, promptDisplayed_ );
	CB_DEPIMPL( int, blitToX_ );
	CB_DEPIMPL( GuiDisplayable*, pPassEventsTo_ );
	CB_DEPIMPL( MachInGameScreen*, pInGameScreen_ );

	setLayer( GuiDisplayable::LAYER4 );

	promptBmp_ = RenSurface::createAnonymousSurface( width(), height(), Gui::backBuffer() );
	promptBmp_.enableColourKeying();
	pCameras_ 		= pCameras;
	promptDisplayed_= NOTEXT;
	blitToX_ 		= 0;
	pPassEventsTo_ 	= pPassEventsTo;
	pInGameScreen_  = pParent;

	redrawEveryFrame( true );

	maxChars( MAX_INGAMECHATMESSAGE_LEN - MAX_PLAYERNAME_LEN - 5 );

    TEST_INVARIANT;
}

MachPromptText::~MachPromptText()
{
    TEST_INVARIANT;
	_DELETE( pImpl_ );
}

void MachPromptText::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPromptText& t )
{

    o << "MachPromptText " << (void*)&t << " start" << std::endl;
    o << "MachPromptText " << (void*)&t << " end" << std::endl;

    return o;
}

void MachPromptText::cursorPromptText( const string& prompt )
{
	cursorPromptText( prompt, true );
}

void MachPromptText::cursorPromptText( const string& prompt, bool restartScroll )
{
	CB_DEPIMPL( string, cursorPromptText_ );
	CB_DEPIMPL( MachPromptText::PromptDisplayed, promptDisplayed_ );
	CB_DEPIMPL( bool, refresh_ );
	CB_DEPIMPL( bool, restartScroll_ );
	CB_DEPIMPL( strings, cursorPromptTextLines_ );
	CB_DEPIMPL( GuiBmpFont, font_ );

	if ( prompt != cursorPromptText_ )
	{
    	cursorPromptText_ = prompt;

		cursorPromptTextLines_.erase( cursorPromptTextLines_.begin(), cursorPromptTextLines_.end() );
		MachGuiMenuText::chopUpText( prompt, 2000, font_, &cursorPromptTextLines_ );

		refresh_ = true;

		if ( promptDisplayed_ != CURSORTEXT )
		{
			restartScroll_ = true;
		}
		else
		{
			restartScroll_ = restartScroll;
		}
	}
}

const string& MachPromptText::cursorPromptText() const
{
	CB_DEPIMPL( string, cursorPromptText_ );

    return cursorPromptText_;
}

void MachPromptText::clearCursorPromptText()
{
	CB_DEPIMPL( string, cursorPromptText_ );
	CB_DEPIMPL( MachPromptText::PromptDisplayed, promptDisplayed_ );
	CB_DEPIMPL( bool, refresh_ );
	CB_DEPIMPL( bool, restartScroll_ );

	if ( cursorPromptText_ != "" )
	{
    	cursorPromptText_ = "";
		promptDisplayed_ = NOTEXT;
	}
}


void MachPromptText::commandPromptText( const string& prompt )
{
	CB_DEPIMPL( string, commandPromptText_ );
	CB_DEPIMPL( MachPromptText::PromptDisplayed, promptDisplayed_ );
	CB_DEPIMPL( bool, refresh_ );
	CB_DEPIMPL( bool, restartScroll_ );
	CB_DEPIMPL( strings, commandPromptTextLines_ );
	CB_DEPIMPL( GuiBmpFont, font_ );

	if ( commandPromptText_ != prompt )
	{
    	commandPromptText_ = prompt;

		commandPromptTextLines_.erase( commandPromptTextLines_.begin(), commandPromptTextLines_.end() );
		MachGuiMenuText::chopUpText( prompt, 2000, font_, &commandPromptTextLines_ );

		if ( promptDisplayed_ != CURSORTEXT )
		{
		   refresh_ = true; // Force prompt text to be refreshed
		   restartScroll_ = true;
		}
	}
}

const string& MachPromptText::commandPromptText() const
{
	CB_DEPIMPL( string, commandPromptText_ );

    return commandPromptText_;
}

void MachPromptText::clearCommandPromptText()
{
	CB_DEPIMPL( string, commandPromptText_ );
	CB_DEPIMPL( MachPromptText::PromptDisplayed, promptDisplayed_ );
	CB_DEPIMPL( bool, refresh_ );
	CB_DEPIMPL( bool, restartScroll_ );

	if ( commandPromptText_ != "" )
	{
    	commandPromptText_ = "";

		if ( promptDisplayed_ == COMMANDTEXT )
		{
			promptDisplayed_ = NOTEXT;
		}
	}
}

void MachPromptText::doDisplay()
{
	CB_DEPIMPL( string, commandPromptText_ );
	CB_DEPIMPL( string, cursorPromptText_ );
	CB_DEPIMPL( strings, cursorPromptTextLines_ );
	CB_DEPIMPL( strings, commandPromptTextLines_ );
	CB_DEPIMPL( MachPromptText::PromptDisplayed, promptDisplayed_ );
	CB_DEPIMPL( GuiBitmap, promptBmp_ );
	CB_DEPIMPL( GuiBitmap, lightOn_ );
	CB_DEPIMPL( GuiBitmap, lightOff_ );
	CB_DEPIMPL( GuiBmpFont, font_ );
	CB_DEPIMPL( GuiBmpFont, shadowFont_ );
	CB_DEPIMPL( int, blitToX_ );
	CB_DEPIMPL( bool, refresh_ );
	CB_DEPIMPL( bool, restartScroll_ );
	CB_DEPIMPL( bool, enteringChatMessage_ );

	const int textScrollSpeed = 20;

	if ( enteringChatMessage_ )
	{
		displayChatMessage();
	}
	//Decide which text to display. Cursor prompt takes priority
    else if( cursorPromptText_.length() != 0 )
	{
		if ( refresh_ or promptDisplayed_ != CURSORTEXT  )
		{
			promptDisplayed_ = CURSORTEXT;
			refresh_ = false;
			promptBmp_.filledRectangle( RenSurface::Rect( 0, 0, promptBmp_.width(), promptBmp_.height() ), Gui::MAGENTA() );

			// Render all the lines
			int startY = 0;
			if ( cursorPromptTextLines_.size() == 1 )
			{
				startY += shadowFont_.charHeight() / 2;
			}

			for ( strings::iterator iter = cursorPromptTextLines_.begin(); iter != cursorPromptTextLines_.end(); ++iter )
			{
				shadowFont_.drawText( &promptBmp_, *iter, Gui::Coord(1,startY + 1), promptBmp_.width() - 1 );
				font_.drawText( &promptBmp_, *iter, Gui::Coord(0,startY), promptBmp_.width() );
				startY += shadowFont_.charHeight() + 1;
			}

			if ( restartScroll_ )
				blitToX_ = 0;
		}
		// Blit text.
		GuiPainter::instance().blit(promptBmp_,
									Gui::Box( 0, 0, blitToX_, promptBmp_.height() ),
									Gui::Coord( absoluteBoundary().minCorner().x() + lightOn_.width() + 1, absoluteBoundary().minCorner().y() + 7 ) );

		// Scroll text to new pos for next frame.
		if ( blitToX_ != promptBmp_.width() )
		{
			blitToX_ += textScrollSpeed;
			if ( blitToX_ >= width() )
			{
				blitToX_ = promptBmp_.width();
			}

			// Blit light on graphic
			GuiPainter::instance().blit( lightOn_, absoluteBoundary().minCorner() );
		}
		else
		{
			// Blit light off graphic
			GuiPainter::instance().blit( lightOff_, absoluteBoundary().minCorner() );
		}
	}
    else if( commandPromptText_.length() != 0 )
	{
        if ( refresh_ or promptDisplayed_ != COMMANDTEXT )
		{
			promptDisplayed_ = COMMANDTEXT;
			refresh_ = false;
			promptBmp_.filledRectangle( RenSurface::Rect( 0, 0, promptBmp_.width(), promptBmp_.height() ), Gui::MAGENTA() );

			// Render all the lines
			int startY = 0;
			if ( commandPromptTextLines_.size() == 1 )
			{
				startY += shadowFont_.charHeight() / 2;
			}

			for ( strings::iterator iter = commandPromptTextLines_.begin(); iter != commandPromptTextLines_.end(); ++iter )
			{
				shadowFont_.drawText( &promptBmp_, *iter, Gui::Coord(1,startY + 1), promptBmp_.width() - 1 );
				font_.drawText( &promptBmp_, *iter, Gui::Coord(0,startY), promptBmp_.width() );
				startY += shadowFont_.charHeight() + 1;
			}

			if ( restartScroll_ )
				blitToX_ = 0;
		}
		// Blit text.
		GuiPainter::instance().blit(promptBmp_,
									Gui::Box( 0, 0, blitToX_, promptBmp_.height() ),
									Gui::Coord( absoluteBoundary().minCorner().x() + lightOn_.width() + 1, absoluteBoundary().minCorner().y() + 7 ) );


		if ( blitToX_ != promptBmp_.width() )
		{
			blitToX_ += textScrollSpeed;
			if ( blitToX_ >= width() )
			{
				blitToX_ = promptBmp_.width();
			}

			// Blit light on graphic
			GuiPainter::instance().blit( lightOn_, absoluteBoundary().minCorner() );
		}
		else
		{
			// Blit light off graphic
			GuiPainter::instance().blit( lightOff_, absoluteBoundary().minCorner() );
		}
	}
	else
	{
		promptDisplayed_ = NOTEXT;

		// Scroll text back into control panel
		if ( blitToX_ != 0 )
		{
			{
				blitToX_ -= std::min( blitToX_, textScrollSpeed );

				// Blit text.
				GuiPainter::instance().blit(promptBmp_,
											Gui::Box( 0, 0, blitToX_, promptBmp_.height() ),
											Gui::Coord( absoluteBoundary().minCorner().x() + lightOn_.width() + 1, absoluteBoundary().minCorner().y() + 7 ) );


			}
		}

		// Blit light off graphic
		GuiPainter::instance().blit( lightOff_, absoluteBoundary().minCorner() );
	}
}

//virtual
bool MachPromptText::doHandleCharEvent( const GuiCharEvent& event )
{
	CB_DEPIMPL( bool, enteringChatMessage_ );

	if ( enteringChatMessage_ )
		return GuiSingleLineEditBox::doHandleCharEvent( event );

	return false;
}

void MachPromptText::displayChatMessage()
{
	CB_DEPIMPL( GuiBitmap, promptBmp_ );
	CB_DEPIMPL( GuiBitmap, lightOn_ );
	CB_DEPIMPL( GuiBmpFont, font_ );
	CB_DEPIMPL( GuiBmpFont, shadowFont_ );
	CB_DEPIMPL( string, chatMessageIntendedForStr_ );
	CB_DEPIMPL( string, lastDisplayedChatMessage_ );
	CB_DEPIMPL( int, beginningTextWidth_ );

	// Update the caret
	update();

	string displayChatMessageStr = chatMessageIntendedForStr_ + text();
	int caretPosition = caretPos() + beginningTextWidth_;
	int startY = shadowFont_.charHeight() / 2;

	// Make sure we only update when the text has changed
	if ( lastDisplayedChatMessage_ != displayChatMessageStr )
	{
		lastDisplayedChatMessage_ = displayChatMessageStr;

		promptBmp_.filledRectangle( RenSurface::Rect( 0, 0, promptBmp_.width(), promptBmp_.height() ), Gui::MAGENTA() );

		// Work out starting position for text
		Gui::Coord startBeginningText = Gui::Coord( 0, startY );
		Gui::Coord shadowStartBeginningText = Gui::Coord( 1, startY + 1 );
		Gui::Coord startText = Gui::Coord( caretPosition, startY );
		Gui::Coord shadowStartText = Gui::Coord( caretPosition + 1, startY + 1 );

		// Draw shadow and normal text
		shadowFont_.drawText( &promptBmp_, chatMessageIntendedForStr_, shadowStartBeginningText, promptBmp_.width() );
		shadowFont_.drawText( &promptBmp_, rightText(), shadowStartText, promptBmp_.width() - caretPosition - 1 - beginningTextWidth_ );
		shadowFont_.drawText( &promptBmp_, leftText(), shadowStartText, caretPosition - beginningTextWidth_, GuiBmpFont::RIGHT_JUSTIFY );
		font_.drawText( &promptBmp_, chatMessageIntendedForStr_, startBeginningText, promptBmp_.width() );
		font_.drawText( &promptBmp_, rightText(), startText, promptBmp_.width() - caretPosition - beginningTextWidth_ );
		font_.drawText( &promptBmp_, leftText(), startText, caretPosition - beginningTextWidth_, GuiBmpFont::RIGHT_JUSTIFY );
	}

	// Blit text.
	GuiPainter::instance().blit(promptBmp_,
								Gui::Box( 0, 0, promptBmp_.width(), promptBmp_.height() ),
								Gui::Coord( absoluteBoundary().minCorner().x() + lightOn_.width() + 1, absoluteBoundary().minCorner().y() + 7 ) );

	if ( GuiManager::instance().charFocusExists() and
		 &GuiManager::instance().charFocus() == this and
		 showCaret() ) // Only show caret if we have focus
	{
		GuiPainter::instance().line(Gui::Coord( absoluteBoundary().minCorner().x() + caretPosition + lightOn_.width() + 1, absoluteBoundary().minCorner().y() + startY + 7 ),
								 	Gui::Coord( absoluteBoundary().minCorner().x() + caretPosition + lightOn_.width() + 1, absoluteBoundary().minCorner().y() + startY + font_.charHeight() + 7 ),
								 	caretColour(), 1 );
	}

	// Blit light on graphic
	GuiPainter::instance().blit( lightOn_, absoluteBoundary().minCorner() );
}

// virtual
bool MachPromptText::doHandleKeyEvent( const GuiKeyEvent& event )
{
	CB_DEPIMPL( bool, enteringChatMessage_ );
	CB_DEPIMPL( bool, refresh_ );
	CB_DEPIMPL( bool, restartScroll_ );
	CB_DEPIMPL( MachPhys::Race, chatMessageIntendedForRace_ );
	CB_DEPIMPL( string, chatMessageIntendedForStr_ );
	CB_DEPIMPL( int, beginningTextWidth_ );
	CB_DEPIMPL( GuiBmpFont, shadowFont_ );
	CB_DEPIMPL( int, opponentIndex_ );
	CB_DEPIMPL( int, standardMessageIndex_ );
	CB_DEPIMPL( MachInGameScreen*, pInGameScreen_ );

	bool processed = false;

	if ( event.state() == Gui::PRESSED )
	{
		if ( 	( event.key() == DevKey::F1 or event.key() == DevKey::F2 or event.key() == DevKey::F3 ) and
				not event.isShiftPressed() and
				MachLogNetwork::instance().isNetworkGame() )
		{
			// Work out who we are going to send the message to
			int newOpponentIndex = 0;

			if ( event.key() == DevKey::F2 )
				newOpponentIndex = 1;
			else if ( event.key() == DevKey::F3 )
				newOpponentIndex = 2;

			if( event.isCtrlPressed() )
			{
				if( MachGuiInGameChatMessages::instance().opponentExists( newOpponentIndex ) )
				{
					MachPhys::Race targetRace = MachGuiInGameChatMessages::instance().opponentRace( newOpponentIndex );
					MachPhys::Race ourRace = MachLogRaces::instance().pcController().race();
					MachLogRaces::DispositionToRace disp = MachLogRaces::instance().dispositionToRace( ourRace, targetRace );
					MachLogRaces::DispositionToRace newDisp = MachLogRaces::ALLY;
					if( disp == MachLogRaces::ALLY )
						newDisp = MachLogRaces::NEUTRAL;
					MachLogRaces::instance().dispositionToRace( ourRace, targetRace, newDisp );
				}
			}
			else
			{
				//Are we already entering a message and want to cycle through a standard list of messages?
				if ( enteringChatMessage_ and opponentIndex_ == newOpponentIndex )
				{
					if ( MachGuiInGameChatMessages::instance().standardMessages().size() > 0 )
					{
						if ( standardMessageIndex_ >= MachGuiInGameChatMessages::instance().standardMessages().size() )
						{
							standardMessageIndex_ = 0;
						}

						text( MachGuiInGameChatMessages::instance().standardMessages()[standardMessageIndex_] );
						// Display next message next time
						++standardMessageIndex_;
					}
				}
				else // A new message ( or new opponent )...
				{
					opponentIndex_ = newOpponentIndex;
					standardMessageIndex_ = 0;

					if ( MachGuiInGameChatMessages::instance().opponentExists( opponentIndex_ ) )
					{
						enteringChatMessage_ = true;
						GuiManager::instance().charFocus( this );
						chatMessageIntendedForRace_ = MachGuiInGameChatMessages::instance().opponentRace( opponentIndex_ );

						// Add coloured token to beginning of message
						switch ( chatMessageIntendedForRace_ )
						{
							case MachPhys::RED:
								chatMessageIntendedForStr_ = "©";
								break;
							case MachPhys::GREEN:
								chatMessageIntendedForStr_ = "ª";
								break;
							case MachPhys::BLUE:
								chatMessageIntendedForStr_ = "«";
								break;
							case MachPhys::YELLOW:
								chatMessageIntendedForStr_ = "¬";
								break;
						}
						GuiResourceString sendToStr( IDS_SENDTO );
						chatMessageIntendedForStr_ += sendToStr.asString();
						chatMessageIntendedForStr_ += MachGuiInGameChatMessages::instance().opponentName( opponentIndex_ );
						chatMessageIntendedForStr_ += ": ";
						beginningTextWidth_ = shadowFont_.textWidth( chatMessageIntendedForStr_ );
					}
				}
			}
		}
   		else if ( 	event.key() == DevKey::F4 and
   					not event.isShiftPressed() and
   					MachLogNetwork::instance().isNetworkGame() )
		{
			int newOpponentIndex = 3;

			// Are we already entering a message and want to cycle through a standard list of messages?
			if ( enteringChatMessage_ and opponentIndex_ == newOpponentIndex )
			{
				if ( MachGuiInGameChatMessages::instance().standardMessages().size() > 0 )
				{
					if ( standardMessageIndex_ >= MachGuiInGameChatMessages::instance().standardMessages().size() )
					{
						standardMessageIndex_ = 0;
					}

					text( MachGuiInGameChatMessages::instance().standardMessages()[standardMessageIndex_] );
					// Display next message next time
					++standardMessageIndex_;
				}
			}
			else
			{
				standardMessageIndex_ = 0;
				opponentIndex_ = newOpponentIndex;
				enteringChatMessage_ = true;
				GuiManager::instance().charFocus( this );
				chatMessageIntendedForRace_ = MachPhys::N_RACES;

				GuiResourceString sendToEveryoneStr( IDS_SENDTOEVERYONE );
				chatMessageIntendedForStr_ = sendToEveryoneStr.asString();
				beginningTextWidth_ = shadowFont_.textWidth( chatMessageIntendedForStr_ );
			}
		}
		else if ( event.key() == DevKey::F11 and event.isShiftPressed() and event.isCtrlPressed() and event.isAltPressed() )
		{
		  	opponentIndex_ = SYSTEM_MESSAGE;
		  	enteringChatMessage_ = true;
		  	GuiManager::instance().charFocus( this );
		  	chatMessageIntendedForRace_ = MachPhys::N_RACES;

		  	GuiResourceString sendToSystemStr( IDS_SENDTOSYSTEM );
		  	chatMessageIntendedForStr_ = sendToSystemStr.asString();
		  	beginningTextWidth_ = shadowFont_.textWidth( chatMessageIntendedForStr_ );
		}
		else if ( 	event.key() == DevKey::ESCAPE or
					event.key() == DevKey::ENTER  or
					event.key() == DevKey::F10 )
		{
			if ( enteringChatMessage_ )
			{
				// Send a chat message
				if ( event.key() == DevKey::ENTER )
				{
					if ( opponentIndex_ != SYSTEM_MESSAGE )
					{
						string chatMessageStr;

						// Add coloured token to beginning of message
						switch ( MachGuiInGameChatMessages::instance().playerRace() )
						{
							case MachPhys::RED:
								chatMessageStr += "©";
								break;
							case MachPhys::GREEN:
								chatMessageStr += "ª";
								break;
							case MachPhys::BLUE:
								chatMessageStr += "«";
								break;
							case MachPhys::YELLOW:
								chatMessageStr += "¬";
								break;
						}
						chatMessageStr += MachGuiInGameChatMessages::instance().playerName();
						chatMessageStr += ": ";
						chatMessageStr += text();

						MachGuiInGameChatMessages::instance().sendMessage( chatMessageStr, chatMessageIntendedForRace_ );
						MachGuiInGameChatMessages::instance().addMessage( chatMessageIntendedForStr_ + text() );
					}
					else
					{
//						if ( strnicmp( text().c_str(), "MUSIC", 5 ) == 0 and text().length() > 5 )
						if ( strncasecmp( text().c_str(), "MUSIC", 5 ) == 0 and text().length() > 5 )
						{
							int track = atoi( &text().c_str()[5] );

							if ( 	track > 0 and // Valid track number entered
									DevCD::instance().isAudioCDPresent() and // Audio CD is in CD-Rom
									track < DevCD::instance().numberOfTracks() ) // Track number is not outside the number of tracks on CD
							{
								DevCD::instance().play( track );
							}
						}
						else if ( strcasecmp( text().c_str(), "EXIT" ) == 0 )	// Instant Exit
						{
							pInGameScreen_->instantExit( true );
						}
						else if ( checkEnteredText( pPromptText1, pPromptTextKey1, text() ) )	// Remove Fog Of War
						{
							pInGameScreen_->fogOfWarOn( false );
						}
						else if ( checkEnteredText( pPromptText2, pPromptTextKey2, text() ) ) // Instant heal
						{
							const MachInGameScreen::Actors& actors = pInGameScreen_->selectedActors();
							for ( MachInGameScreen::Actors::const_iterator iter = actors.begin(); iter != actors.end(); ++iter )
							{
								MachActor* pActor = *iter;

								const MachPhysObjectData& objData = pActor->objectData();
								pActor->setHPAndArmour( objData.hitPoints(), objData.armour() );
							}
						}
						else if ( checkEnteredText( pPromptText3, pPromptTextKey3, text() ) ) // Activate Ion Cannon
						{
							MachLogRaces::Pods& pods = MachLogRaces::instance().pods( MachLogRaces::instance().pcController().race() );
							for ( MachLogRaces::Pods::iterator iter = pods.begin(); iter != pods.end(); ++iter )
							{
								(*iter)->activateIonCannon();
								(*iter)->weapons().front()->setChargeToMaximum();
							}
						}
						else if ( checkEnteredText( pPromptText4, pPromptTextKey4, text() ) ) // Max accuracy
						{
							const MachInGameScreen::Actors& actors = pInGameScreen_->selectedActors();
							for ( MachInGameScreen::Actors::const_iterator iter = actors.begin(); iter != actors.end(); ++iter )
							{
								MachActor* pActor = *iter;

								if ( pActor->objectIsCanAttack() )
								{
									pActor->asCanAttack().decreaseMinimumInaccuracy( 100.0 );
								}
							}
						}
						else if ( checkEnteredText( pPromptText7, pPromptTextKey7, text() ) ) // Recharge Nukes
						{
							MachLogRaces::MissileEmplacements& silos = MachLogRaces::instance().missileEmplacements( MachLogRaces::instance().pcController().race() );
							for ( MachLogRaces::MissileEmplacements::iterator iter = silos.begin(); iter != silos.end(); ++iter )
							{
								if ( (*iter)->subType() == MachPhys::ICBM )
								{
									(*iter)->weapons().front()->setChargeToMaximum();
								}
							}
						}
						else if ( checkEnteredText( pPromptText8, pPromptTextKey8, text() ) ) // a test
						{
							const SysPathName testFile = "gui/menu/sa.bmp";
							if (testFile.existsAsFile())
							{
								string test;
								if (RenSurfaceManager::extractBMPText(testFile, &test))
									W4dManager::instance().sceneManager()->pDevice()->horizontalBannerText(test.c_str(), 200, 2);
							}
						}
						else if ( checkEnteredText( pPromptText9, pPromptTextKey9, text() ) ) // a test
						{
							MachLogResearchItem& resItem = MachLogRaces::instance().researchTree().researchItem( MachLog::AGGRESSOR, MachPhys::BALLISTA, 4, MachPhys::LR_LARGE_MISSILE_X2 );
							for( MachPhys::Race i = MachPhys::RED; i != MachPhys::N_RACES; ++((int&)i) )
								resItem.researched( i, true );
						}
						else if ( checkEnteredText( pPromptText5, pPromptTextKey5, text() ) ) // Kill *all* AI Machines/ME
						{
							MachLogRaces& races = MachLogRaces::instance();
							for( MachPhys::Race i = MachPhys::RED; i != MachPhys::N_RACES; ++((int&)i) )
							{
								if( races.raceInGame( i ) and races.controller( i ).type() == MachLogController::AI_CONTROLLER )
								{
									for( MachLogRaces::Objects::iterator j = races.raceObjects( i ).begin(); j != races.raceObjects( i ).end(); ++j )
									{
										MachActor& actor = **j;
										if( actor.objectIsMachine() or actor.objectType() == MachLog::MISSILE_EMPLACEMENT )
										{
											actor.selfDestruct( 0.3 );
										}
									}
								}
							}
						}
						else if ( checkEnteredText( pPromptText6, pPromptTextKey6, text() ) ) // Set off a nuke underneath some actors
						{
							MachLogRaces& races = MachLogRaces::instance();
							for( MachPhys::Race i = MachPhys::RED; i != MachPhys::N_RACES; ++((int&)i) )
							{
								if( races.raceInGame( i ) )
								{
									//we will detonate a maximum of 10 nukes per race
									//so one nuke will be set off (actors.size / 10 ) actors.
									int nukegap = races.raceObjects( i ).size() / 10;
									if( nukegap == 0 and races.raceObjects( i ).size() > 0 )
										nukegap = 1;
									if( nukegap > 0 )
									{
										int gap = nukegap;
										for( MachLogRaces::Objects::iterator j = races.raceObjects( i ).begin(); j != races.raceObjects( i ).end(); ++j )
										{
											--gap;
											if( gap == 0 )
											{
												MachActor& actor = **j;
												if( actor.objectIsMachine() )
												{
													_NEW( MachLogNuclearBomb( &races.race( i ),
																			  (*j)->position(),
																			  MachPhysData::instance().weaponData( MachPhys::NUCLEAR_MISSILE ),
																			  *j ) );
												}
												gap = nukegap;
											}
										}
									}
								}
							}
						}
					}
				}

				// Clear message from prompt text
				enteringChatMessage_ = false;
				GuiManager::instance().removeCharFocus();
				text( "" );
				refresh_ = true;
				restartScroll_ = true;
				processed = true;

			}
		}
		else if ( enteringChatMessage_ )
		{
			processed = GuiSingleLineEditBox::doHandleKeyEvent( event );
		}
	}

	return processed or enteringChatMessage_;
}

//virtual
int MachPromptText::maxWidth() const
{
	// Work out how much room there is to type in a chat message
	CB_DEPIMPL( int, beginningTextWidth_ );
	CB_DEPIMPL( GuiBitmap, lightOn_ );

	// Work out how much of the prompt text is being displayed on-screen
    const int w = W4dManager::instance().sceneManager()->pDevice()->windowWidth();
	Gui::XCoord promptTextXPos = absoluteBoundary().minCorner().x();

	int availableWidth = w - promptTextXPos;

	// Take into account other things that limit the available typing space
	return availableWidth - beginningTextWidth_ - lightOn_.width() - 1;
}

/* NA 30/11/98. New processesMouseEvents added to GuiDisplayable means I don't need these
//virtual
void MachPromptText::doHandleMouseClickEvent( const GuiMouseEvent& rel )
{
	CB_DEPIMPL( GuiDisplayable*, pPassEventsTo_ );

	GuiMouseEvent relToOther = rel;

	if ( passEventTo( &relToOther ) )
	{
		pPassEventsTo_->doHandleMouseClickEvent( relToOther );
	}
}

//virtual
void MachPromptText::doHandleMouseEnterEvent( const GuiMouseEvent& rel )
{
	CB_DEPIMPL( GuiDisplayable*, pPassEventsTo_ );

	GuiMouseEvent relToOther = rel;

	if ( passEventTo( &relToOther ) )
	{
		pPassEventsTo_->doHandleMouseEnterEvent( relToOther );
	}
}

//virtual
void MachPromptText::doHandleMouseExitEvent( const GuiMouseEvent& rel )
{
	CB_DEPIMPL( GuiDisplayable*, pPassEventsTo_ );

	GuiMouseEvent relToOther = rel;

	if ( passEventTo( &relToOther ) )
	{
		pPassEventsTo_->doHandleContainsMouseEvent( relToOther );
	}
	else
	{
		pPassEventsTo_->doHandleMouseExitEvent( relToOther );
	}
}

//virtual
void MachPromptText::doHandleContainsMouseEvent( const GuiMouseEvent& rel )
{
	CB_DEPIMPL( GuiDisplayable*, pPassEventsTo_ );

	GuiMouseEvent relToOther = rel;

	if ( passEventTo( &relToOther ) )
	{
		pPassEventsTo_->doHandleContainsMouseEvent( relToOther );
	}
}

bool MachPromptText::passEventTo( GuiMouseEvent* pMouseEvent )
{
	CB_DEPIMPL( GuiDisplayable*, pPassEventsTo_ );

	bool returnValue = false;

	Gui::Coord absEventPos = absoluteBoundary().minCorner();
	absEventPos.x( absEventPos.x() + pMouseEvent->coord().x() );
	absEventPos.y( absEventPos.y() + pMouseEvent->coord().y() );

	if ( pPassEventsTo_->contains( absEventPos ) )
	{
		pMouseEvent->translate( Gui::Vec( ( absEventPos.x() - pPassEventsTo_->absoluteCoord().x() ) - pMouseEvent->coord().x(),
										  ( absEventPos.y() - pPassEventsTo_->absoluteCoord().y() ) - pMouseEvent->coord().y() ) );
		returnValue = true;
	}

	return returnValue;
}
*/

//virtual
bool MachPromptText::processesMouseEvents()	const
{
	return false;
}

// Forced recompile 16/2/99 CPS
/* End PROMTEXT.CPP *************************************************/
