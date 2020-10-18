#include "world4d/internal/snddata.hpp"

#ifndef _INLINE
#include "world4d/internal/snddata.ipp"
#endif


W4dSoundData::W4dSoundData( SysPathName path, uint id, double maxPercentageVolume, MATHEX_SCALAR nearAttenuationDistance,
                  MATHEX_SCALAR farAttenuationDistance, bool is3D, bool isHWMixed, int priority):
path_(path),
id_(id),
maxPercentageVolume_(maxPercentageVolume),
nearAttenuationDistance_(nearAttenuationDistance),
farAttenuationDistance_(farAttenuationDistance),
is3D_(is3D),
isHWMixed_(isHWMixed),
priority_(priority)
{
}

W4dSoundData::~W4dSoundData()
{
}

//W4dSoundData::W4dSoundData( const W4dSoundData& copyMe):
//id_(copyMe.id_),
//maxPercentageVolume_(copyMe.maxPercentageVolume_),
//nearAttenuationDistance_(copyMe.nearAttenuationDistance_),
//farAttenuationDistance_(copyMe.farAttenuationDistance_),
//is3D_(copyMe.is3D_),
//isHWMixed_(copyMe.isHWMixed_),
//priority_(copyMe.priority_)
//{
//	SOUND_STREAM("Construct this motherfucker ... ");
//	path_=copyMe.path_;
//	SOUND_STREAM("Ok" << endl);
//}

ostream& operator <<( ostream& o, const W4dSoundData& t );
