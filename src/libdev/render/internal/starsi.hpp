/***********************************************************

  S T A R S I . H P P
  (c) Charybdis Limited, 1998. All Rights Reserved

***********************************************************/

/*
  Implementation class for RenStars. Uses the DirectX structs and calls for max speed.
*/

#ifndef _INTERNAL_RENDER_STARS_IMPL_HPP
#define _INTERNAL_RENDER_STARS_IMPL_HPP

#include "base/base.hpp"

#include "render/stars.hpp"
#include "render/colour.hpp"

#include "mathex/mathex.hpp"
#include "mathex/radians.hpp"
#include "render/internal/vtxdata.hpp"

#include "ctl/vector.hpp"

class UtlPercentage;

class MexTransform3d;

class RenColour;

class RenIStarsImpl
// Cannonical form revoked.
{
public:
	RenIStarsImpl(
		RenStars::Configuration,
		MATHEX_SCALAR radius,
		uint nStars);

	void render(
		UtlPercentage opacity,
		const MexTransform3d& cameraXform,
	    MexRadians verticalFOV,
	    MexRadians horizontalFOV);
	// PRE(horizontalFOV <= Mathex::PI_DIV_2 and verticalFOV <= maxVerticalFOV());

	RenStars::Configuration configuration() const;
	uint nStars() const;
	MATHEX_SCALAR radius() const;

	void CLASS_INVARIANT;

private:
	// Operations deliberatley revoked.
	RenIStarsImpl(const RenIStarsImpl& copyMe);
	RenIStarsImpl& operator =(const RenIStarsImpl& assignMe);

	// The height of the latitudinal line that marks the polar caps.
	MATHEX_SCALAR absCapHeight(MexRadians verticalFOV) const;

	enum RenderRegion
	{
		NORTH_CAP,
		SOUTH_CAP,
		TROPICS,
		NONE
	};

	RenderRegion renderRegion(MexRadians elevation, MexRadians verticalFOV) const;

	void renderNorthCap(
		MexRadians verticalFOV,
		ctl_vector<RenIVertex*>* const pVertexPtrs,
		ctl_vector<int>* const pSizes);

	void renderSouthCap(
		MexRadians verticalFOV,
		ctl_vector<RenIVertex*>* const pVertexPtrs,
		ctl_vector<int>* const pSizes);
	// PRE(configuration() == RenStars::SPHERICAL);

	void renderTropics(
		const MexTransform3d& cameraXform,
		MexRadians verticalFOV,
		MexRadians horizontalFOV,
		ctl_vector<RenIVertex*>* const pVertexPtrs,
		ctl_vector<int>* const pSizes);

	void cullSectors(
		const MexTransform3d& cameraXform,
		MATHEX_SCALAR halfAngularWidth,
	    ctl_vector<ctl_vector<RenIVertex>*>* const pVertexPtrs);

	RenStars::Configuration configuration_;
	uint nStars_;
	MATHEX_SCALAR radius_;
	RenColour colourFilter_;

	// The actual store of the vertices that represent stars. The points are sorted
	// by their azimuthal angle into sectors.
	ctl_vector< ctl_vector<RenIVertex> > sectors_;
};

#ifdef _INLINE
	#include "internal/starsi.ipp"
#endif

#endif /* _INTERNAL_RENDER_STARS_IMPL_HPP *****************/

