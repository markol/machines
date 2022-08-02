/***********************************************************

  S T A R S . H P P
  (c) Charybdis Limited, 1998. All Rights Reserved

***********************************************************/

#ifndef _WORLD4D_STARS_HPP
#define _WORLD4D_STARS_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "world4d/entity.hpp"

#include "mathex/mathex.hpp"

#include "render/colour.hpp"
#include "render/stars.hpp"

#include "utility/percent.hpp"

class MexRadians;

class W4dCamera;

// WARNING: Do not try to persist this class, you'll get burned! //

// Encapsulates the RenStars class for the w4d community. Used by
// the environment library so stars can be rendered far, far away.
class W4dStars : public W4dEntity
{
public:
	// Attach to parent and construct stars.
	W4dStars(
		W4dEntity* pParent,
		RenStars::Configuration,
		MATHEX_SCALAR radius,
		uint nStars);
	W4dStars(const W4dStars& copyMe, W4dEntity* pParent);

	virtual ~W4dStars();

	// Have to provide this.
    virtual bool intersectsLine(
        const MexLine3d& line,
        MATHEX_SCALAR* pDistance,
		Accuracy accuracy) const;

	RenStars::Configuration configuration() const;
	MATHEX_SCALAR radius() const;
	uint nStars() const;

	UtlPercentage opacity() const;
	void opacity(UtlPercentage newOpacity);

	// It needs to get to the render() method.
	friend class W4dCamera;

	void CLASS_INVARIANT;

	// DO NOT USE. Persitence is needed as W4dEntity uses it. The preRead and perWrite
	// are dummy functions that will assert out.
	PER_MEMBER_PERSISTENT_VIRTUAL(W4dStars);
	PER_FRIEND_READ_WRITE(W4dStars);

protected:
	// Overidden so the stars can be rendered as part of the environment tree.
    virtual RenderType render(const W4dCamera&,  W4dComplexity& );

private:
	// Operation deliberately revoked.
	W4dStars(const W4dStars&);

	// Used by the RenStars::render() call from within render().
	UtlPercentage opacity_;
	RenStars* pStars_;
};

PER_DECLARE_PERSISTENT(W4dStars);

#ifdef _INLINE
	#include "world4d/stars.ipp"
#endif

#endif /* _WORLD4D_STARS_HPP ******************************/

