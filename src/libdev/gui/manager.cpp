//#include <windows.h>
#include <algorithm>
#include <functional>

//#include "ctl/algorith.hpp"
#include "ctl/list.hpp"
#include "device/eventq.hpp"
#include "gui/internal/managei.hpp"
#include "gui/manager.hpp"
#include "gui/displaya.hpp"
#include "gui/event.hpp"
#include "gui/mouse.hpp"
#include "gui/painter.hpp"
#include "render/device.hpp"
#include "render/display.hpp"

#ifndef PRODUCTION
#include "world4d/scenemgr.hpp"
#include "world4d/manager.hpp"
#endif

#define CB_GUIMANAGER_DEPIMPL() \
		CB_DEPIMPL( GuiColourScheme, colourScheme_ ); \
		CB_DEPIMPL( GuiRoot *, pRoot_ ); \
		CB_DEPIMPL( GuiDisplayable *, pMouseFocus_ ); \
		CB_DEPIMPL( GuiDisplayable *, pKeyboardFocus_ ); \
		CB_DEPIMPL( GuiDisplayable *, pCharacterFocus_ );

//////////////////////////////////////////////////////////////////////

GuiManager& GuiManager::instance()
{
	static GuiManager instance_;
	return instance_;
}

//////////////////////////////////////////////////////////////////////

typedef ctl_list< DevEventQueue::ScanCode > ScanCodes;
static ScanCodes& scanCodes()
{
	static ScanCodes s_;
	if( s_.empty() )
	{
		s_.push_back( DevKey::UP_ARROW_PAD ); 		s_.push_back( DevKey::DOWN_ARROW_PAD );
		s_.push_back( DevKey::LEFT_ARROW_PAD );		s_.push_back( DevKey::RIGHT_ARROW_PAD );
		s_.push_back( DevKey::ENTER_PAD ); 			s_.push_back( DevKey::INSERT_PAD );
		s_.push_back( DevKey::DELETE_PAD ); 		s_.push_back( DevKey::HOME_PAD );
		s_.push_back( DevKey::END_PAD ); 			s_.push_back( DevKey::PAGE_UP_PAD );
		s_.push_back( DevKey::PAGE_DOWN_PAD ); 		s_.push_back( DevKey::FIVE_PAD );
		s_.push_back( DevKey::ASTERISK_PAD ); 		s_.push_back( DevKey::FORWARD_SLASH_PAD );
		s_.push_back( DevKey::PLUS_PAD ); 			s_.push_back( DevKey::MINUS_PAD );
		s_.push_back( DevKey::PAD_0 ); 				s_.push_back( DevKey::PAD_1 );
		s_.push_back( DevKey::PAD_2 ); 				s_.push_back( DevKey::PAD_3 );
		s_.push_back( DevKey::PAD_4 ); 				s_.push_back( DevKey::PAD_5 );
		s_.push_back( DevKey::PAD_6 ); 				s_.push_back( DevKey::PAD_7 );
		s_.push_back( DevKey::PAD_8 ); 				s_.push_back( DevKey::PAD_9 );
		s_.push_back( DevKey::SCROLL_LOCK ); 		s_.push_back( DevKey::BREAK );
		s_.push_back( DevKey::RIGHT_CONTROL );		s_.push_back( DevKey::LEFT_CONTROL );
		s_.push_back( DevKey::UP_ARROW );			s_.push_back( DevKey::DOWN_ARROW );
		s_.push_back( DevKey::LEFT_ARROW );			s_.push_back( DevKey::RIGHT_ARROW );
		s_.push_back( DevKey::INSERT );				s_.push_back( DevKey::KEY_DELETE );
		s_.push_back( DevKey::HOME );				s_.push_back( DevKey::END );
		s_.push_back( DevKey::PAGE_UP );			s_.push_back( DevKey::PAGE_DOWN );
		s_.push_back( DevKey::ESCAPE );				s_.push_back( DevKey::ENTER );
		s_.push_back( DevKey::SPACE );				s_.push_back( DevKey::BACK_SPACE );
		s_.push_back( DevKey::TAB );				s_.push_back( DevKey::LEFT_SHIFT );
		s_.push_back( DevKey::RIGHT_SHIFT );		s_.push_back( DevKey::CAPS_LOCK );
		s_.push_back( DevKey::NUM_LOCK );
		s_.push_back( DevKey::F1 ); 				s_.push_back( DevKey::F2 );
		s_.push_back( DevKey::F3 );					s_.push_back( DevKey::F4 );
		s_.push_back( DevKey::F5 );					s_.push_back( DevKey::F6 );
		s_.push_back( DevKey::F7 );					s_.push_back( DevKey::F8 );
		s_.push_back( DevKey::F9 );					s_.push_back( DevKey::F10 );
		s_.push_back( DevKey::F11 ); 				s_.push_back( DevKey::F12 );
		s_.push_back( DevKey::KEY_A ); 				s_.push_back( DevKey::KEY_B );
		s_.push_back( DevKey::KEY_C ); 				s_.push_back( DevKey::KEY_D );
		s_.push_back( DevKey::KEY_E ); 				s_.push_back( DevKey::KEY_F );
		s_.push_back( DevKey::KEY_G ); 				s_.push_back( DevKey::KEY_H );
		s_.push_back( DevKey::KEY_I ); 				s_.push_back( DevKey::KEY_J );
		s_.push_back( DevKey::KEY_K ); 				s_.push_back( DevKey::KEY_L );
		s_.push_back( DevKey::KEY_M ); 				s_.push_back( DevKey::KEY_N );
		s_.push_back( DevKey::KEY_O ); 				s_.push_back( DevKey::KEY_P );
		s_.push_back( DevKey::KEY_Q ); 				s_.push_back( DevKey::KEY_R );
		s_.push_back( DevKey::KEY_S ); 				s_.push_back( DevKey::KEY_T );
		s_.push_back( DevKey::KEY_U ); 				s_.push_back( DevKey::KEY_V );
		s_.push_back( DevKey::KEY_W ); 				s_.push_back( DevKey::KEY_X );
		s_.push_back( DevKey::KEY_Y ); 				s_.push_back( DevKey::KEY_Z );
		s_.push_back( DevKey::KEY_0 ); 				s_.push_back( DevKey::KEY_1 );
		s_.push_back( DevKey::KEY_2 ); 				s_.push_back( DevKey::KEY_3 );
		s_.push_back( DevKey::KEY_4 ); 				s_.push_back( DevKey::KEY_5 );
		s_.push_back( DevKey::KEY_6 ); 				s_.push_back( DevKey::KEY_7 );
		s_.push_back( DevKey::KEY_8 ); 				s_.push_back( DevKey::KEY_9 );
		s_.push_back( DevKey::LEFT_MOUSE ); 		s_.push_back( DevKey::RIGHT_MOUSE );
	}

	return s_;
}

