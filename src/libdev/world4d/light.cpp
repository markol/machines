/*
 * L I G H T . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "world4d/light.hpp"
#include "world4d/lllist.hpp"
#include "world4d/manager.hpp"
#include "world4d/scenemgr.hpp"
#include "world4d/camera.hpp"
#include "world4d/internal/lighti.hpp"

#include "render/colour.hpp"
#include "render/light.hpp"

#include "phys/sclplan.hpp"
#include "mathex/point3d.hpp"
#include "mathex/sphere3d.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/vec3.hpp"
#include "ctl/countptr.hpp"
#include "ctl/pvector.hpp"

#include <algorithm>
#include <limits.h>

PER_DEFINE_PERSISTENT_ABSTRACT(W4dLight);
PER_DEFINE_PERSISTENT(W4dDirectionalLight);
PER_DEFINE_PERSISTENT(W4dPointLight);
PER_DEFINE_PERSISTENT(W4dUniformLight);
PER_DEFINE_PERSISTENT(W4dLightImpl);

void W4dLightImpl::dynamicDisable(bool b)
{
	dynamicDisable_ = b;
	updateRenOn();
}


W4dLightImpl::W4dLightImpl( W4dLight* light)
: pW4dLight_( light )
{
}

W4dLightImpl::~W4dLightImpl()
{
}

void W4dLightImpl::updateRenOn()
{
	if (on_ && enabled_ && !dynamicDisable_)
		pLight_->turnOn();
	else
		pLight_->turnOff();
}

const MexSphere3d& W4dLightImpl::boundingSphere( const W4dCamera* camera ) const
{
	PRE(on_);
	PRE(pW4dLight_);

	const ulong cameraPass = (camera)? camera->renderPassId(): ULONG_MAX-1;
	ASSERT(implies(!camera, cameraPass != spherePassId_), "program ran for too many frames");

	// Spheres must be updated even for disabled entites because they could be
	// re-enabled later and meanwhile their spheres are used for culling.
	if (spherePassId_ != cameraPass)
	{
		W4dLightImpl* ncThis = _CONST_CAST(W4dLightImpl*, this);
		ncThis->spherePassId_ = cameraPass;
		pW4dLight_->updateSphere();
	}

	ASSERT(sphere_, "light bounding sphere not defined");
	return *(sphere_);
}

W4dLight::W4dLight(W4dEntity* pParent, const W4dTransform3d& localXform, RenLight* l, bool hasBoundingSphere):
	W4dEntity(pParent, localXform, NOT_SOLID),
	pImpl_(_NEW(W4dLightImpl( this )))
{
	PRE(l);
	PRE(W4dManager::instance().sceneManager());

	pImpl_->pEntities_ = (_NEW(W4dLightImpl::Entities));
    pImpl_->pEntities_->reserve(4);
	pImpl_->pLight_ = l;
	pImpl_->enabled_ = true;
	pImpl_->dynamicDisable_ = false;
	pImpl_->on_ = true;
	pImpl_->intensityPlanPtr_ = NULL;
	pImpl_->pUnintensifiedColour_ = NULL;
	pImpl_->sphere_ = NULL;
	pImpl_->renPassId_    = ULONG_MAX;
	pImpl_->spherePassId_ = ULONG_MAX;
	pImpl_->nRepetations_ = 0;
	scope(GLOBAL);

	pImpl_->hasBoundingSphere_ = hasBoundingSphere;

	W4dSceneManager* man = W4dManager::instance().sceneManager();
	man->addLight(this);

	updateDynamicOn();

    TEST_INVARIANT;
}
// virtual

// virtual
W4dLight::~W4dLight()
{
	TEST_INVARIANT;

	if ( W4dManager::instance().hasSceneManager() )
		W4dManager::instance().sceneManager()->removeLight(this);

	if( pImpl_ != NULL )
	{
		_DELETE(pImpl_->pLight_);
		_DELETE(pImpl_->sphere_);

		while(pImpl_->pEntities_->size()!=0)
			dontIlluminate(pImpl_->pEntities_->back());

		_DELETE(pImpl_->pEntities_);

		if( unintensifiedColourSaved() )
			_DELETE( pImpl_->pUnintensifiedColour_ );
	}

	_DELETE(pImpl_);
}

const W4dLightImpl& W4dLight::impl() const
{
	PRE(pImpl_);
	return *pImpl_;
}

W4dLightImpl& W4dLight::impl()
{
	PRE(pImpl_);
	return *pImpl_;
}

bool W4dLight::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

void W4dLight::lazyUpdate()
{
	PRE(W4dManager::instance().sceneManager());

	// The render attributes need only be updated when the sphere is truly going
	// to be used and is enabled.  The enable method catches the second case.
	// Spheres are different -- see boundingSphere methods.
	if (!pImpl_->on_ && !pImpl_->enabled_)
		return;

	const W4dCamera* camera = W4dManager::instance().sceneManager()->currentCamera();
	const ulong cameraPass = (camera)? camera->renderPassId(): ULONG_MAX-1;
	ASSERT(implies(!camera, cameraPass != pImpl_->renPassId_),    "program ran for too many frames");

	// Update the render library only if we haven't done so this frame.
	if (pImpl_->renPassId_ != cameraPass)
	{
		pImpl_->renPassId_ = cameraPass;
		updatePosition();
		updatePlans();
	}

	updateDynamicOn();
}

void W4dLight::updateDynamicOn()
{
	pImpl_->dynamicEnabledOn_ = scope() == DYNAMIC && pImpl_->on_ && pImpl_->enabled_;
}

void W4dLight::updatePlans()
{
	if (hasIntensityPlan())
	{
		ASSERT(unintensifiedColourSaved(), "needs to call saveUnintensifiedColour() first")

		PhysAbsoluteTime timeNow = W4dManager::instance().time();
		const PhysRelativeTime cycle = pImpl_->intensityPlanPtr_->duration();
		const PhysRelativeTime lifeTime = ( pImpl_->nRepetations_ + 1 ) * cycle;

		PhysRelativeTime timeOffset = timeNow - pImpl_->startTime_;

		if( timeOffset >= 0 and timeOffset <= lifeTime ) //pImpl_->intensityPlanPtr_->duration() )
		{
			timeOffset = timeOffset - int(timeOffset/cycle)	* cycle;
			MATHEX_SCALAR intensity = pImpl_->intensityPlanPtr_->scalar(timeOffset);
			RenColour newColour = *(pImpl_->pUnintensifiedColour_);

			newColour.r( intensity*( pImpl_->pUnintensifiedColour_->r() ) );
			newColour.g( intensity*( pImpl_->pUnintensifiedColour_->g() ) );
			newColour.b( intensity*( pImpl_->pUnintensifiedColour_->b() ) );

			colour( newColour );
		}
	}
}

void W4dLight::turnOn()
{
	pImpl_->on_ = true;
	pImpl_->updateRenOn();
	lazyUpdate();
}

void W4dLight::turnOff()
{
	pImpl_->on_ = false;
	pImpl_->updateRenOn();
	updateDynamicOn();
}

void W4dLight::enable()
{
	pImpl_->enabled_ = true;
	pImpl_->updateRenOn();
	lazyUpdate();
}

void W4dLight::disable()
{
	pImpl_->enabled_ = false;
	pImpl_->updateRenOn();
	updateDynamicOn();
}

bool W4dLight::enabled() const
{
	return pImpl_->enabled_;
}

void W4dLight::dynamicDisable(bool b)
{
	pImpl_->dynamicDisable( b );
	pImpl_->updateRenOn();
}

bool W4dLight::dynamicDisable() const
{
	return pImpl_->dynamicDisable_;
}

bool W4dLight::isDynamicEnabledOn() const
{
	return pImpl_->dynamicEnabledOn_;
}

RenLight* W4dLight::pRenLight()
{
	return pImpl_->pLight_;
}

void W4dLight::dontIlluminate(W4dEntity* pEntityItem)
{
	PRE(!isGlobal());
    PRE( pEntityItem );

	if( pImpl_ != NULL )
	{
	    W4dLightImpl::Entities::iterator it = find( pImpl_->pEntities_->begin(), pImpl_->pEntities_->end(), pEntityItem );
	    if( it != pImpl_->pEntities_->end() )
	    {
	        pImpl_->pEntities_->erase( it );
	    }
	}
	pEntityItem->lightListForEdit().removeLight(this);
}

void W4dLight::illuminate(W4dEntity* pEntityItem)
{
	PRE(!isGlobal());
    PRE( pEntityItem );

    pImpl_->pEntities_->push_back( pEntityItem );
	pEntityItem->lightListForEdit().addLight(this);
}

void W4dLight::illuminateNone()
{
	PRE(!isGlobal());

	// Remove the back pointers from any illuminated entities.
	W4dLightImpl::Entities::iterator it = pImpl_->pEntities_->begin();
	while (it != pImpl_->pEntities_->end())
	{
		(*it)->lightListForEdit().removeLight(this);
		++it;
	}

	pImpl_->pEntities_->erase(pImpl_->pEntities_->begin(), pImpl_->pEntities_->end());
}

bool W4dLight::isOn() const
{
	return pImpl_->on_;
}

void W4dLight::intensityPlan( const PhysScalarPlanPtr& planPtr, const PhysAbsoluteTime& startTime, uint nRepetations )
{
	pImpl_->nRepetations_ = nRepetations;
	pImpl_->intensityPlanPtr_ = planPtr;
	pImpl_->startTime_ = startTime;
	saveUnintensifiedColour();

	POST( unintensifiedColourSaved() );
	POST( hasIntensityPlan() );
}

void W4dLight::saveUnintensifiedColour()
{
	if( pImpl_->pUnintensifiedColour_ == NULL )
		pImpl_->pUnintensifiedColour_ = _NEW(RenColour(colour()));
}

bool W4dLight::unintensifiedColourSaved() const
{
	return 	pImpl_->pUnintensifiedColour_ != NULL;
}

bool W4dLight::hasIntensityPlan() const
{
	return pImpl_->intensityPlanPtr_.isDefined();
}

// non virtual
const MexSphere3d& W4dLight::boundingSphere() const
{
	PRE(isOn());
	PRE(hasBoundingSphere());

	const W4dCamera* camera = W4dManager::instance().sceneManager()->currentCamera();
	return pImpl_->boundingSphere( camera );
}

void W4dLight::boundingSphere(const MexPoint3d& centre, MATHEX_SCALAR radius)
{
	PRE(hasBoundingSphere());

	if (!pImpl_->sphere_)
		pImpl_->sphere_ = _NEW(MexSphere3d(centre, radius));
	else
	{
		pImpl_->sphere_->center(centre);
		pImpl_->sphere_->radius(radius);
	}
}

bool W4dLight::hierarchicallyVisible() const
{
	// IAIN_STREAM("Checking visibility for light " << (W4dEntity*) this << "\n");
	const W4dEntity* ent = this;
	if (!ent->visible())
	{
		// IAIN_STREAM("  self *not* visible\n");
		return false;
	}

	while (ent->hasParent())
	{
		ent = ent->pParent();

		if (!ent->visible())
		{
			// IAIN_STREAM("  parent " << ent << " *not* visible\n");
			return false;
		}
		else
		{
			; //IAIN_STREAM("  parent " << ent << " is visible\n");
		}
	}

	// IAIN_STREAM("  ligth *is* visible\n");
	return true;
}

bool W4dLight::wasUsedThisFrame() const
{
	return pImpl_->pLight_->wasUsedThisFrame();
}

void W4dLight::CLASS_INVARIANT
{
	INVARIANT(pImpl_);
	INVARIANT(pImpl_->pW4dLight_);
	INVARIANT(pImpl_->pLight_);
	INVARIANT(pImpl_->pEntities_);

	// The on/off state of the render light should always be in sync. with the
	// on and enabled states of this light.
	INVARIANT(implies(pImpl_->on_  &&  pImpl_->enabled_,  pImpl_->pLight_->isOn()));
	INVARIANT(implies(!pImpl_->on_ || !pImpl_->enabled_, !pImpl_->pLight_->isOn()));
}

void perWrite( PerOstream& ostr, const W4dLight& light )
{
    const W4dEntity& base = light;

    ostr << base;
    int temp = light.scope();
    PER_WRITE_RAW_OBJECT( ostr, temp );

	ostr << light.pImpl_;
}

void perWrite( PerOstream& ostr, const W4dLightImpl& light )
{
	ostr << light.pW4dLight_;
	ostr << light.pLight_;
    ostr << light.enabled_;
    ostr << light.on_;

    //This is necessary because writing a ctl_pvector* causes a crash
    ostr << *light.pEntities_;

	ostr << light.pUnintensifiedColour_;
	ostr << light.intensityPlanPtr_;
	ostr << light.startTime_;
	ostr << light.hasBoundingSphere_;
}

void perRead( PerIstream& istr, W4dLight& light )
{
    W4dEntity& base = light;

    istr >> base;

	int temp;
    PER_READ_RAW_OBJECT( istr, temp );
    light.scope( _STATIC_CAST( W4dLight::Scope, temp ) );

	istr >> light.pImpl_;
}

void perRead( PerIstream& istr, W4dLightImpl& light )
{
    istr >> light.pW4dLight_;
    istr >> light.pLight_;
    istr >> light.enabled_;
    istr >> light.on_;
	istr >> *light.pEntities_;

	istr >> light.pUnintensifiedColour_;
	istr >> light.intensityPlanPtr_;
	istr >> light.startTime_;
	istr >> light.hasBoundingSphere_;
}

W4dLight::W4dLight( PerConstructor con )
: W4dEntity( con ),
   pImpl_( NULL )
{
}

W4dLightImpl::W4dLightImpl( PerConstructor )
{
	pEntities_ = (_NEW(W4dLightImpl::Entities));
    pEntities_->reserve(4);
}

// Create a transform which represents the direction of a given light.
static MexTransform3d lightTransform(const MexVec3& direction)
{
	MexPoint3d pos(-direction.x(), -direction.y(), -direction.z());
	MexTransform3d result(pos);
	return result;
}

/*
// Create a transform which represents the direction of a given light.
static MexTransform3d lightTransform(const MexVec3& direction)
{
	MexVec3 xBasis = direction;
	xBasis.makeUnitVector();

	// Use the light's direction as the transform's x-basis.  The other
	// basis vectors can be arbitrary, so we make some up.
	MexVec3 yBasis(0,0,1);
	yBasis.crossProduct(xBasis);

	// We can't use zBasis if it is parallel to the xBasis (zero cross product).
	if (fabs(yBasis.modulus()) < MexEpsilon::instance())
	{
		// Try again with the y-axis instead of the z-axis.
		yBasis.setVector(0, 1, 0);
		yBasis.crossProduct(xBasis);

		// The direction can't be parallel to both the z- and the y-axes.
		ASSERT(fabs(yBasis.modulus()) >= MexEpsilon::instance(), "Parallel basis vector mix up.");
	}

	MexVec3 zBasis = xBasis;
	zBasis.crossProduct(yBasis);

	// This funciton isn't efficiency critical, so do the normalisation
	// just to make sure.
	xBasis.makeUnitVector();
	yBasis.makeUnitVector();
	zBasis.makeUnitVector();

	MexTransform3d result(xBasis, yBasis, zBasis, MexPoint3d(0,0,0));
	return result;
}
*/

