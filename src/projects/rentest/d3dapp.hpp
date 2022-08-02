#ifndef _D3DAPP_HPP
#define _D3DAPP_HPP

#define MOST_DERIVED_APP D3DApp

#include "rentest/target.hpp"
#include "render/render.hpp"
#include "device/timer.hpp"
#include "mathex/transf3d.hpp"
#include "ctl/countptr.hpp"
#include "ctl/pvector.hpp"
#include "afx/osapp.hpp"

class SysPathName;
class FlyControl;
class RenDisplay;
class RenDevice;
class RenSurface;
class RenCamera;
class RenMaterialVec;
class RenSpinTFPolygon;
class RenTTFTriangles;
class RenCursor2d;
class RenUVTransform;
class RenUVFlip;
class TestUVAnim;
class RenStars;
class EnvNVGMatTransform;

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
class D3DApp : public AfxOSSpecificApp<D3DApp>
{
public:
	virtual ~D3DApp();
private:
	friend class AfxSingletonApp;
	D3DApp();

	// Provide virtual fns required by base classes.
	virtual bool clientStartup ();
	virtual void clientShutdown();
	virtual void loopCycle();
	virtual void updateDisplay();
	virtual bool activate(WORD wParam);

	// Allow a derived class to override this and provide a window size.
	// This shouldn't be necessary when DevDisplay works with Win95 & D3D.
	virtual void getWindowSize(int& ox, int& oy, int& w, int& h);

	// Internal convenience methods.
	void processInput();
	bool readModels(const SysPathName&);
	void createTTFs(const Ren::MeshPtr& mesh);
	void createSpinTree(const Ren::MeshPtr& mesh);
	void createSpinGunBarrel(const Ren::MeshPtr& mesh);
	void createPlasmaBolt(const Ren::MeshPtr& mesh);
	void createLightning(const Ren::MeshPtr& mesh);
	void createTTFDepthTest(const Ren::MeshPtr& mesh);
	void createSTFDepthTest(const Ren::MeshPtr& mesh);
	void createTestCube(const Ren::MeshPtr& mesh);
	void readEnvironment(const SysPathName& planetName);
	void queueEvents();
	void testCursor();
	void exhaustTexMem();
	void saveScreenShot();

	ControlTargetVector<ControlledModel>	models_;
	ControlTargetVector<ControlledLight>	lights_;

	DevTimer				animTimer_;
	RenDisplay*				display_;
	RenDevice*				device_;
	RenCursor2d*			cursor_;

	// An eyepoint, its position and a control for moving it.
	MexTransform3d			eyeXform_;
 	RenCamera*				camera_;
	PhysFlyControl*   		eyeControl_;

	bool					initialised_;
	int						winWidth_, winHeight_;
	RenUVTransform*			uvAnim_;
	RenMaterialVec*			matVec_;
	RenTTFTriangles*		ttf_;

	// Blit testing stuff.
	RenSurface*			testSurf_;

	bool				clearBack_, doFog_, nvgOn_;
	double				testTimer_;

	float				fogStart_, fogEnd_, fogDensity_;
	bool				renderingForShot_;

	RenStars*			pStars_;
	EnvNVGMatTransform*	matXform_;
};

#endif
