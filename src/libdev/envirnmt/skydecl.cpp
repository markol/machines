/***********************************************************

  S K Y D E C L . H P P
  Copyright (c) 1998 Charybdis Limited, All rights reserved.

***********************************************************/

#include "envirnmt/internal/skydecl.hpp"

#include "stdlib/string.hpp"

#include "mathex/angle.hpp"

#include "ctl/vector.hpp"

#include "system/pathname.hpp"

#include "render/colour.hpp"

#include "envirnmt/internal/plaparse.hpp"
#include "envirnmt/sky.hpp"

class EnvSatellite;

void EnvISkyDeclaration::UniformSky::controller(const string& name)
{
	PRE(not controllerSet_);

	EnvSatellite* pController = EnvIPlanetParser::instance().lookUpSatellite(&name);
	ASSERT(pController, "An invalid satellite name was parsed.");

	pSky_->controller(pController);
	controllerSet_ = true;
}

void EnvISkyDeclaration::UniformSky::colourTable(const string& name)
{
	PRE(not colourTableSet_);

	EnvElevationColourTable* pElevationClut = EnvIPlanetParser::instance().lookUpClut(&name);
	ASSERT(pElevationClut, "An invalid elevation clut name was parsed.");

	pSky_->colourTable(pElevationClut);
	colourTableSet_ = true;
}

void EnvISkyDeclaration::UniformSky::completeSky()
{
	PRE(controllerSet_);
	PRE(colourTableSet_);

	EnvIPlanetParser::instance().skyDeclaration().completeSky(pSky_);
}

EnvISkyDeclaration::UniformSky::UniformSky(EnvUniformSky* const pSky)

	:controllerSet_(false),
	colourTableSet_(false),
	pSky_(pSky)
{
	PRE(pSky);
	PRE(not pSky_->controller());
}

////////////////////////////////////////////////////////////

void EnvISkyDeclaration::StaticSky::meshFile(const string& pathname)
{
	PRE(not meshFileSet_);

	pSky_->mesh(pathname);
	meshFileSet_ = true;
}

void EnvISkyDeclaration::StaticSky::backgroundColour(const RenColour& backgrdColour)
{
	PRE(not backgroundColourSet_);

	pSky_->backgroundColour(backgrdColour);
	backgroundColourSet_ = true;
}

void EnvISkyDeclaration::StaticSky::completeSky()
{
	PRE(meshFileSet_);
	PRE(backgroundColourSet_);

	EnvIPlanetParser::instance().skyDeclaration().completeSky(pSky_);
}

EnvISkyDeclaration::StaticSky::StaticSky(EnvStaticSky* const pSky)

	:meshFileSet_(false),
	backgroundColourSet_(false),
	pSky_(pSky)
{
	PRE(pSky);
}

////////////////////////////////////////////////////////////

void EnvISkyDeclaration::DynamicSky::meshFile(const string& pathname)
{
	PRE(not meshFileSet_);

	pSky_->mesh(pathname);
	meshFileSet_ = true;
}

void EnvISkyDeclaration::DynamicSky::controller(const string& name)
{
	PRE(not controllerSet_);

	EnvSatellite* pController = EnvIPlanetParser::instance().lookUpSatellite(&name);
	ASSERT(pController, "An invalid satellite name was parsed.");

	pSky_->controller(pController);
	controllerSet_ = true;
}

void EnvISkyDeclaration::DynamicSky::colourTable(MexDegrees at, const string& name)
{
	EnvElevationColourTable* pElevationClut = EnvIPlanetParser::instance().lookUpClut(&name);
	ASSERT(pElevationClut, "An invalid elevation clut name was parsed.");

	pSky_->colourTable(at, pElevationClut);
	atLeastOneColourTableSet_ = true;
}

void EnvISkyDeclaration::DynamicSky::completeSky()
{
	PRE(meshFileSet_);
	PRE(controllerSet_);
	PRE(atLeastOneColourTableSet_);

	EnvIPlanetParser::instance().skyDeclaration().completeSky(pSky_);
}

