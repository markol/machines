/*
 * C D . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */


/*
    DevCD - this class was to handle a CD drive audio, now it loads and plays ogg music tracks
*/

#ifndef _DEV_CD_HPP
#define _DEV_CD_HPP

#include <cstdlib>

#include "base/base.hpp"

#include "device/cddefs.hpp"

#include <AL/al.h>

//////////////////////////////////////////////////////////////////////

class AfxWin95App;
class DevCDPlayList;
class DevCDImpl;

static void eosCallback(void*, ALuint);

class DevCD
// singleton
{
public:
        enum CDMessage
        {
                ABORT,
                FAIL,
                SUCCESS,
                SUPERSEDED,
                UNKNOWN
        };

        ///////////////////////////////

    static DevCD& instance();
    ~DevCD();

        ///////////////////////////////
        void initialise();
        void update();

        bool isPlayingAudioCd() const;
        // POST( implies( result, isAudioCDPresent() ) );

        //Does this PC support volume control
        bool supportsVolumeControl() const;
        Volume volume() const;

        //The track currently playing
        DevCDTrackIndex currentTrackIndex() const;
        // PRE( isPlayingAudioCd() );

        //Total number of CD tracks
        DevCDTrackIndex numberOfTracks() const;

        Seconds currentTrackLengthInSeconds() const;
        // PRE( isPlayingAudioCd() );

        // time the current track has played for
        Seconds currentTrackRunningTime() const;
        // PRE( isPlayingAudioCd() );

        // time the current track has left to play
        Seconds currentTrackTimeRemaining() const;
        // PRE( isPlayingAudioCd() );

        ///////////////////////////////

        // play from beginning to end
        void play();

        // play only the specified track, auto track repeat can be set
        void play( DevCDTrackIndex track, bool repeat = false );
        // PRE( track < nTracks() );

        // play from the specified track to the end
        void playFrom( DevCDTrackIndex track );

        // play in a requested fashion
        void play( const DevCDPlayList& params );

        // Play random track between specified start and end tracks. You can specify first track to be played.
        void randomPlay( DevCDTrackIndex startTrack, DevCDTrackIndex endTrack, DevCDTrackIndex firstTrack = -1 );

        void stopPlaying();

        ///////////////////////////////

        void volume( Volume newLevel );
        // POST( volume() == newLevel );

        ///////////////////////////////

        // Is an audio CD in the drive?
        bool isAudioCDPresent();

    void CLASS_INVARIANT
    {
                INVARIANT( this != NULL );
    }

        friend ostream& operator <<( ostream&, const DevCD& );

private:

        enum PlayStatus
        {
                PROGRAMMED,
                CONTINUOUS,
                SINGLE,
                REPEAT,
                NORMAL,
                STOPPED,
                RANDOM
        };

    DevCD( const DevCD& );
    DevCD& operator =( const DevCD& );
    bool operator ==( const DevCD& );
    void handleMessages( CDMessage, unsigned int);

    DevCD();


        unsigned int    deviceID_;

        PlayStatus              status_;

        DevCDImpl*              pImpl_;

        DevCDTrackIndex trackPlaying_;

        bool        needsUpdate_;

        ///////////////////////////////

        friend class AfxWin95App;
        friend void eosCallback(void*, ALuint);
};

#endif

/* End CD.HPP *******************************************************/
