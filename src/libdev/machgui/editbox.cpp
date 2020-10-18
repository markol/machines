/*
 * E D I T B O X . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/editbox.hpp"
#include "machgui/startup.hpp"
#include "gui/painter.hpp"
#include "device/butevent.hpp"

unsigned char pPromptText1[] = { 87, 244, 105, 128, 48, 182, 149, 151, 72, 203, 222, 59, 150, 19, 218, 139, 154, 243, 40, 190, 242, 54, 243, 50, 98, 159, 84, 250, 37, 136, 151, 150, 0 };
unsigned char pPromptText8[] = { 4, 212, 167, 27, 173, 11, 155, 126, 58, 37, 114, 151, 128 };

MachGuiSingleLineEditBox::MachGuiSingleLineEditBox( MachGuiStartupScreens* pStartupScreens, const Gui::Box& box, const GuiBmpFont& font )
:	GuiSingleLineEditBox( pStartupScreens, box, font ),
	pStartupScreens_( pStartupScreens ),
	clearTextOnNextChar_( false ),
	ignoreSpaceAtBeginning_( true )
{

    TEST_INVARIANT;
}

MachGuiSingleLineEditBox::~MachGuiSingleLineEditBox()
{

    TEST_INVARIANT;
}

//virtual 
void MachGuiSingleLineEditBox::drawBackground()
{
	pStartupScreens_->blitBackdrop( absoluteBoundary(), 
									absoluteBoundary().minCorner() );
}

void MachGuiSingleLineEditBox::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiSingleLineEditBox& t )
{

    o << "MachGuiSingleLineEditBox " << (void*)&t << " start" << std::endl;
    o << "MachGuiSingleLineEditBox " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual 
bool MachGuiSingleLineEditBox::doHandleCharEvent( const GuiCharEvent& e )
{
	DEBUG_STREAM( DIAG_NEIL, "MachGuiSingleLineEditBox::doHandleCharEvent " << e.getChar() << " " << (int)e.getChar() << std::endl );

	if ( ignoreSpaceAtBeginning_ and leftText() == "" and e.getChar() == ' ' )
	{
		return true;
	}
	
	if ( clearTextOnNextChar_ )
	{
		text( "" );
		clearTextOnNextChar_ = false;
	}	

	return GuiSingleLineEditBox::doHandleCharEvent( e );
}

void MachGuiSingleLineEditBox::clearTextOnNextChar( bool newVal )
{
	clearTextOnNextChar_ = newVal;
}

bool MachGuiSingleLineEditBox::clearTextOnNextChar() const
{
	return clearTextOnNextChar_;
}

void MachGuiSingleLineEditBox::ignoreSpaceAtBeginning( bool ignore )
{
	ignoreSpaceAtBeginning_ = ignore;
}


/* End EDITBOX.CPP **************************************************/
