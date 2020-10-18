/*
 * D D R A W D R O P . C P P 
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/ddrawdrop.hpp"

MachGuiDDrawDropDownListBoxCreator::MachGuiDDrawDropDownListBoxCreator( GuiDisplayable* pParent, MachGuiStartupScreens* pStartup, int width, const GuiStrings& availText, MachGuiDropDownCallback* pCallbackHandler )
:	MachGuiDropDownListBoxCreator( pParent, pStartup, width, availText ),
	callbackHandler_( pCallbackHandler )
{

    TEST_INVARIANT;
}

MachGuiDDrawDropDownListBoxCreator::MachGuiDDrawDropDownListBoxCreator( GuiDisplayable* pParent, MachGuiStartupScreens* pStartup, int width, const GuiStrings& availText, bool whiteFont, MachGuiDropDownCallback* pCallbackHandler )
:	MachGuiDropDownListBoxCreator( pParent, pStartup, width, availText, whiteFont ),
	callbackHandler_( pCallbackHandler )
{

    TEST_INVARIANT;
}

MachGuiDDrawDropDownListBoxCreator::MachGuiDDrawDropDownListBoxCreator( GuiDisplayable* pParent, MachGuiStartupScreens* pStartup, const Gui::Coord& relCoord, int width, const GuiStrings& availText, bool whiteFont, bool border, MachGuiDropDownCallback* pCallbackHandler )
:	MachGuiDropDownListBoxCreator( pParent, pStartup, relCoord, width, availText, whiteFont, border ),
	callbackHandler_( pCallbackHandler )
{

    TEST_INVARIANT;
}

MachGuiDDrawDropDownListBoxCreator::~MachGuiDDrawDropDownListBoxCreator()
{
    TEST_INVARIANT;
	// newed by client but deleted here
	_DELETE( callbackHandler_ );
}

void MachGuiDDrawDropDownListBoxCreator::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

// virtual 
MachGuiDropDownList* MachGuiDDrawDropDownListBoxCreator::createDropDownList( 	MachGuiStartupScreens* pParent, const Gui::Box& box, 
					    														size_t horizontalSpacing, size_t verticalSpacing,
																				size_t scrollInc, size_t itemWidth, const ctl_vector<string>& itemText,
																				MachGuiDropDownListBoxCreator* listBoxCreator )
{
	return _NEW( MachGuiDDrawDropDownList( pParent, box, horizontalSpacing, verticalSpacing, scrollInc, itemWidth, itemText, listBoxCreator, callbackHandler_ ) );
}

ostream& operator <<( ostream& o, const MachGuiDDrawDropDownListBoxCreator& t )
{

    o << "MachGuiDDrawDropDownListBoxCreator " << (void*)&t << " start" << std::endl;
    o << "MachGuiDDrawDropDownListBoxCreator " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual 
bool MachGuiDDrawDropDownListBoxCreator::doHandleNavigationKey( NavKey navKey, MachGuiFocusCapableControl** pFocusCapableControl )
{
	bool retValue = MachGuiDropDownListBoxCreator::doHandleNavigationKey( navKey, pFocusCapableControl );

	if ( 	navKey == MachGuiFocusCapableControl::UP_ARROW or 
			navKey == MachGuiFocusCapableControl::DOWN_ARROW )
	{
		callbackHandler_->callBack();
	}

	return retValue;
}


MachGuiDDrawDropDownList::MachGuiDDrawDropDownList( MachGuiStartupScreens* pParent, const Gui::Box& box, 
					    							size_t horizontalSpacing, size_t verticalSpacing,
													size_t scrollInc, size_t itemWidth, const ctl_vector<string>& itemText,
													MachGuiDropDownListBoxCreator* listBoxCreator, MachGuiDropDownCallback* callbackHandler )
:	MachGuiDropDownList( pParent, box, horizontalSpacing, verticalSpacing, scrollInc, itemWidth, itemText, listBoxCreator ),
	callbackHandler_( callbackHandler )
{
	// Intentionally empty
}

MachGuiDDrawDropDownList::MachGuiDDrawDropDownList( MachGuiStartupScreens* pParent, const Gui::Box& box, 
					    							size_t horizontalSpacing, size_t verticalSpacing,
													size_t scrollInc, size_t itemWidth, const ctl_vector<string>& itemText,
													MachGuiDropDownListBoxCreator* listBoxCreator, bool whiteFont, MachGuiDropDownCallback* callbackHandler )
:	MachGuiDropDownList( pParent, box, horizontalSpacing, verticalSpacing, scrollInc, itemWidth, itemText, listBoxCreator, whiteFont ),
	callbackHandler_( callbackHandler )

{
	// Intentionally empty
}

// virtual 
MachGuiDropDownListBoxItem*  MachGuiDDrawDropDownList::createListBoxItem(	MachGuiStartupScreens* pStartupScreens, 
																			MachGuiDropDownListBox* pListBox, 
																			size_t width, 
																			const string& text,
																			bool whiteFont )
{
	return _NEW( MachGuiDDrawDropDownListBoxItem( pStartupScreens, pListBox, width, text, whiteFont, callbackHandler_ ) );
}

MachGuiDDrawDropDownListBoxItem::MachGuiDDrawDropDownListBoxItem( 	MachGuiStartupScreens* pStartupScreens, 
																	MachGuiDropDownListBox* pListBox, 
																	size_t width, 
																	const string& text,
																	bool whiteFont,
																	MachGuiDropDownCallback* callbackHandler )
:	MachGuiDropDownListBoxItem( pStartupScreens, pListBox, width, text, whiteFont ),
	callbackHandler_( callbackHandler )
{
	// Intentionally empty
}

// virtual
void MachGuiDDrawDropDownListBoxItem::select()
{
	MachGuiDropDownListBoxItem::select();
	callbackHandler_->callBack();
}
/* End DDRAWDROP.CPP ************************************************/
