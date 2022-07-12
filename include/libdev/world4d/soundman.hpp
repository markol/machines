/*
 * S O U N D M A N . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    W4dSoundManager

    Controls the playing of sounds linked to entities, with volume controlled by
    distance from a camera
*/

#ifndef _WORLD4D_SOUNDMAN_HPP
#define _WORLD4D_SOUNDMAN_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"
#include "mathex/mathex.hpp"
#include "sound/snd.hpp"

#include "stdlib/string.hpp"

//forward decls
class W4dEntity;
class MexPoint3d;
class W4dSoundManagerImpl;
class SysPathName;
class MexTransform3d;

//singleton
class W4dSoundManager
{
public:
    //  Singleton class
    static W4dSoundManager& instance();
    ~W4dSoundManager();

    //Preload the sound file with specified path
    void load( const SysPathName& path );

    //Clear all playing and loaded sounds from memory
    void unloadAll();

    //This will stop all playing sounds, and frees their resources.
	//It doesn't clear the internal list of samples use unloadAll()
	//for that
    void stopAll();

    //Register a sound linked to pEntity, which has specified path.
    //The volume when within 10 metres of pEntity will be maxPercentageVolume.
    //If LOOP_CONTINUOUSLY is specified, the sound will be stopped/started, as
    //and when near to the ear.
    //If PLAY_ONCE is specified, the sound will be played if audible.
    //When closer to ear than nearDistance, will be at full volume.
    enum Repetition
    {
    	LOOP_CONTINUOUSLY,
    	PLAY_ONCE
    };

	//Play the sound for the requested amount of time
    void playForDuration( const W4dEntity* pEntity, uint id, const PhysAbsoluteTime& startTime, MATHEX_SCALAR duration );

    void play( const W4dEntity* pEntity, uint id, const PhysAbsoluteTime& startTime, size_t repetition );
    //PRE( pEntity != NULL )
    //PRE( 0 <= maxPercentageVolume <= 100 )

    //Stop playing the sound related to pEntity
    void stop( const W4dEntity* pEntity );
    //PRE( pEntity != NULL )

    //Update the volumes of all the sounds as heard by an ear at global position earLocation
    //No need to call this every cycle, only as often as desired
    void updateVolumes( const MexPoint3d& earLocation );

    void updateVolumes( const MexTransform3d& earTransform );

    //To be called at least every core loop cycle - otherwise there is a danger of
    //overrunning DAM buffers!!
    void updateCycle();

	//Register a sound id with the matching enummed type in W4dSoundData
	void registerSoundID(const string& id, uint soundId);
	//Read in the sound definition file and create the W4dSoundData members
	void readSoundDefinitionFile(const SysPathName& definitionFileName);
	//PRE(definitionFileName.existsAsFile());

	//Set the heights between which the sound is to be scaled differently
	void setScaleFactor(MATHEX_SCALAR newScale);
	MATHEX_SCALAR scaleFactor() const;
	void scalingActivate(bool scalingOn);
	bool scalingActive() const;

	MATHEX_SCALAR maxScaleFactor() const;
    MATHEX_SCALAR minScaleFactor() const;
    void setMinScaleFactor(MATHEX_SCALAR newScale);
    void setMaxScaleFactor(MATHEX_SCALAR newScale);

	bool isPlayingSounds() const;
	void setPlayingSoundsStatus(bool playStatus);

	//Logically converts all sounds in the game to the specified dimensions
	void convertSoundDimensions(Snd::AudioType);

	//What is the current overall setting for world sounds
	Snd::AudioType currentSoundDimensions() const;
	void currentSoundDimensions(Snd::AudioType);

    void CLASS_INVARIANT;

private:
    // Operations deliberately revoked
    W4dSoundManager( const W4dSoundManager& );
    W4dSoundManager& operator =( const W4dSoundManager& );
    bool operator ==( const W4dSoundManager& );

    W4dSoundManager();

	W4dSoundManagerImpl* pImpl_;

};

#endif

/* End SOUNDMAN.HPP *************************************************/