//////////////////////////////////////////////////////////////////////////////////////////////////
W4dDirectionalLight::W4dDirectionalLight(W4dEntity* pParent, const MexVec3& direction):
	W4dLight(pParent, lightTransform(direction), _NEW(RenDirectionalLight()), false),
	pDirLight_(_STATIC_CAST(RenDirectionalLight*, pRenLight()))
{
}

W4dDirectionalLight::W4dDirectionalLight(W4dEntity* pParent, RenDirectionalLight* l):
	W4dLight(pParent, lightTransform(l->direction()), l, false),
	pDirLight_(l)
{
	PRE(l);
}

// virtual
void W4dDirectionalLight::updatePosition()
{
	// Use the vector from the light's position to the global origin as the
	// light direction.
	MexVec3 globalPos = globalTransform().position();
	globalPos.makeUnitVector();
	globalPos *= -1;
	pDirLight_->direction(globalPos);
}

// virtual
void W4dDirectionalLight::updateSphere()
{

}

const MexVec3& W4dDirectionalLight::direction() const
{
	return pDirLight_->direction();
}

void perWrite( PerOstream& ostr, const W4dDirectionalLight& light )
{
    const W4dEntity& base = light;
    ostr << base;
}

void perRead( PerIstream& istr, W4dDirectionalLight& light )
{
    W4dEntity& base = light;
    istr >> base;
}