//////////////////////////////////////////////////////////////////////

GuiManager::GuiManager()
:	pImpl_( _NEW( GuiManagerImpl( GuiColourScheme::defaultScheme() ) ) )
{
	CB_GUIMANAGER_DEPIMPL();

	colourScheme_ 	= GuiColourScheme::defaultScheme();
	pRoot_ 			= NULL;
	pMouseFocus_ 	= NULL;
	pKeyboardFocus_ = NULL;
	pCharacterFocus_= NULL;

	ScanCodes::const_iterator i = scanCodes().begin();
	for( ; i != scanCodes().end(); ++i )
		DevEventQueue::instance().queueEvents( *i );

    // Event Queue is explicity told to handle mouse wheel scrolling events
    DevEventQueue::instance().queueEvents(DevKey::MIDDLE_MOUSE, DevButtonEvent::SCROLL_UP);
    DevEventQueue::instance().queueEvents(DevKey::MIDDLE_MOUSE, DevButtonEvent::SCROLL_DOWN);
}

GuiManager::~GuiManager()
{
	_DELETE( pImpl_ );
}

//////////////////////////////////////////////////////////////////////

bool GuiManager::hasRoot() const
{
	CB_GUIMANAGER_DEPIMPL();

	return pRoot_ != NULL;
}

const GuiRoot& GuiManager::root() const
{
	PRE( hasRoot() );

	CB_GUIMANAGER_DEPIMPL();

	return *pRoot_;
}

GuiRoot& GuiManager::root()
{
	PRE( hasRoot() );

	CB_GUIMANAGER_DEPIMPL();

	return *pRoot_;
}

//////////////////////////////////////////////////////////////////////

bool GuiManager::keyboardFocusExists() const
{
	CB_GUIMANAGER_DEPIMPL();

	return pKeyboardFocus_ != NULL;
}

