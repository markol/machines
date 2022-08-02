#include "world4d/internal/snddata.hpp"

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

_CODE_INLINE
const SysPathName& W4dSoundData::path() const
{
	return path_;
}

_CODE_INLINE
uint W4dSoundData::id() const
{
	return id_;
}

_CODE_INLINE
double W4dSoundData::maxPercentageVolume() const
{
	return maxPercentageVolume_;
}

_CODE_INLINE
MATHEX_SCALAR W4dSoundData::nearAttenuationDistance() const
{
	return nearAttenuationDistance_;
}

_CODE_INLINE
MATHEX_SCALAR W4dSoundData::farAttenuationDistance() const
{
	return farAttenuationDistance_;
}

_CODE_INLINE
bool W4dSoundData::is3D() const
{
	return is3D_;
}

_CODE_INLINE
bool W4dSoundData::isHWMixed() const
{
	return isHWMixed_;
}

_CODE_INLINE
int W4dSoundData::priority() const
{
	return priority_;
}
