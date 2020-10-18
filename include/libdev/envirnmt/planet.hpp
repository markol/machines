/*
 * P L A N E T . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#ifndef _ENVIRNMT_PLANET_HPP
#define _ENVIRNMT_PLANET_HPP

#include "base/base.hpp"
#include "world4d/envirnmt.hpp"
#include "render/colour.hpp"
#include "render/stars.hpp"
#include "ctl/pvector.hpp"

class RenCamera;
class RenLightColourTransform;
class W4dRoot;
class W4dGeneric;
class W4dSceneManager;
class W4dStars;

class SysPathName;

class EnvSatellite;
class EnvElevationColourTable;
class EnvSky;
class EnvOrbit;
class EnvUniformSky;
class EnvStaticSky;
class EnvDynamicSky;
class EnvSkyline;
class EnvIOrbitParams;
class EnvISatelliteParams;

// A concrete class which represents the environment of a planet.  The fog 
// corresonponds to weather.  The light sources are suns, moons, etc.  This
// class is responsible for a day-night cycle and, correspondingly, the named
// lighting states are typically "day" and "night".
class EnvPlanetEnvironment : public W4dEnvironment
{
public:
	enum VisibilityLevel { NO_SKY, SKY_WO_SATELLITES, FULL_SKY };

	// PRE(envFile.existsAsFile());
    EnvPlanetEnvironment(const SysPathName& envFile, W4dSceneManager*);
    virtual ~EnvPlanetEnvironment();

	virtual void update();
	virtual void enable();
	virtual void disable();
	virtual bool enabled() const;
	virtual void lightConfiguration(const string& c, PhysRelativeTime);
	// The mean r, g and b of all satellites and the ambient light.
	virtual MATHEX_SCALAR averageIllumination() const;
	virtual void nvgOn(bool yesNo);
	virtual bool isNvgOn() const;
	virtual void fogOn(bool);
	virtual bool fogOn() const;
	virtual void fogZenith(bool);
	virtual bool fogZenith() const;
	virtual MATHEX_SCALAR fogVisibility() const;

	size_t nSatellites() const;
	EnvSatellite* satellite(size_t n);				// PRE(n<nSatellites());
	const EnvSatellite* satellite(size_t n) const;	// PRE(n<nSatellites());

	// For timing purposes, you can disable the dome on the fly.	
	void skyDomeOn(bool);
	bool skyDomeOn() const;

	RenColour nvgFilterColour() const;
	MATHEX_SCALAR nvgFogMultiplier() const;
	MATHEX_SCALAR nvgLightMultiplier() const;

	void nvgFilterColour(const RenColour& newFilterColour);
	void nvgSkyColour(const RenColour&);
	void nvgFogColour(const RenColour&);
	void nvgSkylineColour(const RenColour&);
	void nvgFogMultiplier(MATHEX_SCALAR newMultiplier);
	void nvgLightMultiplier(MATHEX_SCALAR newMultiplier);

	// Fog set-up.
	void fogMaxVisibility(MATHEX_SCALAR v);			// PRE(v > 0);
	void fogColourFixed(const RenColour&);
	void fogColourTable(const EnvElevationColourTable*);
	void fogColourController(const EnvSatellite*);
	void fogZenithScale(MATHEX_SCALAR s);		  	// PRE(s > 0);
	MATHEX_SCALAR fogZenithScale();

	// visibility setup
	void setVisibilityLevel( VisibilityLevel level );
	void visibleSky( bool setVisible );
	void visibleSatellites( bool setVisible );
	void visibleStars( bool setVisible );

	EnvOrbit* createOrbit(EnvIOrbitParams*);
	EnvSatellite* createSatellite(EnvISatelliteParams*);
	EnvUniformSky* createUniformSky( void );
	EnvStaticSky* createStaticSky( void );
	EnvDynamicSky* createDynamicSky( void );
	W4dStars* createStars(RenStars::Configuration, MATHEX_SCALAR radius, int nStars);
	EnvSkyline* createSkyline(const SysPathName& meshFile);

    void CLASS_INVARIANT;
    friend ostream& operator <<( ostream& o, const EnvPlanetEnvironment& t );

private:
	void updateSky();
	void updateSatellites();
	void updateShadow();
	void updateAmbient();
	void updateFog();
	
	W4dSceneManager*			manager_;
	W4dRoot* const				bgRoot_;		// Initialisation...
	W4dGeneric* const			skyRoot_;		// order is...
	W4dGeneric* const			satelliteRoot_;	// important.
	
	ctl_pvector<EnvSatellite>	satellites_;
	bool						enabled_;
	EnvSky*						sky_;
	W4dStars*					pStars_;
	EnvSkyline*					skyline_;

	MATHEX_SCALAR 				averageIllumination_;

	bool 						isNvgOn_, hasNvgChanged_;
	RenColour					nvgFilterColour_;
	RenColour					nvgFogColour_;
	RenColour					nvgSkyColour_;
	RenColour					nvgSkylineColour_;
	MATHEX_SCALAR 				nvgFogMultiplier_;
	MATHEX_SCALAR 				nvgLightMultiplier_;
	RenCamera*					nvgCamera_;
	RenLightColourTransform*	lightXform_;
	
	// So the change of the ambient caused by NVG doesn't tart up the 
	// darkness, the ambient is stored.
	RenColour					preNvgAmbient_;
	
	// Fog parameters.
	bool							fogOn_, fogZenith_;
	RenColour						fogColour_;
	const EnvElevationColourTable*	fogClut_;		// overrides fogColour_
	const EnvSatellite*				fogController_;
	MATHEX_SCALAR					fogMaxVisibility_;
	MATHEX_SCALAR					fogZenithScale_;
	MATHEX_SCALAR					fogVisibility_;
	
    EnvPlanetEnvironment( const EnvPlanetEnvironment& );
    EnvPlanetEnvironment& operator =( const EnvPlanetEnvironment& );
    bool operator ==( const EnvPlanetEnvironment& );
};

#endif

/* End PLANET.HPP ***************************************************/