void GuiManager::keyboardFocus( GuiDisplayable * pNewKeyboardFocus )
{
	PRE( pNewKeyboardFocus != NULL );

	CB_GUIMANAGER_DEPIMPL();

	pKeyboardFocus_  = pNewKeyboardFocus;
	POST( keyboardFocusExists() );
	POST( &keyboardFocus() == pNewKeyboardFocus );
}

GuiDisplayable& GuiManager::keyboardFocus()
{
	PRE( keyboardFocusExists() );

	CB_GUIMANAGER_DEPIMPL();

	return *pKeyboardFocus_;
}

//////////////////////////////////////////////////////////////////////

bool GuiManager::update()
{
    if( hasRoot() )
    {
    	processEvents();

		updateMouseFocus();

		// Call update on GuiRoot
		root().update();
    }

	bool finished = DevKeyboard::instance().keyCode( DevKey::KEY_Q );
	return not finished;
}

void GuiManager::display()
{
	CB_GUIMANAGER_DEPIMPL();

    if( hasRoot() )
    {
		root().display();
 	}

	// Reset 4x rendering. Only needed for first frame after a mode change
	GuiDisplayable::useFourTimesRender() = false;
 }

//////////////////////////////////////////////////////////////////////

//static
Gui::ButtonState buttonState( DevButtonEvent::Action a )
{
	return ( a == DevButtonEvent::PRESS ) ? Gui::PRESSED :Gui::RELEASED;
}

//static
Gui::ButtonState buttonState( bool isPressed )
{
	return isPressed ? Gui::PRESSED :Gui::RELEASED;
}

//////////////////////////////////////////////////////////////////////

GuiEventBase::ModifierKeys pollModifierKeys()
{
	GuiEventBase::ModifierKeys result = 0;

	if( DevKeyboard::instance().shiftPressed() )
		result |= GuiEventBase::SHIFT;
	if( DevKeyboard::instance().ctrlPressed() )
		result |= GuiEventBase::CTRL;
	if( DevKeyboard::instance().altPressed() )
		result |= GuiEventBase::ALT;

	return result;
}

GuiMouseEvent pollMouse()
{
	Gui::ButtonState left = buttonState( DevMouse::instance().leftButton() );
	Gui::ButtonState right = buttonState( DevMouse::instance().rightButton() );

	ASSERT( left != DevMouse::NO_CHANGE, "logic error" );
	ASSERT( right != DevMouse::NO_CHANGE, "logic error" );

	Gui::Coord c( DevMouse::instance().position().first, DevMouse::instance().position().second );
	return GuiMouseEvent( c, left, right, pollModifierKeys() );
}

//////////////////////////////////////////////////////////////////////

Gui::Vec unaryMinus( const Gui::Coord& c )
{
	return Gui::Vec( -c.x(), -c.y() );
}

void GuiManager::changeRoot( GuiRoot* pNewRoot )
{
	CB_GUIMANAGER_DEPIMPL();

	pMouseFocus_ = NULL;

	pRoot_ = pNewRoot;

	GuiDisplayable::useFourTimesRender() = true;
}

void GuiManager::updateMouseFocus()
{
	CB_GUIMANAGER_DEPIMPL();

	GuiMouseEvent me = pollMouse();

	GuiDisplayable* pFocus = NULL;
	const Gui::Coord& p = me.coord();

	// Find out which displayable the mouse is over
 	bool gotFocus = false;

	// Note : This probably isn't that safe if we end up with multiple devices.
	RenDisplay* pDisplay = RenDevice::current()->display();
	// If there is a mouse cursor on screen then find out what it's pointing at
	if ( pDisplay->currentCursor() != NULL )
	 	gotFocus = pRoot_->innermostContainingCheckProcessesMouseEvents( p, &pFocus );

	ASSERT( implies( gotFocus, pFocus != NULL ), "logic error" );
	ASSERT( implies( gotFocus, pFocus->processesMouseEvents() ), "logic error" );

	#ifndef PRODUCTION
	//##NA include for useful debug info. Tells you what the mouse is pointing at
	if ( getenv( "cb_mouseover" ) and pFocus )
		W4dManager::instance().sceneManager()->out() << "Mouse is over " << pFocus->description() << " : " << pFocus << " " << pFocus->relativeCoord() << " processesMouseEvents " << pFocus->processesMouseEvents() << std::endl;
	//##NA
	#endif

	bool focusChanged = pFocus != pMouseFocus_;
	if( focusChanged )
	{
		if( pMouseFocus_ != NULL )
		{
			GuiMouseEvent meTmp = me;
			meTmp.translate( unaryMinus( pMouseFocus_->absoluteCoord() ) );
			pMouseFocus_->doHandleMouseExitEvent( meTmp );
		}

		if( gotFocus )
		{
			pMouseFocus_ = pFocus;
			me.translate( unaryMinus( pMouseFocus_->absoluteCoord() ) );
			pMouseFocus_->doHandleMouseEnterEvent( me );
		}
		else
		{
			pMouseFocus_ = NULL;
		}
	}
	else
	{
		if( pMouseFocus_ != NULL )
		{
			me.translate( unaryMinus( pMouseFocus_->absoluteCoord() ) );
			pMouseFocus_->doHandleContainsMouseEvent( me );
		}
	}
}

