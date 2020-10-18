/*
 * A N I M C U R S . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "render/animcurs.hpp"
#include "ctl/vector.hpp"
#include "device/timer.hpp"
#include "render/surface.hpp"

struct RenIAnimCursor2d
{
	RenIAnimCursor2d();
	const RenSurface& currentBitmap();
	
	typedef ctl_vector<RenSurface> Vec;
	Vec						surfaces_;
	Vec::const_iterator		it_;
	double					freq_, lambda_;
	DevTimer				frameTimer_;
	bool					started_;
};

RenIAnimCursor2d::RenIAnimCursor2d(): 
	freq_(10),
	lambda_(0.1),
	started_(false)
{
	surfaces_.reserve(15);
	frameTimer_.pause();
	frameTimer_.time(0);
}

RenAnimCursor2d::RenAnimCursor2d():
	internals_(_NEW(RenIAnimCursor2d))
{
	ASSERT(internals_, "Out of memory");
    TEST_INVARIANT;
}

// virtual
RenAnimCursor2d::~RenAnimCursor2d()
{
    TEST_INVARIANT;
    _DELETE( internals_ );
}

// This is written so that the animation will attempt to run at the target
// frequency.  However, it will *not* skip frames if the frame rate is low.
const RenSurface& RenIAnimCursor2d::currentBitmap()
{
	if (surfaces_.size() == 0)
	{
		static RenSurface null;
		return null;
	}
	
	// The first time this method is called, we must initialise the animation.
	if (!started_)
	{
		it_ = surfaces_.begin();
		frameTimer_.resume();
		started_ = true;
	}
	else if (frameTimer_.time() > lambda_)
	{
		++it_;
		frameTimer_.time(0);
		
		// Restart at the beginning again if necessary.
		if (it_ == surfaces_.end())
			it_ = surfaces_.begin();
	}
	
	return *it_;
}

// virtual 
const RenSurface& RenAnimCursor2d::currentBitmap() const
{
	return _CONST_CAST(RenIAnimCursor2d*, internals_)->currentBitmap();
}

void RenAnimCursor2d::addFrame(const RenSurface& newSurf)
{
	internals_->surfaces_.push_back(newSurf);
}

size_t RenAnimCursor2d::nFrames() const
{
	return internals_->surfaces_.size();
}

RenSurface RenAnimCursor2d::frame(size_t no) const
{
	PRE(no < nFrames());
	return internals_->surfaces_[no];
}	

// virtual 
void RenAnimCursor2d::print(ostream& o) const
{
	o << "animated cursor (" << internals_->freq_ << "Hz) using frames:\n";
	
	for (int i=0; i!=nFrames(); ++i)
		o << "\t" << frame(i) << "\n";
}

// The frequency at which the bitmap images are changed.
void   RenAnimCursor2d::targetFrequency(double hertz)
{
	PRE(hertz > 0);
	internals_->freq_   = hertz;
	internals_->lambda_ = 1 / hertz;
}

double RenAnimCursor2d::targetFrequency() const
{
	return internals_->freq_;
}	

void RenAnimCursor2d::CLASS_INVARIANT
{
	INVARIANT(internals_);
	INVARIANT(internals_->freq_ == 1 / internals_->lambda_);	// epsilon??
}

/* End ANIMCURS.CPP *************************************************/
