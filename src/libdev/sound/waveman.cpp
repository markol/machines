/*
 * W A V E M A N . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "sound/internal/waveman.hpp"
#include "sound/internal/alsound.hpp"

#include <time.h>
#include "profiler/profiler.hpp"
//diag.hpp used for file existence checking in debug and release
#ifdef PRODUCTION
#include "base/diag.hpp"
#endif

// TBD: We need much better error handling for the sound

static  bool    bobFirstSoundError = true;
static  int    	numberOfRefCreations = 0;
static  int    	numberOfRefReleases = 0;

/*unsigned long getRefCount(IUnknown* pThisInterface)
{
	unsigned long theRefCount = 0;
	if(pThisInterface)
	{
		pThisInterface->AddRef();
		theRefCount = pThisInterface->Release();
	}
	return theRefCount;
}*/

int bobSoundError(
    char* lpText,	// address of text in message box
    char* lpCaption,	// address of title of message box
    uint   	// style of message box
   )
{
//We don't want spurious diagnostic files appearing in production
//versions
#ifndef PRODUCTION
    if( bobFirstSoundError )
    {
        bobFirstSoundError = false;

        //  Clear the file
        std::ofstream ostr( "snderr.log" );
    }

    std::ofstream ostr( "snderr.log", std::ios::app );

    time_t  t = time( NULL );
    ostr << ctime( &t );

    ostr << std::endl;

    ostr << "Error : " << lpText << " " << lpCaption << std::endl;

//    ostr << "GetLastError " << GetLastError() << "  ";


    bool writeStackAnchor = true;
    ulong lineNumber = 0;
    const char* extraString = NULL;

    ostr << std::endl;
    ostr << "------ Stack trace start ------------------------------" << std::endl;
    ProProfiler::instance().traceStack( ostr, writeStackAnchor, lineNumber, extraString );
    ostr << "------ Stack trace finish -----------------------------" << std::endl << std::endl;

    ostr << ctime( &t );

    ostr << std::endl << std::endl << std::endl << std::endl;

    ostr.close();
#endif
    return 0;
}


extern void debugTiming( const char*, bool );

// static
SndWaveManager& SndWaveManager::instance()
{
    static SndWaveManager instance_;
    return instance_;
}

SndWaveManager::SndWaveManager()
{

    TEST_INVARIANT;
}

SndWaveManager::~SndWaveManager()
{
    TEST_INVARIANT;
}

void SndWaveManager::shutdown( )
{
	freeAll();
}

void SndWaveManager::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ALuint SndWaveManager::getSoundBuffer( const SndWaveformId& id, bool& isFound )
{
    PRE(id.pathname().existsAsFile());
    ALBufferMap::iterator i = loadedSoundBuffers_.find(id);
	bool found = (i != loadedSoundBuffers_.end());

	ALuint result = 0;
	if(found)
	{
		result = (*i).second.buffer_;
		(*i).second.count_++;
	}
	else
	{
        ALuint buffer;
        alGenBuffers(1, &buffer);
        alTestError("buffer generation");
        loadedSoundBuffers_.insert(id, SndCountedSoundBuffer( buffer ) );
        numberOfRefCreations++;
        result = buffer;
	}
	isFound = found;
	return result;
}

void SndWaveManager::freeSoundBuffer( const SndWaveformId& id )
{
	ALBufferMap::iterator i = loadedSoundBuffers_.find(id);
    SOUND_STREAM("Wav3" << std::endl);
	bool found = (i != loadedSoundBuffers_.end());
    SOUND_STREAM("Wav4" << std::endl);

	if(found)
	{
        SOUND_STREAM("Wav5" << std::endl);
		ALuint originalBuffer = (*i).second.buffer_;
        SOUND_STREAM("Wav6" << std::endl);

		//Work out how many references there are still outstanding.
		int count = (*i).second.count_;
        SOUND_STREAM("Wav7" << std::endl);

		//if there was 1 - we are releasing must be last reference
		if( count == 1 )
		{
            alDeleteBuffers(1, &originalBuffer);
            numberOfRefReleases++;
			loadedSoundBuffers_.erase( i );
            SOUND_STREAM("Wav17" << std::endl);
		}
		else
		{
            SOUND_STREAM("Wav22" << std::endl);
			(*i).second.count_--;
            SOUND_STREAM("Wav23" << std::endl);
		}
        SOUND_STREAM("Wav24" << std::endl);
	}
    SOUND_STREAM("Wav25" << std::endl);
}

