/*
 * S C E N E M G R . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _REN_SCENEMGR_HPP
#define _REN_SCENEMGR_HPP

#include "base/base.hpp"
#include "mathex/mathex.hpp"

class BaseLogBuffer;
class RenColour;
class RenDevice;
class RenDisplay;
class W4dEntity;
class W4dRoot;
class W4dCamera;
class W4dLight;
class W4dEnvironment;
class W4dCameraShake;
class W4dDomainAssignor;
class W4dSceneManagerImpl;
class W4dComplexity;
class W4dLightImpl;

// Objects of this class orchestrate all the elements required to produce
// a timely rendered scene on the display.  Within the Render library,
// these objects have top-level control of a rendering process.  There
// can be multiple scene managers -- one per window.
class W4dSceneManager
{
public:
	// There is a choice of ctors.  The window version shows the scene in
	// the given window; the display version takes over the display and goes
	// into full-screen exclusive mode.
	// PRE(root);
	// PRE(!root->hasParent());
    W4dSceneManager( RenDisplay* pDisplay, W4dRoot* pRoot );
    ~W4dSceneManager();

	// Render the tree using the current camera.
	// PRE(currentCamera()) -- a camera must have been specified.
	void render();

	// Use another camera.  The given camera must be part of the rendering
	// tree which this object is managing:
	// PRE(cam);
	// PRE(cam->findRoot() == root_);
	// PRE(cam->hitherClipDistance() < cam->yonClipDistance());
	void useCamera(W4dCamera* cam);

	// Which camera is currently in use?
	W4dCamera* currentCamera() const;

	// Disable LOD for highest detail rendering
	void useLevelOfDetail( bool enableLod );

	void ambient(const RenColour&);
	const RenColour& ambient() const;

    void clearAllLights(); //Remove all lights currently attached

	void turnOffAllLocalLights();

	// If set by the client app, this object is supposed to organise lights,
	// background objects and the background colour.
	void environment(W4dEnvironment*);
	W4dEnvironment* environment();

	// If set by the client app, this object is used to assign intersecting
	// domains to dynamic lights (and possibly other objects in the future).
	// If this object isn't set then dynamic lights will behave as if global,
	// i.e. inefficiently.  (It should be set except in test harnesses.)
	void domainAssignor(W4dDomainAssignor*);
	W4dDomainAssignor* domainAssignor() const;

	// Every frame the back buffer can optionally be cleared to a flat colour.
	// Alternatively, meshes can be drawn as background objects.  If these
	// completely fill the frame buffer, then clearing is unnecessary.
	// The default is a cleared background with no W4dBackground objects.
	void clearBackground(bool);
	bool clearBackground() const;
	void backgroundColour(const RenColour&);
	const RenColour& backgroundColour() const;

	// Only one background tree can be in use at any time.
	void useBackground(W4dRoot*);

    //  Over ride the root that was passed in at the constructor.
    void useRoot(W4dRoot* r);

    //Export the root entity for the scene
    W4dRoot& root() const;

    //  Push and pop all background data (colour, clear flag, tree) onto an
    //  internal stack so that it can be reset then restored.
    void    pushBackgroundData();
    void    popBackgroundData();

	// Various rendering stats can be shown: frame times, nos. of polys, etc.
	// The stats are not necessarily updated every frame because that's
	// difficult to read.  Averages are computed over the given time interval.
	void showStats(double updateInterval = 0.333);
	void hideStats();		// default

	// Check the bounding volume of the given entity against any dynamic lights
	// which are operational.  Disable any lights which don't intersect the
	// entity's bounding volume.
	void cullDynamicLights(const W4dEntity* entity);

	const W4dComplexity& complexity() const;
	W4dComplexity& complexity();

	// Clients can print any debugging text here.  Any text will be overlayed
	// on the 3D graphics.  The 3D is drawn first, then any internal text such
	// as frame rate, then the client's text below that.
	ostream& out();

    void CLASS_INVARIANT;

	void updateDisplay();

    RenDevice* pDevice( void );

	W4dCameraShake& cameraShakeForEdit();

	// Set the minimum requested frame rate to a high value for debugging
	void setFrameRateForDebug();

	// true if we want the complexity of the scene
	// to be adjusted so as to garantee a steady frame rate
    bool autoAdjustFrameRate( void ) const;
    void autoAdjustFrameRate( bool autoAdjust );

	const double& requestedMinFrameRate( void ) const;
	void requestedMinFrameRate( const double & );

	const double& highEnoughFrameRate( void ) const;
	void highEnoughFrameRate( const double & );

	// enable/disable dynamic lights
	void dynamicLightsEnabled( bool enabled );
	bool dynamicLightsEnabled( void ) const;

private:
	// Modify the list of objects which can potentially light the scene.
	// Only the light ctor and dtor should do this.
	friend class W4dLight;
	void    addLight(W4dLight* pLight);		// PRE(pLight);
	void removeLight(W4dLight* pLight);		// PRE(pLight);

	void setShades();
	void  printText(BaseLogBuffer& buf, int& lineNo);
	void shakeCurrentCamera();
	bool shakeCamera() const;
	void cancelCameraShake();
	void updateLights();

	W4dSceneManagerImpl*	pImpl_;

    // Operation deliberately revoked
    W4dSceneManager( const W4dSceneManager& );
    W4dSceneManager& operator =( const W4dSceneManager& );
    bool operator ==( const W4dSceneManager& );
};

#endif

/* End SCENEMGR.HPP *************************************************/