W4dDirectionalLight::W4dDirectionalLight( PerConstructor con )
: W4dLight( con )
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////
W4dPointLight::W4dPointLight(W4dEntity* pParent, const MexVec3& position, MATHEX_SCALAR range):
	W4dLight(pParent, W4dTransform3d(position), _NEW(RenPointLight(range)), true),
	pPtLight_(_STATIC_CAST(RenPointLight*, pRenLight()))
{
}

W4dPointLight::W4dPointLight(W4dEntity* pParent, RenPointLight* l):
	W4dLight(pParent, W4dTransform3d(l->position()), l, true),
	pPtLight_(l)
{
	PRE(l);
}

// virtual
void W4dPointLight::updatePosition()
{
	pPtLight_->position(globalTransform().position());
}

// virtual
void W4dPointLight::updateSphere()
{
	boundingSphere(globalTransform().position(), pPtLight_->maxRange());
}

bool W4dLight::hasBoundingSphere() const
{
	return pImpl_->hasBoundingSphere_;
}

void perWrite( PerOstream& ostr, const W4dPointLight& light )
{
    const W4dEntity& base = light;
    ostr << base;
}

void perRead( PerIstream& istr, W4dPointLight& light )
{
    W4dEntity& base = light;
    istr >> base;
}

W4dPointLight::W4dPointLight( PerConstructor con )
: W4dLight( con )
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////
W4dUniformLight::W4dUniformLight(W4dEntity* pParent, const MexVec3& position, MATHEX_SCALAR range):
	W4dLight(pParent, W4dTransform3d(position), _NEW(RenUniformLight(range)), true),
	pUniLight_(_STATIC_CAST(RenUniformLight*, pRenLight()))
{
}

