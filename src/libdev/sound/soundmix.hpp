/*
 * M I X E R . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _SOUND_MIXER_HPP
#define _SOUND_MIXER_HPP

/*
 *  SndMixer.
 *
 *  The main sound class. Controls sound system initialision and the
 *  playing of samples.
 *
 *  A sample is active if it has been started and has not yet reached
 *  its finishing time. Every audible sample is active, but not every
 *  active sample is necessarily audible.
 *  Active samples will be assigned to available
 *  channels depending on their volume and priority. Any active sample
 *  with a volume of zero will not be assigned to a channel. Other
 *  samples will then be sorted into priority order and assigned highest
 *  priority first. Samples with equal priority will be further sorted
 *  on their volume level.
 *
 *  As samples start, stop, have their volume or priority changed etc.
 *  it is possible for active samples to change from being audible to
 *  being inaudible. Even when an active sample is inaudible it is
 *  still logically playing - when it next becomes audible it will
 *  become audible at the same point it would have reached had it been
 *  audible all along.
 */

#include "base/base.hpp"

#include "sound/snd.hpp"
#include "sound/smphandl.hpp"
#include "sound/smpid.hpp"

#include "ctl/list.hpp"
#include "ctl/vector.hpp"

#include "mathex/eulerang.hpp"
#include "mathex/point3d.hpp"

class SndWaveform;
class Sample;
class MexTransform3d;
class SndMixerImpl;
class SndMixerParameters;
class SndWaveformId;
class SndSampleParameters;
class IDirectSound;

