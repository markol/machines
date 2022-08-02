/***********************************************************

  W A V E F O R M . H P P

***********************************************************/

/*

  Used as an interface to a waveform on disk.

  At the moment the only files it will work on are
  .wav files. From these it can get the data size, format
  and read the data off.

  As you can see from looking at the implementation
  windows multimedia I/O is used so any further
  supported formats would need to use another
  I/O scheme.

  isWaveFile is provided as static so you do not
  need to incur any intialisation overhead.

*/

#ifndef WAVEFORM_HPP
	#define WAVEFORM_HPP

#include "base/base.hpp"
#include "system/pathname.hpp"
#include "sound/internal/wavefmt.hpp"
#include "sound/sndwavid.hpp"
#include <audio/wave.h>
#include <AL/al.h>


////////////////////////////////////////////////////////////

	class SndWaveform
	{
	public:
		SndWaveform(  const SndWaveformId& id );
		~SndWaveform();

		static bool isWaveFile( const char* path );

		WaveFormat* format();
		// returns size of PCM data in bytes
		uint dataSize();

		// reads all the wave data to buf and
		// returns the size of buf in bytes
		uint read( void* buf );
		// as above but copies from an offset into the
		// wave data specified by from of size len.
		uint read( void* buf, uint from, uint len );

		// return the pathname of the SndWaveform
		const SysPathName& pathname() const;

		const SndWaveformId& id() const;

		SndWaveform& addReference();
		SndWaveform& removeReference();

		size_t ref() const;

		ALenum toALformat();

	private:
		/** revoked operations **/
		SndWaveform();
		SndWaveform( const SndWaveform& );
		SndWaveform& operator =( const SndWaveform& );

		SndWaveformId	id_;
		WaveInfo*       waveInfo_;

		size_t 			ref_;
        uint dataSize_; //The cached size of the sound. 0 ==> not calculated yet
		WaveFormat*		pFormat_;
	};

////////////////////////////////////////////////////////////

#endif /* SndWaveform_HPP ************************************/
