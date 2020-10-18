/*
 * N U M P D R O P . C P P
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/numpdrop.hpp"
#include "machgui/ctxskirm.hpp"
#include "machgui/internal/strings.hpp"
#include "system/registry.hpp"
#include <cstring>

unsigned char pPromptTextKey3[] = { 91, 30, 219, 198, 38, 235, 91, 65, 137, 222, 133, 82, 205, 65, 237, 5, 118, 0, 225, 63, 59, 56, 112, 160, 143, 211, 82, 76 };

MachGuiNumPlayersDropDownListBoxCreator::MachGuiNumPlayersDropDownListBoxCreator( GuiDisplayable* pParent, MachGuiStartupScreens* pStartup, int width, const GuiStrings& availText )
:	MachGuiDropDownListBoxCreator( pParent, pStartup, width, availText )
{

    TEST_INVARIANT;
}

MachGuiNumPlayersDropDownListBoxCreator::MachGuiNumPlayersDropDownListBoxCreator( GuiDisplayable* pParent, MachGuiStartupScreens* pStartup, int width, const GuiStrings& availText, bool whiteFont )
:	MachGuiDropDownListBoxCreator( pParent, pStartup, width, availText, whiteFont )
{

    TEST_INVARIANT;
}

MachGuiNumPlayersDropDownListBoxCreator::MachGuiNumPlayersDropDownListBoxCreator( GuiDisplayable* pParent, MachGuiStartupScreens* pStartup, const Gui::Coord& relCoord, int width, const GuiStrings& availText, bool whiteFont, bool border )
:	MachGuiDropDownListBoxCreator( pParent, pStartup, relCoord, width, availText, whiteFont, border )
{

    TEST_INVARIANT;
}

MachGuiNumPlayersDropDownListBoxCreator::~MachGuiNumPlayersDropDownListBoxCreator()
{
    TEST_INVARIANT;
}

void MachGuiNumPlayersDropDownListBoxCreator::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

// virtual
MachGuiDropDownList* MachGuiNumPlayersDropDownListBoxCreator::createDropDownList( 	MachGuiStartupScreens* pParent, const Gui::Box& box,
						    														size_t horizontalSpacing, size_t verticalSpacing,
																					size_t scrollInc, size_t itemWidth, const ctl_vector<string>& itemText,
																					MachGuiDropDownListBoxCreator* listBoxCreator )
{
	return _NEW( MachGuiNumPlayersDropDownList( pParent, box, horizontalSpacing, verticalSpacing, scrollInc, itemWidth, itemText, listBoxCreator ) );
}

ostream& operator <<( ostream& o, const MachGuiNumPlayersDropDownListBoxCreator& t )
{

    o << "MachGuiNumPlayersDropDownListBoxCreator " << (void*)&t << " start" << std::endl;
    o << "MachGuiNumPlayersDropDownListBoxCreator " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachGuiNumPlayersDropDownListBoxCreator::doHandleNavigationKey( NavKey navKey, MachGuiFocusCapableControl** pFocusCapableControl )
{
	bool retValue = MachGuiDropDownListBoxCreator::doHandleNavigationKey( navKey, pFocusCapableControl );

	if ( 	navKey == MachGuiFocusCapableControl::UP_ARROW or
			navKey == MachGuiFocusCapableControl::DOWN_ARROW )
	{
		GuiResourceString two( IDS_MENU_NUMPLAYERS2 );
		GuiResourceString three( IDS_MENU_NUMPLAYERS3 );
		GuiResourceString four( IDS_MENU_NUMPLAYERS4 );

//		if ( stricmp( two.asString().c_str(), text().c_str() ) == 0 )
		if ( strcasecmp( two.asString().c_str(), text().c_str() ) == 0 )
		{
			MachGuiCtxSkirmish::chosenNumPlayers() = 2;
		}
//		else if ( stricmp( three.asString().c_str(), text().c_str() ) == 0 )
		else if ( strcasecmp( three.asString().c_str(), text().c_str() ) == 0 )
		{
			MachGuiCtxSkirmish::chosenNumPlayers() = 3;
		}
//		else if ( stricmp( four.asString().c_str(), text().c_str() ) == 0 )
		else if ( strcasecmp( four.asString().c_str(), text().c_str() ) == 0 )
		{
			MachGuiCtxSkirmish::chosenNumPlayers() = 4;
		}
	 	else
		{
			ASSERT_INFO( text() );
			ASSERT( false, "MachGuiDropDownListBoxCreator::doHandleNavigationKey has invalid text()" );
		}

		SysRegistry::instance().setIntegerValue( "Game Settings\\Num Players", "Value", MachGuiCtxSkirmish::chosenNumPlayers(), SysRegistry::CURRENT_USER );
	}

	return retValue;
}


MachGuiNumPlayersDropDownList::MachGuiNumPlayersDropDownList( MachGuiStartupScreens* pParent, const Gui::Box& box,
								    							size_t horizontalSpacing, size_t verticalSpacing,
																size_t scrollInc, size_t itemWidth, const ctl_vector<string>& itemText,
																MachGuiDropDownListBoxCreator* listBoxCreator )
:	MachGuiDropDownList( pParent, box, horizontalSpacing, verticalSpacing, scrollInc, itemWidth, itemText, listBoxCreator )
{
	// Intentionally empty
}

MachGuiNumPlayersDropDownList::MachGuiNumPlayersDropDownList( MachGuiStartupScreens* pParent, const Gui::Box& box,
					    							size_t horizontalSpacing, size_t verticalSpacing,
													size_t scrollInc, size_t itemWidth, const ctl_vector<string>& itemText,
													MachGuiDropDownListBoxCreator* listBoxCreator, bool whiteFont )
:	MachGuiDropDownList( pParent, box, horizontalSpacing, verticalSpacing, scrollInc, itemWidth, itemText, listBoxCreator, whiteFont )
{
	// Intentionally empty
}

// virtual
MachGuiDropDownListBoxItem*  MachGuiNumPlayersDropDownList::createListBoxItem(	MachGuiStartupScreens* pStartupScreens,
																				MachGuiDropDownListBox* pListBox,
																				size_t width,
																				const string& text,
																				bool whiteFont )
{
	return _NEW( MachGuiNumPlayersDropDownListBoxItem( pStartupScreens, pListBox, width, text, whiteFont ) );
}

MachGuiNumPlayersDropDownListBoxItem::MachGuiNumPlayersDropDownListBoxItem( MachGuiStartupScreens* pStartupScreens,
																			MachGuiDropDownListBox* pListBox,
																			size_t width,
																			const string& text,
																			bool whiteFont )
:	MachGuiDropDownListBoxItem( pStartupScreens, pListBox, width, text, whiteFont )
{
	// Intentionally empty
}

// virtual
void MachGuiNumPlayersDropDownListBoxItem::select()
{
	GuiResourceString two( IDS_MENU_NUMPLAYERS2 );
	GuiResourceString three( IDS_MENU_NUMPLAYERS3 );
	GuiResourceString four( IDS_MENU_NUMPLAYERS4 );

//	if ( stricmp( two.asString().c_str(), text().c_str() ) == 0 )
	if ( strcasecmp( two.asString().c_str(), text().c_str() ) == 0 )
	{
		MachGuiCtxSkirmish::chosenNumPlayers() = 2;
	}
//	else if ( stricmp( three.asString().c_str(), text().c_str() ) == 0 )
	else if ( strcasecmp( three.asString().c_str(), text().c_str() ) == 0 )
	{
		MachGuiCtxSkirmish::chosenNumPlayers() = 3;
	}
//	else if ( stricmp( four.asString().c_str(), text().c_str() ) == 0 )
	else if ( strcasecmp( four.asString().c_str(), text().c_str() ) == 0 )
	{
		MachGuiCtxSkirmish::chosenNumPlayers() = 4;
	}
 	else
	{
		ASSERT_INFO( text() );
		ASSERT( false, "MachGuiNumPlayersDropDownListBoxItem::select() has invalid text()" );
	}

	SysRegistry::instance().setIntegerValue( "Game Settings\\Num Players", "Value", MachGuiCtxSkirmish::chosenNumPlayers(), SysRegistry::CURRENT_USER );

	// This base class function indirecty deletes this object, therefore it is called last
	MachGuiDropDownListBoxItem::select();
}

/* End NUMPDROP.CPP ************************************************/