class SndMixer
// Singleton.
{
public:

	static bool isInitialised();

    static void initialise( const SndMixerParameters& params );
    // PRE( not SndMixer::isInitialised() );
	// POST( isInitialised() );

    static SndMixer& instance();
    // PRE( isInitialised() );

	static Snd::RelativeTime SndSampleLength(const SndWaveformId& id);
	//PRE(SndWaveform::isWaveFile(id.pathname().c_str());

    ~SndMixer();

	///////////////////////////////
	//Return a pointer to the DirectSound object
	IDirectSound* pIDirectSound();

	bool isActive( const SndSampleHandle& handle ) const;
    // A sample is active iff it has been started and has not yet reached
    // its finishing time.

	bool isAudible( const SndSampleHandle& handle ) const;
	// POST( implies( result, isActive( handle ) );

	//Has this sample stopped playing
	bool isStopped( const SndSampleHandle& handle ) const;
	//PRE(isAllocated(pHandle));

	//Is this sample handle a valid allocated handle
	bool isAllocated( const SndSampleHandle& handle ) const;

	//When a sample is played via this method it does not start playing
	//automatically, it has a sample handle allocated to it,
	//and only starts playing after the next call to update()
	//NB Default resource management responsibility is
	//with the CLIENT
	SndSampleHandle playSample( const SndSampleParameters& p );
	// PRE(noOfFreeLogicalChannels() > 0);
	// POST(isAllocated(handle));

	//Query methods for the libraries responsibility
	bool isClientResponsible( const SndSampleHandle& handle ) const;
	//PRE(isAllocated(handle));
	bool isLibraryResponsible( const SndSampleHandle& handle ) const;
	//PRE(isAllocated(handle));

	//Set the responsibility for freeing sample resources
	void setResourcesResponsibilityLibrary( const SndSampleHandle& handle );
	//PRE(isAllocated(handle));
	//PRE(activeSamples_[handle]);
	//POST(isLibraryResponsible(handle));

	void setResourcesResponsibilityClient( const SndSampleHandle& handle );
	//PRE(isAllocated(handle));
	//PRE(activeSamples_[handle]);
	//POST(isClientResponsible(handle));

	//Stopping a sample only stops a sample playing,
	//it will not free any sample resources.
	//This is not a valid operation on a sample
	//with library resource responsibility
	void stopSample( const SndSampleHandle& handle );
	//PRE(isClientResponsible(handle));
    //PRE(isAllocated(handle));
    //POST(!isActive(handle));

	//Frees sample memory and its handle
	void freeSampleResources( const SndSampleHandle& handle );
    //PRE( isAllocated(handle) );
    //POST( !isAllocated(handle) );

	///////////////////////////////

	bool hasLoadedWaveform( const SndWaveformId& pathname ) const;

    //  Load a waveform into memory. If the waveform has already been loaded,
    // return a reference to the existing SndWaveform.
	const SndWaveform& loadWaveform( const SndWaveformId& id, bool threeD = false);
	// POST( hasLoadedWaveform( id ) );

    // Unload the waveform from memory. No attempt should be made to play
    // the waveform after it has been unloaded.
    void unloadWaveform( const SndWaveformId& id );
	// PRE( hasLoadedWaveform( id ) );

	//Unload all preloaded waveforms
    void unloadAll();

	///////////////////////////////

	void update();

	///////////////////////////////

	typedef ctl_list< SndSampleHandle > SampleHandles;

	// currently active samples
    //  This call has not currently been implemented. At such point as it is
    //  implemented it will need to be recorded as it might affect the running
    //  of the application. Bob 7 Oct 1998
    // const SampleHandles& activeSamples() const;

	///////////////////////////////

	typedef ctl_list< SndWaveform* > Waveforms;

	// waveforms currently resident in memory
    //  This call has not currently been implemented. At such point as it is
    //  implemented it will need to be recorded as it might affect the running
    //  of the application. Bob 7 Oct 1998
   	// const Waveforms& waveforms() const;

	///////////////////////////////

	typedef ctl_vector<Sample*> SampleList;

	///////////////////////////////

	typedef ctl_vector<SndSampleId> SampleIdVector;

	///////////////////////////////

	bool is3dSound( const SndSampleHandle& handle ) const;
	// PRE( isActive( handle ) );

	MexPoint3d samplePosition(const SndSampleHandle& handle);
	// PRE( isActive( handle ) );
	// PRE( is3dSound( handle ) );

	void samplePosition(const SndSampleHandle& handle, const MexPoint3d& newPos);
	// PRE( isActive( handle ) );
	// POST( samplePosition( handle ) == newPos );

	MexPoint3d listenerPosition() const;

	void listenerPosition(const MexPoint3d& newPos);
	// PRE(is3dMixer());
	// POST( listenerPosition() == newPos );

	MexEulerAngles listenerOrientation() const;

	void listenerOrientation(const MexEulerAngles& newOrientation);
	// PRE(is3dMixer());
	// POST( listenerOrientation() == newOrientation );

	MexTransform3d listenerTransform() const;

	void listenerTransform(const MexTransform3d& newTransform);

	Snd::Volume sampleVolume( const SndSampleHandle& handle ) const;
	// PRE( isActive( handle ) );

	void sampleVolume( const SndSampleHandle& handle, Snd::Volume newVolume);
	// PRE( isActive( handle ) );
	// POST( sampleVolume( handle ) == newVolume );

	bool is3dMixer() const;

	//The number of channels available for allocation
	ulong noOfFreeLogicalChannels() const;

	//The number of channels already allocated
	ulong noOfUsedLogicalChannels() const;

	Snd::Volume masterSampleVolume() const;
	void masterSampleVolume( Snd::Volume newVolume );
	// POST( masterSampleVolume() == newVolume );

	void shutdown();

	//Is the sound switched on?
	//This is accomplished by not setting CB_SOUND_OFF
	static bool soundOn();

	void CLASS_INVARIANT;

private:
    SndMixer( const SndMixer& );
    SndMixer& operator =( const SndMixer& );
    bool operator ==( const SndMixer& );
	SndMixer();

    //  The "NoRecord" versions of these functions will not call
    //  the RecRecorder code at all. This means they can be used
    //  in PREconditions etc.
    bool is3dSoundNoRecord( const SndSampleHandle& handle ) const;
    bool isActiveNoRecord( const SndSampleHandle& handle ) const;
    MexPoint3d samplePositionNoRecord( const SndSampleHandle& handle ) const;
	ulong noOfFreeLogicalChannelsNoRecord() const;
	Snd::Volume sampleVolumeNoRecord( const SndSampleHandle& handle ) const;
	void actualFreeSampleResources( const SndSampleHandle& handle );

	SndMixerImpl* pImpl_;
};

//////////////////////////////////////////////////////////////////////

#endif	// #ifndef _SND_MIXER_HPP

/* End MIXER.HPP ****************************************************/
