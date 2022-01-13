#include "ani/AniSmackerRegular.hpp"
#include "render/display.hpp"
#include "recorder/recorder.hpp"
#include "recorder/private/recpriv.hpp"
#include "device/time.hpp"

//////////////////////////////////////////////////////////////////////
#define BUFFERS_NUM 6

static inline ALenum toALformat(short channels, short samples)
{
    bool stereo = (channels > 1);

    switch (samples) {
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

//////////////////////////////////////////////////////////////////////
// PUBLIC CTOR
AniSmackerRegular::AniSmackerRegular( const SysPathName& path, size_t xCoordTo, size_t yCoordTo )
    : AniSmackerRegular(path, xCoordTo, yCoordTo, false)
{
    //delegate :o
}

// PUBLIC CTOR
AniSmackerRegular::AniSmackerRegular( const SysPathName& path, size_t xCoordTo, size_t yCoordTo, bool fast )
{
    pSmack_ 			= nullptr;
    pBuffer_ 			= nullptr;
    xCoordTo_ 			= xCoordTo;
    yCoordTo_ 			= yCoordTo;
    advanceToNextFrame_ = true;
    fast_ 				= fast;
    useFrontBuffer_ 	= false;
    frameTime_          = 0.0;
    lastFrameTime_      = 0.0;
    frame_ 				= 0;
    width_              = 0;
    height_             = 0;
    fileName_			= path;
    finished_			= false;

    alFormat_           = -1;
    alSource_           = 0;
    alFrequency_        = 0;

    synchronisation_ = false;
}

// DTOR
AniSmackerRegular::~AniSmackerRegular()
{
    if( pBuffer_ )
        delete[] pBuffer_;

    if ( pSmack_ )
        smk_close( pSmack_ );

    if(alSource_)
    {
        alDeleteSources(1, &alSource_);
        for(int i = 0; i < BUFFERS_NUM; ++i)
            alDeleteBuffers(1, &alBuffers_[i]);
    }
}


bool AniSmackerRegular::isFinished() const
{
    bool finished = false;

    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
        finished = RecRecorderPrivate::instance().playbackAniSmackerFinished();
    else
    {
        finished = isFinishedNoRecord();

        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
            RecRecorderPrivate::instance().recordAniSmackerFinished( finished );
    }

    return finished;
}

unsigned int AniSmackerRegular::height() const
{
    return height_;
}

unsigned int AniSmackerRegular::width() const
{
    return width_;
}

void AniSmackerRegular::playNextFrame(RenDevice *pDevice)
{
    // First time in? If so open the smack animation
    if ( not pSmack_ )
    {
        pSmack_ = smk_open_file(fileName_.pathname().c_str(), SMK_MODE_MEMORY);
        // In debug version, assert that SmackOpen has worked
        ASSERT( pSmack_ != nullptr, "SmackOpen failed" );

        // Failed to open smacker animation, finish straight away, this will help release builds cope more
        // gracefully with failed SmackOpen.
        if ( pSmack_ == nullptr )
        {
            finished_ = true;
        }
        if ( pSmack_ )
        {
            // Turn on audio track 0 if present and prepare audio buffers
            unsigned char	a_tracks, a_channels[7], a_bitdepth[7];
            unsigned long	a_rate[7];
            smk_info_audio(pSmack_, &a_tracks, a_channels, a_bitdepth, a_rate);
            if (a_tracks & (1 << 0))
            {
                smk_enable_audio(pSmack_, 0, 1);
                // OpenAL stuff
                alFormat_   = toALformat(a_channels[0], a_bitdepth[0]);
                alFrequency_= a_rate[0];

                alGenSources((ALuint)1, &alSource_);
                alTestError("gen source smacker");
                alSourcef(alSource_, AL_PITCH, 1);
                ALfloat fVol = (float)SndMixer::instance().masterSampleVolume() / 100.0f ;
                alSourcef(alSource_, AL_GAIN, fVol);
                alSource3f(alSource_, AL_VELOCITY, 0, 0, 0);
                //alSourcei(alSource_, AL_LOOPING, AL_TRUE);
                alSourcei(alSource_, AL_SOURCE_RELATIVE, AL_TRUE);
                alSource3f(alSource_, AL_POSITION, 0.0f, 0.0f, 0.0f);
                // Create the buffers
                alGenBuffers(BUFFERS_NUM, &alBuffers_[0]);
                alTestError("gen buffer smacker");

                /*// Get audio all samples - testcode
                unsigned char* audioBuffer;
                unsigned int audioBufferSize = 0;
                do
                {
                    const unsigned char* pAudioBuffer =   smk_get_audio(pSmack_, 0);
                    unsigned long audioSize =       smk_get_audio_size(pSmack_, 0);
                    unsigned char* tmpBuff = _NEW_ARRAY(unsigned char, audioSize + audioBufferSize);
                    if(audioBufferSize > 0)
                    {
                        memcpy(tmpBuff, audioBuffer, audioBufferSize);
                        _DELETE_ARRAY(audioBuffer);
                    }
                    memcpy(&tmpBuff[audioBufferSize], pAudioBuffer, audioSize);
                    audioBufferSize += audioSize;
                    audioBuffer = tmpBuff;
                }
                while(smk_next( pSmack_ ) != SMK_DONE);

                smk_enable_audio(pSmack_, 0, 0);
                alBufferData(alBuffers_[0], alFormat_, audioBuffer, audioBufferSize, alFrequency_);
                alSourcei(alSource_, AL_BUFFER, alBuffers_[0]);
                _DELETE_ARRAY(audioBuffer);*/
            }

            long unsigned int w, h;
            // Turn on decoding for palette, video
            smk_enable_video(pSmack_, 1);

            // Get a pointer to first frame
            smk_first(pSmack_);
            // One frame time in ms
            smk_info_all(pSmack_, nullptr, nullptr, &frameTime_);
            //frameTime_ = 0.000001 * frameTime_;
            frameTime_ *= 0.000000826;
            smk_info_video(pSmack_, &w, &h, nullptr);
            width_ = w; height_ = h;
            surface_ = this->createSmackerSurface(pDevice);
            pBuffer_ = new uint[width_ * height_];

            // TBD: replace this assertion with something more reasonable
            ASSERT( pBuffer_ != nullptr, "" );

            // Start playing the first chunk
            if(alSource_)
            {
                // Fill bufers with WAV data
                const unsigned char* pAudioBuffer =   smk_get_audio(pSmack_, 0);
                unsigned long audioSize =       smk_get_audio_size(pSmack_, 0);

                ALsizei    size = audioSize / BUFFERS_NUM;
                for(int i = 0; i < BUFFERS_NUM; ++i)
                {
                    const ALvoid*    data = &pAudioBuffer[i*size];
                    alBufferData(alBuffers_[i], alFormat_, data, size, alFrequency_);
                    alTestError("start buffer data smacker on start");
                }
                alSourceQueueBuffers(alSource_, BUFFERS_NUM, &alBuffers_[0]);
                alSourcePlay(alSource_);
                alTestError("start play source smacker");
            }
        }
    } // FIRST FRAME

    if ( pSmack_ ) // Check pSmack_ just so release version copes with SmackOpen not working
    {
        bool shouldRender = true;

        // This method is called every frame after changes, check if frame time has passed
        double timeNow = DevTime::instance().time();
        advanceToNextFrame_ = (timeNow - lastFrameTime_ >= frameTime_);

        // Copy next frame from smacker file to the buffer.
        if ( advanceToNextFrame_ or fast_ or synchronisation_ )
        {
            copyCurrentFrameToBuffer( surface_ );
            lastFrameTime_ = timeNow;
            shouldRender = true;
        }

        if ( shouldRender or not useFrontBuffer() )
        {
            // Render the animation to a surface ( usually the screen ).
            if ( useFrontBuffer() )
            {
                unpackBufferToSurface( pDevice->frontSurface(), surface_ );
                pDevice->display()->flipBuffers();
            }
            else
                unpackBufferToSurface( pDevice->backSurface(), surface_ );

        }

        if ( fast_ )
        {
            getNextFrame();
        }
        else
        {
            if ( synchronisation_ )
            {
                getNextFrame();

            }
            else
            {
                if ( advanceToNextFrame_ )
                {
                    getNextFrame();
                }

                advanceToNextFrame_ = true;
            }
        }
        ++frame_;
    }
}

void AniSmackerRegular::displaySummaryInfo() const
{
    double usf;
    unsigned long f;
    smk_info_all(pSmack_, nullptr, &f, &usf);
    printf("Opened file %s\nWidth: %lu\nHeight: %lu\nFrames: %lu\nFPS: %f\n", fileName_.pathname().c_str(), width_, height_, f, 1000000.0 / usf);

    unsigned char	a_t, a_c[7], a_d[7];
    unsigned long	a_r[7];

    smk_info_audio(pSmack_, &a_t, a_c, a_d, a_r);
    int		i;
    for (i = 0; i < 7; i++)
    {
        printf("Audio track %d: %u bits, %u channels, %luhz\n", i, a_d[i], a_c[i], a_r[i]);
    }
}

void AniSmackerRegular::rewind()
{
    if ( pSmack_ )
    {
        smk_seek_keyframe( pSmack_, 0 );
        finished_ = false;

        if(alSource_)
            alSourceStop(alSource_);
    }
}

void AniSmackerRegular::useFrontBuffer( bool ufb )
{
    useFrontBuffer_ = ufb;
}

bool AniSmackerRegular::useFrontBuffer() const
{
    return useFrontBuffer_;
}

RenSurface AniSmackerRegular::createSmackerSurface(RenDevice *pDevice)
{
    return RenSurface::createAnonymousSurface( width_, height_, pDevice->backSurface() );
}

uint* AniSmackerRegular::fillBufferForCurrentFrame()
{
    const unsigned char *pal = smk_get_palette(pSmack_);
    const unsigned char *image_data = smk_get_video(pSmack_);
    uint *img_buff = pBuffer_;

    // Prepare a RGBA colours palette, TODO: this can be called once after file is loaded
    uint	col_palette[256];
    for (int i = 0; i < 256; i++)
    {
        uint    colour = 0xFF00;
        colour |= pal[(i * 3) + 2];
        colour <<= 8;
        colour |= pal[(i * 3) + 1];
        colour <<= 8;
        colour |= pal[(i * 3) + 0];
        col_palette[i] = colour;
    }

    // Fill image buffer
    unsigned int pixel = 0;
    for (unsigned int i = 0; i < height_; ++i)
    {
        for (unsigned int j = 0; j < width_; ++j)
        {
            img_buff[pixel] = col_palette[image_data[i * width_ + j]];
            ++pixel;
        }
    }

    return img_buff;
}

void AniSmackerRegular::copyCurrentVideoFrameToBuffer(RenSurface& renderSurface)
{
    renderSurface.copyFromRGBABuffer(fillBufferForCurrentFrame());
}

void AniSmackerRegular::copyCurrentFrameToBuffer( RenSurface& dst )
{
    /*
    const unsigned char *pal = smk_get_palette(pSmack_);
    const unsigned char *image_data = smk_get_video(pSmack_);
    uint *img_buff = pBuffer_;

    // Prepare a RGBA colours palette, TODO: this can be called once after file is loaded
    uint	col_palette[256];
    for (int i = 0; i < 256; i++)
    {
        uint    colour = 0xFF00;
        colour |= pal[(i * 3) + 2];
        colour <<= 8;
        colour |= pal[(i * 3) + 1];
        colour <<= 8;
        colour |= pal[(i * 3) + 0];
        col_palette[i] = colour;
    }

    // Fill image buffer
    unsigned int pixel = 0;
    for (unsigned int i = 0; i < height_; ++i)
    {
        for (unsigned int j = 0; j < width_; ++j)
        {
            img_buff[pixel] = col_palette[image_data[i * width_ + j]];
            ++pixel;
        }
    }
    dst.copyFromRGBABuffer(img_buff);
    */
    copyCurrentVideoFrameToBuffer(dst);

    //Fill audio buffer
    if(alSource_)
    {
        // Check for finished buffers, fill them with data and/or add to vector
        ALint val;
        alGetSourcei(alSource_, AL_BUFFERS_PROCESSED, &val);

        ALuint uiBuffer = 0;
        if(val <= 0)
        {
            if(freedBuffers_.empty())
            {
                //std::cout << "SKIPPING sample - all buffers in use" << std::endl;
                return;
            }
            else
            {
                uiBuffer = freedBuffers_.back();
                freedBuffers_.pop_back();
            }
        }
        else
        {
            ALuint buffs[BUFFERS_NUM] = {0};
            alSourceUnqueueBuffers(alSource_, val, &buffs[0]);
            uiBuffer = buffs[0];
            for(int i = 1; i < val; ++i)
                freedBuffers_.push_back(buffs[i]);

            //if(val > 1) std::cout << "Empty space: "<< val << std::endl;
        }
        // Fill bufer with WAV data
        const unsigned char* pAudioBuffer   = smk_get_audio(pSmack_, 0);
        unsigned long audioSize             = smk_get_audio_size(pSmack_, 0);

        ALsizei    size = audioSize;
        const ALvoid*    data = pAudioBuffer;

        //alBufferData(alBuffer_, alFormat_, data, size, alFrequency_);
        alBufferData(uiBuffer, alFormat_, data, size, alFrequency_);
        alTestError("buffer data smacker");

        alSourceQueueBuffers(alSource_, 1, &uiBuffer);
        //alSourcei(alSource_, AL_BUFFER, alBuffer_);

        alGetSourcei(alSource_, AL_SOURCE_STATE, &val);
        if(val != AL_PLAYING)
        {
            alSourcePlay(alSource_);
            alTestError("play source smacker");
            //std::cout << "stopped" << std::endl;
        }
    }
}

void AniSmackerRegular::unpackBufferToSurface( const RenSurface& dst, const RenSurface& src)
{
    bool    doUnpack = true;

    //  We need this extra check when we're playing back a recording because
    //  we might be pretending to continue to play even though the flic has
    //  actually finished.
    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
        doUnpack = not isFinishedNoRecord();

    if( doUnpack )
    {
        ASSERT( not isFinishedNoRecord(), "" );

        const_cast<RenSurface&>(dst).simpleBlit(src, xCoordTo_, yCoordTo_);
    }
}

bool AniSmackerRegular::isFinishedNoRecord() const
{
    return finished_;
}

bool AniSmackerRegular::isPenultimateFrame() const
{
    unsigned long frame, frame_count;
    smk_info_all(pSmack_, &frame, &frame_count, nullptr);
    return frame == frame_count - 1;
}

void AniSmackerRegular::getNextFrame()
{
    if ( not isPenultimateFrame() )
    {
        smk_next( pSmack_ );
    }
    else
    {
        smk_next( pSmack_ );
        finished_ = true;
    }
}
