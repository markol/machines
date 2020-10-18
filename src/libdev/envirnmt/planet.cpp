/*
 * P L A N E T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include "envirnmt/planet.hpp"
#include "envirnmt/satelite.hpp"
#include "envirnmt/orbit.hpp"
#include "envirnmt/elevclut.hpp"
#include "envirnmt/sky.hpp"
#include "envirnmt/skyline.hpp"
#include "envirnmt/internal/plaparse.hpp"
#include "envirnmt/internal/orbparam.hpp"
#include "envirnmt/internal/satparam.hpp"
#include "envirnmt/internal/skydecl.hpp"

#include "world4d/generic.hpp"
#include "world4d/root.hpp"
#include "world4d/scenemgr.hpp"
#include "world4d/shadow.hpp"
#include "world4d/light.hpp"
#include "world4d/stars.hpp"

#include "render/device.hpp"
#include "render/camera.hpp"
#include "render/colour.hpp"
#include "render/hsv.hpp"
#include "render/light.hpp"
#include "render/litxform.hpp"

#include "device/timer.hpp"

#include "mathex/mathex.hpp"
#include "mathex/degrees.hpp"
#include "mathex/radians.hpp"
#include "mathex/transf3d.hpp"

#include <algorithm>
#include <cmath>

// This transform fn. turns all lights green and increases the intensity
// of point and uniform lights.
class EnvNVGLightTransform : public RenLightColourTransform
{
public:
	EnvNVGLightTransform(const RenColour& filterColour);
	virtual void transform(const RenColour& in, Ren::LightType, RenColour* out) const;
private:
	const RenColour filterColour_;
};

EnvNVGLightTransform::EnvNVGLightTransform(const RenColour& fc):
	filterColour_(fc)
{
}

// virtual
void EnvNVGLightTransform::transform(const RenColour& in, Ren::LightType t, RenColour* out) const
{
	float maxRGB = std::max(std::max(in.r(), in.g()), in.b());

	// If this is a point or a uniform light, then push the intensity up.
	if (t == Ren::POINT || t == Ren::UNIFORM)
		maxRGB *= 1.3;

	out->r(filterColour_.r() * maxRGB);
	out->g(filterColour_.g() * maxRGB);
	out->b(filterColour_.b() * maxRGB);
}

// The background is drawn without the z-buffer, in order.  The sky root must
// be created *before* the roots for any "closer" objects.
EnvPlanetEnvironment::EnvPlanetEnvironment(const SysPathName& envFile, W4dSceneManager* m):
	bgRoot_(_NEW(W4dRoot(W4dRoot::W4dRootId()))),
	skyRoot_(_NEW(W4dGeneric(bgRoot_,  MexTransform3d()))),
	satelliteRoot_(_NEW(W4dGeneric(bgRoot_,  MexTransform3d()))),
	pStars_(0),
	manager_(m),
	enabled_(true),
	sky_(NULL),
	skyline_(NULL),
	isNvgOn_(false),
	hasNvgChanged_(false),
	nvgFilterColour_(RenColour(0.0, 1.0, 0.0)),
	nvgFogMultiplier_(1.5),
	nvgLightMultiplier_(7.0),
	fogOn_(true),
	fogZenith_(false),
	fogZenithScale_(1),
	fogColour_(RenColour::white()),
	fogClut_(NULL),
	fogController_(NULL),
	fogMaxVisibility_(200),
	fogVisibility_(200),
	nvgCamera_(NULL),
	lightXform_(NULL),
	nvgFogColour_(RenColour(0, 0.6, 0.1)),
	nvgSkyColour_(RenColour(0, 0.9, 0.3)),
	nvgSkylineColour_(RenColour(0, 0.6, 0.1)),
	averageIllumination_( 0.0 )
{
    satellites_.reserve( 16 );

	// Horrilble cruft: the background root must be set before we create any
	// global lights, otherwise W4dSceneManager::addLight won't actually add
	// the lights to its list.
	manager_->useBackground(bgRoot_);

	EnvIPlanetParser::instance().parse(envFile, this);
	EnvIPlanetParser::instance().copySatellites(&satellites_);

	sky_ = EnvIPlanetParser::instance().skyDeclaration().completedSky();

	// If there are stars attach them.
	if(EnvIPlanetParser::instance().pStars())
		sky_->attachStars(EnvIPlanetParser::instance().pStars());

	enable();
	setVisibilityLevel( FULL_SKY );

	ASSERT(sky_, "No sky description parsed.");

    TEST_INVARIANT;
}

EnvPlanetEnvironment::~EnvPlanetEnvironment()
{
    TEST_INVARIANT;

    _DELETE( sky_ );
    _DELETE( satelliteRoot_ );
    _DELETE( skyRoot_ );
    _DELETE( bgRoot_ );
}

// virtual
void EnvPlanetEnvironment::update()
{
    TEST_INVARIANT;

	if (enabled_)
	{
		manager_->useBackground(bgRoot_);
		updateSatellites();
		updateShadow();
		updateAmbient();
		updateFog();
		updateSky();

		// Do average illumination calculations.
		MATHEX_SCALAR result = 0.0;
		RenColour ambient;

		if(isNvgOn())
			ambient = preNvgAmbient_;
		else
			ambient = RenDevice::current()->ambient();

		result = ambient.r() + ambient.g() + ambient.b();

		for(int index = 0; index < nSatellites(); ++index)
		{
			EnvSatellite* pSat = satellites_[index];

			if(pSat->light() and pSat->light()->isOn())
			{
				RenColour colour = pSat->light()->colour();
				result += colour.r() + colour.g() + colour.b();

				// While we're here we'll add the NVG changes if any. Probably not the
				// best place but it's the most efficient and doesn't break existing code
				if(isNvgOn())
				{
					colour.r(colour.r() * nvgLightMultiplier_);
					colour.g(colour.g() * nvgLightMultiplier_);
					colour.b(colour.b() * nvgLightMultiplier_);

					pSat->light()->colour(colour);
				}
			}
		}

		averageIllumination_ = result / 3;

		static const bool printDebug = getenv("CB_DEBUG_ENV") != NULL;
		if (printDebug)
		{
			Ren::out() << "Average illumination=" << averageIllumination_ << "\n";
			Ren::out() << "Perceived darkness=" << darkness() << "\n";
		}
	}
}

void EnvPlanetEnvironment::updateFog()
{
    RenDevice* dev = manager_->pDevice();

	if (!fogOn_)
	{
		dev->fogOff();
	}
	else
	{
		// Set the fog colour.
		if(isNvgOn())
		{
			dev->fogColour(nvgFogColour_);
		}
		else if (!fogClut_ || !fogController_)
		{
			// Use fixed fog.
			dev->fogColour(fogColour_);
		}
		else	// use the look-up table
		{
			RenColour fogCol = fogClut_->colour(fogController_->elevation());

			fogCol.a(1);
			dev->fogColour(fogCol);
		}

		// Set the fog visibility/density.
		if (!fogClut_ || !fogController_)
			fogVisibility_ = fogMaxVisibility_;
		else	// use the look-up table
		{
			RenColour fogCol = fogClut_->colour(fogController_->elevation());

			// Use the fog colour alpha as a multiplier for the visibility.
			// Thus the fog can be made more dense by setting alpha.
			fogVisibility_ = fogCol.a() * fogMaxVisibility_;
		}

		// Apply fog multipliers for NVG and zenith.  In practice these should be
		// mutually exclusive, so it probably doesn't matter if we apply them both.
		if(isNvgOn())
			fogVisibility_ *= nvgFogMultiplier_;

		if (fogZenith_)
			fogVisibility_ *= fogZenithScale_;

		dev->fogOn(0.1, fogVisibility_, 1/fogVisibility_);

		// If the sky is invisible, set the background colour to match the fog
		// colour.  Otherwise you get exceedingly silly looking skylines.  This
		// assumes that the sky won't be setting the background colour.
		if (!sky_ || !sky_->visible())
		{
			manager_->clearBackground(true);
			manager_->backgroundColour(dev->fogColour());
		}
	}
}

// virtual
double EnvPlanetEnvironment::fogVisibility( void ) const
{
	PRE( fogOn() );
	return fogVisibility_;
}

void EnvPlanetEnvironment::updateSky()
{
	if (sky_)
		sky_->update(manager_);
}

static void updateSatellite(EnvSatellite*& sat)
{
	sat->update();

	static const bool printDebug = getenv("CB_DEBUG_ENV") != NULL;
	if (printDebug)
		Ren::out() << *sat << std::endl;
}

void EnvPlanetEnvironment::updateSatellites()
{
	for_each(satellites_.begin(), satellites_.end(), updateSatellite);
}

void EnvPlanetEnvironment::updateShadow()
{
	// Search thru the satellites for the one with the strongest light and
	// use that as the basis for shadow casting.  Not ideal!
	W4dDirectionalLight* strongest = NULL;
	double maxInt = -1;
	for (int i=0; i!=satellites_.size(); ++i)
	{
		W4dDirectionalLight* light = satellites_[i]->light();

		if (light)
		{
			const RenColourHSV hsv(light->colour());
			if (hsv.value() > maxInt)
			{
				maxInt = hsv.value();
				strongest = light;
			}
		}
	}

	ASSERT(strongest, "There must be at least one light in an environment.");
	W4dShadow::setLight(strongest);
}

static void accumulateAmbient(EnvSatellite*& sat)
{
	sat->update();
	RenColour amb = RenDevice::current()->ambient();
	const RenColour& satAmb = sat->ambient();
	amb += satAmb;
	RenDevice::current()->ambient(amb);
}

void EnvPlanetEnvironment::updateAmbient()
{
	// Add together the ambient lighting from each satellite.
	RenDevice::current()->ambient(RenColour::black());
	for_each(satellites_.begin(), satellites_.end(), accumulateAmbient);

	if(isNvgOn())
	{
		RenColour ambient = RenDevice::current()->ambient();
		preNvgAmbient_ = ambient;

		ambient.r(ambient.r() * nvgLightMultiplier_);
		ambient.g(ambient.g() * nvgLightMultiplier_);
		ambient.b(ambient.b() * nvgLightMultiplier_);

		// Ambient values are typcially low.  Even with the NGV multiplier they
		// may not become very high.  Thus, to get a complete oversaturation
		// effect during day, we have a hardcoded really bright ambient.  We
		// must be careful to avoid an infinite feedbacj loop because darkness
		// is calculated as a funciton of light levels which include ambient.
		// So long as darkness is based on preNvgAmbient_ we should be OK.
		const RenColour dayAmbient(1.5);
		RenColour interpolated;
		interpolated.linearInterpolate(darkness(), ambient, 1, dayAmbient, 0);

		RenDevice::current()->ambient(interpolated);
	}

	static const bool printDebug = getenv("CB_DEBUG_ENV") != NULL;
	if (printDebug)
	{
		RenColour amb = RenDevice::current()->ambient();
		Ren::out() << "Ambient=" << amb << std::endl;
	}
}

// Virtual.
void EnvPlanetEnvironment::nvgOn(bool on)
{
	if (isNvgOn_ && !on || !isNvgOn_ && on)
	{
		isNvgOn_ = on;
		hasNvgChanged_ = true;
	}

	if (hasNvgChanged_)
	{
		hasNvgChanged_ = false;

		RenDevice* pDevice = RenDevice::current();
		ASSERT(pDevice, "There is no current device.");

		RenCamera* pCamera = pDevice->currentCamera();
		ASSERT(pCamera, "There is no current camera.");

		_DELETE(lightXform_);
		lightXform_ = NULL;

		if (isNvgOn_)
		{
			lightXform_ = _NEW(EnvNVGLightTransform(nvgFilterColour_));
			sky_->overrideColour(nvgSkyColour_);
			pDevice->staticOn();

			if (skyline_)
				skyline_->overrideColour(nvgSkylineColour_);

			// Apply a green filter.
			nvgCamera_ = pCamera;
			pCamera->colourFilter(nvgFilterColour_);
		}
		else
		{
			pDevice->staticOff();
			sky_->resetColour();

			if (skyline_)
				skyline_->resetColour();

			// We can't change the filter for the current camera because the
			// current may have changed.  Do it for the camera for which it
			// was originally set.
			if (nvgCamera_)
			{
				nvgCamera_->colourFilter(RenColour::white());
				nvgCamera_ = NULL;
			}
		}

		RenLight::globalColourTransform(lightXform_);
	}
}

// Virtual.
bool EnvPlanetEnvironment::isNvgOn() const
{
	return isNvgOn_;
}

EnvOrbit* EnvPlanetEnvironment::createOrbit(EnvIOrbitParams* p)
{
	PRE(p);
	return p->createOrbit(satelliteRoot_);
}

EnvSatellite* EnvPlanetEnvironment::createSatellite(EnvISatelliteParams* p)
{
	PRE(p);
	return p->createSatellite(manager_);
}

EnvUniformSky* EnvPlanetEnvironment::createUniformSky()
{
	return _NEW(EnvUniformSky());
}

EnvStaticSky* EnvPlanetEnvironment::createStaticSky( void )
{
	return _NEW(EnvStaticSky(skyRoot_));
}

EnvDynamicSky* EnvPlanetEnvironment::createDynamicSky()
{
	return _NEW(EnvDynamicSky(skyRoot_));
}

W4dStars* EnvPlanetEnvironment::createStars(
	RenStars::Configuration config,
	MATHEX_SCALAR radius,
	int nStars)
{
	pStars_ = _NEW(W4dStars(skyRoot_, config, radius, nStars));

	return pStars_;
}

EnvSkyline* EnvPlanetEnvironment::createSkyline(const SysPathName& meshFile)
{
	// Visual ordering is important (and defined by the relative ordering of
	// bgRoot's children).  This mesh must appear visually in front of
	// everything else (stars, satellites, sky).
	skyline_ = _NEW(EnvSkyline(bgRoot_, meshFile));
	return skyline_;
}

size_t EnvPlanetEnvironment::nSatellites() const
{
	return satellites_.size();
}

EnvSatellite* EnvPlanetEnvironment::satellite(size_t n)
{
	PRE(n<nSatellites());
	return satellites_[n];
}

const EnvSatellite* EnvPlanetEnvironment::satellite(size_t n) const
{
	PRE(n<nSatellites());
	return satellites_[n];
}

void EnvPlanetEnvironment::skyDomeOn(bool on)
{
	sky_->visible(on);
}

bool EnvPlanetEnvironment::skyDomeOn() const
{
	return sky_->visible();
}

RenColour EnvPlanetEnvironment::nvgFilterColour() const
{
	return nvgFilterColour_;
}

MATHEX_SCALAR EnvPlanetEnvironment::nvgFogMultiplier() const
{
	return nvgFogMultiplier_;
}

MATHEX_SCALAR EnvPlanetEnvironment::nvgLightMultiplier() const
{
	return nvgLightMultiplier_;
}

void EnvPlanetEnvironment::nvgFilterColour(const RenColour& newFilterColour)
{
	nvgFilterColour_ = newFilterColour;
}

void EnvPlanetEnvironment::nvgFogMultiplier(MATHEX_SCALAR newMultiplier)
{
	nvgFogMultiplier_ = newMultiplier;
}

void EnvPlanetEnvironment::nvgLightMultiplier(MATHEX_SCALAR newMultiplier)
{
	nvgLightMultiplier_ = newMultiplier;
}

void EnvPlanetEnvironment::nvgSkyColour(const RenColour& c)
{
	nvgSkyColour_ = c;
}

void EnvPlanetEnvironment::nvgFogColour(const RenColour& c)
{
	nvgFogColour_ = c;
}

void EnvPlanetEnvironment::nvgSkylineColour(const RenColour& c)
{
	nvgSkylineColour_ = c;
}

// virtual
void EnvPlanetEnvironment::fogOn(bool o)
{
	fogOn_ = o;
}

// virtual
bool EnvPlanetEnvironment::fogOn() const
{
	return fogOn_;
}

// virtual
void EnvPlanetEnvironment::fogZenith(bool on)
{
	fogZenith_ = on;
}

// virtual
bool EnvPlanetEnvironment::fogZenith() const
{
	return fogZenith_;
}

void EnvPlanetEnvironment::fogZenithScale(MATHEX_SCALAR s)
{
  	PRE(s > 0);
	fogZenithScale_ = s;
}

MATHEX_SCALAR EnvPlanetEnvironment::fogZenithScale()
{
	return fogZenithScale_;
}

void EnvPlanetEnvironment::fogMaxVisibility(MATHEX_SCALAR v)
{
	PRE(v > 0);
	fogMaxVisibility_ = v;
}

void EnvPlanetEnvironment::fogColourFixed(const RenColour& c)
{
	fogColour_ = c;
}

void EnvPlanetEnvironment::fogColourTable(const EnvElevationColourTable* c)
{
	fogClut_ = c;
}

void EnvPlanetEnvironment::fogColourController(const EnvSatellite* s)
{
	fogController_ = s;
}

static void enableSatellite(EnvSatellite*& sat)
{
	W4dLight* light = sat->light();

	if (light)
		light->turnOn();
}

static void disableSatellite(EnvSatellite*& sat)
{
	W4dLight* light = sat->light();

	if (light)
		light->turnOff();
}

// virtual
void EnvPlanetEnvironment::enable()
{
	enabled_ = true;
	bgRoot_->visible(true);
	for_each(satellites_.begin(), satellites_.end(), enableSatellite);
}

// virtual
void EnvPlanetEnvironment::disable()
{
	bgRoot_->visible(false);
	manager_->useBackground(NULL);
	for_each(satellites_.begin(), satellites_.end(), disableSatellite);
	enabled_ = false;
}

// virtual
bool EnvPlanetEnvironment::enabled() const
{
	return true;
}

// virtual
void EnvPlanetEnvironment::lightConfiguration(const string&, PhysRelativeTime)
{
}

// virtual
MATHEX_SCALAR EnvPlanetEnvironment::averageIllumination() const
{
	return averageIllumination_;
}

void EnvPlanetEnvironment::setVisibilityLevel( VisibilityLevel vLevel )
{
	switch( vLevel )
	{
		case NO_SKY:
			visibleSky( false );
			visibleSatellites( false );
			visibleStars( false );
			break;
		case SKY_WO_SATELLITES:
			visibleSky( true );
			visibleSatellites( false );
			visibleStars( false );
			break;
		case FULL_SKY:
			visibleSky( true );
			visibleSatellites( true );
			visibleStars( true );
			break;
		default:
            ASSERT_BAD_CASE_INFO( vLevel );
	}
}


static void visibleSatellite(EnvSatellite*& sat)
{
	if( sat )
		sat->visible( true );
}

static void unvisibleSatellite(EnvSatellite*& sat)
{
	if( sat )
	{
		W4dLight* light = sat->light();
		if( not light )
			sat->visible( false );
	}
}

void EnvPlanetEnvironment::visibleSatellites( bool setVisible )
{
	if( setVisible )
		for_each(satellites_.begin(), satellites_.end(), visibleSatellite);
	else
		for_each(satellites_.begin(), satellites_.end(), unvisibleSatellite);

}

void EnvPlanetEnvironment::visibleStars( bool setVisible )
{
	W4dStars* pStars = sky_->pStars();
	if( pStars )
		pStars->visible( setVisible );
}

void EnvPlanetEnvironment::visibleSky( bool setVisible )
{
	if( sky_ )
		sky_->visible( setVisible );
}

void EnvPlanetEnvironment::CLASS_INVARIANT
{
    INVARIANT(this);
    INVARIANT(bgRoot_);
    INVARIANT(skyRoot_);
    INVARIANT(satelliteRoot_);
    INVARIANT(manager_);
}

ostream& operator <<( ostream& o, const EnvPlanetEnvironment& t )
{
	if (t.satellites_.size() > 0)
		o << *(t.satellites_[0]);

    return o;
}

/* End PLANET.CPP ***************************************************/