void GuiManager::processMouseEvent( const GuiMouseEvent& me )
{
	CB_GUIMANAGER_DEPIMPL();

	GuiMouse::instance().update( me.coord() );

	GuiMouseEvent mrel = me;
	mrel.translate( unaryMinus( root().absoluteCoord() ) );

    // If we have a right click, give the current root the opportunity to handle it.
    // Many applications will treat right clicks irrespective of location.
	bool handledRightClick = ( me.rightButton() != Gui::NO_CHANGE )
								and hasRoot()
								and root().doHandleRightClickEvent( mrel );

    bool isMouseWheelEvent = ( me.scrollDirection() != Gui::ScrollState::NO_SCROLL );

	if( pMouseFocus_ != NULL )
	{
        if ( isMouseWheelEvent )
        {
            mrel = me;
            mrel.translate( unaryMinus( pMouseFocus_->absoluteCoord() ) );
            pMouseFocus_->doHandleMouseScrollEvent( mrel );
        }
        else if( not handledRightClick )
		{
			mrel = me;
			mrel.translate( unaryMinus( pMouseFocus_->absoluteCoord() ) );
			pMouseFocus_->doHandleMouseClickEvent( mrel );
		}
	}
}

//////////////////////////////////////////////////////////////////////

GuiEventBase::ModifierKeys
modifierKeys( const DevButtonEvent& be )
{
	GuiEventBase::ModifierKeys result = 0;
	if( be.wasShiftPressed() )
		result |= GuiEventBase::SHIFT;
	if( be.wasCtrlPressed() )
		result |= GuiEventBase::CTRL;
	if( be.wasAltPressed() )
		result |= GuiEventBase::ALT;

	return result;
}

//////////////////////////////////////////////////////////////////////

void GuiManager::processEvents()
{
	while( not DevEventQueue::instance().isEmpty() )
	{
		DevButtonEvent be = DevEventQueue::instance().oldestEvent();
		GuiEventBase::ModifierKeys mKeys = modifierKeys( be );

		if( be.scanCode() == DevKey::LEFT_MOUSE )
		{
			GuiMouseEvent e( be.cursorCoords(), buttonState( be.action() ), Gui::NO_CHANGE, mKeys );
			processMouseEvent( e );
		}
		else if( be.scanCode() == DevKey::RIGHT_MOUSE )
		{
			GuiMouseEvent e( be.cursorCoords(), Gui::NO_CHANGE, buttonState( be.action() ), mKeys );
			processMouseEvent( e );
		}
        else if( be.scanCode() == DevKey::MIDDLE_MOUSE )
        {
            GuiMouseEvent e( be.cursorCoords(), Gui::NO_CHANGE, Gui::NO_CHANGE, getScrollDirection( be.action() ), mKeys );
            processMouseEvent( e );
        }
		else if ( be.isKeyEvent() ) // keyboard event
		{
			if( keyboardFocusExists() )
			{
				GuiKeyEvent e( be.scanCode(), buttonState( be.action() ), mKeys, be );
				if ( not keyboardFocus().doHandleKeyEvent( e ) )
				{
					// If the keyboard focus doesn't want the event then maybe the char focus will
					if ( charFocusExists() and &charFocus() != &keyboardFocus() )
					{
						charFocus().doHandleKeyEvent( e );
					}
				}
			}
		}
		else if ( be.isCharEvent() ) // character event
		{
			if( charFocusExists() )
			{
				charFocus().doHandleCharEvent( be );
			}
		}
	}
}

