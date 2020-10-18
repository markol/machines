#ifndef _MACHLOG_VEMAILINFO
#define _MACHLOG_VEMAILINFO

#include "machlog/vmdata.hpp"

#include "system/pathname.hpp"

#include "phys/phys.hpp"

class MachLogVoiceMailInfo
{
public:
	MachLogVoiceMailInfo(VoiceMailID id, SysPathName wavName, VoiceMailType mailType, PhysAbsoluteTime maxSaveTime):
	id_(id), wavName_(wavName), mailType_(mailType), maxSaveTime_(maxSaveTime){};

	VoiceMailID id_;
	SysPathName wavName_;
	VoiceMailType mailType_;
	PhysAbsoluteTime maxSaveTime_;
};

#endif