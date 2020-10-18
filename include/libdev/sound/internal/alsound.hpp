/***********************************************************

 A L S O U N D . H P P

***********************************************************/

/*

  This is only needed due to the bad structure of
  DirectSound. Only ever used by DXSample and
  in start-up code, when initialise() should
  be called.

*/

#ifndef A_L_SOUND_HPP
	#define A_L_SOUND_HPP

#include "sound/snd.hpp"
#include "sound/sndparam.hpp"
#include "sound/soundmix.hpp"
#include <AL/alc.h>
#include <AL/al.h>

bool alTestError(const char* msg);

class MexVec3;
class MexPoint3d;
class MexEulerAngles;
class MexTransform3d;

////////////////////////////////////////////////////////////

	/** SINGLETON **/
	class ALSound
	{
	public:

		enum PriorityLevel {
			WRITE_PRIMARY,
			EXCLUSIVE,
			HIGH,
			NORMAL
		};

		enum SOUNDCARD {
			DIRECTSOUND,
			AUREAL,
			UNKNOWN
		};

		/** SINGLETON IMPLEMENTATION **/
		static void initialise( const SndMixerParameters& params, PriorityLevel p );
		static ALSound& instance( void );
		static void listDevices(const ALCchar*);

		void shutdown( );

		void compact( void );

		bool listenerPosition(const MexVec3& newval);
		MexPoint3d listenerPosition() const;

		void listenerOrientation(const MexEulerAngles& newOrientation);
		MexEulerAngles listenerOrientation();

		void listenerTransform(const MexTransform3d& newTransform);
		MexTransform3d listenerTransform();


		bool is3DSystem();

		void commitDeferredSettings();

	protected:
		IDirectSound* pIDirectSound( void ) const;
		ALSound( void );
		~ALSound( void );

	private:
		/** revoked methods **/
		ALSound( const ALSound& );
		ALSound& operator =( const ALSound& );

		static bool& isInitialised( void );

		static SOUNDCARD soundcard_;

        static ALCcontext *alContext_;
        static ALCdevice *alDevice_;

		bool is3DSystem_;
		SndMixer::SampleList preLoadedSamples_;

		friend class SndMixer;
		friend class SndWaveManager;
	};

////////////////////////////////////////////////////////////

#endif /* A_L_SOUND_HPP ***********************************/
