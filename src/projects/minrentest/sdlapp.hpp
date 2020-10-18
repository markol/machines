#ifndef _SDLAPP_HPP
#define _SDLAPP_HPP

#define MOST_DERIVED_APP SDLApp

#include "render/render.hpp"
#include "device/timer.hpp"
#include "mathex/transf3d.hpp"
#include "ctl/countptr.hpp"
#include "ctl/pvector.hpp"
#include "afx/osapp.hpp"

class SysPathName;


class WrappingTimer
{
public:
	WrappingTimer(double m): modulo_(m)		{ timer_.time(0); snapshot(); }

//	void snapshot()			{ snapshot_ = fmod(timer_.time(), modulo_); }
// Temporarily disable wrapping.
	void snapshot()			{ snapshot_ = timer_.time(); }
	double time() const		{ return snapshot_; }
	void time(double t)		{ timer_.time(t); }

private:
	const double 	modulo_;
	double			snapshot_;
	DevTimer		timer_;
};

// This class represents an application based on Direct3D.  When we know
// what's common to all D3D apps, this probably ought to become an abstract
// base class.  It's based on the application framework, so there's no main,
// just this class.
class SDLApp : public AfxOSSpecificApp<SDLApp>
{
public:
	virtual ~SDLApp();
private:
	friend class AfxSingletonApp;
	SDLApp();

	// Provide virtual fns required by base classes.
	virtual bool clientStartup ();
	virtual void clientShutdown();
	virtual void loopCycle();
	virtual void updateDisplay();
	//virtual bool activate(WORD wParam);

	// Allow a derived class to override this and provide a window size.
	// This shouldn't be necessary when DevDisplay works with Win95 & D3D.
	virtual void getWindowSize(int& ox, int& oy, int& w, int& h);

	// Internal convenience methods.
	void processInput();
	void queueEvents();



	bool					initialised_;
	int						winWidth_, winHeight_;

	bool				clearBack_, doFog_, nvgOn_;
	double				testTimer_;

	float				fogStart_, fogEnd_, fogDensity_;
	bool				renderingForShot_;
};

#endif
