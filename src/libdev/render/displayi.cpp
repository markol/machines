/*
 * D I S P L A Y I . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "render/render.hpp"
#include "render/surface.hpp"
#include "render/cursor2d.hpp"
#include "render/internal/displayi.hpp"
#include "render/internal/devicei.hpp"
#include "render/internal/devidi.hpp"
#include "render/device.hpp"
#include "render/internal/modeobsv.hpp"
#include "device/mouse.hpp"
#include <algorithm>

//#include <env.h>

//////////////////////////////////////////////////////////////////////////
// Save an area of a surface and restore it later.
class RenISavedArea
{
public:
	// Some systems need to use video or they smudge and the others need system as
	// video produces all kinds weird effects.
	enum SaveTo
	{
		VIDEO_MEMORY,
		SYSTEM_MEMORY
	};

	RenISavedArea(RenSurface& getPixelFormatFrom, int width, int height, SaveTo);
	~RenISavedArea();
	void save(RenSurface& surf, const Ren::Rect& rect);
	void restore();
	bool hasDifferentSize(int width, int height );

	// If the front surface is copied elsewhere, then this function will copy the
	// restore area onto that destination.
	void restoreTo(RenSurface* dest, const RenSurface& front, Ren::Rect srcArea, int destX, int destY) const;

private:
	RenSurface			original_;
	RenSurface			save_;
	Ren::Rect			area_;
};

RenISavedArea::RenISavedArea(RenSurface& getPixelFormatFrom, int width, int height, SaveTo saveTo)
{
	area_.width=width;
	area_.height=height;

	switch(saveTo)
	{
	case VIDEO_MEMORY:
		save_ = RenSurface::createAnonymousVideoSurface(width, height, getPixelFormatFrom);
		break;

	case SYSTEM_MEMORY:
		save_ = RenSurface::createAnonymousSurface(width, height, getPixelFormatFrom);
		break;

	default:
		ASSERT_INFO(saveTo);
		ASSERT_FAIL("Someone did something very naughty, invalid enum value.");
		break;
	}
}

bool RenISavedArea::hasDifferentSize(int width, int height )
{
  return ( area_.width!=width or area_.height!=height );
}

void RenISavedArea::save(RenSurface& surf, const Ren::Rect& rect )
{
	PRE(rect.width == area_.width);
	PRE(rect.height == area_.height);
	PRE(save_.width()  == rect.width);
	PRE(save_.height() == rect.height);

	area_.originX = rect.originX;
	area_.originY = rect.originY;
    original_ = surf;

	ASSERT(!original_.readOnly(), "");
	RENDER_STREAM("Saving cursor blit." << std::endl);
	save_.simpleBlit(original_, area_, 0,0);
}

RenISavedArea::~RenISavedArea()
{
}

void RenISavedArea::restore()
{
	RENDER_STREAM("Restoring cursor blit." << std::endl);
	original_.simpleBlit(save_, area_.originX, area_.originY);
}

static void intersectRects(Ren::Rect& r1, const Ren::Rect& r2)
{
    const int xMax = std::min(r1.originX + r1.width,  r2.originX + r2.width);
    const int yMax = std::min(r1.originY + r1.height, r2.originY + r2.height);

    r1.originX = std::max(r1.originX, r2.originX);
    r1.originY = std::max(r1.originY, r2.originY);

	r1.width  = xMax - r1.originX;
	r1.height = yMax - r1.originY;
}

void RenISavedArea::restoreTo
(
	RenSurface* dest,
	const RenSurface& front,
	Ren::Rect srcArea,
	int destX, int destY
) const
{
	// We can't actually assert that the front and original surfaces are the same.
	// This saved area is created from the back buffer.  The restore will be
	// called for the front buffer.
	PRE(dest);
	PRE(front.width()  == original_.width());
	PRE(front.height() == original_.height());

	// The RenSurface should do most of the clipping efficiently.  However, there
	// is one special case to deal with here.  If the cursor lies on the edge of
	// the original blit's source area, then we should only copy part of the saved
	// area.  The regular clipping won't handle this if the original blit's
	// destination area subsets the destination surface (yuk).
	Ren::Rect copyArea = area_;
	intersectRects(copyArea, srcArea);

	if (copyArea.width > 0 && copyArea.height > 0)
	{
		ASSERT(copyArea.width   <= area_.width,   "Intersection calculation error.");
		ASSERT(copyArea.height  <= area_.height,  "Intersection calculation error.");
		ASSERT(copyArea.originX >= area_.originX, "Intersection calculation error.");
		ASSERT(copyArea.originY >= area_.originY, "Intersection calculation error.");

		// Work out where the cursor would lie on the destination surface.
		const int toX = copyArea.originX - srcArea.originX + destX;
		const int toY = copyArea.originY - srcArea.originY + destY;

		const int ox = copyArea.originX - area_.originX;
		const int oy = copyArea.originY - area_.originY;
		const Ren::Rect fromRect(ox, oy, copyArea.width, copyArea.height);
		dest->simpleBlit(save_, fromRect, toX, toY);
	}
}

//////////////////////////////////////////////////////////////////////////
RenIDisplay::RenIDisplay(SDL_Window* wnd):
	pWnd_(wnd),
	frameNo_(0),
	cursor_(NULL),
	backBufferCursorSave_(NULL),
	frontBufferCursorSave_(NULL),
	fullscreen_(false),
	supportsGammaCorrection_(false),
	gammaCorrection_(1.0),
	isPrimaryDriver_( false )
{
	observers_.reserve(4);
    TEST_INVARIANT;
}

RenIDisplay::~RenIDisplay()
{
    TEST_INVARIANT;

	_DELETE(backBufferCursorSave_);
	_DELETE(frontBufferCursorSave_);
}

void RenIDisplay::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void RenIDisplay::addObserver(RenIDisplayModeObserver* ob)
{
	PRE(ob);

	ctl_pvector<RenIDisplayModeObserver>::iterator it = find(observers_.begin(), observers_.end(), ob);

	ASSERT(it == observers_.end(), "Display mode observer already in display's list.");
	observers_.push_back(ob);
}

void RenIDisplay::removeObserver(RenIDisplayModeObserver* ob)
{
	PRE(ob);

	ctl_pvector<RenIDisplayModeObserver>::iterator it = find(observers_.begin(), observers_.end(), ob);

	ASSERT(it != observers_.end(), "Display mode observer not in display's list.");
	observers_.erase(it);
}

void RenIDisplay::prepareForModeChange(const RenDisplay::Mode& newMode)
{
	// If we have a cursor save area, it will be meaningless after the mode
	// change because the original surface it refers to will simply have been
	// destroyed.  Thus, destroy the save area.
	RENDER_STREAM("Destroying cursor save area.\n");
	_DELETE(backBufferCursorSave_);
	_DELETE(frontBufferCursorSave_);
	backBufferCursorSave_  = NULL;
	frontBufferCursorSave_ = NULL;

	RENDER_STREAM("Informing display observers before mode change.\n");
	ctl_pvector<RenIDisplayModeObserver>::iterator it = observers_.begin();

	while (it != observers_.end())
	{
		RenIDisplayModeObserver* ob = *it;
		ASSERT(ob, "Null display mode observer.");

		ob->prepareForModeChange(currentMode_, newMode);
		++it;
	}
}

bool RenIDisplay::modeChanged()
{
	RENDER_STREAM("Informing display observers after mode change.\n");
	ctl_pvector<RenIDisplayModeObserver>::iterator it = observers_.begin();

	while (it != observers_.end())
	{
		RenIDisplayModeObserver* ob = *it;
		ASSERT(ob, "Null display mode observer.");

		if( not ob->modeChanged(currentMode_) )
			return false;

		++it;
	}
	return true;
}

ostream& operator <<( ostream& o, const RenIDisplay& t )
{

    o << "RenIDisplay " << (void*)&t << " start" << std::endl;
    o << "RenIDisplay " << (void*)&t << " end" << std::endl;

    return o;
}

void RenIDisplay::drawCursor(RenSurface& backBuf)
{
	if (cursor_)
	{
		RENDER_STREAM("Frame " << frameNo_ << ":" << std::endl);

		const DevMouse::Position& pos = DevMouse::instance().position();
		const int destX = pos.first  - cursor_->originX();
		const int destY = pos.second - cursor_->originY();
		const RenSurface& bm = cursor_->currentBitmap();
		const Ren::Rect dstRect(destX, destY, bm.width(), bm.height());

		// The saved area can reside in one of two places. Certain cards need
		// the saved area to by situated in different places.
		RenISavedArea::SaveTo saveTo = RenISavedArea::SYSTEM_MEMORY;
		backBuf.simpleBlit(bm, dstRect.originX, dstRect.originY);
		//RenDevice::current()->renderSurface(bm, dstRect.originX, dstRect.originY);
	}
}

void RenIDisplay::restoreUnderCursor()
{
	RenISavedArea* flipCursors = backBufferCursorSave_;
	backBufferCursorSave_      = frontBufferCursorSave_;
	frontBufferCursorSave_     = flipCursors;

	if(backBufferCursorSave_)
	{
		backBufferCursorSave_->restore();

		// Only restore the back once if the cursor isn't on.
		if(not cursor_)
		{
			_DELETE(backBufferCursorSave_);
			backBufferCursorSave_ = 0;
			_DELETE(frontBufferCursorSave_);
			frontBufferCursorSave_ = 0;
		}
	}
}

void RenIDisplay::restoreUnderCursor
(
	RenSurface* dest,
	const RenSurface& front,
	Ren::Rect srcArea,
	int destX, int destY
) const
{
	PRE(dest);

	RENDER_STREAM("Restoring Under Cursor." << std::endl);
	RENDER_STREAM("  pCursor : " << (void*)cursor_ << std::endl);
	RENDER_STREAM("  pSave : " << (void*)frontBufferCursorSave_ << std::endl);

	if(cursor_ and frontBufferCursorSave_)
		frontBufferCursorSave_->restoreTo(dest, front, srcArea, destX, destY);
}

void RenIDisplay::discardCursorSaves()
{
	_DELETE(backBufferCursorSave_);
	backBufferCursorSave_ = 0;
	_DELETE(frontBufferCursorSave_);
	frontBufferCursorSave_ = 0;
}

// Try a simple front-to-back blit test, return true if it works.
bool RenIDisplay::blitTest()
{
	ASSERT(currentMode_.width()  >= 200, "Modes smaller than 200 pixels not supported.");
	ASSERT(currentMode_.height() >= 200, "Modes smaller than 200 pixels not supported.");

    return true;
}

static bool isVoodoo( RenIDeviceIdentifier::DeviceType devType )
{
	bool result;
	result = ( devType == RenIDeviceIdentifier::VOODOO_GRAPHICS or
			   devType == RenIDeviceIdentifier::VOODOO2 or
			   devType == RenIDeviceIdentifier::VOODOO_BANSHEE or
			   devType == RenIDeviceIdentifier::VOODOO_RUSH_ALLIANCE_AT25_AT3D_CHIP or
			   devType == RenIDeviceIdentifier::VOODOO_RUSH_MACRONIX_CHIP );
	return result;
}

bool RenIDisplay::isPrimaryDriver() const
{
	return isPrimaryDriver_;
}

void RenIDisplay::useCursor(const RenCursor2d* c)
{
	cursor_ = c;

	if (c)
		RENDER_STREAM("Set " << *c << "\n");
	else
		RENDER_STREAM("Set null cursor.\n");
}

const RenCursor2d* RenIDisplay::currentCursor() const
{
	return cursor_;
}

/* End DISPLAYI.CPP *************************************************/
