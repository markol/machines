#ifndef _D3DAPP_HPP
#define _D3DAPP_HPP

#define MOST_DERIVED_APP SDLApp

#include "mathex/transf3d.hpp"
#include "ctl/countptr.hpp"
#include "ctl/vector.hpp"
#include "device/timer.hpp"
#include "render/render.hpp"
#include "afx/osapp.hpp"
#include "world4d/compplan.hpp"
#include "w4dtest/averager.hpp"

class W4dComposite;
class SysPathName;
class PhysFlyControl;
class RenDisplay;
class RenDevice;
class W4dEntity;
class W4dRoot;
class W4dCamera;
class W4dDirectionalLight;
class W4dGeneric;
class W4dShadow;
class W4dSceneManager;
class ModelData;
class MexDegrees;
class PhysGroundFlyControl;

class SDLApp;

class SDLAppPersistence
{
public:
    SDLAppPersistence( SDLApp* pApp );

    PER_MEMBER_PERSISTENT( SDLAppPersistence );
    PER_FRIEND_READ_WRITE( SDLAppPersistence );
private:
    SDLApp* pSDLApp_;
};

PER_DECLARE_PERSISTENT( SDLAppPersistence );

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
//	virtual bool activate(WORD wParam);

	// Allow a derived class to override this and provide a window size.
	// This shouldn't be necessary when DevDisplay works with Win95 & D3D.
	virtual void getWindowSize(int& ox, int& oy, int& w, int& h);

    virtual const string& name() const;

	// Internal convenience methods.
	void processInput();
	void queueEvents();
	bool readModels(const SysPathName&, MexTransform3d* = NULL);
	void readEnvironment(const SysPathName& planetName);
	void pickDisplayMode(bool windowMode, int width, int height);
	bool createBackground();
	bool readBackground(const SysPathName&, W4dEntity*, W4dEntity**, W4dEntity*,
						W4dEntity*, MATHEX_SCALAR*, MexDegrees*);

    void bobTest();

    void displayHelp();

    friend void perWrite( PerOstream& ostr, const SDLAppPersistence& per );
    friend void perRead( PerIstream& istr, SDLAppPersistence& per );

	ctl_vector<ModelData*>	models_;
	int						controledModel_;

	RenDisplay*				display_;
	RenDevice*				device_;
	W4dComposite*			animSequence_;
	W4dSceneManager*		manager_;
	W4dRoot*				root_;
 	W4dCamera*				camera_;
 	W4dCamera*				pGroundCamera_;
	W4dDirectionalLight*	light_;
	W4dEntity*				dummyLight_;
	W4dEntity*				sun_;
	W4dShadow*				shadow_;
	PhysFlyControl* 		eyeControl_;
	PhysGroundFlyControl*	pGroundControl_;
	PhysFlyControl* 		lightControl_;
	DevTimer				keyTimer_, gameTimer_;
	bool					showInfo_, initialised_;
	bool					doFog_, nvgOn_;
    bool                	displayHelp_;

	float					fogStart_, fogEnd_, fogDensity_;

    SDLAppPersistence   persistence_;
	UtlAverager<double>	speedo_;
};

// All the bits needed to load and move a model.
struct ModelData
{
	W4dEntity*			entity_;
	W4dComposite*		composite_;
	W4dCompositePlanPtr plan_;
	PhysFlyControl* 	control_;

    PER_MEMBER_PERSISTENT_DEFAULT( ModelData );
};

PER_READ_WRITE( ModelData );
PER_DECLARE_PERSISTENT( ModelData );

#endif
