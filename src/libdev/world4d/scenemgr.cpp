/*
 * S C E N E M G R . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "world4d/scenemgr.hpp"

#include <algorithm>
#include <stdio.h>

#include "ctl/list.hpp"

#include "device/timer.hpp"

#include "mathex/point3d.hpp"
#include "mathex/line3d.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/sphere3d.hpp"
#include "mathex/abox3d.hpp"

#include "render/device.hpp"
#include "render/stats.hpp"
#include "render/colour.hpp"

#include "world4d/root.hpp"
#include "world4d/camera.hpp"
#include "world4d/light.hpp"
#include "world4d/envirnmt.hpp"
#include "world4d/camshake.hpp"
#include "world4d/domain.hpp"
#include "world4d/domasign.hpp"
#include "world4d/composit.hpp"
#include "world4d/internal/complexi.hpp"
#include "world4d/internal/lighti.hpp"

//#define DO_CULL_TRACING
#ifdef DO_CULL_TRACING
#define LIGHT_CULL_STREAM(x)	IAIN_STREAM(x)
#define LIGHT_CULL_TRACE(x)		x
#define LIGHT_CULL_INDENT(x)	IAIN_INDENT(x)
#else
#define LIGHT_CULL_STREAM(x)	;
#define LIGHT_CULL_TRACE(x)		;
#define LIGHT_CULL_INDENT(x)	;
#endif

class W4dSceneManagerImpl
{

public:
	W4dSceneManagerImpl(RenDisplay* pDisplay, W4dRoot* root);
	~W4dSceneManagerImpl();


private:
	friend class W4dSceneManager;

	double frameRate();
	void updateComplexityLevel();


	//	W4dRoot* const		root_;
    //  Changed to non-const to allow the root to be updated after construction.
    //  This is necessary for persistence.
	W4dRoot* 			root_;
	W4dCamera*	  		currentCamera_;
	W4dDomainAssignor*	domainAssignor_;

	// Scene background stuff.
	bool				clearBg_;
	W4dRoot*			bgRoot_;
	W4dEnvironment*		environment_;

    //  Storage for pushed background details. One day this might
    //  be done using a stack, for the time being we'll pretend it's
    //  a stack that can only have one item on it.
    bool                itemsStacked_;
    bool                stackClearBg_;
    W4dRoot*            pStackBgRoot_;
    RenColour           stackBackgroundColour_;

	// Rendering statistics stuff.
	uint				totalEntities_, totalDomains_;

	// There are two lists of lights, one that contains all the lights and
	// another which contains only the dynamic ones.
    typedef         	ctl_pvector< W4dLight > Lights;
    typedef         	ctl_pvector< W4dLightImpl > LightPimpls;
    Lights          	lights_;
	LightPimpls			dynamicLights_;
	uint				nGlobalLights_, nLocalLights_, nDynamicLights_;

    RenDevice* const	device_;
    W4dCameraShake*     pCameraShake_;

	// set this to true if we want the complexity of the scene
	// to be adjusted so as to guarantee an almost constant frame rate
	W4dComplexity   complexity_;

	DevTimer autoAdjustUpdateTimer_;
	double requestedMinFrameRate_;
	double highEnoughFrameRate_;

	static double requestedMinFrameRateInit_;
	static double highEnoughFrameRateInit_;

	bool dynamicLightsEnabled_;
};

// De-pimple everything in the class.
#define CB_SCENEMANAGER_DEPIMPL \
	CB_DEPIMPL(W4dRoot*, root_);\
	CB_DEPIMPL(W4dCamera*, currentCamera_);\
	CB_DEPIMPL(W4dDomainAssignor*, domainAssignor_);\
	CB_DEPIMPL(bool, clearBg_);\
	CB_DEPIMPL(W4dRoot*, bgRoot_);\
	CB_DEPIMPL(W4dEnvironment*, environment_);\
    CB_DEPIMPL(bool, itemsStacked_);\
    CB_DEPIMPL(bool, stackClearBg_);\
    CB_DEPIMPL(W4dRoot*, pStackBgRoot_);\
    CB_DEPIMPL(RenColour, stackBackgroundColour_);\
	CB_DEPIMPL(uint, totalEntities_);\
	CB_DEPIMPL(uint, totalDomains_);\
	typedef W4dSceneManagerImpl::Lights Lights;\
    CB_DEPIMPL(Lights, lights_);\
    CB_DEPIMPL(RenDevice* const, device_);\
    CB_DEPIMPL(W4dCameraShake*, pCameraShake_);	\
    CB_DEPIMPL(W4dComplexity, complexity_); \
    CB_DEPIMPL(DevTimer, autoAdjustUpdateTimer_);	\
    CB_DEPIMPL(double, requestedMinFrameRateInit_);	\
    CB_DEPIMPL(double, highEnoughFrameRateInit_); \
    CB_DEPIMPL(double, requestedMinFrameRate_);	\
    CB_DEPIMPL(double, highEnoughFrameRate_); \
    CB_DEPIMPL(bool, dynamicLightsEnabled_);

// static
MATHEX_SCALAR W4dSceneManagerImpl::requestedMinFrameRateInit_ = 20;
MATHEX_SCALAR W4dSceneManagerImpl::highEnoughFrameRateInit_ = 30;

//////////////////////////////////////////////////////////////////////////////
W4dSceneManagerImpl::W4dSceneManagerImpl(RenDisplay* pDisplay, W4dRoot* root):
	root_(root),
	currentCamera_(NULL),
	bgRoot_(NULL),
	clearBg_(true),
	environment_(NULL),
	totalEntities_(0),
	totalDomains_(0),
    device_( _NEW(RenDevice(pDisplay)) ),
    itemsStacked_( false ),
	domainAssignor_(NULL),
	pCameraShake_( NULL ),
	requestedMinFrameRate_( requestedMinFrameRateInit_ ),
	highEnoughFrameRate_( highEnoughFrameRateInit_ ),
	dynamicLightsEnabled_( true )
{
	complexity_.enabled( false );
    lights_.reserve( 512 );
}

W4dSceneManagerImpl::~W4dSceneManagerImpl()
{
	_DELETE(device_);
	_DELETE(pCameraShake_);
}

void W4dSceneManagerImpl::updateComplexityLevel()
{
  double framerate=frameRate();

  if( framerate < requestedMinFrameRate_ )
  {
    complexity_.decrease();
  }
  else if( framerate > highEnoughFrameRate_ )
  {
	complexity_.increase();
  }
}

double W4dSceneManagerImpl::frameRate()
{
  return device_->statistics()->frameRate();
}

//////////////////////////////////////////////////////////////////////////////
W4dSceneManager::W4dSceneManager(RenDisplay* pDisplay, W4dRoot* root):
	pImpl_(_NEW(W4dSceneManagerImpl(pDisplay, root)))
{
	CB_SCENEMANAGER_DEPIMPL;
	PRE(root);
	PRE(!root->hasParent());
	ASSERT(device_, runtime_error("Out of memory"));
}

W4dSceneManager::~W4dSceneManager()
{
    TEST_INVARIANT;
	_DELETE(pImpl_);
}

void W4dSceneManager::CLASS_INVARIANT
{
	INVARIANT(this != NULL);
}

static void countLights
(
	const ctl_pvector<W4dLight>& lights,
	uint nGlobal, uint nLocal, uint nDynamic,
	ostream& o
)
{
	uint globalUsed = 0, localUsed = 0, dynamicUsed = 0;

	for (ctl_pvector<W4dLight>::const_iterator i = lights.begin(); i != lights.end(); ++i)
    {
		const W4dLight* light = *i;

		if (light->wasUsedThisFrame())
		{
			if (light->isGlobal())
				++globalUsed;
	        else if (light->isLocal())
				++localUsed;
			else if (light->isDynamic())
				++dynamicUsed;
		}
	}

	o << "Lights (used/total): glob " << globalUsed << "/" << nGlobal;
	o << ", local " << localUsed << "/" << nLocal;
	o << ", dynamic " << dynamicUsed << "/" << nDynamic << "\n";
}

// Render the tree of W4dEntities that the object is managing.
void W4dSceneManager::render()
{
	CB_SCENEMANAGER_DEPIMPL;
    CB_DEPIMPL(uint, nGlobalLights_);
    CB_DEPIMPL(uint, nLocalLights_);
    CB_DEPIMPL(uint, nDynamicLights_);

	// A camera must have been specified.
	PRE(currentCamera());

	LIGHT_CULL_STREAM("\n\n*** Start frame\n\n");

	// update the complexity level
	if( autoAdjustFrameRate() )
	{
		if( autoAdjustUpdateTimer_.time() > 0.25 )
		{
			autoAdjustUpdateTimer_.time( 0 );
			pImpl_->updateComplexityLevel();
		}

		if( complexity().highestComplexityReached() )
			device_->out() << "Maximale ";
		else if ( complexity().lowestComplexityReached() )
			device_->out() << "Minimale ";
		else
			device_->out() << "Adjustable ";
		device_->out() 	<< " scene complexity" << std::endl;

		char buf[1000];
		sprintf(buf, " target frame rate [%.1f, %.1f]; frame rate %.1f\n", requestedMinFrameRate_, highEnoughFrameRate_, pImpl_->frameRate());
		device_->out() << buf;
		complexity().usePriority( 0 );
		device_->out() << " low  priority machines (level, range multiplier): (" << complexity().level( ) << ", " << complexity().rangeMultiplier( ) << ")" << std::endl;
		complexity().usePriority( 1 );
		device_->out() << " high priority machines (level, range multiplier): (" << complexity().level( ) << ", " << complexity().rangeMultiplier( ) << ")" << std::endl;
	}

	MexTransform3d unshakenLocalXform = currentCamera_->localTransform();
	if( shakeCamera() && pCameraShake_->shakeIsOver() )
		cancelCameraShake();

	if ( shakeCamera() )
		shakeCurrentCamera();

	if (environment_)
		environment_->update();

	updateLights();
	currentCamera_->updateRenCamera();

	// update the camera far clipping plane to the fog distance
	currentCamera_->adaptToEnvironment( environment() );
//	device_->out() << "Distance far clipping plane: " << currentCamera()->yonClipDistance() << " m" << std::endl;

	device_->start3D(clearBg_);

	// Attempt a domain render. If the camera is not in a domain, it will use the inOrderRender method.
    currentCamera_->domainRender(36);

	totalEntities_   += currentCamera_->entitiesRendered();
	totalDomains_    += currentCamera_->domainsRendered();

	// Render any background items last.
	if (bgRoot_)
	{
        //Ensure that the camera doesn't clip entities at the original far cliping plane
        currentCamera_->isYonClippingEnabled( false );
		device_->startBackground(110000);
    	currentCamera_->renderTree(bgRoot_, W4dCamera::IN_ORDER_RENDER);
        currentCamera_->isYonClippingEnabled( true );
	}

	device_->end3D();

	if( shakeCamera() )
		currentCamera_->localTransform( unshakenLocalXform );

	if (device_->statistics() && device_->statistics()->shown())
		countLights(lights_, nGlobalLights_, nLocalLights_, nDynamicLights_, device_->out());

}

// Do the per-frame work necessary for each light.
void W4dSceneManager::updateLights()
{
	CB_SCENEMANAGER_DEPIMPL;
    CB_DEPIMPL(uint, nGlobalLights_);
    CB_DEPIMPL(uint, nLocalLights_);
    CB_DEPIMPL(uint, nDynamicLights_);

	// Reset the counts of active lights per frame.
	nGlobalLights_ = nLocalLights_ = nDynamicLights_ = 0;

    for (Lights::iterator i = lights_.begin(); i != lights_.end(); ++i)
    {
		W4dLight* light = *i;

		if (light->isGlobal())
		{
	        light->lazyUpdate();
			++nGlobalLights_;
		}

		// If a light is local, disable it.  It will be enabled again during
		// the render traversal.  Don't call lazyUpdate for the local lights.
		// They will be updated when enabled.
        if (light->isLocal())
        {
        	++nLocalLights_;
        	light->disable();
        }

		// If a client has defined a domain assignor, use it to give every
		// light domains.
		if (light->isDynamic())
		{
		    // Unfortunately, we must update the dynamic lights so that their
			// positions are correct for domain assignment.
	        light->lazyUpdate();

        	light->disable();
			++nDynamicLights_;

			if( dynamicLightsEnabled_ )
			{
				if (!domainAssignor_)
				{
					LIGHT_CULL_STREAM("*Warning: scene manager has no domain assignor.\n");
				}
				else
				{
					domainAssignor_->assignDomains(root_, light);

					// Start by unilluminating any previous entities.
					light->illuminateNone();

					LIGHT_CULL_STREAM("Light " << (W4dEntity*) light << "\n");

					// Then add the containing domain and intersecting domains (if any).
					W4dDomain* containing;
					if (light->hasContainingDomain(&containing))
					{
						LIGHT_CULL_STREAM("  illuminates containing " << (W4dEntity*) containing << "\n");
						light->illuminate(containing);
					}

			    	const W4dEntity::W4dDomains& intersects = light->intersectingDomains();
					W4dEntity::W4dDomains::const_iterator it = intersects.begin();
					while (it != intersects.end())
					{
						light->illuminate(*it);
						LIGHT_CULL_STREAM("  illuminates " << (W4dEntity*) (*it) << "\n");

						++it;
					}

					LIGHT_CULL_TRACE(if (intersects.size() == 0))
						LIGHT_CULL_STREAM("  illuminates nothing\n");

				}
			}
		}
    }
}

void W4dSceneManager::setFrameRateForDebug( )
{
  CB_SCENEMANAGER_DEPIMPL;

  double veryHigh=60;
  double veryVeryHigh=80;

  requestedMinFrameRate_=veryHigh;
  highEnoughFrameRate_=veryVeryHigh;
  autoAdjustFrameRate( true );
}

bool W4dSceneManager::autoAdjustFrameRate() const
{
  CB_SCENEMANAGER_DEPIMPL;
  return complexity_.enabled();
}

void W4dSceneManager::autoAdjustFrameRate( bool setautoAdjustFrameRate )
{
  CB_SCENEMANAGER_DEPIMPL;
  complexity_.enabled( setautoAdjustFrameRate );
  if( autoAdjustFrameRate() )
	pImpl_->autoAdjustUpdateTimer_.time( 0 );
  LIONEL_STREAM("W4dSceneManager::autoAdjustFrameRate(): " << autoAdjustFrameRate() << std::endl);
}

const double& W4dSceneManager::requestedMinFrameRate() const
{
  CB_SCENEMANAGER_DEPIMPL;
  return requestedMinFrameRate_;
}

void W4dSceneManager::requestedMinFrameRate( const double& setRequestedMinFrameRate )
{
  CB_SCENEMANAGER_DEPIMPL;

  requestedMinFrameRate_ = setRequestedMinFrameRate;
}

const double& W4dSceneManager::highEnoughFrameRate() const
{
  CB_SCENEMANAGER_DEPIMPL;
  return highEnoughFrameRate_;
}

void W4dSceneManager::highEnoughFrameRate( const double& sethighEnoughFrameRate )
{
  PRE( sethighEnoughFrameRate > requestedMinFrameRate() );

  CB_SCENEMANAGER_DEPIMPL;
  highEnoughFrameRate_ = sethighEnoughFrameRate;
}

const W4dComplexity& W4dSceneManager::complexity( void ) const
{
  CB_SCENEMANAGER_DEPIMPL;

  return complexity_;
}

W4dComplexity& W4dSceneManager::complexity( void )
{
  CB_SCENEMANAGER_DEPIMPL;

  return complexity_;
}

void W4dSceneManager::cullDynamicLights(const W4dEntity* entity)
{
	CB_SCENEMANAGER_DEPIMPL;
	typedef W4dSceneManagerImpl::LightPimpls LightPimpls;
    CB_DEPIMPL(LightPimpls, dynamicLights_);
    CB_DEPIMPL(uint, nDynamicLights_);

	LIGHT_CULL_STREAM("Culling against lights for " << entity << "\n");
	LIGHT_CULL_INDENT(2);

	// This is a real quick test which will obviate even the bounding volume test in
	// some cases (unfortunately not in battles where it's most critical).
    if (nDynamicLights_ == 0)
    {
		LIGHT_CULL_STREAM("Skipping becuase there are zero dynamic lights.\n");
    }
	else
	{
	    const MexAlignedBox3d* box = NULL;
		if (entity->isComposite())
			box = &(entity->asComposite().compositeBoundingVolume());
		else
	    	box = &(entity->boundingVolume());
		ASSERT(box, "logic error failed to set box pointer");

		const MATHEX_SCALAR entityRadius = box->maxCorner().euclidianDistance(box->minCorner()) / 2;
		const MexPoint3d& entityPosition = entity->globalTransform().position();
		LIGHT_CULL_STREAM("Entity radius=" << entityRadius << "\n");
		LIGHT_CULL_STREAM("Entity pos=" << entityPosition << "\n");

	    for (LightPimpls::iterator i = dynamicLights_.begin(); i != dynamicLights_.end(); ++i)
	    {
			W4dLightImpl* lightImpl = *i;
			ASSERT(lightImpl, "Found null pimple in dynamic light list.");

			if (lightImpl->isDynamicEnabledOn())
			{
				LIGHT_CULL_STREAM("Considering light " << (W4dEntity*) *i << "\n");
				LIGHT_CULL_INDENT(2);

				if (not lightImpl->hasBoundingSphere())
				{
					LIGHT_CULL_STREAM("light has no sphere => rejecting\n");
					lightImpl->dynamicDisable( false );
				}
				else
				{
					// check if the entity intersects the light
					const MexSphere3d& sphere = lightImpl->boundingSphere( pImpl_->currentCamera_ );
					const MexPoint3d& lightCenter = sphere.center();

					const MATHEX_SCALAR centersDist = lightCenter.euclidianDistance(entityPosition);
					const bool result = centersDist > sphere.radius() + entityRadius;

					LIGHT_CULL_STREAM("light radius=" << sphere.radius() << "\n");
					LIGHT_CULL_STREAM("light-entity dist=" << centersDist << "\n");

					LIGHT_CULL_STREAM("enabled=" << (!result? "true": "false") << "\n");

					lightImpl->dynamicDisable( result );
				}

				LIGHT_CULL_INDENT(-2);
			}
		}
	}

	LIGHT_CULL_INDENT(-2);
}

W4dCamera* W4dSceneManager::currentCamera() const
{
	return pImpl_->currentCamera_;
}

void W4dSceneManager::useCamera(W4dCamera* cam)
{
	CB_SCENEMANAGER_DEPIMPL;

	PRE(cam);
	PRE(cam->findRoot() == root_);
	PRE(cam->hitherClipDistance() < cam->yonClipDistance());

	currentCamera_ = cam;
    device_->useCamera( cam->pRenCamera() );

	// modify the camera transform key so as to prevent lazy evaluation to occur at the first
	// render pass
	MexTransform3d trans( currentCamera_->globalTransform() );
	// do nothing
	trans.translate( MexPoint3d() );
	currentCamera_->globalTransform( trans );

}

void W4dSceneManager::useLevelOfDetail( bool enableLod )
{
    complexity().enabledLOD(enableLod);
}

void W4dSceneManager::ambient(const RenColour& amb)
{
    pImpl_->device_->ambient(amb);
}

const RenColour& W4dSceneManager::ambient() const
{
    return pImpl_->device_->ambient();
}

void W4dSceneManager::environment(W4dEnvironment* e)
{
	pImpl_->environment_ = e;
}

W4dEnvironment* W4dSceneManager::environment()
{
	return pImpl_->environment_;
}

void W4dSceneManager::clearBackground(bool f)
{
	pImpl_->clearBg_ = f;
}

bool W4dSceneManager::clearBackground() const
{
	return pImpl_->clearBg_;
}

void W4dSceneManager::backgroundColour(const RenColour& bg)
{
    pImpl_->device_->backgroundColour(bg);
}

const RenColour& W4dSceneManager::backgroundColour() const
{
    return pImpl_->device_->backgroundColour();
}

void W4dSceneManager::useBackground(W4dRoot* r)
{
	pImpl_->bgRoot_ = r;
}

void W4dSceneManager::useRoot(W4dRoot* r)
{
	pImpl_->root_ = r;
}

void W4dSceneManager::showStats(double updateInterval)
{
    RenStats* stats = pImpl_->device_->statistics();

    if (stats)
    {
        stats->show();
        stats->displayInterval(updateInterval);
    }
}

void W4dSceneManager::hideStats()
{
    RenStats* stats = pImpl_->device_->statistics();

    if (stats)
        stats->hide();
}

void W4dSceneManager::addLight(W4dLight* pLight)
{
	CB_SCENEMANAGER_DEPIMPL;
	typedef W4dSceneManagerImpl::LightPimpls LightPimpls;
    CB_DEPIMPL(LightPimpls, dynamicLights_);
    PRE( pLight );

	// Not all lights are attached to the root of the actual rendered world.
	// Some are attached to exemplar roots. This light will be added to the
	// current scene manager only iff it shares the same root.  A bit crufty.
	// The scene manager-root concept needs a redesign.
	W4dRoot* lightsRoot = pLight->findRoot();
	if (root_ == lightsRoot || (bgRoot_ && bgRoot_ == lightsRoot))
	{
		lights_.push_back( pLight );
		device_->addLight( pLight->pRenLight() );

		// Maintain a second list containing only the dynamic lights (or rather their
		// pimples).  This assumes that a light won't change it's pimple during its life.
		if (pLight->isDynamic())
			dynamicLights_.push_back(&(pLight->impl()));
	}
}

void W4dSceneManager::removeLight(W4dLight* pLight)
{
	CB_SCENEMANAGER_DEPIMPL;
	typedef W4dSceneManagerImpl::LightPimpls LightPimpls;
    CB_DEPIMPL(LightPimpls, dynamicLights_);
    PRE( pLight );

	// If it's a dynamic light, remove it from the dynamic-only list.
	if (pLight->isDynamic())
	{
	    LightPimpls::iterator found = find( dynamicLights_.begin(), dynamicLights_.end(), &(pLight->impl()) );
		if (found != dynamicLights_.end())
			dynamicLights_.erase(found);
	}

    Lights::iterator found = find( lights_.begin(), lights_.end(), pLight );
	if (found != lights_.end())
	{
		lights_.erase(found);
	    device_->removeLight( pLight->pRenLight() );
	}
}

void W4dSceneManager::turnOffAllLocalLights()
{
	CB_SCENEMANAGER_DEPIMPL;

    for( Lights::iterator i = lights_.begin(); i != lights_.end(); ++i )
	{
		W4dLight* light = *i;

        if (light->isLocal())
        	light->disable();
	}
}

void W4dSceneManager::updateDisplay()
{
}

ostream& W4dSceneManager::out()
{
    return pImpl_->device_->out();
}

RenDevice* W4dSceneManager::pDevice( void )
{
    return pImpl_->device_;
}

// TBD: this is total hack for Milestone IX.  It ought to be a member
// variable of W4dSceneManager, but I wish to avoid a header file change.
static W4dEnvironment*      w4dPushedEnv   = NULL;

void    W4dSceneManager::pushBackgroundData()
{
	CB_SCENEMANAGER_DEPIMPL;
    PRE( not itemsStacked_ );

    stackClearBg_ = clearBackground();
    pStackBgRoot_ = bgRoot_;
    stackBackgroundColour_ = backgroundColour();

	// Total hack for Milestone IX.
	environment_->disable();
	::w4dPushedEnv = environment_;
	environment_ = NULL;
	ambient(0.0);

    itemsStacked_ = true;

    POST( itemsStacked_ );
}

void    W4dSceneManager::popBackgroundData()
{
	CB_SCENEMANAGER_DEPIMPL;
    PRE( itemsStacked_ );

    clearBackground( stackClearBg_ );
    useBackground( pStackBgRoot_ );
    backgroundColour( stackBackgroundColour_ );

	// Total hack for Milestone IX.
	environment_ = ::w4dPushedEnv;
	environment_->enable();

    itemsStacked_ = false;

    POST( not itemsStacked_ );
}

W4dRoot& W4dSceneManager::root() const
{
    PRE( pImpl_->root_ != NULL );
    return *(pImpl_->root_);
}

void W4dSceneManager::clearAllLights()
{
	CB_SCENEMANAGER_DEPIMPL;

    while( lights_.size() != 0 )
        removeLight( lights_.back() );
}

bool W4dSceneManager::shakeCamera() const
{
	return pImpl_->pCameraShake_ != NULL;
}

W4dCameraShake& W4dSceneManager::cameraShakeForEdit()
{
	CB_SCENEMANAGER_DEPIMPL;

	if( pCameraShake_ == NULL )
		pCameraShake_ = _NEW( W4dCameraShake() );

	return *pCameraShake_;
}


void W4dSceneManager::shakeCurrentCamera()
{
	CB_SCENEMANAGER_DEPIMPL;

	MexLine3d cameraToEpicCentre(currentCamera_->globalTransform().position(), pCameraShake_->epicCentre());
	MATHEX_SCALAR length = cameraToEpicCentre.length();

	const MATHEX_SCALAR epicRadius = 200.0;
	MATHEX_SCALAR normedLength = length/epicRadius;
	MATHEX_SCALAR damp = 1.0 - 2.0*normedLength + normedLength*normedLength;
	if( length > epicRadius ) damp = 0.0;

	MexTransform3d unshakenTransform = currentCamera_->localTransform();

	MexTransform3d shakeTransform;
	pCameraShake_->offsetTransform( &shakeTransform );

	MexEulerAngles angles;
	shakeTransform.rotation( &angles );

    MexRadians azi = angles.azimuth();
    MexRadians ele = angles.elevation();
    MexRadians rol = angles.roll();

    angles.azimuth( damp*azi );
    angles.elevation( damp*ele );
    angles.roll( damp*rol );

	unshakenTransform.rotate( angles );

	currentCamera_->localTransform( unshakenTransform );
}

void W4dSceneManager::cancelCameraShake()
{
	CB_SCENEMANAGER_DEPIMPL;

	_DELETE( pCameraShake_ );
	pCameraShake_ = NULL;

	POST( !shakeCamera() );
}

void W4dSceneManager::domainAssignor(W4dDomainAssignor* d)
{
	pImpl_->domainAssignor_ = d;
}

W4dDomainAssignor* W4dSceneManager::domainAssignor() const
{
	return pImpl_->domainAssignor_;
}

bool W4dSceneManager::dynamicLightsEnabled( void ) const
{
	return pImpl_->dynamicLightsEnabled_;
}

void W4dSceneManager::dynamicLightsEnabled( bool enabled )
{
	pImpl_->dynamicLightsEnabled_ = enabled;
}

/* End SCENEMGR.CPP *************************************************/
