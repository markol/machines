/*
 * S C R O L B A R . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "gui/scrolbar.hpp"
#include "gui/painter.hpp"
#include "gui/scrolist.hpp"
#include "gui/event.hpp"

GuiVerticalScrollBar::GuiVerticalScrollBar( GuiDisplayable* pParent, const Gui::Box& area, GuiSimpleScrollableList* pList ) :
	GuiDisplayable( pParent, area ),
	GuiListObserver( pList ),
	dragBox_( false )
{
	// Work out scroll box size etc.
	listUpdated();

    TEST_INVARIANT;
}

GuiVerticalScrollBar::~GuiVerticalScrollBar()
{
    TEST_INVARIANT;

}

void GuiVerticalScrollBar::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const GuiVerticalScrollBar& t )
{

    o << "GuiVerticalScrollBar " << (void*)&t << " start" << std::endl;
    o << "GuiVerticalScrollBar " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void GuiVerticalScrollBar::listUpdated()
{
	if ( hasList() )
	{
		Gui::Box area = absoluteBoundary();

		if ( list().numListItems() <= list().visiblePositions() )
		{
			// Special case, there are less items than the visible portion of the list box,
			// therefore make the scroll box fill the scroll bar
			Gui::XCoord boxLeft = area.minCorner().x() + 1;
			Gui::YCoord boxTop = area.minCorner().y();

			boxAbsPosition_ = Gui::Box( boxLeft, boxTop, boxLeft + Gui::width( area ) - 2, boxTop + Gui::height( area ) );
			boxRelPosition_ = Gui::Box(	1, 0, 1 + Gui::width( area ) - 2, Gui::height( area ) );
		}
		else
		{
		 	double pixHeight = height();

			// Work out how many pixels represent each item in the list...
			double itemPix = pixHeight / list().numListItems();

			// ...therefore work out the size of the scroll box
			size_t pixBoxHeight = list().visiblePositions() * itemPix;

			// Make sure scroll box size is not less than minimum size
			pixBoxHeight = std::max( minBoxHeight(), pixBoxHeight );

			ASSERT( pixBoxHeight <= pixHeight, "scroll box size has been calculated as bigger than the scroll bar size" );

			// Work out how much of the scroll bar is not covered by the scroll box
			double pixRemainderHeight = pixHeight - pixBoxHeight;

			double itemRemainderPix = pixRemainderHeight / (  list().numListItems() - list().visiblePositions() );

			// Work out the position of the scroll box in relation to the top of the scroll bar
			int pixTopOffset = list().scrollOffset() * itemRemainderPix;

		    Gui::XCoord boxLeft = area.minCorner().x() + 1;
			Gui::YCoord boxTop = area.minCorner().y() + pixTopOffset;

			boxAbsPosition_ = Gui::Box(	boxLeft,
				  						boxTop,
										boxLeft + Gui::width( area ) - 2,
										boxTop + pixBoxHeight );

			boxRelPosition_ = Gui::Box(	1,
				  						pixTopOffset,
										1 + Gui::width( area ) - 2,
										pixTopOffset + pixBoxHeight );
		}
	}

	changed();
}

//virtual
void GuiVerticalScrollBar::doDisplay()
{
	doDisplayBar();
	doDisplayBox( boxAbsPosition_ );
}

//virtual
void GuiVerticalScrollBar::doDisplayBar()
{
	GuiPainter::instance().filledRectangle( absoluteBoundary(), Gui::DARKGREY() );
	GuiPainter::instance().line( absoluteBoundary().minCorner(),
								 Gui::Coord( absoluteBoundary().minCorner().x(), absoluteBoundary().maxCorner().y() ),
								 Gui::BLACK(),
								 1 );
	GuiPainter::instance().line( Gui::Coord( absoluteBoundary().maxCorner().x() - 1, absoluteBoundary().minCorner().y() ),
								 Gui::Coord( absoluteBoundary().maxCorner().x() - 1, absoluteBoundary().maxCorner().y() ),
								 Gui::BLACK(),
								 1 );
}

//virtual
void GuiVerticalScrollBar::doDisplayBox( const Gui::Box& absoluteBox )
{
	GuiPainter::instance().filledRectangle( absoluteBox, Gui::GREY() );

	// Draw low lights and high lights
	GuiPainter::instance().line( Gui::Coord( absoluteBox.maxCorner().x() - 1, absoluteBox.minCorner().y() ),
	  							 Gui::Coord( absoluteBox.maxCorner().x() - 1, absoluteBox.maxCorner().y() ),
								 Gui::BLACK(), 1 );
	GuiPainter::instance().line( Gui::Coord( absoluteBox.minCorner().x(), absoluteBox.maxCorner().y() - 1 ),
	  							 Gui::Coord( absoluteBox.maxCorner().x(), absoluteBox.maxCorner().y() - 1 ),
								 Gui::BLACK(), 1 );
	GuiPainter::instance().line( absoluteBox.minCorner(),
	  							 Gui::Coord( absoluteBox.maxCorner().x(), absoluteBox.minCorner().y() ),
								 Gui::LIGHTGREY(), 1 );
	GuiPainter::instance().line( absoluteBox.minCorner(),
	  							 Gui::Coord( absoluteBox.minCorner().x(), absoluteBox.maxCorner().y() ),
								 Gui::LIGHTGREY(), 1 );

	int halfBoxHeight = ( absoluteBox.maxCorner().y() - absoluteBox.minCorner().y() ) / 2;

	// Draw three lines in middle of scroll box
	GuiPainter::instance().line( Gui::Coord( absoluteBox.minCorner().x() + 2, absoluteBox.minCorner().y() + halfBoxHeight ),
	  							 Gui::Coord( absoluteBox.maxCorner().x() - 2, absoluteBox.minCorner().y() + halfBoxHeight ),
								 Gui::DARKGREY(), 1 );
	GuiPainter::instance().line( Gui::Coord( absoluteBox.minCorner().x() + 2, absoluteBox.minCorner().y() + halfBoxHeight - 2 ),
	  							 Gui::Coord( absoluteBox.maxCorner().x() - 2, absoluteBox.minCorner().y() + halfBoxHeight - 2 ),
								 Gui::DARKGREY(), 1 );
	GuiPainter::instance().line( Gui::Coord( absoluteBox.minCorner().x() + 2, absoluteBox.minCorner().y() + halfBoxHeight + 2 ),
	  							 Gui::Coord( absoluteBox.maxCorner().x() - 2, absoluteBox.minCorner().y() + halfBoxHeight + 2 ),
								 Gui::DARKGREY(), 1 );
}

//virtual
size_t GuiVerticalScrollBar::minBoxHeight() const
{
	return 9;
}

//virtual
void GuiVerticalScrollBar::doHandleMouseClickEvent( const GuiMouseEvent& rel )
{
	if ( hasList() and list().canScroll() and rel.leftButton() == Gui::PRESSED )
	{
		if ( boxRelPosition_.contains( rel.coord() ) )
		{
			dragBox_ = true;

			scrollToMousePos( rel );
		}
		else
		{
			dragBox_ = false;

			if ( rel.coord().y() <= boxRelPosition_.minCorner().y() ) // Scroll backward one page
			{
				int newScrollPos = list().scrollOffset() - list().visiblePositions();

				newScrollPos = std::max( newScrollPos, 0 );

				list().scrollTo( newScrollPos );
			}
			else // Scroll foward one page
			{
				int newScrollPos = list().scrollOffset() + list().visiblePositions();

				int numScrollableListItems = list().numListItems() - list().visiblePositions();

				newScrollPos = std::min( newScrollPos, numScrollableListItems );

				list().scrollTo( newScrollPos );
			}
		}
	}
}

//virtual
void GuiVerticalScrollBar::doHandleContainsMouseEvent( const GuiMouseEvent& rel )
{
	if ( hasList() and list().canScroll() and rel.leftButton() == Gui::PRESSED and dragBox_ )
	{
		scrollToMousePos( rel );
	}
}

Gui::Box GuiVerticalScrollBar::boxAbsPosition() const
{
	return boxAbsPosition_;
}

Gui::Box GuiVerticalScrollBar::boxRelPosition() const
{
	return boxRelPosition_;
}

void GuiVerticalScrollBar::scrollToMousePos( const GuiMouseEvent& rel )
{
	double boxHeight = Gui::height( boxAbsPosition_ );
	double barHeight = height();

	double activeSpace = barHeight - boxHeight;

	double mousePos = rel.coord().y() - ( boxHeight / 2 );
	mousePos = std::max( 0.0, mousePos );

	int numScrollableListItems = list().numListItems() - list().visiblePositions();

	double itemPix = activeSpace / numScrollableListItems;

	int newScrollPos = mousePos / itemPix;

	newScrollPos = std::min( newScrollPos, numScrollableListItems );

	list().scrollTo( newScrollPos );
}

/* End SCROLBAR.CPP *************************************************/
