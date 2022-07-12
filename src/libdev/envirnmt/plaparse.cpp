/*
 * P L A P A R S E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include "envirnmt/internal/plaparse.hpp"
#include "envirnmt/planet.hpp"
#include "envirnmt/satelite.hpp"	// TBD: remove, only needed for debug.
#include "envirnmt/elevclut.hpp"
#include "envirnmt/sky.hpp"
#include "envirnmt/skyline.hpp"
#include "envirnmt/internal/orbparam.hpp"
#include "envirnmt/internal/satparam.hpp"

#include "ctl/pvector.hpp"
#include "system/pathname.hpp"
#include <stdio.h>

#include "world4d/stars.hpp"

#include "system/metafile.hpp"
#include "system/metaistr.hpp"

#include <fstream>

EnvIPlanetParser& EnvIPlanetParser::instance()
{
    static EnvIPlanetParser instance_;
    return instance_;
}

EnvIPlanetParser::EnvIPlanetParser():
	planet_(NULL),
	orbParams_(NULL),
    pStars_( NULL ),
	skylineClut_(NULL),
	skylineController_(NULL),
	contructingSkyline_(false)
{
    TEST_INVARIANT;
}

EnvIPlanetParser::~EnvIPlanetParser()
{
    TEST_INVARIANT;
	_DELETE( orbParams_ );
}

void EnvIPlanetParser::parse(const SysPathName& envFile, EnvPlanetEnvironment* p)
{
	PRE_INFO(envFile);
	PRE(p);

    //Clear out data from any previous pasring session
    clear();

	extern int yylineno;	      /* line # of current line, defined in lex.l */
	extern int error_count;		  /* count of errors, defined in err_skel.c */
	extern int yyparse(void);     /* defined in err_skel.c */
	extern istream* pGlobalLexIstream;

	ASSERT(pGlobalLexIstream == 0, "This should be nulled.");

    void yydoinit();              //Defined in lexer.l

	// Bits of the parser require that this is set.
	planet_ = p;

	// Do the global variable YACC stuff.
    yydoinit();

	SysMetaFile metaFile("mach1.met");

    if(SysMetaFile::useMetaFile())
    {
        //pGlobalLexIstream = _NEW(SysMetaFileIstream(metaFile, envFile, ios::text));
        pGlobalLexIstream = _NEW(SysMetaFileIstream(metaFile, envFile, std::ios::in));
    }
    else
    {
        ASSERT_FILE_EXISTS(envFile.c_str());
        pGlobalLexIstream = _NEW(std::ifstream(envFile.c_str(), std::ios::in));
    }

    yylineno = 1;
    yyparse();

	_DELETE(pGlobalLexIstream);
	pGlobalLexIstream = 0;
	planet_ = NULL;
}

void EnvIPlanetParser::startOrbit(const string* name)
{
	orbParams_ = _NEW(EnvIOrbitParams(name));
	POST(orbitInConstruction());
}

EnvIOrbitParams* EnvIPlanetParser::orbitInConstruction()
{
	return orbParams_;
}

void EnvIPlanetParser::orbitComplete()
{
	PRE(planet_);
	PRE(orbitInConstruction());

	EnvOrbit* orbit = planet_->createOrbit(orbParams_);
	std::pair<const string, EnvOrbit*> value(orbParams_->name(), orbit);
	orbits_.insert(value);

	_DELETE(orbParams_);
	orbParams_ = NULL;

	POST(!orbitInConstruction());
}

EnvOrbit* EnvIPlanetParser::lookUpOrbit(const string* name)
{
	PRE(name);
	OrbitMap::const_iterator it = orbits_.find(*name);

	if (it == orbits_.end())
		return NULL;
	else
		return (*it).second;
}

void EnvIPlanetParser::startSatellite(const string* name)
{
	satParams_ = _NEW(EnvISatelliteParams(name));
	POST(satelliteInConstruction());
}

EnvISatelliteParams* EnvIPlanetParser::satelliteInConstruction()
{
	return satParams_;
}

void EnvIPlanetParser::satelliteComplete()
{
	PRE(planet_);
	PRE(satelliteInConstruction());

	EnvSatellite* satellite = planet_->createSatellite(satParams_);
	std::pair<const string, EnvSatellite*> value(satParams_->name(), satellite);
	satellites_.insert(value);

	_DELETE(satParams_);
	satParams_ = NULL;

	IAIN_STREAM("Created new " << *satellite << std::endl);

	POST(!satelliteInConstruction());
}

