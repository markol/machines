/*
 * P R O G R E S S . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#ifndef _GUI_PROGRESS_HPP
#define _GUI_PROGRESS_HPP

#include "gui/displaya.hpp"

/* //////////////////////////////////////////////////////////////// */

class GuiProgressBar
: public GuiDisplayable
{
public:

	GuiProgressBar( GuiDisplayable * pParent, 
					const Gui::Box& rel );
	// PRE( pParent != NULL );
	// POST( percentageComplete() == 0 );

	///////////////////////////////

	typedef double Percentage;
	Percentage percentageComplete();
	void percentageComplete( Percentage p );
	// PRE( p >= 0.0 and p <= 100.0 );
	
	///////////////////////////////

	// inherited from GuiDisplayable...
	// virtual void display() = 0;

protected:
	
	virtual ~GuiProgressBar();

	// width in pixels for a given percentage	
	static unsigned progressWidthInPixels( Percentage progress, 
											unsigned progressCompleteWidth );

private:
	GuiProgressBar( const GuiProgressBar& );
	bool operator ==( const GuiProgressBar& ) const;
	GuiProgressBar& operator =( const GuiProgressBar& );

	Percentage	percentageComplete_;
};

/* //////////////////////////////////////////////////////////////// */

class GuiFilledProgressBar
: public GuiProgressBar
{
public:

	GuiFilledProgressBar( GuiDisplayable * pParent, 
					const Gui::Box& rel, 
					const GuiColour& backgroundCol,  
					const GuiColour& progressCol );
	// PRE( pParent != NULL );

	virtual ~GuiFilledProgressBar();

	///////////////////////////////

	// inherited from GuiDisplayable...
	virtual void doDisplay();

private:
	GuiFilledProgressBar( const GuiFilledProgressBar& );
	bool operator ==( const GuiFilledProgressBar& ) const;
	GuiFilledProgressBar& operator =( const GuiFilledProgressBar& );
	
	///////////////////////////////

	GuiColour	backgroundCol_;
	GuiColour 	progressCol_;
};

/* //////////////////////////////////////////////////////////////// */

#endif	// #ifndef _GUI_PROGRESS_HPP
