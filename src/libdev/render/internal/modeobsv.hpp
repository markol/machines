/*
 * M O D E O B S V . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#ifndef _RENDER_MODEOBSV_HPP
#define _RENDER_MODEOBSV_HPP

#include "base/base.hpp"
#include "render/display.hpp"

// RenDevice objects must be reinitialised if the display mode changes.
// There could be more than one device per display.  Implement an observer
// pattern for the display mode because it's conceivable that things other
// than the device could depend upon the mode.
class RenIDisplayModeObserver
{
public:
	virtual void prepareForModeChange(const RenDisplay::Mode& currentMode, const RenDisplay::Mode& newMode) =0;
	virtual bool modeChanged(const RenDisplay::Mode& currentMode) =0;
	
	RenDisplay* display()				{ return display_; }
	const RenDisplay* display() const	{ return display_; }
	
protected:
	RenIDisplayModeObserver(RenDisplay* d);			// PRE(d);
    virtual ~RenIDisplayModeObserver();

private:
	RenDisplay* const	display_;

    RenIDisplayModeObserver( const RenIDisplayModeObserver& );
    RenIDisplayModeObserver& operator =( const RenIDisplayModeObserver& );
};

#endif

/* End MODEOBSV.HPP *************************************************/