SndWaveform* SndWaveManager::getWaveForm( const SndWaveformId& id )
{
	PRE(id.pathname().exists());

//	WaveFormMap::iterator i = loadedWaveForms_.begin();
//	WaveFormMap::iterator j = loadedWaveForms_.end();
//
//	SndWaveform* returnForm = NULL;
//
////	for(;i!=j && !returnForm; ++i)
//	while(i!=j && !returnForm)
//	{
//		if(((*i).first).pathname() == id.pathname())
//		{
//			(*i).second->addReference();
//			returnForm = (*i).second;
//		}
//		else
//			++i;
//	}

	SndWaveform* returnForm = NULL;
	WaveFormMap::iterator i = loadedWaveForms_.find(id);

	if(i==loadedWaveForms_.end())
	{
		SndWaveform* newWave= _NEW(SndWaveform(id));
		loadedWaveForms_.insert(id, newWave);
		returnForm = newWave;
	}
	else
	{
		(*i).second->addReference();
		returnForm = (*i).second;
	}

	POST(returnForm);
	return returnForm;
}

void SndWaveManager::freeWaveForm( const SndWaveformId& id )
{
	WaveFormMap::iterator i = loadedWaveForms_.find(id);

	if(i!=loadedWaveForms_.end())
	{
		(*i).second->removeReference();
		if((*i).second->ref() == 0)
		{
			_DELETE((*i).second);
			loadedWaveForms_.erase(i);
		}
	}
	else
	{
		ASSERT(NULL, "Trying to dereference unloaded wav");
	}
}

void SndWaveManager::freeAll( void )
{
	WaveFormMap::iterator i = loadedWaveForms_.begin();

	SOUND_STREAM("Clearing preloads out" << std::endl);
	for(WaveFormMap::iterator i = loadedWaveForms_.begin(); i!=loadedWaveForms_.end(); ++i)
	{
		_DELETE((*i).second);
	}

	loadedWaveForms_.erase(loadedWaveForms_.begin(), loadedWaveForms_.end());

	ALBufferMap::iterator j = loadedSoundBuffers_.begin();
	for(;j!=loadedSoundBuffers_.end();++j)
	{
//		int refNum = (*j).second.pBuffer_->Release();
        alDeleteBuffers(1, &((*j).second.buffer_) );
		numberOfRefReleases++;
	}
	loadedSoundBuffers_.erase(loadedSoundBuffers_.begin(), loadedSoundBuffers_.end());
}

bool SndWaveManager::isLoaded( const SndWaveformId& id )
{
//	WaveFormMap::iterator i = loadedWaveForms_.begin();
//	WaveFormMap::iterator j = loadedWaveForms_.end();
//
//	bool found = false;
//	while(not found && i!=j)
//	{
//		if((*i).first.pathname() == id)
//		{
//			found = true;
//		}
//		++i;
//	}
	return (loadedWaveForms_.find(id)!=loadedWaveForms_.end());
}


ostream& operator <<( ostream& o, const SndWaveManager& t )
{

    o << "SndWaveManager " << (void*)&t << " start" << std::endl;
    o << "SndWaveManager " << (void*)&t << " end" << std::endl;

    return o;
}

bool operator<( const SndCountedSoundBuffer& a, const SndCountedSoundBuffer& b )
{
//	if( a.pBuffer_ < b.pBuffer_ )
	if( a.buffer_ < b.buffer_ )
		return true;
	return false;
}
bool operator==( const SndCountedSoundBuffer& a, const SndCountedSoundBuffer& b )
{
//	if( a.pBuffer_ == b.pBuffer_ )
	if( a.buffer_ == b.buffer_ )
		return true;
	return false;
}
/* End WAVEMAN.CPP **************************************************/