W4dUniformLight::W4dUniformLight(W4dEntity* pParent, RenUniformLight* l):
	W4dLight(pParent, W4dTransform3d(l->position()), l, true),
	pUniLight_(l)
{
	PRE(l);
}

// virtual
void W4dUniformLight::updatePosition()
{
	pUniLight_->position(globalTransform().position());
}

// virtual
void W4dUniformLight::updateSphere()
{
	boundingSphere(globalTransform().position(), pUniLight_->maxRange());
}


void perWrite( PerOstream& ostr, const W4dUniformLight& light )
{
    const W4dEntity& base = light;
    ostr << base;
}

void perRead( PerIstream& istr, W4dUniformLight& light )
{
    W4dEntity& base = light;
    istr >> base;
}

W4dUniformLight::W4dUniformLight( PerConstructor con )
: W4dLight( con )
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// There are many light properties which can simply be forwarded to the render
// equivalents (on/off is the counter-example).
#define LIGHT_FORWARD_PROPERTY(LIGHT_CLASS, TYPE, NAME, POINTER) \
TYPE LIGHT_CLASS::NAME() const	\
{								\
	return POINTER->NAME();		\
}								\
								\
void LIGHT_CLASS::NAME(TYPE p)	\
{								\
	POINTER->NAME(p);			\
}

