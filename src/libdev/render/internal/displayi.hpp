/*
 * D I S P L A Y I . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    RenIDisplay

    A brief description of the class should go in here
*/

#ifndef _DISPLAYI_HPP
#define _DISPLAYI_HPP

#include "base/base.hpp"
#include "ctl/list.hpp"
#include "ctl/pvector.hpp"
#include "render/display.hpp"
#include "render/render.hpp"

class SysPathName;
class RenSurface;
class RenCursor2d;
class RenISavedArea;
class RenIDisplayModeObserver;

class RenIDisplay
// Canonical form revoked
{
public:
    ~RenIDisplay();

    void CLASS_INVARIANT;

	// Observer pattern for display modes.
	void prepareForModeChange(const RenDisplay::Mode& newMode);
	bool modeChanged();
	void    addObserver(RenIDisplayModeObserver*);
	void removeObserver(RenIDisplayModeObserver*);

	bool isPrimaryDriver() const;

	// PRE(dest);
	void restoreUnderCursor(RenSurface* dest, const RenSurface& front, Ren::Rect srcArea, int destX, int destY) const;

	void useCursor(const RenCursor2d* c);
	const RenCursor2d* currentCursor() const;

	// This should be called if we entirely clear the screen, i.e., if the
	// previous saved area is now irrelevant.
	void discardCursorSaves();

private:
 	friend class RenDisplay;
	friend class RenDDEnumerator;
    RenIDisplay( SDL_Window* wnd );

	void drawCursor(RenSurface& backBuf);
	void restoreUnderCursor();
	bool blitTest();

    friend ostream& operator <<( ostream& o, const RenIDisplay& t );

    RenIDisplay( const RenIDisplay& );
    RenIDisplay& operator =( const RenIDisplay& );

	SDL_Window*                 pWnd_;
	ctl_list<RenDisplay::Mode>	modeList_;
	RenDisplay::Mode			currentMode_;
	bool						fullscreen_;
	const RenCursor2d*			cursor_;
	RenISavedArea*				backBufferCursorSave_;
	RenISavedArea*				frontBufferCursorSave_;
	ulong						frameNo_;
	RenDisplay::Mode			lowestAllowedMode_;
	RenDisplay::Mode			highestAllowedMode_;
	bool						supportsGammaCorrection_;
	double 						gammaCorrection_;
	bool						isPrimaryDriver_;

	ctl_pvector<RenIDisplayModeObserver>	observers_;
};

#endif

/* End DISPLAYI.HPP *************************************************/
