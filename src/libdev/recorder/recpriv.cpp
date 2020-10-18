/*
 * R E C P R I V . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "recorder/private/recpriv.hpp"
#include "recorder/internal/recimpl.hpp"

#include "device/butevent.hpp"

#include "mathex/point3d.hpp"

// static
RecRecorderPrivate& RecRecorderPrivate::instance()
{
    static RecRecorderPrivate instance_;
    return instance_;
}

RecRecorderPrivate::RecRecorderPrivate()
{

    TEST_INVARIANT;
}

RecRecorderPrivate::~RecRecorderPrivate()
{
    TEST_INVARIANT;

}

void RecRecorderPrivate::recordTime( double time )
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    recorder.putData( RecRecorderImplementation::TIME,
       sizeof( time ),
       _STATIC_CAST( const void*, &time ) );
}

void RecRecorderPrivate::recordMousePosition( const DevMouse::Position& position )
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    recorder.putData( RecRecorderImplementation::MOUSE_POSITION,
       sizeof( position ),
       _STATIC_CAST( const void*, &position ) );
}

void RecRecorderPrivate::recordLeftButton( bool state )
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    recorder.putBool( state, RecRecorderImplementation::LEFT_BUTTON_TRUE, RecRecorderImplementation::LEFT_BUTTON_FALSE );
}

void RecRecorderPrivate::recordRightButton( bool state )
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    recorder.putBool( state, RecRecorderImplementation::RIGHT_BUTTON_TRUE, RecRecorderImplementation::RIGHT_BUTTON_FALSE );
}

void RecRecorderPrivate::recordKey( bool state )
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    recorder.putBool( state, RecRecorderImplementation::KEY_TRUE, RecRecorderImplementation::KEY_FALSE );
}

void RecRecorderPrivate::recordKeyCode( bool state )
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    recorder.putBool( state, RecRecorderImplementation::KEY_CODE_TRUE, RecRecorderImplementation::KEY_CODE_FALSE );
}

void RecRecorderPrivate::recordAnyKey( bool state )
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    recorder.putBool( state, RecRecorderImplementation::ANY_KEY_TRUE, RecRecorderImplementation::ANY_KEY_FALSE );
}

void RecRecorderPrivate::recordShiftPressed( bool state )
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    recorder.putBool( state, RecRecorderImplementation::SHIFT_TRUE, RecRecorderImplementation::SHIFT_FALSE );
}

void RecRecorderPrivate::recordCtrlPressed( bool state )
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    recorder.putBool( state, RecRecorderImplementation::CTRL_TRUE, RecRecorderImplementation::CTRL_FALSE );
}

void RecRecorderPrivate::recordAltPressed( bool state )
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    recorder.putBool( state, RecRecorderImplementation::ALT_TRUE, RecRecorderImplementation::ALT_FALSE );
}

void RecRecorderPrivate::recordDeltaKeyCode( const DevKeyboard::KeyState& state )
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    recorder.putData( RecRecorderImplementation::DELTA_KEY_CODE,
       sizeof( state ),
       _STATIC_CAST( const void*, &state ) );
}

void RecRecorderPrivate::recordRandomSeed( ulong seed )
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    recorder.putData( RecRecorderImplementation::RANDOM_SEED,
       sizeof( seed ),
       _STATIC_CAST( const void*, &seed ) );
}

void RecRecorderPrivate::recordButtonEvent( const DevButtonEvent& event )
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    //  This relies on the class being bitwise canonical
    recorder.putData( RecRecorderImplementation::BUTTON_EVENT,
       sizeof( event ),
       _STATIC_CAST( const void*, &event ) );
}

void RecRecorderPrivate::recordEventQueueLength( size_t length )
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    recorder.putData( RecRecorderImplementation::EVENT_QUEUE_LENGTH,
       sizeof( length ),
       _STATIC_CAST( const void*, &length ) );
}

void RecRecorderPrivate::recordSoundIsActive( bool value )
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    recorder.putBool( value, RecRecorderImplementation::SOUND_ACTIVE_TRUE, RecRecorderImplementation::SOUND_ACTIVE_FALSE );
}

void RecRecorderPrivate::recordSoundIsAudible( bool value )
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    recorder.putBool( value, RecRecorderImplementation::SOUND_AUDIBLE_TRUE, RecRecorderImplementation::SOUND_AUDIBLE_FALSE );
}

void RecRecorderPrivate::recordSoundIs3dSound( bool value )
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    recorder.putBool( value, RecRecorderImplementation::SOUND_3D_TRUE, RecRecorderImplementation::SOUND_3D_FALSE );
}

void RecRecorderPrivate::recordSoundSamplePosition( const MexPoint3d& position )
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    recorder.putData( RecRecorderImplementation::SOUND_SAMPLE_POSITION,
       sizeof( position ),
       _STATIC_CAST( const void*, &position ) );
}

void RecRecorderPrivate::recordSoundSampleVolume( Snd::Volume volume )
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    recorder.putData( RecRecorderImplementation::SOUND_SAMPLE_VOLUME,
       sizeof( volume ),
       _STATIC_CAST( const void*, &volume ) );
}

void RecRecorderPrivate::recordSoundNoOfFreeLogicalChannels( ulong value )
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    recorder.putData( RecRecorderImplementation::SOUND_FREE_LOGICAL_CHANNELS,
       sizeof( value ),
       _STATIC_CAST( const void*, &value ) );
}

void RecRecorderPrivate::recordSoundNoOfUsedLogicalChannels( ulong value )
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    recorder.putData( RecRecorderImplementation::SOUND_USED_LOGICAL_CHANNELS,
       sizeof( value ),
       _STATIC_CAST( const void*, &value ) );
}

void RecRecorderPrivate::recordAniSmackerFinished( bool value )
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    recorder.putBool( value, RecRecorderImplementation::ANI_SMACKER_FINISHED_TRUE, RecRecorderImplementation::ANI_SMACKER_FINISHED_FALSE );
}

void RecRecorderPrivate::recordRegistryReturnValue( SysRegistry::ReturnValue value )
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    recorder.putData( RecRecorderImplementation::REGISTRY_RETURN_VALUE,
       sizeof( value ),
       _STATIC_CAST( const void*, &value ) );
}

void RecRecorderPrivate::recordRegistryKey( SysRegistry::KeyHandle key )
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    recorder.putData( RecRecorderImplementation::REGISTRY_KEY,
       sizeof( key ),
       _STATIC_CAST( const void*, &key ) );
}

void RecRecorderPrivate::recordRegistryBuffer( const void* pBuffer, int bufferSize )
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    recorder.putData( RecRecorderImplementation::REGISTRY_BUFFER_SIZE,
       sizeof( bufferSize ),
       _STATIC_CAST( const void*, &bufferSize ) );

    recorder.putData( RecRecorderImplementation::REGISTRY_BUFFER,
       bufferSize, pBuffer );
}

void RecRecorderPrivate::recordRegistryStringValue( const std::string& str )
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    const int size = str.length();

    recorder.putData( RecRecorderImplementation::REGISTRY_STRING_SIZE,
       sizeof( size ),
       _STATIC_CAST( const void*, &size ) );

    recorder.putData( RecRecorderImplementation::REGISTRY_STRING,
       size,
       _STATIC_CAST( const void*, str.c_str() ) );
}

void RecRecorderPrivate::recordRegistryIntegerValue( int value )
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    recorder.putData( RecRecorderImplementation::REGISTRY_INTEGER,
       sizeof( value ),
       _STATIC_CAST( const void*, &value ) );
}

void RecRecorderPrivate::recordNetworkInt( int value )
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    recorder.putData( RecRecorderImplementation::NETWORK_INTEGER,
       sizeof( value ),
       _STATIC_CAST( const void*, &value ) );
}

void RecRecorderPrivate::recordNetworkUint( uint value )
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    recorder.putData( RecRecorderImplementation::NETWORK_UNSIGNED_INTEGER,
       sizeof( value ),
       _STATIC_CAST( const void*, &value ) );
}

void RecRecorderPrivate::recordNetworkData( const uint8* buffer, size_t bufferSize )
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    recorder.putData( RecRecorderImplementation::NETWORK_DATA,
       bufferSize,
       _STATIC_CAST( const void*, buffer ) );
}

void RecRecorderPrivate::recordNetworkBool( bool value )
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    recorder.putData( RecRecorderImplementation::NETWORK_BOOL,
       sizeof( value ),
       _STATIC_CAST( const void*, &value ) );
}

void RecRecorderPrivate::recordNetworkString( const std::string& str )
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    const int size = str.length();

    recorder.putData( RecRecorderImplementation::NETWORK_STRING_SIZE,
       sizeof( size ),
       _STATIC_CAST( const void*, &size ) );

    recorder.putData( RecRecorderImplementation::NETWORK_STRING,
       size,
       _STATIC_CAST( const void*, str.c_str() ) );
}

void RecRecorderPrivate::recordNetworkDouble( double value )
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    recorder.putData( RecRecorderImplementation::NETWORK_DOUBLE,
       sizeof( value ),
       _STATIC_CAST( const void*, &value ) );
}





double RecRecorderPrivate::playbackTime() const
{
    double time;

    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    recorder.getData( RecRecorderImplementation::TIME,
      sizeof( time ), _STATIC_CAST( void*, &time ) );

    return time;
}

DevMouse::Position RecRecorderPrivate::playbackMousePosition() const
{
    DevMouse::Position   result;

    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    recorder.getData( RecRecorderImplementation::MOUSE_POSITION,
      sizeof( result ), _STATIC_CAST( void*, &result ) );

    return result;
}

bool RecRecorderPrivate::playbackLeftButton() const
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    return recorder.getBool( RecRecorderImplementation::LEFT_BUTTON_TRUE, RecRecorderImplementation::LEFT_BUTTON_FALSE );
}

bool RecRecorderPrivate::playbackRightButton() const
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    return recorder.getBool( RecRecorderImplementation::RIGHT_BUTTON_TRUE, RecRecorderImplementation::RIGHT_BUTTON_FALSE );
}

bool RecRecorderPrivate::playbackKey() const
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    return recorder.getBool( RecRecorderImplementation::KEY_TRUE, RecRecorderImplementation::KEY_FALSE );
}

bool RecRecorderPrivate::playbackKeyCode() const
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    return recorder.getBool( RecRecorderImplementation::KEY_CODE_TRUE, RecRecorderImplementation::KEY_CODE_FALSE );
}

bool RecRecorderPrivate::playbackAnyKey() const
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    return recorder.getBool( RecRecorderImplementation::ANY_KEY_TRUE, RecRecorderImplementation::ANY_KEY_FALSE );
}

bool RecRecorderPrivate::playbackShiftPressed() const
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    return recorder.getBool( RecRecorderImplementation::SHIFT_TRUE, RecRecorderImplementation::SHIFT_FALSE );
}

bool RecRecorderPrivate::playbackCtrlPressed() const
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    return recorder.getBool( RecRecorderImplementation::CTRL_TRUE, RecRecorderImplementation::CTRL_FALSE );
}

bool RecRecorderPrivate::playbackAltPressed() const
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    return recorder.getBool( RecRecorderImplementation::ALT_TRUE, RecRecorderImplementation::ALT_FALSE );
}

DevKeyboard::KeyState RecRecorderPrivate::playbackDeltaKeyCode() const
{
    DevKeyboard::KeyState state;

    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    recorder.getData( RecRecorderImplementation::DELTA_KEY_CODE,
      sizeof( state ), _STATIC_CAST( void*, &state ) );

    return state;
}

ulong RecRecorderPrivate::playbackRandomSeed() const
{
    ulong seed;

    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    recorder.getData( RecRecorderImplementation::RANDOM_SEED,
      sizeof( seed ), _STATIC_CAST( void*, &seed ) );

    return seed;
}

DevButtonEvent RecRecorderPrivate::playbackButtonEvent() const
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    DevButtonEvent result;

    //  This relies on the class being bitwise canonical
    recorder.getData( RecRecorderImplementation::BUTTON_EVENT,
      sizeof( result ), _STATIC_CAST( void*, &result ) );

    return result;
}

size_t RecRecorderPrivate::playbackEventQueueLength() const
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    size_t result;

    recorder.getData( RecRecorderImplementation::EVENT_QUEUE_LENGTH,
       sizeof( result ),
       _STATIC_CAST( void*, &result ) );

    return result;
}

bool RecRecorderPrivate::playbackSoundIsActive() const
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    return recorder.getBool( RecRecorderImplementation::SOUND_ACTIVE_TRUE, RecRecorderImplementation::SOUND_ACTIVE_FALSE );
}

bool RecRecorderPrivate::playbackSoundIsAudible() const
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    return recorder.getBool( RecRecorderImplementation::SOUND_AUDIBLE_TRUE, RecRecorderImplementation::SOUND_AUDIBLE_FALSE );
}

bool RecRecorderPrivate::playbackSoundIs3dSound() const
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    return recorder.getBool( RecRecorderImplementation::SOUND_3D_TRUE, RecRecorderImplementation::SOUND_3D_FALSE );
}

MexPoint3d RecRecorderPrivate::playbackSoundSamplePosition() const
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    MexPoint3d result;

    //  Relies on MexPoint3d being bitwise canonical

    recorder.getData( RecRecorderImplementation::SOUND_SAMPLE_POSITION,
       sizeof( result ),
       _STATIC_CAST( void*, &result ) );

    return result;
}

Snd::Volume RecRecorderPrivate::playbackSoundSampleVolume() const
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    Snd::Volume result;

    recorder.getData( RecRecorderImplementation::SOUND_SAMPLE_VOLUME,
       sizeof( result ),
       _STATIC_CAST( void*, &result ) );

    return result;
}

ulong RecRecorderPrivate::playbackSoundNoOfFreeLogicalChannels() const
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    ulong result;

    recorder.getData( RecRecorderImplementation::SOUND_FREE_LOGICAL_CHANNELS,
       sizeof( result ),
       _STATIC_CAST( void*, &result ) );

    return result;
}

ulong RecRecorderPrivate::playbackSoundNoOfUsedLogicalChannels() const
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    ulong result;

    recorder.getData( RecRecorderImplementation::SOUND_USED_LOGICAL_CHANNELS,
       sizeof( result ),
       _STATIC_CAST( void*, &result ) );

    return result;
}

bool RecRecorderPrivate::playbackAniSmackerFinished() const
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    return recorder.getBool( RecRecorderImplementation::ANI_SMACKER_FINISHED_TRUE, RecRecorderImplementation::ANI_SMACKER_FINISHED_FALSE );
}

SysRegistry::ReturnValue RecRecorderPrivate::playbackRegistryReturnValue() const
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    SysRegistry::ReturnValue result;

    recorder.getData( RecRecorderImplementation::REGISTRY_RETURN_VALUE,
       sizeof( result ),
       _STATIC_CAST( void*, &result ) );

    return result;
}

SysRegistry::KeyHandle RecRecorderPrivate::playbackRegistryKey() const
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    SysRegistry::KeyHandle result;

    recorder.getData( RecRecorderImplementation::REGISTRY_KEY,
       sizeof( result ),
       _STATIC_CAST( void*, &result ) );

    return result;
}

int RecRecorderPrivate::playbackRegistryBuffer( void* pBuffer ) const
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    int bufferSize;

    recorder.getData( RecRecorderImplementation::REGISTRY_BUFFER_SIZE,
       sizeof( bufferSize ),
       _STATIC_CAST( void*, &bufferSize ) );

    recorder.getData( RecRecorderImplementation::REGISTRY_BUFFER,
       bufferSize, pBuffer );

    return bufferSize;
}

std::string RecRecorderPrivate::playbackRegistryStringValue() const
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    int size;

    recorder.getData( RecRecorderImplementation::REGISTRY_STRING_SIZE,
       sizeof( size ),
       _STATIC_CAST( void*, &size ) );

    char* buffer = _NEW_ARRAY( char, size + 1 );

    recorder.getData( RecRecorderImplementation::REGISTRY_STRING,
       size,
       _STATIC_CAST( void*, buffer ) );

    buffer[ size ] = '\0';

    std::string str = buffer;

    return str;
}

int RecRecorderPrivate::playbackRegistryIntegerValue() const
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    int result;

    recorder.getData( RecRecorderImplementation::REGISTRY_INTEGER,
       sizeof( result ),
       _STATIC_CAST( void*, &result ) );

    return result;
}

int RecRecorderPrivate::playbackNetworkInt() const
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    int result;

    recorder.getData( RecRecorderImplementation::NETWORK_INTEGER,
       sizeof( result ),
       _STATIC_CAST( void*, &result ) );

    return result;
}

uint RecRecorderPrivate::playbackNetworkUint() const
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    uint result;

    recorder.getData( RecRecorderImplementation::NETWORK_UNSIGNED_INTEGER,
       sizeof( result ),
       _STATIC_CAST( void*, &result ) );

    return result;
}

void RecRecorderPrivate::playbackNetworkData( uint8* buffer, size_t bufferSize ) const
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    recorder.getData( RecRecorderImplementation::NETWORK_DATA,
       bufferSize,
       _STATIC_CAST( void*, buffer ) );
}

bool RecRecorderPrivate::playbackNetworkBool() const
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    bool result;

    recorder.getData( RecRecorderImplementation::NETWORK_BOOL,
       sizeof( result ),
       _STATIC_CAST( void*, &result ) );

    return result;
}

std::string RecRecorderPrivate::playbackNetworkString() const
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    int size;

    recorder.getData( RecRecorderImplementation::NETWORK_STRING_SIZE,
       sizeof( size ),
       _STATIC_CAST( void*, &size ) );

    char* buffer = _NEW_ARRAY( char, size + 1 );

    recorder.getData( RecRecorderImplementation::NETWORK_STRING,
       size,
       _STATIC_CAST( void*, buffer ) );

    buffer[ size ] = '\0';

    std::string str = buffer;

    return str;
}

double RecRecorderPrivate::playbackNetworkDouble() const
{
    RecRecorderImplementation& recorder = RecRecorderImplementation::instance();

    double result;

    recorder.getData( RecRecorderImplementation::NETWORK_DOUBLE,
       sizeof( result ),
       _STATIC_CAST( void*, &result ) );

    return result;
}


void RecRecorderPrivate::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const RecRecorderPrivate& t )
{

    o << "RecRecorderPrivate " << (void*)&t << " start" << std::endl;
    o << "RecRecorderPrivate " << (void*)&t << " end" << std::endl;

    return o;
}

// {
// 	while( not istr.eof() )
// 	{
// 		RecordType	type;
//
// 		istr.read( &type, sizeof( type ) );
//
// 	    switch( type )
// 	    {
// 	        case RecRecorderImplementation::TIME:
// 			{
// 				double time;
// 				istr.read( &time, sizeof( time ) );
// 				o << time;
// 	            break;
// 	        case RecRecorderImplementation::MESSAGE_TIME:
// 	            o << "MESSAGE_TIME";
// 	            break;
// 	        case RecRecorderImplementation::MOUSE_POSITION:
// 	            o << "MOUSE_POSITION";
// 	            break;
// 	        case RecRecorderImplementation::LEFT_BUTTON_TRUE:
// 	            o << "LEFT_BUTTON_TRUE";
// 	            break;
// 	        case RecRecorderImplementation::LEFT_BUTTON_FALSE:
// 	            o << "LEFT_BUTTON_FALSE";
// 	            break;
// 	        case RecRecorderImplementation::RIGHT_BUTTON_TRUE:
// 	            o << "RIGHT_BUTTON_TRUE";
// 	            break;
// 	        case RecRecorderImplementation::RIGHT_BUTTON_FALSE:
// 	            o << "RIGHT_BUTTON_FALSE";
// 	            break;
// 	        case RecRecorderImplementation::KEY_TRUE:
// 	            o << "KEY_TRUE";
// 	            break;
// 	        case RecRecorderImplementation::KEY_FALSE:
// 	            o << "KEY_FALSE";
// 	            break;
// 	        case RecRecorderImplementation::KEY_CODE_TRUE:
// 	            o << "KEY_CODE_TRUE";
// 	            break;
// 	        case RecRecorderImplementation::KEY_CODE_FALSE:
// 	            o << "KEY_CODE_FALSE";
// 	            break;
// 	        case RecRecorderImplementation::ANY_KEY_TRUE:
// 	            o << "ANY_KEY_TRUE";
// 	            break;
// 	        case RecRecorderImplementation::ANY_KEY_FALSE:
// 	            o << "ANY_KEY_FALSE";
// 	            break;
// 	        case RecRecorderImplementation::SHIFT_TRUE:
// 	            o << "SHIFT_TRUE";
// 	            break;
// 	        case RecRecorderImplementation::SHIFT_FALSE:
// 	            o << "SHIFT_FALSE";
// 	            break;
// 	        case RecRecorderImplementation::CTRL_TRUE:
// 	            o << "CTRL_TRUE";
// 	            break;
// 	        case RecRecorderImplementation::CTRL_FALSE:
// 	            o << "CTRL_FALSE";
// 	            break;
// 	        case RecRecorderImplementation::ALT_TRUE:
// 	            o << "ALT_TRUE";
// 	            break;
// 	        case RecRecorderImplementation::ALT_FALSE:
// 	            o << "ALT_FALSE";
// 	            break;
// 	        case RecRecorderImplementation::DELTA_KEY:
// 	            o << "DELTA_KEY";
// 	            break;
// 	        case RecRecorderImplementation::DELTA_KEY_CODE:
// 	            o << "DELTA_KEY_CODE";
// 	            break;
// 	        case RecRecorderImplementation::RANDOM_SEED:
// 	            o << "RANDOM_SEED";
// 	            break;
// 	        case RecRecorderImplementation::BUTTON_EVENT:
// 	            o << "BUTTON_EVENT";
// 	            break;
// 	        case RecRecorderImplementation::EVENT_QUEUE_LENGTH:
// 	            o << "EVENT_QUEUE_LENGTH";
// 	            break;
// 	        default:
// 	            o << "UNKNOWN (" << _STATIC_CAST( int, type ) << ")";
// 	            break;
// 	    }
// 	}
// }


/* End RECPRIV.CPP **************************************************/