//static
inline Gui::ScrollState GuiManager::getScrollDirection(DevButtonEvent::Action act)
{
    switch (act)
    {
        case DevButtonEvent::SCROLL_UP:
            return Gui::ScrollState::SCROLL_UP;

        case DevButtonEvent::SCROLL_DOWN:
            return Gui::ScrollState::SCROLL_DOWN;

        case DevButtonEvent::PRESS:
        case DevButtonEvent::RELEASE:
        default:
            return Gui::ScrollState::NO_SCROLL;
    }
}

//////////////////////////////////////////////////////////////////////

const GuiColourScheme& GuiManager::colourScheme() const
{
	CB_GUIMANAGER_DEPIMPL();

	return colourScheme_;
}

//////////////////////////////////////////////////////////////////////

GuiColourScheme::GuiColourScheme()
: mainBack_( Gui::LIGHTGREY() ),
  mainInk_( Gui::BLACK() ),
  borderBack_( Gui::LIGHTGREY() ),
  borderLo_( Gui::DARKGREY() ),
  borderHi_( Gui::WHITE() ),
  headingBack_( Gui::BLUE() ),
  headingInk_( Gui::WHITE() ),
  buttonBack_( Gui::LIGHTGREY() ),
  buttonInk_( Gui::BLACK() ),
  buttonLo_( Gui::DARKGREY() ),
  buttonHi_( Gui::WHITE() ),
  buttonDefault_( Gui::BLACK() )
{
	// Intentionally Empty
}

//////////////////////////////////////////////////////////////////////

const GuiColourScheme&
GuiColourScheme::defaultScheme()
{
	static GuiColourScheme default_;
	return default_;
}

//////////////////////////////////////////////////////////////////////

const Gui::Colour& GuiColourScheme::mainBack() const { return mainBack_; }
const Gui::Colour& GuiColourScheme::mainInk() const { return mainInk_; }
const Gui::Colour& GuiColourScheme::borderBack() const { return borderBack_; }
const Gui::Colour& GuiColourScheme::borderLo() const { return borderLo_; }
const Gui::Colour& GuiColourScheme::borderHi() const { return borderHi_; }
const Gui::Colour& GuiColourScheme::headingBack() const { return headingBack_; }
const Gui::Colour& GuiColourScheme::headingInk() const { return headingInk_; }
const Gui::Colour& GuiColourScheme::buttonBack() const { return buttonBack_; }
const Gui::Colour& GuiColourScheme::buttonInk() const { return buttonInk_; }
const Gui::Colour& GuiColourScheme::buttonLo() const { return buttonLo_; }
const Gui::Colour& GuiColourScheme::buttonHi() const { return buttonHi_; }
const Gui::Colour& GuiColourScheme::buttonDefault() const { return buttonDefault_; }

//////////////////////////////////////////////////////////////////////

void GuiManager::isBeingDeleted( GuiDisplayable* pDisplayable )
{
	CB_GUIMANAGER_DEPIMPL();

    //Cancel any cached pointers
    if( pDisplayable == pMouseFocus_ )
        pMouseFocus_ = NULL;

    if( pDisplayable == pKeyboardFocus_ )
        pKeyboardFocus_ = NULL;

    if( pDisplayable == pCharacterFocus_ )
        pCharacterFocus_ = NULL;
}

void GuiManager::isBeingCreated( GuiDisplayable* /*pDisplayable*/ )
{}

//////////////////////////////////////////////////////////////////////

bool GuiManager::charFocusExists() const
{
	CB_GUIMANAGER_DEPIMPL();

	return pCharacterFocus_ != NULL;
}

GuiDisplayable& GuiManager::charFocus()
{
	PRE( charFocusExists() );

	CB_GUIMANAGER_DEPIMPL();

	return *pCharacterFocus_;
}

void GuiManager::charFocus( GuiDisplayable* pNewCharFocus )
{
	PRE( pNewCharFocus != NULL );

	CB_GUIMANAGER_DEPIMPL();

	pCharacterFocus_ = pNewCharFocus;

	POST( charFocusExists() );
}

void GuiManager::removeCharFocus()
{
	CB_GUIMANAGER_DEPIMPL();

	pCharacterFocus_ = NULL;

	POST( not charFocusExists() );
}

//////////////////////////////////////////////////////////////////////

/* End MANAGER.CPP **************************************************/
