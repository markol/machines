/***********************************************************

 A L S O U N D . C P P
 copyright (c) Charybdis 1997 all rights reserved

***********************************************************/

/*
*/

#include "stdlib/string.hpp"
#include "sound/internal/alsound.hpp"
#include "sound/internal/alsample.hpp"
#include "sound/internal/dxs.hpp"
#include "sound/internal/wavefmt.hpp"
#include "sound/soundmix.hpp"

#include "mathex/transf3d.hpp"
#include "mathex/vec3.hpp"
#include "mathex/point3d.hpp"
#include "mathex/eulerang.hpp"
#include "profiler/profiler.hpp"

bool alTestError(const char* msg)
{
	ALCenum error = alGetError();
	if (error != AL_NO_ERROR) {
		std::cerr << "ALerr: " << msg << std::endl;
		return false;
    }
    return true;
}

////////////////////////////////////////////////////////////

ALSound::SOUNDCARD ALSound::soundcard_ = UNKNOWN;


/** PROTECTED **/
ALSound::ALSound( void )
{
    preLoadedSamples_.reserve( 512 );
}

//static
void ALSound::shutdown( )
{
    SndMixer::SampleList::iterator m = preLoadedSamples_.begin();
    SndMixer::SampleList::iterator n = preLoadedSamples_.end();

    for(;m!=n;++m)
    {
		if((*m))
		{
	        _DELETE((*m));
		}
    }
    preLoadedSamples_.erase( preLoadedSamples_.begin(), preLoadedSamples_.end() );
}

ALSound::~ALSound( void )
{
    alDevice_ = alcGetContextsDevice(alContext_);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(alContext_);
	alcCloseDevice(alDevice_);
}

void ALSound::listDevices(const ALCchar *devices)
{
	const ALCchar *device = devices, *next = devices + 1;
	size_t len = 0;

	SOUND_STREAM("Devices list:" << std::endl);
	SOUND_STREAM("-------------" << std::endl);
	while (device && *device != '\0' && next && *next != '\0')
	{
		SOUND_STREAM(device << std::endl );
		len = strlen(device);
		device += (len + 1);
		next += (len + 2);
	}
	SOUND_STREAM("-------------" << std::endl);
}


/** SINGLETON IMPLEMENTATION **/
ALCcontext* ALSound::alContext_ = NULL; ALCdevice* ALSound::alDevice_ = NULL;

/** STATIC **/
void ALSound::initialise( const SndMixerParameters& params, PriorityLevel p )
{
	//Check ALSound is not already initialised
	PRE( not ALSound::isInitialised() );

    //I don't entirely agree with setting this here as
    //nothing has actually been initialised yet, but it's a bit of a
    //hangover from Daniels code and I haven't been so concerened
    //as to 'fix' it.
	isInitialised() = true;

	ALboolean enumeration;
	const ALCchar *defaultDeviceName = NULL; // TODO this should be a parameter

    enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
	if (enumeration == AL_FALSE)
		std::cerr << "audio enumeration extension not available" << std::endl;

	listDevices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));

	if (!defaultDeviceName)
		defaultDeviceName = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

	alDevice_ = alcOpenDevice(defaultDeviceName);
	if (!alDevice_)
	{
		std::cerr << "unable to open default audio device" << std::endl;
		return;
	}

	SOUND_STREAM("Device: " << alcGetString(alDevice_, ALC_DEVICE_SPECIFIER) << std::endl);

	alGetError();

	alContext_ = alcCreateContext(alDevice_, NULL);
	if (!alcMakeContextCurrent(alContext_))
	{
		std::cerr << "failed to make default OpenAL context" << std::endl;
		return;
	}

    //We know we are using DirectSound acceleration/hardware
    //emulation
    soundcard_ = DIRECTSOUND;

	//If the sound system is 3D
    if(params.audioType() == Snd::THREE_D)
    {
        instance().is3DSystem_ = true;
		instance().commitDeferredSettings();
    }
	else
		instance().is3DSystem_ = false;
}

/** STATIC **/
bool& ALSound::isInitialised( void )
{
	static bool isInitialised_ = false;

	return isInitialised_;
}

/** STATIC **/
ALSound& ALSound::instance( void )
{
//	PRE( instance().pIDirectSound_);
	PRE( ALSound::isInitialised() );
	static ALSound openALsound;

	return openALsound;
}

////////////////////////////////////////////////////////////

