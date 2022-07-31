/*
 * V M A I L I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogVoiceMailImpl

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_VMAILI_HPP
#define _MACHLOG_VMAILI_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"
#include "mathex/point3d.hpp"
#include "sound/smphandl.hpp"
#include "machlog/vmdata.hpp"

class MachLogVoiceMailImpl
// Canonical form revoked
{
public:
    MachLogVoiceMailImpl();
    ~MachLogVoiceMailImpl();

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachLogVoiceMailImpl& t );

    MachLogVoiceMailImpl( const MachLogVoiceMailImpl& );
    MachLogVoiceMailImpl& operator =( const MachLogVoiceMailImpl& );

	friend class MachLogVoiceMail;

	PhysAbsoluteTime 	timeStamp_;
	VoiceMailID 		id_;
	bool 				hasPosition_;
	MexPoint3d 			position_;
	UtlId 				actorId_;
	SndSampleHandle 	sampleHandle_;
	bool 				hasStarted_;
	bool 				sampleHandleValid_;
};

#define CB_MachLogVoiceMail_DEPIMPL() \
	CB_DEPIMPL(	PhysAbsoluteTime, timeStamp_ ); \
	CB_DEPIMPL(	VoiceMailID, id_ ); \
	CB_DEPIMPL(	bool, hasPosition_ ); \
	CB_DEPIMPL(	MexPoint3d, position_ ); \
	CB_DEPIMPL(	UtlId, actorId_ ); \
	CB_DEPIMPL(	SndSampleHandle, sampleHandle_ ); \
	CB_DEPIMPL(	bool, sampleHandleValid_ );\
	CB_DEPIMPL(	bool, hasStarted_ );


#endif

/* End VMAILI.HPP ***************************************************/