EnvSatellite* EnvIPlanetParser::lookUpSatellite(const string* name)
{
	PRE(name);
	SatelliteMap::const_iterator it = satellites_.find(*name);

	if (it == satellites_.end())
		return NULL;
	else
		return (*it).second;
}

void EnvIPlanetParser::copySatellites(ctl_pvector<EnvSatellite>* vec)
{
	PRE(vec);

	SatelliteMap::const_iterator it = satellites_.begin();
	while (it != satellites_.end())
	{
		vec->push_back((*it).second);
		++it;
	}

	// Remove the satellites from this object's list, effectively transferring
	// ownership to the vector.
	satellites_.erase(satellites_.begin(), satellites_.end());
}

void EnvIPlanetParser::addClut(EnvElevationColourTable* clut)
{
	PRE(clut);
	PRE(clut->name().length() > 0);

	std::pair<const string, EnvElevationColourTable*> value(clut->name(), clut);
	cluts_.insert(value);
}

EnvElevationColourTable* EnvIPlanetParser::lookUpClut(const string* name)
{
	PRE(name);
	ClutMap::const_iterator it = cluts_.find(*name);

	if (it == cluts_.end())
		return NULL;
	else
		return (*it).second;
}

EnvISkyDeclaration& EnvIPlanetParser::skyDeclaration(void)
{
	return skyDeclaration_;
}

EnvUniformSky* EnvIPlanetParser::createUniformSky()
{
	PRE(planet_);

	return planet_->createUniformSky();
}

EnvStaticSky* EnvIPlanetParser::createStaticSky()
{
	PRE(planet_);

	return planet_->createStaticSky();
}

EnvDynamicSky* EnvIPlanetParser::createDynamicSky()
{
	PRE(planet_);

	return planet_->createDynamicSky();
}

W4dStars* EnvIPlanetParser::pStars() const
{
	return pStars_;
}

void EnvIPlanetParser::createStars(RenStars::Configuration config, MATHEX_SCALAR radius, int nStars)
{
	PRE(planet_);
	pStars_ = planet_->createStars(config, radius, nStars);
	POST(pStars());
}

void EnvIPlanetParser::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const EnvIPlanetParser& t )
{
    o << "EnvIPlanetParser " << (void*)&t << " start" << std::endl;
    o << "EnvIPlanetParser " << (void*)&t << " end" << std::endl;

    return o;
}

void EnvIPlanetParser::clear()
{
	orbParams_ = NULL;
	satParams_ = NULL;
    pStars_ = NULL;

   	skyDeclaration().clear();

    //Clear out the maps
    if( cluts_.size() != 0 )
        cluts_.erase( cluts_.begin(), cluts_.end() );

    if( orbits_.size() != 0 )
        orbits_.erase( orbits_.begin(), orbits_.end() );

    if( satellites_.size() != 0 )
        satellites_.erase( satellites_.begin(), satellites_.end() );
}

void EnvIPlanetParser::startSkyline()
{
	contructingSkyline_ = true;
	POST(contructingSkyline_);
}

void EnvIPlanetParser::completeSkyline()
{
	PRE(skylineMesh_.length() > 0);
	EnvSkyline* skyline = planet_->createSkyline(skylineMesh_);

	if (skylineClut_ && skylineController_)
		skyline->colourTable(skylineClut_, skylineController_);

	skylineMesh_ = "";
	skylineClut_ = NULL;
	skylineController_ = NULL;

	contructingSkyline_ = false;
	POST(!contructingSkyline_);
}

void EnvIPlanetParser::skylineMeshFile(const string* m)
{
	PRE(m);
	PRE(contructingSkyline_);
	skylineMesh_ = *m;
}

void EnvIPlanetParser::skylineController(const string* s)
{
	PRE(s);
	PRE(contructingSkyline_);
	skylineController_ = lookUpSatellite(s);
	POST(skylineController_);
}

void EnvIPlanetParser::skylineColourTable(const string* c)
{
	PRE(c);
	PRE(contructingSkyline_);
	skylineClut_ = lookUpClut(c);
	POST(skylineClut_);
}

/* End PLAPARSE.CPP *************************************************/
