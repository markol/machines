/***********************************************************

  W A V E F O R M . C P P

***********************************************************/

#include "sound/internal/waveform.hpp"
#include "sound/internal/wavefmt.hpp"
#include "system/pathname.hpp"
#include "stdlib/string.hpp"

////////////////////////////////////////////////////////////

/** STATIC **/
bool SndWaveform::isWaveFile( const char* path )
{
	bool isGood = false;
	if(!path)
    	return isGood;

	SysPathName temp(path);

	isGood = temp.existsAsFile();

    if( isGood )
	{
        WaveInfo *wave;
        wave = WaveOpenFileForReading(path);
        if (!wave) {
            isGood = false;
        }
        else
        {
            isGood = !WaveSeekFile(0, wave);
            WaveCloseFile(wave);
        }
    }
	return isGood;
}

////////////////////////////////////////////////////////////

SndWaveform::SndWaveform( const SndWaveformId& id ) :
id_(id),
ref_(1),
dataSize_( 0 ),
pFormat_( NULL )
{
	PRE( SndWaveform::isWaveFile( id_.pathname().c_str() ) );

    waveInfo_ = WaveOpenFileForReading(id.pathname().c_str());
    POST( waveInfo_ );
}

SndWaveform::~SndWaveform()
{
    WaveCloseFile(waveInfo_);
}

////////////////////////////////////////////////////////////

WaveFormat* SndWaveform::format()
{
	if(!pFormat_)
	{

        pFormat_ = _NEW( WaveFormat(
			(Channels)waveInfo_->channels,
			(SampleRateHz)waveInfo_->sampleRate,
			(BitsPerSample)waveInfo_->bitsPerSample ) );
	}

	return pFormat_;
}

uint SndWaveform::dataSize()
{
    //Ensure the size is cached
    if( dataSize_ == 0 )
    {
        dataSize_ = waveInfo_->dataSize;
    }

	return dataSize_;
}

ALenum SndWaveform::toALformat()
{
    bool stereo = (waveInfo_->channels > 1);

    switch (waveInfo_->bitsPerSample)
    {
        case 16:
            if (stereo)
                return AL_FORMAT_STEREO16;
            else
                return AL_FORMAT_MONO16;
        case 8:
            if (stereo)
                return AL_FORMAT_STEREO8;
            else
                return AL_FORMAT_MONO8;
        default:
            return -1;
    }
}

////////////////////////////////////////////////////////////

// this reads all the wave data into a buffer
// , assigns the pointer to the buffer to buf
// and returns the number of bytes read.
uint SndWaveform::read( void* buf )
{
	uint bufSize = dataSize();
	char* buffer = new char[ bufSize ];
/*	MMCKINFO dataChunk, chunk;

	// initialise chunks
	chunk.fccType = mmioFOURCC( 'W', 'A', 'V', 'E' );
	dataChunk.ckid = mmioFOURCC( 'd', 'a', 't', 'a' );

	mmioDescend( hmmio_, &chunk, NULL, MMIO_FINDRIFF );

	mmioDescend( hmmio_, &dataChunk, &chunk, MMIO_FINDCHUNK );

	mmioRead( hmmio_, (HPSTR)buffer, (LONG)bufSize );

	// go to begining of file
	mmioSeek( hmmio_, 0, SEEK_SET );

	buf = buffer;*/

	WaveSeekFile(0, waveInfo_);

	uint result = WaveReadFile(buffer, bufSize, waveInfo_);
	if (result != bufSize) {
		std::cerr << "Mismatch in wav buffer size when reading from file." << std::endl;
	}
    buf = buffer;
	return bufSize;
}

// unlike above this reads directly to the passed
// buf pointer. from specifies an offset into the
// data and len specifies the size of the buf array.
// the function returns the number of bytes read
uint SndWaveform::read( void* buf, uint from,
	uint len )
{
/*	MMCKINFO dataChunk, chunk;
	uint nBytesRead =0;

	mmioSeek( hmmio_, 0, SEEK_SET );

	// initialise chunks
	chunk.fccType = mmioFOURCC( 'W', 'A', 'V', 'E' );
	dataChunk.ckid = mmioFOURCC( 'd', 'a', 't', 'a' );

	mmioDescend( hmmio_, &chunk, NULL, MMIO_FINDRIFF );

	mmioDescend( hmmio_, &dataChunk, &chunk, MMIO_FINDCHUNK );

	mmioSeek( hmmio_, from, SEEK_CUR );

	nBytesRead = mmioRead( hmmio_, (HPSTR)buf, (LONG)len );

	// go to begining of file
	mmioSeek( hmmio_, 0, SEEK_SET );*/

	uint nBytesRead = 0;
	WaveSeekFile(from, waveInfo_);

	nBytesRead = WaveReadFile((char*)buf, len, waveInfo_);
	WaveSeekFile(from, waveInfo_);

	return nBytesRead;
}

////////////////////////////////////////////////////////////

const SysPathName& SndWaveform::pathname() const
{
	return id_.pathname();
}

////////////////////////////////////////////////////////////

SndWaveform& SndWaveform::addReference()
{
	++ref_;
	return *this;
}

////////////////////////////////////////////////////////////

SndWaveform& SndWaveform::removeReference()
{
	--ref_;
	return *this;
}

size_t SndWaveform::ref() const
{
	return ref_;
}

/* SndWaveformINFO.CPP ***************************************/
