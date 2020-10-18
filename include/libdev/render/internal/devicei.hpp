/***********************************************************

  D E V I C E I . H P P
  Copyright (c) 1998 Charybdis Limited, All rights reserved.

***********************************************************/

#ifndef _RENDER_INTERNAL_DEVICE_IMPL_HPP
#define _RENDER_INTERNAL_DEVICE_IMPL_HPP

#include "base/base.hpp"
#include "base/logbuff.hpp"

#include "render/internal/modeobsv.hpp"
#include "render/internal/internal.hpp"
#include "render/internal/devidi.hpp"
#include "render/colour.hpp"
#include "device/timer.hpp"
#include <glm/gtc/matrix_transform.hpp>

class RenDevice;
class RenCamera;
class RenDisplay;
class RenSurface;
class RenCapabilities;
class RenStats;
class RenMaterial;
class RenIIlluminator;
class RenIViewportMapping;
class RenIDepthPostSorter;
class RenIPriorityPostSorter;
class RenDriverSelector;

class RenIDeviceImpl : public RenIDisplayModeObserver
{
public:
	// Mode observer pure virtuals.
	virtual void prepareForModeChange(const RenDisplay::Mode& currentMode, const RenDisplay::Mode& newMode);
	virtual bool modeChanged(const RenDisplay::Mode& currentMode);
	virtual ~RenIDeviceImpl();

	const glm::mat4& projViewMatrix() const;

	static RenDevice*& current();
	static RenIDeviceImpl*& currentPimpl();

	bool isAlphaSortingEnabled() const;
	RenIDepthPostSorter& alphaSorter();			// PRE(isAlphaSortingEnabled());
	RenIPriorityPostSorter& coplanarSorter();

	void resetFrameTimer();
	double frameTime() const;

	RenStats* statistics();
	const RenStats* statistics() const;

	RenIIlluminator* illuminator() const;

	RenCamera* currentCamera() const;


	void setMaterialHandles(const RenMaterial& mat);

	const RenIViewportMapping* viewportMapping() const;

	bool rendering()   const;
	bool rendering2D() const;
	bool rendering3D() const;
	bool idleRendering() const;

	const RenCapabilities& capabilities() const;

	RenIDeviceIdentifier::DeviceType getDeviceType() const;

	bool hasSharedVideoMemory() const;
	void hasSharedVideoMemory( bool );

	void enableAlphaBlending();
	void disableAlphaBlending();

	void updateFogMultiplier( const RenMaterial& );
	bool fogMultiplierIsNeutral() const;

private:
	friend class RenDevice;
	RenIDeviceImpl(RenDisplay* dis, RenDevice* parent);		// PRE(dis && parent);
	static void useDevice(RenDevice* d, RenI::UpdateType updateType);

//	void createViewport();
//	void updateMatrices();
//	void updateViewMatrix();
//	void updateProjMatrix(double hither, double yon, double h);
//	void setFog(float, float, float, const RenColour&);
//	void addStatic();
//	void addInterference();
//	void commonEndFrame();

	// Sets the rendering parameters appropriately for the 3D world.
	// Draws all alpha polygons. start3D must have been called first.
	// PRE(rendering());
	// PRE(rendering3D());
//	void flush3DAlpha();

	// Change the clipping planes mid-frame.  The effect lasts until endFrame
	// is called.  Intended for drawing backgrounds.
	// PRE(hither < yon);
//	void overrideClipping(double hither, double yon);

	RenDevice* const			parent_;			// back-pointer to public class
	RenCamera*	  				currentCamera_;
	RenDisplay*					display_;
	RenSurface*					surfBackBuf_;
	RenSurface*					surfFrontBuf_;
	bool						rendering_;
	bool						rendering2D_;
	bool						rendering3D_;
	RenColour					background_;
	RenCapabilities*			caps_;
	RenStats*					stats_;
	RenIIlluminator*			illuminator_;
	int							surfacesMayBeLost_;
	bool						shouldBeginScene_;

	glm::mat4*					projViewMatrix_;
	RenIViewportMapping*		vpMapping_;

	// These are used for sorting and drawing polygons which can't be z-buffered.
	RenIPriorityPostSorter*		coplanarSorter_;
	RenIDepthPostSorter* 		alphaSorter_;
	bool						alphaBlendingEnabled_;

	// Special support for rendering background objects.
	bool 						doingBackground_;
	RenIDepthPostSorter*		normalAlphaSorter_;
	RenDriverSelector*			driverSelector_;

	// Any stuff streamed into here is printed after the 3D scene is drawn.
	// The statistics output is printed first.
	BaseLogBuffer 				extOut_;

	bool 						fogOn_;
	RenColour					fogColour_;
	float 						fogStart_, fogEnd_, fogDensity_;
	float						materialFogMultiplier_;

	bool						staticOn_;
	double 						interference_;
	int							debugX_, debugY_;
	bool						antiAliasingOn_;

	DevTimer							frameTimer_;
	RenIDeviceIdentifier::DeviceType	deviceType_;

	bool						videoMemoryShared_;
	bool						videoMemorySharedInitialized_;
};

// This info can't be enabled all the time because it makes the debug
// version fail a pre-condition.
#ifdef DEBUG_FRAME_TIMES
#define DEBUG_FRAME_TIME		1000 * pImpl_->frameTime()
#define DEBUG_RESET_FRAME_TIME	pImpl_->resetFrameTimer()
#else
#define DEBUG_FRAME_TIME		0;
#define DEBUG_RESET_FRAME_TIME	;
#endif

#ifdef _INLINE
    #include "render/internal/devicei.ipp"
#endif

#endif /* _RENDER_INTERNAL_DEVICE_IMPL_HPP ****************/

