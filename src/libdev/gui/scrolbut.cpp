/*
 * S C R O L B U T . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "gui/scrolbut.hpp"
#include "gui/border.hpp"
#include "gui/gui.hpp"
#include "gui/scrolist.hpp"

GuiScrollButton::GuiScrollButton(GuiDisplayable* pParent, 
			    				 const Gui::Coord& rel, 
			    				 GuiSimpleScrollableList* pList,
			    				 ScrollDir dir, 
			    				 const GuiFilledBorderColours& colours,
								 const GuiBitmap& bitmap) :
	GuiListObserver( pList ),
	GuiBitmapButtonWithFilledBorder( pParent, rel, GuiBorderMetrics(1,1,1), colours, bitmap, Gui::Coord(1,1) ),
	scrollDir_( dir ),
	disabledBitmap_( bitmap ),
	enabledBitmap_( bitmap )
{
	listUpdated();

    TEST_INVARIANT;
}

GuiScrollButton::GuiScrollButton(GuiDisplayable* pParent, 
			    				 const Gui::Coord& rel, 
			    				 GuiSimpleScrollableList* pList,
			    				 ScrollDir dir, 
								 const GuiBitmap& bitmap) :
	GuiListObserver( pList ),
	GuiBitmapButtonWithFilledBorder(pParent, rel, GuiBorderMetrics(1,1,1), 
									GuiFilledBorderColours( Gui::BLACK(), Gui::LIGHTGREY(), Gui::DARKGREY(), Gui::RED() ), 
									bitmap, Gui::Coord(1,1) ),
	scrollDir_( dir ),
	disabledBitmap_( bitmap ),
	enabledBitmap_( bitmap )
{
	listUpdated();

    TEST_INVARIANT;
}

GuiScrollButton::GuiScrollButton(GuiDisplayable* pParent, 
			    				 const Gui::Coord& rel, 
			    				 GuiSimpleScrollableList* pList,
			    				 ScrollDir dir, 
			    				 const GuiFilledBorderColours& colours,
								 const GuiBitmap& enabledBitmap,
								 const GuiBitmap& disabledBitmap ) :
	GuiListObserver( pList ),
	GuiBitmapButtonWithFilledBorder( pParent, rel, GuiBorderMetrics(1,1,1), colours, enabledBitmap, Gui::Coord(1,1) ),
	scrollDir_( dir ),
	disabledBitmap_( disabledBitmap ),
	enabledBitmap_( enabledBitmap )
{
	listUpdated();

    TEST_INVARIANT;
}

GuiScrollButton::GuiScrollButton(GuiDisplayable* pParent, 
			    				 const Gui::Coord& rel, 
			    				 GuiSimpleScrollableList* pList,
			    				 ScrollDir dir, 
								 const GuiBitmap& enabledBitmap,
								 const GuiBitmap& disabledBitmap ) :
	GuiListObserver( pList ),
	GuiBitmapButtonWithFilledBorder(pParent, rel, GuiBorderMetrics(1,1,1), 
									GuiFilledBorderColours( Gui::BLACK(), Gui::LIGHTGREY(), Gui::DARKGREY(), Gui::RED() ), 
									enabledBitmap, Gui::Coord(1,1) ),
	scrollDir_( dir ),
	disabledBitmap_( disabledBitmap ),
	enabledBitmap_( enabledBitmap )
{
	listUpdated();

    TEST_INVARIANT;
}

GuiScrollButton::~GuiScrollButton()
{
    TEST_INVARIANT;

}

void GuiScrollButton::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const GuiScrollButton& t )
{

    o << "GuiScrollButton " << (void*)&t << " start" << std::endl;
    o << "GuiScrollButton " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual 
void GuiScrollButton::listUpdated()
{
	if ( hasList() )
	{
		if ( not isDisabled() )
		{
			switch ( scrollDir_ )
			{
				case FOWARD:
				case END:
					if ( not list().canScrollFoward() )
						disable();
					break;
				case BACKWARD:
				case BEGIN:
					if ( not list().canScrollBackward() )
						disable();
					break;			
				DEFAULT_ASSERT_BAD_CASE( scrollDir_ );
			}
		}
		else
		{
			switch ( scrollDir_ )
			{
				case FOWARD:
				case END:
					if ( list().canScrollFoward() )
						enable();
					break;
				case BACKWARD:
				case BEGIN:
					if ( list().canScrollBackward() )
						enable();
					break;			
				DEFAULT_ASSERT_BAD_CASE( scrollDir_ );
			}
		}
	}
}

//virtual 
void GuiScrollButton::doBeReleased( const GuiMouseEvent& /*rel*/ )
{
	if ( hasList() )
	{
		switch ( scrollDir_ )
		{
			case FOWARD:
				list().scrollFoward();
				break;
			case BACKWARD:
				list().scrollBackward();
				break;			
			case END:
				list().scrollToEnd();
				break;
			case BEGIN:
				list().scrollToBegin();
				break;
			DEFAULT_ASSERT_BAD_CASE( scrollDir_ );
		}
	}
}

//virtual 
void GuiScrollButton::doBeDepressed( const GuiMouseEvent& /*rel*/ )
{}

//virtual 
const GuiBitmap& GuiScrollButton::getBitmap() const
{
	if ( isDisabled() )
		return disabledBitmap_;

	return enabledBitmap_;
}

/* End SCROLBUT.CPP *************************************************/
