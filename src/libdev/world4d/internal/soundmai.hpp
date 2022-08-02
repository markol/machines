/*
 * S O U N D M A I . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dSoundManagerImpl

    A brief description of the class should go in here
*/

#ifndef _WORLD4D_SOUNDMAI_HPP
#define _WORLD4D_SOUNDMAI_HPP

#include "base/base.hpp"

#include "mathex/mathex.hpp"
#include "mathex/point3d.hpp"
#include "mathex/transf3d.hpp"

#include "phys/phys.hpp"

#include "sound/smphandl.hpp"
#include "sound/snd.hpp"

#include "ctl/list.hpp"
#include "ctl/map.hpp"
#include "ctl/pvector.hpp"

#include "system/pathname.hpp"

#include "world4d/soundman.hpp"

class W4dEntity;
class W4dSoundData;

class W4dSoundManagerImpl
{
public:
	W4dSoundManagerImpl();
	~W4dSoundManagerImpl();

    struct EntitySound
    {
        const W4dEntity* 	pEntity_; //The entity we're linked to
        double 				maxPercentageVolume_; //Percentage of volume when on top of it
        double 				currentPercentageVolume_; //Current value as at last update
		MATHEX_SCALAR 		duration_;
        MATHEX_SCALAR 		nearDistance_; //Distance to which volume maintains 100%
        MATHEX_SCALAR 		farDistance_; //Distance to which volume is silent
        MATHEX_SCALAR 		sqrDistance_; //Working value caching square of distance from current ear
                                    //during updateVolumes().
		SndSampleHandle		id_;
		SysPathName			pathname_;
        size_t	 			repetitions_; //Number of times to play
        PhysAbsoluteTime 	startTime_; //When to start it
        bool 				isPlaying_ :1; //true if actively being played
        bool 				isSelected_ :1; //used during updateVolumes to denote which ones selected for playing
		bool				idSet_;
		bool 				is3D_;
		MexPoint3d			lastPosition_;
		W4dSoundData* 		pData_;

		float				selectionValue_;
		int 				priority_;

        EntitySound( const W4dEntity* pEntity, double maxPercentageVolume,
                     MATHEX_SCALAR nearDistance,
                     MATHEX_SCALAR farDistance,
                     double currentPercentageVolume, const SysPathName& fileName,
                     W4dSoundManager::Repetition repetition, const PhysAbsoluteTime& startTime,
                     bool isPlaying,
                     MATHEX_SCALAR duration,
                     bool is3D,
                     W4dSoundData* pData);
        EntitySound();

		const SndSampleHandle& id() const;
		void setId(const SndSampleHandle& newId);

//		//Operation revoked
//		EntitySound(const EntitySound&);
//		//Operation revoked
//		EntitySound operator=(const EntitySound&);

        friend bool operator==( const EntitySound& lhs, const EntitySound& rhs );
        friend bool operator<( const EntitySound& lhs, const EntitySound& rhs );
    };

    typedef ctl_list< EntitySound > EntitySounds;
    typedef ctl_map< string, uint, std::less< string > > SoundIDMap;
	typedef ctl_pvector< W4dSoundData> SoundDataVector;

    double computeVolume( const EntitySound& entitySound );
	//Plays this entitySound
	//NB it sets its own ID
	void playEntitySound(EntitySound*);

    //data member
    MexPoint3d lastEarLocation_; //Last used ear location
    MexTransform3d lastEarTransform_; //Last used ear location
    bool hasPendingSound_; //True if a sound has been scheduled to start playing in the future
    PhysAbsoluteTime pendingStartTime_; //earliest time at which a new sound scheduled to start.
    EntitySounds sounds_; //All the registered sounds
	SoundIDMap registeredSounds_;//All the registered sound IDs
	bool definitionFileRead_; //Has the sound definition file been read?
	MATHEX_SCALAR scaleFactor_;	//Sound scaling value
	bool scalingOn_;
    SoundDataVector* pAvailableSounds_;//All the available W4dSoundData objects
    MATHEX_SCALAR maxScaleFactor_;
    MATHEX_SCALAR minScaleFactor_;
	bool playSounds_;
	bool using3DSounds_;
	Snd::AudioType currentAudioType_;
};

#endif

/* End SOUNDMAI.HPP *************************************************/
