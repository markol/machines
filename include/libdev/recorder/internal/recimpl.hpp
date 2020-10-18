/*
 * R E C I M P L . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    RecRecorderImplementation

    A brief description of the class should go in here
*/

#ifndef _RECORDER_RECIMPL_HPP
#define _RECORDER_RECIMPL_HPP

#include "base/base.hpp"
#include "device/rate.hpp"

#include "recorder/recorder.hpp"

class RecRecorderImplementation
// Canonical form revoked
{
public:
    //  Singleton class
    static RecRecorderImplementation& instance( void );
    ~RecRecorderImplementation( void );

    enum RecordType
    {
        REPEAT,         //  Repeat the previous record
        TIME,
        MOUSE_POSITION,
        LEFT_BUTTON_TRUE,
        LEFT_BUTTON_FALSE,
        RIGHT_BUTTON_TRUE,
        RIGHT_BUTTON_FALSE,
        KEY_TRUE,
        KEY_FALSE,
        KEY_CODE_TRUE,
        KEY_CODE_FALSE,
        ANY_KEY_TRUE,
        ANY_KEY_FALSE,
        SHIFT_TRUE,
        SHIFT_FALSE,
        CTRL_TRUE,
        CTRL_FALSE,
        ALT_TRUE,
        ALT_FALSE,
        DELTA_KEY,
        DELTA_KEY_CODE,
        RANDOM_SEED,
        BUTTON_EVENT,
        EVENT_QUEUE_LENGTH,

        SOUND_ACTIVE_TRUE,
        SOUND_ACTIVE_FALSE,
        SOUND_AUDIBLE_TRUE,
        SOUND_AUDIBLE_FALSE,
        SOUND_3D_TRUE,
        SOUND_3D_FALSE,
        SOUND_SAMPLE_POSITION,
        SOUND_SAMPLE_VOLUME,
        SOUND_FREE_LOGICAL_CHANNELS,
        SOUND_USED_LOGICAL_CHANNELS,

        ANI_SMACKER_FINISHED_TRUE,
        ANI_SMACKER_FINISHED_FALSE,

        REGISTRY_RETURN_VALUE,
        REGISTRY_KEY,
        REGISTRY_BUFFER_SIZE,
        REGISTRY_BUFFER,
        REGISTRY_STRING_SIZE,
        REGISTRY_STRING,
        REGISTRY_INTEGER,

        NETWORK_INTEGER,
        NETWORK_UNSIGNED_INTEGER,
        NETWORK_DATA,
        NETWORK_BOOL,
        NETWORK_STRING_SIZE,
        NETWORK_STRING,
        NETWORK_DOUBLE,
    };

    void putData( RecordType type, size_t nBytes, const void* pData );
    void putBool( bool value, RecordType trueType, RecordType falseType );

    void getData( RecordType type, size_t nBytes, void* pData );
    bool getBool( RecordType trueType, RecordType falseType );

    RecRecorder::State state() const;

    void recordingAllowed( bool );

	double percentageComplete() const;
	//	PRE( state() == RecRecorder::PLAYING );
	//	POST( result >= 0.0 );
	//	POST( result <= 100.0 );

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const RecRecorderImplementation& t );

    RecRecorderImplementation( const RecRecorderImplementation& );
    RecRecorderImplementation& operator =( const RecRecorderImplementation& );

    RecRecorderImplementation( void );

    static  void closeStreams();
    void checkOKToRecord();

    bool recordingAllowed() const;

    void writeDriveInfo( ostream& o ) const;

    //  This is a crude, 1/18 sec. resolution timer
    double time() const;
    void writeTime( ostream& o, double timeSeconds ) const;

    RecRecorder::State  state_;

    std::ofstream    recordStream_;
    std::ifstream    playbackStream_;

    size_t      nBytesRead_;
    size_t      nBytesInFile_;
    size_t      nBytesWritten_;

    bool        fatalErrorFound_;

    int         allowedCount_;

    DevRate     freeDiskSpaceRate_;
    DevRate     fileRate_;

    char        drive_[ 4 ];

    double      updateDiskSpaceTime_;
    double      diskSpaceFreeBytes_;
    double      diskSpaceFreeSeconds_;

};

ostream& operator <<( ostream&, RecRecorderImplementation::RecordType );

#endif

/* End RECIMPL.HPP **************************************************/
