/*
 * O R B I T . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#ifndef _ENVIRNMT_ORBIT_HPP
#define _ENVIRNMT_ORBIT_HPP

#include "base/base.hpp"
#include "utility/property.hpp"
#include "phys/phys.hpp"
#include "mathex/angle.hpp"
#include "mathex/vec3.hpp"
#include "stdlib/string.hpp"

class W4dEntity;
class W4dGeneric;

// Represents the orbit of a sun, moon or other satellite.  A client can
// arbitrarily request that the satellite rise or set at any time.  Otherwise
// it follows a continuous orbit -- tilted when risen, flat when set.  This
// class is responsible for creating the appropriate motion plans.
class EnvOrbit
{
public:
	// Changes the local transform of satelliteRep to match the given initial
	// position.  Velocity is radians-per-second.  The satellite rises to max
	// and falls again to min taking period() time to complete the whole cycle.
	// Initially, an orbit is created with the satellite at startTime thru its
	// period.  A start time of zero will give an initial elevation of min; a
	// start time of period/2 will give max elevation initially.  setHeading is
	// the heading at which the satellite will attain minimum elevation.
	// PRE(parent); POST(movingEntity());
    EnvOrbit(const string& name, MexRadians minEl, MexRadians maxEl,
			 MATHEX_SCALAR radius, MexRadians setHeading, PhysRelativeTime period,
			 PhysRelativeTime startTime, W4dEntity* parent);
    ~EnvOrbit();

	// The tilted orbit will only be tilted if the axis is off-vertical.
	// Otherwise it will degenerate to a flat orbit also (which is fine).
	void changeOrbit(MexRadians elevation, PhysRelativeTime within);
	void changeOrbit(MexRadians min, MexRadians max, PhysRelativeTime within);

	// This is what the motion is applied to.  The client should load a mesh
	// into this object or attach other entities to it.
	W4dGeneric* movingEntity() const;

	MexRadians minElevation() const;
	MexRadians maxElevation() const;

	// This is the current elevation of movingEntity() above z=0.
	MexRadians elevation() const;

	MexRadians velocity() const;
	PhysRelativeTime period() const;
	MATHEX_SCALAR radius() const;
	const string& name() const;

	// This is the angle of elevation subtended by the satellite's physical
	// representation.  It only affects the isSet method.  Defaults to zero.
	UtlProperty<MexRadians> size;

	// Returns true if the satellite's elevation is less than -size()/2.
	// If size is set correctly, then it should be completely below the
	// horizon if this method returns true.
	bool isSet() const;

	void CLASS_INVARIANT;
    friend ostream& operator <<( ostream& o, const EnvOrbit& t );

private:
	const MATHEX_SCALAR		radius_;
	const string	   		name_;
	const MexRadians		velocity_;		// 1/period_
	const PhysRelativeTime	period_;
	MexRadians				minElevation_, maxElevation_;

	W4dGeneric* const	satellite1_;	// NB: initialisation order is important!
	W4dGeneric*	const	satellite2_;	// NB: initialisation order is important!
	W4dGeneric*	const	satellite3_;	// NB: initialisation order is important!

	// This is the time at which the first minimum occurs.
	const PhysAbsoluteTime firstMinTime_;

    EnvOrbit( const EnvOrbit& );
    EnvOrbit& operator =( const EnvOrbit& );
    bool operator ==( const EnvOrbit& );
};

#endif

/* End ORBIT.HPP ****************************************************/