LIGHT_FORWARD_PROPERTY(W4dLight, const RenColour&, colour, pImpl_->pLight_)

LIGHT_FORWARD_PROPERTY(W4dPointLight,	MATHEX_SCALAR,	  maxRange,			    pPtLight_)
LIGHT_FORWARD_PROPERTY(W4dPointLight,	MATHEX_SCALAR,	  constantAttenuation,  pPtLight_)
LIGHT_FORWARD_PROPERTY(W4dPointLight,	MATHEX_SCALAR,	  linearAttenuation,    pPtLight_)
LIGHT_FORWARD_PROPERTY(W4dPointLight,	MATHEX_SCALAR,	  quadraticAttenuation, pPtLight_)

LIGHT_FORWARD_PROPERTY(W4dUniformLight, MATHEX_SCALAR,	  maxRange,			    pUniLight_)
LIGHT_FORWARD_PROPERTY(W4dUniformLight, MATHEX_SCALAR,	  constantAttenuation,  pUniLight_)
LIGHT_FORWARD_PROPERTY(W4dUniformLight, MATHEX_SCALAR,	  linearAttenuation,    pUniLight_)
LIGHT_FORWARD_PROPERTY(W4dUniformLight, MATHEX_SCALAR,	  quadraticAttenuation, pUniLight_)

/* End LIGHT.CPP ****************************************************/
