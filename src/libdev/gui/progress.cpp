/*
 * P R O G R E S S . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include "gui/progress.hpp"
#include "gui/painter.hpp"

/* //////////////////////////////////////////////////////////////// */

GuiProgressBar::GuiProgressBar( GuiDisplayable *pPtr, const Gui::Box& rel )
: GuiDisplayable( pPtr, rel ),
  percentageComplete_( 0.0 )
{
	POST( percentageComplete() == 0 );	
}
	
GuiProgressBar::~GuiProgressBar() 
{
	// Intentionally Empty
}

/* //////////////////////////////////////////////////////////////// */

GuiProgressBar::Percentage 
GuiProgressBar::percentageComplete()
{
	return percentageComplete_;
}

void GuiProgressBar::percentageComplete( Percentage p )
{
	PRE( p >= 0.0 and p <= 100.0 );

	unsigned width = Gui::width( absoluteBoundary() );
	unsigned oldWidth = progressWidthInPixels( percentageComplete_, width );
	unsigned newWidth = progressWidthInPixels( p, width );

	if( newWidth != oldWidth )
		changed();

	percentageComplete_ = p;
}

// static 
unsigned 
GuiProgressBar::progressWidthInPixels( Percentage progress, 
											unsigned progressCompleteWidth )
// width in pixels for a given percentage
{
	return ( progress * progressCompleteWidth ) / 100.0;
}

/* //////////////////////////////////////////////////////////////// */

GuiFilledProgressBar::GuiFilledProgressBar( GuiDisplayable *pPtr, 
						const Gui::Box& rel, 
						const GuiColour& backgroundCol,  
						const GuiColour& progressCol )
: GuiProgressBar( pPtr, rel ),
  backgroundCol_( backgroundCol ),
  progressCol_( progressCol )
{
	// Intentionally Empty
}
	
GuiFilledProgressBar::~GuiFilledProgressBar() 
{
	// Intentionally Empty
}

void GuiFilledProgressBar::doDisplay()
{
	GuiPainter::instance().filledRectangle( absoluteBoundary(), backgroundCol_ );

	unsigned width = progressWidthInPixels( percentageComplete(), Gui::width( absoluteBoundary() ) );
	Gui::Box progressBox( absoluteBoundary().minCorner(), width, Gui::height( absoluteBoundary() ) );
	GuiPainter::instance().filledRectangle( progressBox, progressCol_ );
}

/* //////////////////////////////////////////////////////////////// */

/* End PROGRESS.CPP *************************************************/
