
/*
 * W A V E M A N . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    SndWaveManager

    A brief description of the class should go in here
*/

#ifndef _SOUND_WAVEMAN_HPP
#define _SOUND_WAVEMAN_HPP

#include "base/base.hpp"

#include "sound/internal/waveform.hpp"

#include "ctl/map.hpp"
#include <AL/al.h>


class SndCountedSoundBuffer
{
public:
	SndCountedSoundBuffer( ALuint buffer )
		: 	buffer_( buffer ),
			count_( 1 )
			{};
	SndCountedSoundBuffer():
			buffer_( 0 ),
			count_( 0 )
			{};

    ALuint              buffer_;
	int					count_;
};

bool operator<( const SndCountedSoundBuffer& a, const SndCountedSoundBuffer& b );
bool operator==( const SndCountedSoundBuffer& a, const SndCountedSoundBuffer& b );

class SndWaveManager
// Canonical form revoked
{
public:
    typedef ctl_map< SndWaveformId, SndWaveform*, std::less<SndWaveformId> > WaveFormMap;
    typedef ctl_map< SndWaveformId, SndCountedSoundBuffer, std::less<SndWaveformId> > ALBufferMap;


    //  Singleton class
    static SndWaveManager& instance( void );
    ~SndWaveManager( void );
	void shutdown( );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const SndWaveManager& t );

	SndWaveform* getWaveForm( const SndWaveformId& id );
	ALuint getSoundBuffer( const SndWaveformId& id, bool& isFound );

	void freeWaveForm( const SndWaveformId& id );
    void freeSoundBuffer( const SndWaveformId& id);

	bool isLoaded( const SndWaveformId& id );

	void freeAll( void );

private:
    SndWaveManager( const SndWaveManager& );
    SndWaveManager& operator =( const SndWaveManager& );
    bool operator ==( const SndWaveManager& );

    SndWaveManager( void );

	WaveFormMap loadedWaveForms_;
	ALBufferMap loadedSoundBuffers_;

	friend class SndMixer;
};


#endif

/* End WAVEMAN.HPP **************************************************/
