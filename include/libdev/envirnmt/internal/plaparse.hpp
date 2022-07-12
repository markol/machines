/*
 * P L A P A R S E . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#ifndef _ENVIRNMT_PLAPARSE_HPP
#define _ENVIRNMT_PLAPARSE_HPP

#include "base/base.hpp"

#include "ctl/map.hpp"

#include "stdlib/string.hpp"

#include "mathex/mathex.hpp"
#include "render/stars.hpp"
#include "envirnmt/internal/skydecl.hpp"

class SysPathName;

class EnvOrbit;
class EnvSatellite;
class EnvElevationColourTable;
class EnvPlanetEnvironment;
class EnvUniformSky;
class EnvStaticSky;
class EnvDynamicSky;
class EnvIOrbitParams;
class EnvIClutParams;
class EnvISatelliteParams;

class W4dStars;

template <class T> class ctl_pvector;

class EnvIPlanetParser
{
public:
    //  Singleton class
    static EnvIPlanetParser& instance();
    ~EnvIPlanetParser();

	void parse(const SysPathName&, EnvPlanetEnvironment*);

    //Clear out any stored data following a parse. This leaves the maps etc in virgin state.
    void clear();

	void startOrbit(const string*);
	EnvIOrbitParams* orbitInConstruction();
	void orbitComplete();
	EnvOrbit* lookUpOrbit(const string*);

	void startSatellite(const string*);
	EnvISatelliteParams* satelliteInConstruction();
	void satelliteComplete();
	EnvSatellite* lookUpSatellite(const string*);
	void copySatellites(ctl_pvector<EnvSatellite>* vec);	// PRE(vec);

	void addClut(EnvElevationColourTable*);
	EnvElevationColourTable* lookUpClut(const string*);

	EnvISkyDeclaration& skyDeclaration(void);
	EnvUniformSky* createUniformSky(void);
	EnvStaticSky* createStaticSky(void);
	EnvDynamicSky* createDynamicSky(void);

	void startSkyline();
	void completeSkyline();
	void skylineMeshFile(const string*);
	void skylineController(const string*);
	void skylineColourTable(const string*);

	W4dStars* pStars() const;
	void createStars(RenStars::Configuration, MATHEX_SCALAR radius, int nStars);

	EnvPlanetEnvironment* planet()	{ return planet_; }

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const EnvIPlanetParser& t );

private:
	EnvPlanetEnvironment*	planet_;
	EnvIOrbitParams*		orbParams_;
	EnvISatelliteParams*	satParams_;
	EnvISkyDeclaration 		skyDeclaration_;
	W4dStars* 				pStars_;

	// Sky-line parameters.
	bool						contructingSkyline_;
	string						skylineMesh_;
	EnvElevationColourTable*	skylineClut_;
	EnvSatellite*				skylineController_;

	typedef ctl_map<string, EnvElevationColourTable*, std::less<string> > ClutMap;
	typedef ctl_map<string, EnvSatellite*, 			  std::less<string> > SatelliteMap;
	typedef ctl_map<string, EnvOrbit*,				  std::less<string> > OrbitMap;

	ClutMap			cluts_;
	OrbitMap		orbits_;
	SatelliteMap	satellites_;

    EnvIPlanetParser( const EnvIPlanetParser& );
    EnvIPlanetParser& operator =( const EnvIPlanetParser& );
    bool operator ==( const EnvIPlanetParser& );

    EnvIPlanetParser();
};

#endif

/* End PLAPARSE.HPP *************************************************/