EnvISkyDeclaration::DynamicSky::DynamicSky(EnvDynamicSky* const pSky)

	:meshFileSet_(false),
	controllerSet_(false),
	atLeastOneColourTableSet_(false),
	pSky_(pSky)
{
	PRE(pSky);
	PRE(not pSky_->controller());
}

////////////////////////////////////////////////////////////

EnvISkyDeclaration::EnvISkyDeclaration()

	:pCompletedSky_(0),
	pUniformSky_(0),
	pStaticSky_(0),
	pDynamicSky_(0)
{
}

EnvISkyDeclaration::~EnvISkyDeclaration()
{
	TEST_INVARIANT;

	clear();
}

void EnvISkyDeclaration::createUniformSkyDeclaration(EnvUniformSky* const pSky)
{
	PRE(isClear());

	pUniformSky_ = _NEW(UniformSky(pSky));

	POST(isUniformSkyDeclaration());
}

void EnvISkyDeclaration::createStaticSkyDeclaration(EnvStaticSky* const pSky)
{
	PRE(isClear());

	pStaticSky_ = _NEW(StaticSky(pSky));

	POST(isStaticSkyDeclaration());
}

void EnvISkyDeclaration::createDynamicSkyDeclaration(EnvDynamicSky* const pSky)
{
	PRE(isClear());

	pDynamicSky_ = _NEW(DynamicSky(pSky));

	POST(isDynamicSkyDeclaration());
}

void EnvISkyDeclaration::clear()
{
	PRE(isClear() or isComplete());

	pCompletedSky_ = 0;

	if(pUniformSky_)
	{
		_DELETE(pUniformSky_);
		pUniformSky_ = 0;
	}

	if(pStaticSky_)
	{
		_DELETE(pStaticSky_);
		pStaticSky_ = 0;
	}

	if(pDynamicSky_)
	{
		_DELETE(pDynamicSky_);
		pDynamicSky_ = 0;
	}
}

const EnvISkyDeclaration::UniformSky& EnvISkyDeclaration::uniformSky() const
{
	PRE(isUniformSkyDeclaration());

	return *pUniformSky_;
}

EnvISkyDeclaration::UniformSky& EnvISkyDeclaration::uniformSky()
{
	PRE(isUniformSkyDeclaration());

	return *pUniformSky_;
}

const EnvISkyDeclaration::StaticSky& EnvISkyDeclaration::staticSky() const
{
	PRE(isStaticSkyDeclaration());

	return *pStaticSky_;
}

EnvISkyDeclaration::StaticSky& EnvISkyDeclaration::staticSky()
{
	PRE(isStaticSkyDeclaration());

	return *pStaticSky_;
}

const EnvISkyDeclaration::DynamicSky& EnvISkyDeclaration::dynamicSky() const
{
	PRE(isDynamicSkyDeclaration());

	return *pDynamicSky_;
}

EnvISkyDeclaration::DynamicSky& EnvISkyDeclaration::dynamicSky()
{
	PRE(isDynamicSkyDeclaration());

	return *pDynamicSky_;
}

bool EnvISkyDeclaration::isUniformSkyDeclaration() const
{
	return (pUniformSky_) ? true : false;
}

bool EnvISkyDeclaration::isStaticSkyDeclaration() const
{
	return (pStaticSky_) ? true : false;
}

bool EnvISkyDeclaration::isDynamicSkyDeclaration() const
{
	return (pDynamicSky_) ? true : false;
}

bool EnvISkyDeclaration::isClear() const
{
	return (pUniformSky_ or pStaticSky_ or pDynamicSky_ or pCompletedSky_) ? false : true;
}

bool EnvISkyDeclaration::isBuilding() const
{
	return not isComplete() and not isClear();
}

bool EnvISkyDeclaration::isComplete() const
{
	return (pCompletedSky_) ? true : false;
}

EnvSky*	EnvISkyDeclaration::completedSky() const
{
	PRE(isComplete());

	return pCompletedSky_;
}

void EnvISkyDeclaration::CLASS_INVARIANT
{
	INVARIANT(this);
}

void EnvISkyDeclaration::completeSky(EnvSky* const pSky)
{
	PRE(not pCompletedSky_);

	pCompletedSky_ = pSky;
}

/* End SkyDecl.hpp ****************************************/

