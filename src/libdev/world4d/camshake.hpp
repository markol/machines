/*
 * C A M S H A K E . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dCameraShake

    defines data for a cameraShake. These data will affect the final transform
	applied to the current camera in the W4dSceneManager.
*/

#ifndef _WORLD4D_CAMSHAKE_HPP
#define _WORLD4D_CAMSHAKE_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"
#include "mathex/mathex.hpp"
#include "mathex/point3d.hpp"

class MexTransform3d;

class W4dCameraShake
// Canonical form revoked
{
public:
    W4dCameraShake();
    ~W4dCameraShake();

	enum QuakeScale {FIRST_DEGREE, SECOND_DEGREE, THIRD_DEGREE, NDEGREES};

	void epicCentre( const MexPoint3d& centre );
	void scale( const QuakeScale& scale );
	void startTime( const PhysAbsoluteTime& startTime );
	void duration( const PhysRelativeTime& duration );

	const MexPoint3d& epicCentre() const;
	const QuakeScale& scale() const;
	const PhysAbsoluteTime& startTime() const;
	const PhysRelativeTime& duration() const;

	void offsetTransform( MexTransform3d* pTransform );
	bool shakeIsOver() const;

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const W4dCameraShake& t );

    W4dCameraShake( const W4dCameraShake& );
    W4dCameraShake& operator =( const W4dCameraShake& );

	const PhysMotionPlanPtr& shakePlanPtr1() const;
	const PhysMotionPlanPtr& shakePlanPtr2() const;
	const PhysMotionPlanPtr& shakePlanPtr3() const;

	MexPoint3d epicCentre_;
	QuakeScale scale_;
	PhysAbsoluteTime startTime_;
	PhysRelativeTime duration_;
};

#ifdef _INLINE
    #include "world4d/camshake.ipp"
#endif

#endif

/* End CAMSHAKE.HPP *************************************************/
