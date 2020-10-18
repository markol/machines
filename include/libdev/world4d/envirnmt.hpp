/*
 * E N V I R N M T . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#ifndef _WORLD4D_ENVIRNMT_HPP
#define _WORLD4D_ENVIRNMT_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"
#include "ctl/vector.hpp"
#include "ctl/pvector.hpp"
#include "stdlib/string.hpp"

class W4dRoot;
class W4dLight;
class RenColour;

// An environment is a high-level representation of various rendering
// paramters which are often inter-related and which often change in concert:
// global lights, background, fog, etc.  This base class defines an abstract
// interface which allows W4dSceneManager to use various environments defined
// by higher-level libraries.  Anticipated subclasses are: the various rooms
// in a Doom-style maze; the outside environment of a planet surface.
class W4dEnvironment
{
public:
	enum PhaseOfDay
	{
		DAY,
		NIGHT,
		TRANSITION
	};

	// The scene manager calls this once per frame so that the environment can
	// make necessary adjustments to lights, colours, fog, etc.
	virtual void update() = 0;

	// Environments can be unused or currently in use.	A scene manager will
	// typically have one enabled environment with which the scene is rendered.
	// The scene manager calls enable once to switch on an environment; it
	// calls disable to switch it off.
	virtual void enable() = 0;
	virtual void disable() = 0;
	virtual bool enabled() const = 0;

	// Fog needs to be disabled for certain views.
	virtual void fogOn(bool) = 0;
	virtual bool fogOn() const = 0;

	// This unfortunately is a bit Machines specific, the fog should be
	// different in the zenith view.
	virtual void fogZenith(bool) = 0;
	virtual bool fogZenith() const = 0;
/*
	// Are there any background objects?  Should the screen be cleared?
	virtual W4dRoot* backGroundRoot() const = 0;
	virtual bool clearBackGround() const = 0;
	virtual const RenColour& backGroundColour() const = 0;

	// An environment contains a list of light sources which globally illuminate
	// the scene.  For performance, this is typically one directional light.
    typedef ctl_pvector<W4dLight> Lights;
	virtual const Lights& globalLights() const = 0;

	// An environment exports a set of named lighting configurations which can
	// be selected by clients (game AI).  For example, a planet environment may
	// provide day and night states; a room might provide lights-on and lights-
	// off, allowing the game to spring a trap when the player enters.
	virtual const ctl_vector<string>& lightConfigurations() const;
	virtual bool isValidConfiguration(const string&) const;

	// Ask the environment to adopt a new configuration within a given time.
	// PRE(isValidConfiguration(c));
	virtual void lightConfiguration(const string& c, PhysRelativeTime) = 0;
*/

	// It is not possible for a client to determine what configuration the
	// environment is in.  (It may not be in *any* named configuration -- they
	// are not necessarily complete.)  Instead, a client can enquire, in
	// general, how light is it?  This is the answer, range.  Pitch black is
	// 0, bright sunlight is 1.  This value accounts for global and ambient
	// lighting only.  Any local point lights are not included.
	virtual MATHEX_SCALAR averageIllumination() const = 0;		// POST(result >= 0);

	// Specify the range of averageIllumination() values that bound the transition day phase.
	// It defaults to [0.25, 0.35].
	void darknessRange(MATHEX_SCALAR lower, MATHEX_SCALAR upper);
	// PRE(lower <= upper);
	// PRE(inIntervalOO(lower, 0.0, 1.0));
	// PRE(inIntervalOO(upper, 0.0, 1.0));

	MATHEX_SCALAR lowerTransitionBound() const;
	MATHEX_SCALAR upperTransitionBound() const;

	// This returns a value in the range [0,1] representing day and night respectivly.
	// The in between values are for giving a value to transition. Based on the
	// averageIllumination implementation and the range passed to darknessRange().
	MATHEX_SCALAR darkness() const;

	// Uses darkness() and therefore based on averageIllumination().
	PhaseOfDay phaseOfDay() const;

	virtual void nvgOn(bool yesNo) = 0;

	virtual bool isNvgOn() const = 0;

	virtual MATHEX_SCALAR fogVisibility() const = 0;

    virtual ~W4dEnvironment();

    void CLASS_INVARIANT;
    friend ostream& operator <<( ostream& o, const W4dEnvironment& t );

protected:
    W4dEnvironment();

	// The base class maintains a list of configuration names.  Derived
	// classes are responsible for creating and implementing the
	// corresponding lighting states.
	void addLightConfiguration(const string&);	 	// PRE(!isValidConfiguration(c));
	void removeLightConfiguration(const string&);	// PRE(isValidConfiguration(c));

private:
	ctl_vector<string>	lightConfigurations_;
	MATHEX_SCALAR lowerTransitionBound_;
	MATHEX_SCALAR upperTransitionBound_;

    W4dEnvironment( const W4dEnvironment& );
    W4dEnvironment& operator =( const W4dEnvironment& );
    bool operator ==( const W4dEnvironment& );
};

#endif

/* End ENVIRNMT.HPP *************************************************/
