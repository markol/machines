/***********************************************************

 S A M P L E . H P P

***********************************************************/

/*

  Sample

*/

#ifndef _SAMPLE_HPP
	#define _SAMPLE_HPP

#include "sound/internal/wavefmt.hpp"
#include "sound/internal/waveform.hpp"

#include "sound/smpparam.hpp"

#include "mathex/vec3.hpp"

class DevTimer;

////////////////////////////////////////////////////////////

class Sample
{
	public:

		enum SampleState {
			INITIALISED = 0,
			PLAYING = 1,
			STOPPED = 2,
			PAUSED = 3
		};

		enum SampleStorage {
			WAVE_STATIC,
			WAVE_STREAMING
		};

		enum Participant
		{
			CLIENT,
			LIBRARY
		};

		Sample( const SndSampleParameters& params );

		virtual ~Sample();

		virtual void 			play() = 0;
		virtual void 			stop() = 0;
		virtual void 			stopAtEnd() = 0;
		virtual bool 			isPlaying() = 0;
		virtual Snd::RelativeTime	length() const = 0;
		virtual SampleState 	update() = 0;
		virtual void 			volume( int newVolume ) = 0;
		virtual void 			pause() = 0;
		virtual void			restart(void) = 0;
		virtual void 			restart( int offset) = 0;
		virtual void 			newPosition( const MexVec3& newPos ) = 0;
		virtual void			position3D( MexVec3* position ) = 0;

		//Set and get methods for mainting resource responibility
		void setResourceResponsibility(Participant owner);
		Participant getResourceResponsibility();

		const char*			path() const;
		int 				volume() const;
		bool				isPaused() const;
		bool				isSilenced() const;
		bool				isAudible() const;
		bool				is3D() const;
    	Snd::SamplePriority priority() const;
		size_t 				id() const;
		bool				loopForever() const;
		SampleState 		state() const;
		bool				stopPending() const;
		void				stopPending(bool);

		virtual void		silence();
		virtual void		unsilence();


		bool operator ==(const Sample& rhs);
		bool operator <(const Sample& rhs);
		bool operator >(const Sample& rhs);

	protected:
		static size_t 		nextID();

		size_t					id_;
		bool					isPaused_;
		bool					isSilenced_;
		size_t					loopCount_;
		SampleState				state_;
		SampleStorage			storageType_;
		Snd::Volume				volume_;
		Snd::Volume				unSilencedVolume_;
		Snd::SamplePriority		samplePriority_;
		bool					loopForever_;
		//Who is responsible for this samples resources
		Participant 			resourceResponsibility_;

		DevTimer*				internalTimer_;

		const SndSampleParameters		sampleParameters_;

		bool					stopPending_;

		//Operations revoked
		Sample( const Sample& );
		Sample& operator =( const Sample& );
};


#ifdef _INLINE
	#include "sample.ipp"
#endif

////////////////////////////////////////////////////////////
#endif /* _SAMPLE_HPP **********************************/
