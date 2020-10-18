/*
 * D I S P L A Y . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _REN_DISPLAY_HPP
#define _REN_DISPLAY_HPP

#include "base/base.hpp"
#include <SDL2/SDL.h>
#include "render/render.hpp"

template <class T> class ctl_list;
class SysPathName;
class RenSurface;
class RenCursor2d;
class RenIDisplay;

// When a Direct3D app goes into full-screen exculsive mode, this class
// is used to set the mode.  Probably should be merged with DevDisplay?
class RenDisplay
{
public:
    RenDisplay(SDL_Window* wnd);
    ~RenDisplay();

	// Go to full-screen exclusive mode.  Returns true to indicate success.
	bool useFullScreen();
	void resetToNormalScreen();
	bool fullScreen() const;

	class Mode
	{
	public:
		Mode(): width_(0), height_(0), depth_(0), rate_(0) {}
		Mode(const Mode& m): width_(m.width_), height_(m.height_), depth_(m.depth_), rate_(m.rate_), mode_(m.mode_) {}
		const Mode& operator=(const Mode& m) { width_ = m.width_; height_ = m.height_; depth_ = m.depth_;
                                                rate_=m.rate_; mode_ = m.mode_; return *this; }
		bool operator==(const Mode& m) const { return width_ == m.width_ && height_ == m.height_ &&
                                                ( rate_ == m.rate_ || rate_ == 0 || m.rate_ == 0 ); }

		int width ()        const		{ return width_; }
		int height()        const		{ return height_; }
		int bitDepth()      const		{ return depth_; }
		int refreshRate()   const		{ return rate_; }
		int pixels()        const		{ return width_ * height_; }
		int memoryRequired() const	    { return (pixels() * depth_) / 8; }
		const SDL_DisplayMode& mode() const    { return mode_; }

		//bool operator< (const Mode& mode) const;
		// For STL sorting.
		bool operator< (const Mode& mode);
		bool operator<=(const Mode& mode);
		bool operator>=(const Mode& mode);

	private:
		// Only RenDisplay can create modes.  Thus, clients are prevented from
		// requesting modes that aren't actually possible.
		friend class RenDisplay;
		friend class RenDDEnumerator;
		Mode(int w, int h, int r): width_(w), height_(h), depth_(32), rate_(r), mode_() {}
		Mode(SDL_DisplayMode& m): width_(m.w), height_(m.h), depth_(32), rate_(m.refresh_rate), mode_(m) {}
		int width_, height_, depth_, rate_;
		SDL_DisplayMode mode_;

		friend bool operator< (const RenDisplay::Mode& mode1, const RenDisplay::Mode& mode2);
        friend bool operator> (const RenDisplay::Mode& mode1, const RenDisplay::Mode& mode2);
	};

	// Clients cannot create modes, they must use modeList to get
	// a list of modes, choose one and pass a reference to useMode.
	typedef ctl_list<Mode> Modes;
	const Modes& modeList() const;
	bool useMode(const Mode&);
	const Mode& currentMode() const;
	void buildDisplayModesList();
	const Mode getDesktopDisplayMode() const;

	bool useMode(int width, int height, int refresh);

	const Mode& lowestAllowedMode() const;
	bool lowestAllowedMode(const Mode& lowMode );
	bool lowestAllowedMode(int width, int height, int depth);

	const Mode& highestAllowedMode() const;
	bool setHighestAllowedMode( ulong maxMemory );
	bool setHighestAllowedMode();

	// If there's not enough video memory to create all the required
	// surfaces at the current resolution, this method can be used to
	// switch to a lower resolution, at the *same* bit depth.  Returns
	// false if there isn't a lower mode available.
	// PRE(modeList().size() > 0);
	bool useLowerMode();

	// Finds the nearest mode for which Mode::pixels() <= pixels and
	// for which the depth is an *exact* match.  Returns false if there
	// was no suitable mode.
	// PRE(modeList().size() > 0);
	bool useNearestMode(int pixels, int bitDepth);

	// Finds the lowest resolution mode for the given depth.  Returns false
	// if there was no mode of the given depth.  If no depth is specified,
	// you get the lowest mode.
	// PRE(modeList().size() > 0);
	bool useLowestMode(int bitDepth);

	enum MemoryType { SYSTEM_MEMORY, VIDEO_MEMORY };

	// Call when rendering is finished to swap the front and back buffers.
	// If zbDepth==0 no z-buffer is created.
	bool createSurfaces(MemoryType zbType, int zbDepth);

	// Updates the cursor, if necessary.
	void startFrame();

	void flipBuffers(RenSurface backSurf);
	void flipBuffers();

	// Flip the buffers but copy and stretch a sub-area of the back buffer
	// onto the front buffer.  The fromArea rectangle in the back buffer is
	// stretched to entirely cover the frontBuffer.
	void stretchFlipBuffers(RenSurface backSurf, RenSurface frontSurf, const Ren::Rect& fromArea);

	// This method should be called every frame in case Alt-Tab has been
	// pressed.  Returns false if one of the buffers is lost and it could
	// not be restored.  Rendering should not proceed if this returns false.
	bool checkForLostSurfaces();

	// Beware: if this display releases the front buffer, back buffer
	// pointers may become invalid.  There's not much that can be done
	// about this, apart from writing a very sophisticated version of
	// COMPtr which knows about the dependencies.
	// LPDIRECTDRAWSURFACE	backBuffer();
	//IDirectDrawSurface* backBuffer();
	//IDirectDrawSurface* frontBuffer();

	void useCursor(const RenCursor2d*);
	const RenCursor2d* currentCursor() const;

	void supportsGammaCorrection( bool );
	bool supportsGammaCorrection() const;
	void gammaCorrection( const double& gammaCorrection );
	const double& gammaCorrection() const;

	bool isPrimaryDriver() const;

	ulong frameNumber() const;

	SDL_Window* window();

	RenIDisplay& displayImpl();
	const RenIDisplay& displayImpl() const;
	// PRE(pImpl_!=NULL);

    void CLASS_INVARIANT;

private:
	// **Important** the back buffer must be released before the primary.  The
	// order of declartion is critical.  See note in fallibleCreateSurfaces.
	RenIDisplay*				pImpl_;

	bool fallibleCreateSurfaces(MemoryType zbType, int zbDepth);

	friend class RenDDEnumerator;

    // Operations deliberately revoked
    RenDisplay( const RenDisplay& );
    RenDisplay& operator =( const RenDisplay& );
    bool operator ==( const RenDisplay& );
};

ostream& operator <<( ostream& o, const RenDisplay& t );
ostream& operator <<( ostream& o, const RenDisplay::Mode& t );

bool operator< (const RenDisplay::Mode& mode1, const RenDisplay::Mode& mode2);
bool operator> (const RenDisplay::Mode& mode1, const RenDisplay::Mode& mode2);

#endif

/* End DISPLAY.HPP **************************************************/
