/*
 * R E C I M P L . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <string>
#include <cstring>
#include <ctime>

#include "recorder/internal/recimpl.hpp"
#include "system/pathname.hpp"

#include "profiler/profiler.hpp"

// static
RecRecorderImplementation& RecRecorderImplementation::instance()
{
    static RecRecorderImplementation instance_;
    return instance_;
}

RecRecorderImplementation::RecRecorderImplementation()
: nBytesRead_( 0 ),
  nBytesWritten_( 0 ),
  state_( RecRecorder::INACTIVE ),
  fatalErrorFound_( false ),
  allowedCount_( 0 ),
  freeDiskSpaceRate_( 3.0 ),
  fileRate_( 3.0 ),
  updateDiskSpaceTime_( -10.0 ),
  diskSpaceFreeBytes_( 0 ),
  diskSpaceFreeSeconds_( 0 )
{
    const char* filename = getenv( "cb_playback_from" );

    if( filename )
    {
        SysPathName pathname( filename );

        if( pathname.existsAsFile() )
        {
            playbackStream_.open( filename, std::ios::in | std::ios::binary );

            //  This assumes that the seek value returned from the end of
            //  a binary file is the number of characters in the file. This
            //  is true for a Win95 binary file, it is not necessarily true
            //  on other operating systems.

            playbackStream_.seekg( 0, std::ios::end );
            nBytesInFile_ = playbackStream_.tellg();
            playbackStream_.seekg( 0, std::ios::beg );

            state_ = RecRecorder::PLAYING;
        }
    }
    else
    {
        filename = getenv( "cb_record_to" );

        if( filename )
        {
            recordStream_.open( filename, std::ios::out | std::ios::binary );
            state_ = RecRecorder::RECORDING;
        }
    }
//TODO windows dir
    if( filename )
    {
        if( strlen( filename ) >= 2 and filename[ 1 ] == ':' )
            drive_[ 0 ] = filename[ 0 ];
        //else
            //drive_[ 0 ] = _STATIC_CAST( char, 'a' + _getdrive() - 1 );

        drive_[ 1 ] = ':';
        drive_[ 2 ] = '\\';
        drive_[ 3 ] = '\0';
    }

    //  Make sure that the streams are properly closed on shutdown

    Diag::instance().addCrashFunction( closeStreams );

    TEST_INVARIANT;
}

RecRecorderImplementation::~RecRecorderImplementation()
{
    TEST_INVARIANT;

}

RecRecorder::State RecRecorderImplementation::state() const
{
    return state_;
}

void RecRecorderImplementation::putData( RecordType type, size_t nBytes, const void* pData )
{
    PRE( type < 256 );
    PRE( state() == RecRecorder::RECORDING );

    checkOKToRecord();

    const char   header = type;

    recordStream_.write( &header, sizeof( header ) );
    recordStream_.write( _STATIC_CAST( const char*, pData ), nBytes );

    nBytesWritten_ += sizeof( header ) + nBytes;

    if( REC_RECORD_STREAM_ENABLED )
    {
        REC_RECORD_STREAM( type << std::endl );
        ProProfiler::instance().traceStack( Diag::instance().recRecordStream(), true, 0, "" );
    }
}

void RecRecorderImplementation::putBool( bool value, RecordType trueType, RecordType falseType )
{
    PRE( state() == RecRecorder::RECORDING );

    checkOKToRecord();

    if( value )
        putData( trueType, 0, NULL );
    else
        putData( falseType, 0, NULL );
}

void RecRecorderImplementation::getData( RecordType type, size_t nBytes, void* pData )
{
    PRE( state() == RecRecorder::PLAYING );

    checkOKToRecord();

    char   header;

    playbackStream_.read( &header, sizeof( header ) );

    RecordType readType = _STATIC_CAST( RecordType, header );

    if( REC_PLAYBACK_STREAM_ENABLED )
    {
        REC_PLAYBACK_STREAM( readType << std::endl );
        ProProfiler::instance().traceStack( Diag::instance().recPlaybackStream(), true, 0, "" );

        if( type != readType )
        {
            REC_PLAYBACK_STREAM( "**************** ERROR *****************" << std::endl );
            REC_PLAYBACK_STREAM( "Expected to read " << type << " actually read " << readType << std::endl );
        }
    }

    ASSERT_INFO( type );
    ASSERT_INFO( readType );

    ASSERT( type == readType, "" );

    playbackStream_.read( _STATIC_CAST( char*, pData ), nBytes );

    nBytesRead_ += sizeof( header ) + nBytes;

    if( playbackStream_.eof() or playbackStream_.bad() )
        state_ = RecRecorder::INACTIVE;

}

bool RecRecorderImplementation::getBool( RecordType trueType, RecordType falseType )
{
    PRE( state() == RecRecorder::PLAYING );

    checkOKToRecord();

    //uint8   header;
    char   header;

    playbackStream_.read( &header, sizeof( header ) );

    RecordType readType = _STATIC_CAST( RecordType, header );

    if( REC_PLAYBACK_STREAM_ENABLED )
    {
        REC_PLAYBACK_STREAM( readType << std::endl );
        ProProfiler::instance().traceStack( Diag::instance().recPlaybackStream(), true, 0, "" );

        if( readType != trueType and readType != falseType )
        {
            REC_PLAYBACK_STREAM( "**************** ERROR *****************" << std::endl );
            REC_PLAYBACK_STREAM( "Expected to read " << trueType << " or " << falseType << " actually read " << readType << std::endl );
        }
    }

    ASSERT_INFO( readType );
    ASSERT_INFO( trueType );
    ASSERT_INFO( falseType );

    ASSERT( readType == trueType or readType == falseType, "" );

    bool result;

    if( readType == trueType )
        result = true;
    else
        result = false;

    nBytesRead_ += sizeof( header );

    if( playbackStream_.eof() or playbackStream_.bad() )
        state_ = RecRecorder::INACTIVE;

    return result;
}

// static
void RecRecorderImplementation::closeStreams()
{
    RecRecorderImplementation::instance().playbackStream_.close();
    RecRecorderImplementation::instance().recordStream_.close();
}

//  To allow recorded files to be shared between debug and release
//  versions of the application we must not do any recording whilst
//  we are in any debug specific code - this is mainly the macros
//  associated with assertions.
void RecRecorderImplementation::checkOKToRecord()
{
#ifndef NDEBUG
    bool inAssertion = BaseAssertion::inAssertion();
    bool inAssertionInfo = BaseAssertion::inAssertionInfo();
    bool inAssertionData = BaseAssertion::inAssertionData();

    std::string errorText;
    if( inAssertion )
        errorText = "in assertion";
    if( inAssertionInfo )
        errorText = "in assertion info";
    if( inAssertionData )
        errorText = "in assertion data";

    if( inAssertion or inAssertionInfo or inAssertionData )
    {
        REC_RECORD_STREAM( "***************** ERROR *****************" << std::endl );
        REC_RECORD_STREAM( "Trying to record whilst " << errorText << std::endl );
        ProProfiler::instance().traceStack( Diag::instance().recRecordStream(), true, 0, "" );

        REC_PLAYBACK_STREAM( "***************** ERROR *****************" << std::endl );
        REC_PLAYBACK_STREAM( "Trying to record whilst " << errorText << std::endl );
        ProProfiler::instance().traceStack( Diag::instance().recPlaybackStream(), true, 0, "" );

        ASSERT_INFO( "***************** ERROR *****************" << std::endl );
        ASSERT_INFO( "Trying to record whilst " << errorText << std::endl );

        fatalErrorFound_ = true;
    }

    if( not recordingAllowed() )
        fatalErrorFound_ = true;

    ASSERT_INFO( recordingAllowed() );

    //  The use of fatalErrorFound_ is because we might call this function
    //  whilst in a PRE, POST or ASSERT in which case any further assertion
    //  checks are disabled. We therefore save the fact that there has been
    //  an error so we can assert out as soon as possible.
    ASSERT( not fatalErrorFound_, "Cannot record whilst doing debug code or when in Windows event loop. The stack trace produced in assert.log may be innaccurate - see record or playback streams for accurate stack" );
#endif
}

void RecRecorderImplementation::recordingAllowed( bool allowed )
{
    if( allowed )
        --allowedCount_;
    else
        ++allowedCount_;
}

bool RecRecorderImplementation::recordingAllowed() const
{
    return allowedCount_ == 0;
}

void RecRecorderImplementation::writeDriveInfo( ostream& o ) const
{
    if( time() > updateDiskSpaceTime_ )
    {
        /*TODO DWORD  sectorsPerCluster;
        DWORD  bytesPerSector;
        DWORD  freeClusters;
        DWORD  totalClusters;

        GetDiskFreeSpace( drive_, &sectorsPerCluster, &bytesPerSector, &freeClusters, &totalClusters );

        RecRecorderImplementation* nonConstThis = _CONST_CAST( RecRecorderImplementation*, this );

        nonConstThis->diskSpaceFreeBytes_ = freeClusters;
        nonConstThis->diskSpaceFreeBytes_ *= sectorsPerCluster;
        nonConstThis->diskSpaceFreeBytes_ *= bytesPerSector;

        nonConstThis->freeDiskSpaceRate_.update( diskSpaceFreeBytes_, time() );*/

        if( freeDiskSpaceRate_.rateDefined() )
        {
            if( freeDiskSpaceRate_.rate() < 0.0 )
            {
                //nonConstThis->diskSpaceFreeSeconds_ = - ( diskSpaceFreeBytes_ / freeDiskSpaceRate_.rate() );
            }
        }

        //nonConstThis->updateDiskSpaceTime_ = time() + 10.0;
    }

    o << " Disk space free " << diskSpaceFreeBytes_ << " bytes";

    if( freeDiskSpaceRate_.rateDefined() )
    {
        o << " ( approx ";
        writeTime( o, diskSpaceFreeSeconds_ );
        o << " )";
    }

}

