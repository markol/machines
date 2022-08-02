/*
 * C U R S O R 2 D . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _RENDER_CURSOR2D_HPP
#define _RENDER_CURSOR2D_HPP

#include "base/base.hpp"

class RenSurface;

// An abstraction for a mouse cursor bitmap.  The displayed bitmap may be 
// animated or static.  Derived classes are expected to own the cursor 
// bitmap(s) and to provide the animation logic.
class RenCursor2d
{
public:
    virtual ~RenCursor2d();

	// This is a const method.  It is very likely that sub-classes will modify
	// thier state when this method is called and thus will need to cast-away-
	// const.  This is actually what's desired: a client who displays a cursor
	// shouldn't be able to change a cursor's bitmaps, however, they do need
	// to advance the cursor to the next animated image (or whatever).
	virtual const RenSurface& currentBitmap() const = 0;
	virtual void print(ostream& o) const = 0;
	
	// The cursor will be at these coordinates in the bitmap.
	void origin(int x, int y);
	int  originX() const;
	int  originY() const;
	
protected:
	RenCursor2d();
	
private:
	int ox_, oy_;
};

ostream& operator <<( ostream& o, const RenCursor2d& t );

#endif

/* End CURSOR2D.HPP *************************************************/
