/***********************************************************

  S T A R S . C P P
  (c) Charybdis Limited, 1997. All Rights Reserved

***********************************************************/

#include "render/stars.hpp"
#include "render/internal/starsi.hpp"
#include "render/colour.hpp"

#include "mathex/transf3d.hpp"
#include "mathex/vec3.hpp"
#include "mathex/radians.hpp"
#include "mathex/eulerang.hpp"

#include "utility/percent.hpp"

RenStars::RenStars(
	Configuration config,
	MATHEX_SCALAR radius,
	uint nStars)

	:pImpl_(_NEW(RenIStarsImpl(config, radius, nStars)))
{
	TEST_INVARIANT;
}

// VIRTUAL //
RenStars::~RenStars()
{
	TEST_INVARIANT;

	_DELETE(pImpl_);
}

void RenStars::render(
    UtlPercentage opacity,
    const MexTransform3d& cameraXform,
    MexRadians verticalFOV,
    MexRadians horizontalFOV)
{
	pImpl_->render(
		opacity,
	    cameraXform,
		verticalFOV,
	    horizontalFOV);

	TEST_INVARIANT;
}

RenStars::Configuration RenStars::configuration() const
{
	TEST_INVARIANT;

	return pImpl_->configuration();
}

MATHEX_SCALAR RenStars::radius() const
{
	TEST_INVARIANT;

    return pImpl_->radius();
}

uint RenStars::nStars() const
{
	TEST_INVARIANT;

	return pImpl_->nStars();
}

void RenStars::CLASS_INVARIANT
{
	INVARIANT(this);
	INVARIANT(pImpl_);
}

/* End STARS.CPP ******************************************/