void RecRecorderImplementation::writeTime( ostream& o, double timeSeconds ) const
{
    size_t hours = timeSeconds / 3600.0;
    timeSeconds -= hours * 3600.0;
    size_t minutes = timeSeconds / 60.0;
    timeSeconds -= minutes * 60;
    size_t seconds = timeSeconds;

    o << hours << "h " << minutes << "m " << seconds << "s";
}

double RecRecorderImplementation::time() const
{
    double result = clock();
    result /= CLOCKS_PER_SEC;

    return result;
}

void RecRecorderImplementation::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const RecRecorderImplementation& t )
{
    if( t.state() == RecRecorder::RECORDING )
    {
        o << "RECORDING " << t.nBytesWritten_ << " bytes written";

        t.writeDriveInfo( o );
    }
    else if( t.state() == RecRecorder::PLAYING )
    {
        o << "PLAYING " << t.nBytesRead_ << " / " << t.nBytesInFile_;
        o << " (" << 100.0 * t.nBytesRead_ / t.nBytesInFile_ << "%)";
    }
    else
    {
        o << "Recording inactive";
    }

    return o;
}

ostream& operator <<( ostream& o, RecRecorderImplementation::RecordType type )
{
    switch( type )
    {
        case RecRecorderImplementation::TIME:
            o << "TIME";
            break;
        case RecRecorderImplementation::MOUSE_POSITION:
            o << "MOUSE_POSITION";
            break;
        case RecRecorderImplementation::LEFT_BUTTON_TRUE:
            o << "LEFT_BUTTON_TRUE";
            break;
        case RecRecorderImplementation::LEFT_BUTTON_FALSE:
            o << "LEFT_BUTTON_FALSE";
            break;
        case RecRecorderImplementation::RIGHT_BUTTON_TRUE:
            o << "RIGHT_BUTTON_TRUE";
            break;
        case RecRecorderImplementation::RIGHT_BUTTON_FALSE:
            o << "RIGHT_BUTTON_FALSE";
            break;
        case RecRecorderImplementation::KEY_TRUE:
            o << "KEY_TRUE";
            break;
        case RecRecorderImplementation::KEY_FALSE:
            o << "KEY_FALSE";
            break;
        case RecRecorderImplementation::KEY_CODE_TRUE:
            o << "KEY_CODE_TRUE";
            break;
        case RecRecorderImplementation::KEY_CODE_FALSE:
            o << "KEY_CODE_FALSE";
            break;
        case RecRecorderImplementation::ANY_KEY_TRUE:
            o << "ANY_KEY_TRUE";
            break;
        case RecRecorderImplementation::ANY_KEY_FALSE:
            o << "ANY_KEY_FALSE";
            break;
        case RecRecorderImplementation::SHIFT_TRUE:
            o << "SHIFT_TRUE";
            break;
        case RecRecorderImplementation::SHIFT_FALSE:
            o << "SHIFT_FALSE";
            break;
        case RecRecorderImplementation::CTRL_TRUE:
            o << "CTRL_TRUE";
            break;
        case RecRecorderImplementation::CTRL_FALSE:
            o << "CTRL_FALSE";
            break;
        case RecRecorderImplementation::ALT_TRUE:
            o << "ALT_TRUE";
            break;
        case RecRecorderImplementation::ALT_FALSE:
            o << "ALT_FALSE";
            break;
        case RecRecorderImplementation::DELTA_KEY:
            o << "DELTA_KEY";
            break;
        case RecRecorderImplementation::DELTA_KEY_CODE:
            o << "DELTA_KEY_CODE";
            break;
        case RecRecorderImplementation::RANDOM_SEED:
            o << "RANDOM_SEED";
            break;
        case RecRecorderImplementation::BUTTON_EVENT:
            o << "BUTTON_EVENT";
            break;
        case RecRecorderImplementation::EVENT_QUEUE_LENGTH:
            o << "EVENT_QUEUE_LENGTH";
            break;
        case RecRecorderImplementation::SOUND_ACTIVE_TRUE:
            o << "SOUND_ACTIVE_TRUE";
            break;
        case RecRecorderImplementation::SOUND_ACTIVE_FALSE:
            o << "SOUND_ACTIVE_FALSE";
            break;
        case RecRecorderImplementation::SOUND_AUDIBLE_TRUE:
            o << "SOUND_AUDIBLE_TRUE";
            break;
        case RecRecorderImplementation::SOUND_AUDIBLE_FALSE:
            o << "SOUND_AUDIBLE_FALSE";
            break;
        case RecRecorderImplementation::SOUND_3D_TRUE:
            o << "SOUND_3D_TRUE";
            break;
        case RecRecorderImplementation::SOUND_3D_FALSE:
            o << "SOUND_3D_FALSE";
            break;
        case RecRecorderImplementation::SOUND_SAMPLE_POSITION:
            o << "SOUND_SAMPLE_POSITION";
            break;
        case RecRecorderImplementation::SOUND_SAMPLE_VOLUME:
            o << "SOUND_SAMPLE_VOLUME";
            break;
        case RecRecorderImplementation::SOUND_FREE_LOGICAL_CHANNELS:
            o << "SOUND_FREE_LOGICAL_CHANNELS";
            break;
        case RecRecorderImplementation::SOUND_USED_LOGICAL_CHANNELS:
            o << "SOUND_USED_LOGICAL_CHANNELS";
            break;
        case RecRecorderImplementation::ANI_SMACKER_FINISHED_FALSE:
            o << "ANI_SMACKER_FINISHED_FALSE";
            break;
        case RecRecorderImplementation::ANI_SMACKER_FINISHED_TRUE:
            o << "ANI_SMACKER_FINISHED_TRUE";
            break;
        case RecRecorderImplementation::REGISTRY_RETURN_VALUE:
            o << "REGISTRY_RETURN_VALUE";
            break;
        case RecRecorderImplementation::REGISTRY_KEY:
            o << "REGISTRY_KEY";
            break;
        case RecRecorderImplementation::REGISTRY_BUFFER_SIZE:
            o << "REGISTRY_BUFFER_SIZE";
            break;
        case RecRecorderImplementation::REGISTRY_BUFFER:
            o << "REGISTRY_BUFFER";
            break;
        case RecRecorderImplementation::REGISTRY_STRING_SIZE:
            o << "REGISTRY_STRING_SIZE";
            break;
        case RecRecorderImplementation::REGISTRY_STRING:
            o << "REGISTRY_STRING";
            break;
        case RecRecorderImplementation::REGISTRY_INTEGER:
            o << "REGISTRY_INTEGER";
            break;
        case RecRecorderImplementation::NETWORK_INTEGER:
            o << "NETWORK_INTEGER";
            break;
        case RecRecorderImplementation::NETWORK_UNSIGNED_INTEGER:
            o << "NETWORK_UNSIGNED_INTEGER";
            break;
        case RecRecorderImplementation::NETWORK_DATA:
            o << "NETWORK_DATA";
            break;
        case RecRecorderImplementation::NETWORK_BOOL:
            o << "NETWORK_BOOL";
            break;
        case RecRecorderImplementation::NETWORK_STRING_SIZE:
            o << "NETWORK_STRING_SIZE";
            break;
        case RecRecorderImplementation::NETWORK_STRING:
            o << "NETWORK_STRING";
            break;
        case RecRecorderImplementation::NETWORK_DOUBLE:
            o << "NETWORK_DOUBLE";
            break;
        default:
            o << "UNKNOWN (" << _STATIC_CAST( int, type ) << ")";
            break;
    }
    return o;
}

double RecRecorderImplementation::percentageComplete() const
{
	PRE( state() == RecRecorder::PLAYING );

	double result = 100.0 * nBytesRead_ / nBytesInFile_;

	result = std::max( 0.0, result );
	result = std::min( 100.0, result );

	POST( result >= 0.0 );
	POST( result <= 100.0 );

	return result;
}

/* End RECIMPL.CPP **************************************************/