void ALSound::compact( void )
{
	PRE( ALSound::isInitialised() );
}

////////////////////////////////////////////////////////////
bool ALSound::is3DSystem()
{
	return is3DSystem_;
}

////////////////////////////////////////////////////////////

bool ALSound::listenerPosition(const MexVec3& /*newval*/)
{
	//TBD remove this function, it gives no idea of
	//orientation
    return false;
}

MexPoint3d ALSound::listenerPosition() const
{
	//TBD alter this function so it translates
	//from DX to Charybdis coords
//	PRE(p3dListener_);
//	D3DVECTOR position;
//	MexPoint3d returnVal;
//    HRESULT hr = instance().p3dListener_->GetPosition(&position);
//    if(!SUCCEEDED(hr))
//		ASSERT(false, "Couldn't get position");
//
//    returnVal.x(position.x);
//    returnVal.y(position.y);
//    returnVal.z(position.z);

    MexPoint3d returnVal;
	return returnVal;
}

void ALSound::listenerOrientation(const MexEulerAngles& /*newOrientation*/)
{
	//TBD implement this function
	ASSERT(false, "Shouldn't be calling this");
}

MexEulerAngles ALSound::listenerOrientation()
{
	//TBD implement this function correctly
	//to return meaningful values to the
	//client

	ASSERT(false, "Shouldn't be calling this");

	MexPoint3d pos;
	MexEulerAngles returnVal;
	ALfloat orientation[6];

	alGetListenerfv(AL_ORIENTATION, orientation);
	alTestError("listener orientation");
	pos.x(orientation[0]);
	pos.y(orientation[1]);
	pos.z(orientation[2]);

	MexTransform3d trans(pos);
	trans.rotation(&returnVal);
	return returnVal;
}

void ALSound::listenerTransform(const MexTransform3d& newTransform)
{
	//This function also converts from Charybdis
	//coords to DX, this should probably be done
	//inside another member function.

	ALfloat posX,   posY,   posZ;
	ALfloat frontX, frontY, frontZ;
	ALfloat topX,   topY,   topZ;

	MexTransform3d myNewTransform(newTransform);

	MexVec3 listenerPos(myNewTransform.position());
	MexVec3 listenerHead(myNewTransform.zBasis());
	MexVec3 listenerNose(myNewTransform.xBasis());

//	RICHARD_STREAM("MPos " << listenerPos << std::endl);
//	RICHARD_STREAM("MHead " << listenerHead << std::endl);
//	RICHARD_STREAM("MNose " << listenerNose << std::endl);

	posY = listenerPos.y();
	posZ = listenerPos.z();
	posX = listenerPos.x();

	topY = listenerHead.y();
	topZ = listenerHead.z();
	topX = listenerHead.x();

	frontY = listenerNose.y();
	frontZ = listenerNose.z();
	frontX = listenerNose.x();

//	RICHARD_STREAM("DXPos " << position.x << " , " << position.y << " , " << position.z << std::endl);
//	RICHARD_STREAM("DXHead " << top.x << " , " << top.y << " , " << top.z << std::endl);
//	RICHARD_STREAM("DXFront " << front.x << " , " << front.y << " , " << front.z << std::endl);

    alListener3f(AL_POSITION, posX, posY, posZ);
	alTestError("listener position");
    alListener3f(AL_VELOCITY, 0, 0, 0);
	alTestError("listener velocity");
	ALfloat listenerOri[] = {topX, topY, topZ, frontX, frontY, frontZ};
	alListenerfv(AL_ORIENTATION, listenerOri);
	alTestError("listener orientation");

//RICHARD_STREAM("ActualDXPos " << actualpos.x << " , " << actualpos.y << " , " << actualpos.z << std::endl);
//RICHARD_STREAM("ActualDXHead " << actualhead.x << " , " << actualhead.y << " , " << actualhead.z << std::endl);
//RICHARD_STREAM("ActualDXNose " << actualnose.x << " , " << actualnose.y << " , " << actualnose.z << std::endl);
//RICHARD_STREAM("************************" << std::endl);
}

MexTransform3d ALSound::listenerTransform()
{
	//TBD implement this function to return
	//a meaningful value.
	ASSERT(false, "Shouldn't be calling this");
	MexTransform3d temp;
	return temp;
}

void ALSound::commitDeferredSettings()
{
	if(SndMixer::soundOn())
	{

	}
}

////////////////////////////////////////////////////////////

/* ALSOUND.CPP ********************************************/
