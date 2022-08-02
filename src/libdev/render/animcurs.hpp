/*
 * A N I M C U R S . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _RENDER_ANIMCURS_HPP
#define _RENDER_ANIMCURS_HPP

#include "render/cursor2d.hpp"

class RenIAnimCursor2d;

// A cursor which animates by cycling through a series of bitmaps.
class RenAnimCursor2d : public RenCursor2d
{
public:
    RenAnimCursor2d();
    virtual ~RenAnimCursor2d();

	virtual const RenSurface& currentBitmap() const;
	virtual void print(ostream& o) const;
	
	// Build up an animation by calling this method multiple times.
	void addFrame(const RenSurface&);
	size_t nFrames() const;
	RenSurface frame(size_t no) const;	// PRE(no < nFrames());
	
	// The frequency at which the bitmap images are changed.
	void   targetFrequency(double hertz);		// PRE(hertz > 0);
	double targetFrequency() const;
	
    void CLASS_INVARIANT;

private:
	RenIAnimCursor2d* internals_;
	
    // Operations deliberately revoked.
    RenAnimCursor2d( const RenAnimCursor2d& );
    RenAnimCursor2d& operator =( const RenAnimCursor2d& );
    bool operator ==( const RenAnimCursor2d& );
};

#endif

/* End ANIMCURS.HPP *************************************************/
