/*
 * S M P P A R A M . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _SOUND_SMPPARAM_HPP
#define _SOUND_SMPPARAM_HPP

//////////////////////////////////////////////////////////////////////

/*
 * SndSampleParameters
 * This class contains the data needed to start a sample playing. 
 * It allows the user to specify the source of the sample and information 
 * such as volume, number of iterations etc.
 */

#include "sound/snd.hpp"
#include "mathex/mathex.hpp"

class SndSampleParametersImpl;
class SndWaveformId;
class MexPoint3d;

class SndSampleParameters
// memberwise cannonical
{

public:
	
	// construct a 2d sound
	SndSampleParameters( const SndWaveformId& id,
							Snd::LoopCount loopcount = 1,
							Snd::Volume volume = Snd::defaultSampleVolume(),
							Snd::SamplePriority priority = Snd::defaultSamplePriority(),
							Snd::RelativeTime startOffset = 0.0
							);
	// POST( not is3dSound() );

	// construct a 2d sound
	SndSampleParameters( const SndWaveformId& id,
							MATHEX_SCALAR time,
							Snd::Volume volume = Snd::defaultSampleVolume(),
							Snd::SamplePriority priority = Snd::defaultSamplePriority(),
							Snd::RelativeTime startOffset = 0.0
							);

	// construct a 3d sound
	SndSampleParameters( const SndWaveformId& id,
					const MexPoint3d& initialPosition,
					Snd::LoopCount loopcount = 1,
					Snd::Distance maxAudibleDistance = Snd::defaultMaxDistance(),
					Snd::Distance minAudibleDistance = Snd::defaultMinDistance(),
					Snd::Volume volume = Snd::defaultSampleVolume(),
					Snd::SamplePriority priority = Snd::defaultSamplePriority(),
					Snd::RelativeTime startOffset = 0.0);
	// POST( is3dSound() );

	SndSampleParameters( const SndWaveformId& id,
					const MexPoint3d& initialPosition,
					MATHEX_SCALAR time,
					Snd::Distance maxAudibleDistance = Snd::defaultMaxDistance(),
					Snd::Distance minAudibleDistance = Snd::defaultMinDistance(),
					Snd::Volume volume = Snd::defaultSampleVolume(),
					Snd::SamplePriority priority = Snd::defaultSamplePriority(),
					Snd::RelativeTime startOffset = 0.0);
	// POST( is3dSound() );

	SndSampleParameters( const SndSampleParameters& copyMe);

	~SndSampleParameters();
	///////////////////////////////
    
	Snd::Volume				volume() const;
	Snd::SamplePriority		priority() const;
	Snd::LoopCount			loopCount() const;
	bool 					is3dSound() const;
	const MexPoint3d&		initialPosition() const;
	Snd::RelativeTime 		startOffset() const;
	const SndWaveformId&	id() const;
	// PRE( is3dSound() );
	Snd::Distance 		maxAudibleDistance() const;
	// PRE( is3dSound() );
	Snd::Distance 		minAudibleDistance() const;
	// PRE( is3dSound() );

	//How long will the sound be playing for.
	//This will return 0.0 if that paramater
	//has not been specified.
	MATHEX_SCALAR			duration() const;

	void					setHWMixing(bool hwMixingOn);
	bool					isHWMixing(void) const;

	//The preloading should be set to true when preloading.
	//The default for this is false.
	//Setting this to true will cause DirectX to preload the
	//sound to a software buffer, rather than a hardware buffer
	//which is consequently never used.
	void					setPreload(bool preloadingOn);
	bool					isPreloading(void) const;

	///////////////////////////////
	
	void CLASS_INVARIANT;

    SndSampleParameters& operator =( const SndSampleParameters& );

	friend ostream& operator <<( ostream& o, const SndSampleParameters& t );

	///////////////////////////////
private:

	SndSampleParametersImpl* pImpl_;
};

//////////////////////////////////////////////////////////////////////

#endif // #ifndef _SOUND_SMPPARAM_HPP

/* End SMPPARAM.HPP *************************************************/

