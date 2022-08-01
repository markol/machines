#pragma once

#include <AL/alure.h>
#include "device/cdlist.hpp"
#include "mathex/random.hpp"

struct DevCDImpl
{
    enum CDVOLUME
    {
        MAX_CDVOLUME = 65535,
        MIN_CDVOLUME = 0
    };

    alureStream*        stream_;
    ALuint              source_;

    unsigned int savedVolume_;

    DevCDPlayList* pPlayList_;

    bool haveMixer_;

    DevCDTrackIndex randomStartTrack_;
    DevCDTrackIndex randomEndTrack_;
    MexBasicRandom    randomGenerator_;

    bool musicEnabled_;

};
