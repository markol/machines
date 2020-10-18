/*
 * S A T P A R A M . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include "envirnmt/internal/satparam.hpp"
#include "envirnmt/satelite.hpp"
#include "envirnmt/internal/plaparse.hpp"
#include "system/pathname.hpp"

EnvISatelliteParams::EnvISatelliteParams(const string* n):
	name_(*n),
	orbit_(NULL),
	meshClut_(NULL),
	dirClut_(NULL),
	ambClut_(NULL)
{
	PRE(n);
    TEST_INVARIANT;
}

EnvISatelliteParams::~EnvISatelliteParams()
{
    TEST_INVARIANT;
}

void EnvISatelliteParams::orbit(const string* name)
{
	PRE(name);
	orbit_ = EnvIPlanetParser::instance().lookUpOrbit(name);

	ASSERT_INFO(*name);
	ASSERT(orbit_, "Orbit not found");
}

void EnvISatelliteParams::dirLightClut(const string* name)
{
	PRE(name);
	dirClut_ = EnvIPlanetParser::instance().lookUpClut(name);

	ASSERT_INFO(*name);
	ASSERT(dirClut_, "Directional light colour table not found");
}

void EnvISatelliteParams::ambLightClut(const string* name)
{
	PRE(name);
	ambClut_ = EnvIPlanetParser::instance().lookUpClut(name);

	ASSERT_INFO(*name);
	ASSERT(ambClut_, "Ambient light colour table not found");
}

void EnvISatelliteParams::meshColourClut(const string* name)
{
	PRE(name);
	meshClut_ = EnvIPlanetParser::instance().lookUpClut(name);

	ASSERT_INFO(*name);
	ASSERT(meshClut_, "Mesh colour table not found");
}

void EnvISatelliteParams::mesh(const string* name)
{
	PRE(name);
	meshName_ = *name;
}

EnvSatellite* EnvISatelliteParams::createSatellite(W4dSceneManager*)
{
    EnvSatellite* result = _NEW(EnvSatellite(name_, orbit_));

	if (meshName_.length() > 0)
		result->loadMesh(meshName_, meshClut_);

	if (dirClut_)
		result->setDirectionalLight(dirClut_);
	if (ambClut_)
		result->setAmbientLight(ambClut_);

	return result;
}

void EnvISatelliteParams::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

/* End SATPARAM.CPP *************************************************/
