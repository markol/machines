/*
 * S N D D A T A . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dSoundData

    Represents a single sound to be played by the W4dSoundsManager at various times.
    Holds the data required for the sound, including wav file name, percentage volumes
    and attenuation distances.
*/

#ifndef _WORLD4D_SNDDATA_HPP
#define _WORLD4D_SNDDATA_HPP

#include "base/base.hpp"
#include "system/pathname.hpp"
#include "mathex/mathex.hpp"

class W4dSoundData
// Canonical form revoked
{
public:
    //data for the sound: path specifies the sound file name.
    //id is the unique id used to identify the sound.
    //Loudest volume to be played at is maxPercentageVolume, up to nearAttenuationDistance from
    //associated entity. Then fades to inaudible at farAttenuationDistance.
    W4dSoundData( SysPathName path, uint id, double maxPercentageVolume, MATHEX_SCALAR nearAttenuationDistance,
                  MATHEX_SCALAR farAttenuationDistance, bool is3D, bool isHWMixed, int priority );

    ~W4dSoundData();

    void CLASS_INVARIANT;

    //Accessors
    const SysPathName& path() const;
    uint id() const;
    double maxPercentageVolume() const;
    MATHEX_SCALAR nearAttenuationDistance() const;
    MATHEX_SCALAR farAttenuationDistance() const;
	bool is3D() const;
	bool isHWMixed() const;
	int priority() const;

private:
    friend ostream& operator <<( ostream& o, const W4dSoundData& t );

    W4dSoundData( const W4dSoundData& );
    W4dSoundData& operator =( const W4dSoundData& );

    //data members
    SysPathName path_; //Pathname of the sound file
    uint id_; //Sound id
    double maxPercentageVolume_; //Loudest volume to be used
    MATHEX_SCALAR nearAttenuationDistance_; //Distance from entity up to which sound played at max volume
    MATHEX_SCALAR farAttenuationDistance_; //Distance from entity at which sound is inaudible
	bool is3D_;
	bool isHWMixed_;
	int priority_;
};

#ifdef _INLINE
#include "internal\snddata.ipp"
#endif

#endif

/* End SNDDATA.HPP **************************************************/
