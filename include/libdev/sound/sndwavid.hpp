/*
 * S N D W A V I D . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#ifndef _SOUND_WAVID_HPP
#define _SOUND_WAVID_HPP

#include "base/base.hpp"

class SndWaveformIdImpl;
class SysPathName;

class SndWaveformId
{
public:
	SndWaveformId();

	SndWaveformId(const SysPathName& pathname);

	SndWaveformId(const SndWaveformId& newId);

	~SndWaveformId();

	const SysPathName& pathname() const;

	SndWaveformId& operator=(const SndWaveformId& rhs);

	friend bool operator==(const SndWaveformId& lhs, const SndWaveformId& rhs);
	friend bool operator<(const SndWaveformId& lhs, const SndWaveformId& rhs);

private:

	SndWaveformIdImpl* pImpl_;
};

#endif