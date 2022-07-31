/*
 * V M M A N I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogVoiceMailManagerImpl

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_VMMANI_HPP
#define _MACHLOG_VMMANI_HPP

#include "base/base.hpp"
#include "stdlib/string.hpp"
#include "ctl/map.hpp"
#include "ctl/pvector.hpp"
#include "machlog/vmman.hpp"
#include "machlog/vmdata.hpp"

class MachLogVoiceMailManagerImpl
// Canonical form revoked
{
public:
    MachLogVoiceMailManagerImpl();
    ~MachLogVoiceMailManagerImpl();

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachLogVoiceMailManagerImpl& t );

    MachLogVoiceMailManagerImpl( const MachLogVoiceMailManagerImpl& );
    MachLogVoiceMailManagerImpl& operator =( const MachLogVoiceMailManagerImpl& );

	friend class MachLogVoiceMailManager;

	MachLogVoiceMailManager::BoolVec 		voiceMailPlaying_;		// a 'true' value means this actor has a voice mail associated with it that is currently playing.
	MachLogVoiceMailManager::MailInfoVector* pAvailableVEMails_;
	MachLogVoiceMailManager::VEmailIDMap 	registeredIds_;
	MachLogVoiceMailManager::VEmailTypeMap 	registeredTypes_;
	MachLogVoiceMailManager::MailVector 	incomingMailQueue_;
	MachLogVoiceMailManager::MailVector 	savedMail_;
	size_t 		currentMailMessage_;
	size_t 		lastMessage_;
	size_t 		savedMailslots_;
	bool 		definitionFileRead_;
  	bool 		podMailPlaying_;			// ensures pod (actor-independent mails) is only saying one thing at a time									
	int 		noOfMailsPlaying_;			// if we keep track of this, we can insist upon a maximum number allowed to play
	bool 		acceptMailPostings_;		// if this is false, no mails sent to most methods will be added to the queue.
};

#define CB_MachLogVoiceMailManager_DEPIMPL() \
	CB_DEPIMPL( MachLogVoiceMailManager::BoolVec, voiceMailPlaying_ ); \
	CB_DEPIMPL( MailInfoVector*, pAvailableVEMails_ ); \
	CB_DEPIMPL( VEmailIDMap, registeredIds_ ); \
	CB_DEPIMPL( VEmailTypeMap, registeredTypes_ ); \
	CB_DEPIMPL( MailVector, incomingMailQueue_ ); \
	CB_DEPIMPL( MailVector, savedMail_ ); \
	CB_DEPIMPL( size_t, currentMailMessage_ ); \
	CB_DEPIMPL( size_t, lastMessage_ ); \
	CB_DEPIMPL( size_t, savedMailslots_ ); \
	CB_DEPIMPL( bool, definitionFileRead_ ); \
  	CB_DEPIMPL( bool, podMailPlaying_ ); \
	CB_DEPIMPL( int, noOfMailsPlaying_ ); \	
	CB_DEPIMPL( bool, acceptMailPostings_ );

#endif

/* End VMMANI.HPP ***************************************************/
