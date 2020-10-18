/***********************************************************

  S T A R S . H P P
  (c) Charybdis Limited, 1998. All Rights Reserved

***********************************************************/

/*
  This class is a highly optimised creater of points. These points can be rendered
  in one of two configurations (see class declaration).

  They are given random alpha values and coloured white (so they look like stars).
*/

#ifndef _RENDER_STARS_HPP
#define _RENDER_STARS_HPP

#include "base/base.hpp"

#include "mathex/mathex.hpp"

class MexTransform3d;
class MexRadians;

class UtlPercentage;

class RenColour;
class RenIStarsImpl;

class RenStars
// Cannonical form revoked.
{
public:
	enum Configuration
	{
		SPHERICAL,
		HEMISPHERICAL
	};

	RenStars(Configuration, MATHEX_SCALAR radius, uint nStars);
	virtual ~RenStars( void );

	void render(
	    UtlPercentage opacity,
	    const MexTransform3d& cameraXform,
	    MexRadians verticalFOV,
	    MexRadians horizontalFOV);

	Configuration configuration() const;	
	MATHEX_SCALAR radius() const;
	uint nStars() const;

	void CLASS_INVARIANT;

private:
	// Operations deliberately revoked.
	RenStars(const RenStars&);
	RenStars& operator =(const RenStars&);

	RenIStarsImpl* pImpl_;
};

#endif /* _RENDER_STARS_HPP *******************************/

