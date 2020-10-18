/*
 * V M M A N . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"

#include "machlog/vmman.hpp"

#include "mathex/point2d.hpp"

#include "machphys/random.hpp"

#include "machlog/cntrl_pc.hpp"
#include "machlog/races.hpp"
#include "machlog/vmdata.hpp"
#include "machlog/internal/vminfo.hpp"
#include "machlog/internal/vmail.hpp"
#include "machlog/internal/vmmani.hpp"

#include "sim/manager.hpp"
#include "sound/soundmix.hpp"
#include "sound/sndwavid.hpp"
#include "utility/linetok.hpp"

#include "system/metafile.hpp"
#include "system/metaistr.hpp"
#include <memory>

// static
MachLogVoiceMailManager& MachLogVoiceMailManager::instance()
{
    static MachLogVoiceMailManager instance_;
    return instance_;
}

MachLogVoiceMailManager::MachLogVoiceMailManager()
:	pImpl_( _NEW( MachLogVoiceMailManagerImpl() ) )
{
	CB_MachLogVoiceMailManager_DEPIMPL();

	definitionFileRead_ = false;
	savedMailslots_ 	= 5;
	currentMailMessage_ = 0;
	lastMessage_ 		= 0;
	podMailPlaying_ 	= false;
	noOfMailsPlaying_ 	= 0;
	acceptMailPostings_ = true;

	//Reset all mail status to unplayed
	if( voiceMailsActivated() )
		registerVoiceMailIDs();

	int totalPossibleActors = MachLogRaces::instance().maxActors();
	voiceMailPlaying_.reserve( totalPossibleActors );
	for( int initLoop = 0; initLoop < totalPossibleActors; ++initLoop )
		voiceMailPlaying_.push_back( false );


    TEST_INVARIANT;
}

MachLogVoiceMailManager::~MachLogVoiceMailManager()
{
	CB_MachLogVoiceMailManager_DEPIMPL();

    TEST_INVARIANT;

	clearMailQueue();

	for ( MailInfoVector::iterator i = pAvailableVEMails_->begin(); i != pAvailableVEMails_->end(); ++i )
		_DELETE( *i );

	_DELETE( pAvailableVEMails_ );

	_DELETE( pImpl_ );
}

void MachLogVoiceMailManager::update()
{
	CB_MachLogVoiceMailManager_DEPIMPL();

	if( not voiceMailsActivated() )
		return;

	if(!(incomingMailQueue_.empty()))
	{
		bool finished = false;
		int queueSize = incomingMailQueue_.size();
		int indexPos = 0;

		// we have already confirmed that the queue is of at least size one, so we're safe first time around.
		while( not finished )
		{
			MachLogVoiceMail* pMail = incomingMailQueue_[indexPos];
			ASSERT(pMail, "Invalid mail");
			//If the mail has been previously played
			if( pMail->hasStarted() )
			{
				if(!(pMail->isPlaying()))
				{
					//The mail has stopped playing therefore the handle
					//is no longer valid and the resources should be
					//freed
					pMail->invalidateSample();

					// sample has finished - if it had an associated actor id, set the associated "sample playing" slot
					// to false
					if( pMail->hasActorId() )
					{
						ASSERT_INFO( pMail->actorId() );
						ASSERT( pMail->actorId() < MachLogRaces::instance().maxActors(), "SHRIEK! We have an actor ID larger than the expected actor size (and our voiceMailPlaying_ array)" );
						voiceMailPlaying_[ pMail->actorId() ] = false;
					}
					else
						// must have been a pod mail. That slot is now free.
						podMailPlaying_ = false;

					--noOfMailsPlaying_;

					ASSERT( noOfMailsPlaying_ >= 0, "noOfMailsPlaying_ should never be less than 0." );

					// revoked saved mail queue 10/2/99 CPS
					/*
					// now deal with possibly putting onto the savedmail queue, or deleting it outright.
					if(((*pAvailableVEMails_)[pMail->id()])->mailType_ == VM_FULL_FUNCTION)
					{
						//Time stamp the message for the save buffer
						pMail->timeStamp( SimManager::instance().currentTime() );
						savedMail_.push_back(pMail);
						currentMailMessage_ = savedMail_.size();
					}
					else
					*/
					{
						_DELETE(pMail);
					}

					incomingMailQueue_.erase(incomingMailQueue_.begin() + indexPos);

					--queueSize;
					ASSERT( queueSize == incomingMailQueue_.size(), "Our tracking variable is out of synch with the actual queue size." ) ;


				}
				else
					// as the queue has not reduced in size under our feet, have to move on to the next entry ourselves
					++indexPos;
			}
			else
			{

				//we want to play this mail. But have we exceeded our voicemail cap?

				ASSERT( noOfMailsPlaying_ <= 2, "Something's gone wrong, as there should NEVER be more than 2 voicemails on the go." );

				// arbitrary temporary number. Should really be parameterised.
				// Also check sound lib has free channel.
				// Note that VM_WAIT_UNTIL_NOTHING_PLAYING

				VoiceMailType mailType = ( (*pAvailableVEMails_)[pMail->id()] )->mailType_;

				if( noOfMailsPlaying_ == 2
					or SndMixer::instance().noOfFreeLogicalChannels() == 0
					or ( mailType == VM_WAIT_UNTIL_NOTHING_PLAYING
						and noOfMailsPlaying_ > 0 )	)
				{
					// if this isn't a full-function or wait-until mail, just delete the bugger.
					// It's tough luck. We're too busy.
					if( not ( mailType == VM_FULL_FUNCTION or mailType == VM_WAIT_UNTIL_NOTHING_PLAYING ) )
					{
						_DELETE(pMail);
						incomingMailQueue_.erase(incomingMailQueue_.begin() + indexPos);

						--queueSize;
						ASSERT( queueSize == incomingMailQueue_.size(), "Our tracking variable is out of synch with the actual queue size." ) ;
					}
					else
						// as the queue has not reduced in size under our feet, have to move on to the next entry ourselves
						++indexPos;
				}
				else
				{
					// we're free to try playing the mail.

						// if this mail has an associated actor, only play this mail if the actor does not already have
					// a mail playing.
					if( pMail->hasActorId() )
					{
						UtlId actorId = pMail->actorId();
						ASSERT_INFO( actorId );
						ASSERT( pMail->actorId() < MachLogRaces::instance().maxActors(), "SHRIEK! We have an actor ID larger than the expected actor size (and our voiceMailPlaying_ array)" );

						if( not voiceMailPlaying_[ actorId ] )
						{
							pMail->play();
							pMail->hasStarted( true );
							voiceMailPlaying_[ actorId ] = true;
							++noOfMailsPlaying_;

							ASSERT( noOfMailsPlaying_ > 0, "noOfMailsPlaying_ should have been >0 here." );
						}
					}
					else
					{
						// must be a pod mail. Only play if we're not already playing a pod mail.
						if( not podMailPlaying_ )
						{
							pMail->play();
							pMail->hasStarted( true );
							podMailPlaying_ = true;
							++noOfMailsPlaying_;

							ASSERT( noOfMailsPlaying_ > 0, "noOfMailsPlaying_ should have been >0 here." );
						}
					}
					++indexPos;
				}




			}
			finished = ( indexPos >= queueSize );
		}
	}
	// revoked saved mail functionality 10/2/99 CPS
	/*
	if(!savedMail_.empty())
	{
		int indexPos = savedMail_.size();
		PhysAbsoluteTime now = SimManager::instance().currentTime();
		while(indexPos--)
		{
			MachLogVoiceMail* pSavedMail = savedMail_[indexPos];
			PhysAbsoluteTime bestBefore =  pSavedMail->timeStamp() +
				(((*pAvailableVEMails_)[pSavedMail->id()])->maxSaveTime_);
			if(now > bestBefore)
			{
				if(pSavedMail->isPlaying())
					pSavedMail->stop();
				_DELETE(pSavedMail);
				savedMail_.erase(savedMail_.begin() + indexPos);
			}
		}
	}
	*/
}


bool MachLogVoiceMailManager::incCurrentMail()
{
	CB_MachLogVoiceMailManager_DEPIMPL();

	bool increment = true;

	if(!savedMail_.empty())
	{
		if(currentMailMessage_ >= savedMail_.size()-1)
		{
			currentMailMessage_ = 0;
			increment = false;
		}
		if(lastMessage_ > savedMail_.size()-1)
		{
			lastMessage_ = currentMailMessage_;
		}

		if(savedMail_[lastMessage_]->isPlaying())
		{
			savedMail_[lastMessage_]->stop();
		}
		if(increment)
		{
			currentMailMessage_++;
		}
		lastMessage_ = currentMailMessage_;
		savedMail_[lastMessage_]->play();
	}

	return true;
}

bool MachLogVoiceMailManager::decCurrentMail()
{
	CB_MachLogVoiceMailManager_DEPIMPL();

	//If current mail message is outwith the bounds
	//of the saved mail buffer then something has changed
	//so reset the current mail message.

	if(!savedMail_.empty())
	{
		if(currentMailMessage_ > savedMail_.size() || currentMailMessage_ == 0)
		{
			currentMailMessage_ = savedMail_.size();
		}


		if(lastMessage_ > savedMail_.size() - 1)
		{
			lastMessage_ = savedMail_.size()-1;
		}

		if(savedMail_[lastMessage_]->isPlaying())
			savedMail_[lastMessage_]->stop();

		currentMailMessage_--;
		lastMessage_ = currentMailMessage_;
		savedMail_[currentMailMessage_]->play();
	}

	return true;
}

void MachLogVoiceMailManager::postNewMail(VoiceMailID id, MachPhys::Race targetRace )
{
	CB_MachLogVoiceMailManager_DEPIMPL();

	if( voiceMailsActivated() and acceptMailPostings_ and MachLogRaces::instance().hasPCRace() and targetRace == MachLogRaces::instance().pcController().race() )
	{
		// only bother pushing it onto the queue if the actor in question isn't already playing a mail
		// OR it's not a selection-affirmation mail.
		if( not podMailPlaying_
		    or ( (*pAvailableVEMails_)[id] )->mailType_ != VM_SELECTION_AFFIRMATION )
	    {
	    	MachLogVoiceMail* pNewMail = _NEW( MachLogVoiceMail( id ) );
			incomingMailQueue_.push_back( pNewMail );
	    }
	}
}

void MachLogVoiceMailManager::postNewMail(VoiceMailID id, UtlId actorId, MachPhys::Race targetRace )
{
	CB_MachLogVoiceMailManager_DEPIMPL();

	if( voiceMailsActivated() and acceptMailPostings_ and MachLogRaces::instance().hasPCRace() and targetRace == MachLogRaces::instance().pcController().race() )
	{
		// now, will we actually accept this posting onto the queue?

		bool acceptMail = false;

		// acceptance of mail is contingent upon voicemail type.
		VoiceMailType mailType = ( (*pAvailableVEMails_)[id] )->mailType_;

		if( mailType == VM_FULL_FUNCTION or mailType == VM_WAIT_UNTIL_NOTHING_PLAYING )
		{
			// yes, we'll definitely keep that
			acceptMail = true;
		}
		else if( voiceMailPlaying_[ actorId ] and mailType == VM_SELECTION_AFFIRMATION )
		{
			// nope, we certainly won't bother keeping one of those
			acceptMail = false;
		}
		else
		{
			// acceptance now depends on how many mails this actor already has in the queue. If 2 or
			// more, don't accept this one.
			int nActorMailsInQueue = 0;

			for( MachLogVoiceMailManager::MailVector::iterator i = incomingMailQueue_.begin();
									nActorMailsInQueue <= 1 and i != incomingMailQueue_.end();
																						  ++i )
			{
				MachLogVoiceMail* pMail = (*i);

				if( pMail->hasActorId() and pMail->actorId() == actorId )
				{
					++nActorMailsInQueue;
				}
			}

			acceptMail =( nActorMailsInQueue <= 1 );
		}

		if( acceptMail )
	    {
	    	MachLogVoiceMail* pNewMail = _NEW( MachLogVoiceMail( id, actorId ) );
			incomingMailQueue_.push_back( pNewMail );
			update();
	    }
	}
}

void MachLogVoiceMailManager::postNewMail(VoiceMailID id, MexPoint3d position, MachPhys::Race targetRace )
{
	CB_MachLogVoiceMailManager_DEPIMPL();

	if( voiceMailsActivated() and acceptMailPostings_ and MachLogRaces::instance().hasPCRace() and targetRace == MachLogRaces::instance().pcController().race() )
	{
		MachLogVoiceMail* pNewMail = _NEW(MachLogVoiceMail(id, position));
		incomingMailQueue_.push_back(pNewMail);
	}
}

void MachLogVoiceMailManager::postDeathMail( UtlId actorId, MachPhys::Race targetRace )
{
	CB_MachLogVoiceMailManager_DEPIMPL();

	if( voiceMailsActivated() and acceptMailPostings_ and MachLogRaces::instance().hasPCRace() and targetRace == MachLogRaces::instance().pcController().race() )
	{

		bool finished = false;
		int queueSize = incomingMailQueue_.size();
		int indexPos = 0;

		MachLogVoiceMail* pStaticMail = NULL;	// may or may not actually get used

		// if the actor is currently playing a mail, make him stop, and replace it with static.
		// note that the mail MAY have actually finished by now in the interrim period since we last checked,
		// and if this is the case we will simply deal with the finished mail as in the update mthod, and not bother
		// with static bursts.
	   	if( voiceMailPlaying_[ actorId ] )
	   	{
	   		bool found = false;

			while( not found )
			{
				ASSERT( indexPos < queueSize, "Didn't find a mail for the actor on the queue even though that actor was marked as playing a mail!" );

				MachLogVoiceMail* pMail = incomingMailQueue_[indexPos];
				if( pMail->hasActorId() and pMail->actorId() == actorId and pMail->hasStarted() )
				{
					// that's the one

					// ASSERT( pMail->hasStarted(), "Even if it has finished, this mail should vertainly have been registered as having started." );

					// has this mail now finished?
					if(!(pMail->isPlaying()))
					{
						// sample has finished - set the associated "sample playing" slot to false
						ASSERT_INFO( pMail->actorId() );
						ASSERT( pMail->actorId() < MachLogRaces::instance().maxActors(), "SHRIEK! We have an actor ID larger than the expected actor size (and our voiceMailPlaying_ array)" );
						voiceMailPlaying_[ pMail->actorId() ] = false;

						//This mail is about to be discarded so invalidate
						//the sample
						pMail->invalidateSample();

						--noOfMailsPlaying_;

						ASSERT( noOfMailsPlaying_ >= 0, "noOfMailsPlaying_ should never be less than 0." );

						// revoked saved mail queue functionality 10/2/99 CPS
						/*
						// now deal with possibly putting onto the savedmail queue, or deleting it outright.
						if(((*pAvailableVEMails_)[pMail->id()])->mailType_ == VM_FULL_FUNCTION)
						{
							//Time stamp the message for the save buffer
							pMail->timeStamp( SimManager::instance().currentTime() );
							savedMail_.push_back(pMail);
							currentMailMessage_ = savedMail_.size();
						}
						else
						*/
						{
							_DELETE( pMail );
						}

						incomingMailQueue_.erase(incomingMailQueue_.begin() + indexPos);

						--queueSize;
						ASSERT( queueSize == incomingMailQueue_.size(), "Our tracking variable is out of synch with the actual queue size." ) ;

						// note that the queue has reduced in size under our feet, so no need to increment indexPos
  					}
					else
					{
						// it's still playing, and must be stopped and replaced with a static burst.

						// stop it playing. RIGHT now.
						pMail->stop();


						VoiceMailID staticId = VID_INTERFERENCE_0;

						switch( MachPhysRandom::randomInt( 0, 7 ) )
						{
  							case 0:
								staticId = VID_INTERFERENCE_0;
								break;
							case 1:
								staticId = VID_INTERFERENCE_1;
								break;
							case 2:
								staticId = VID_INTERFERENCE_2;
								break;
							case 3:
								staticId = VID_INTERFERENCE_3;
								break;
							case 4:
								staticId = VID_INTERFERENCE_4;
								break;
							default:
								staticId = VID_INTERFERENCE_5;
						}

						pStaticMail = _NEW( MachLogVoiceMail( staticId, actorId ) );
						pStaticMail->play();
						pStaticMail->hasStarted( true );

						ASSERT_INFO( actorId );
						ASSERT( actorId < MachLogRaces::instance().maxActors(), "SHRIEK! We have an actor ID larger than the expected actor size (and our voiceMailPlaying_ array)" );
						//voiceMailPlaying_[ actorId ] = false;

						// okay, now have to dispose of the old mail.

						// revoked saved mail queue functionality 10/2/99 CPS
						/*
						// now deal with possibly putting onto the savedmail queue, or deleting it outright.
						if(((*pAvailableVEMails_)[pMail->id()])->mailType_ == VM_FULL_FUNCTION)
						{
							//Time stamp the message for the save buffer
							pMail->timeStamp( SimManager::instance().currentTime() );
							savedMail_.push_back(pMail);
							currentMailMessage_ = savedMail_.size();
						}
						else
						*/
						{
							_DELETE(pMail);
						}

						// now replace the queue pointer of the interrupted mail to that of the newly-created static burst sample
						incomingMailQueue_[ indexPos ] = pStaticMail;

						// as the queue has not reduced in size under our feet, have to move on to the next entry ourselves
						++indexPos;
					}
					found = true;
				}
				else
					// as the queue has not reduced in size under our feet, have to move on to the next entry ourselves
					++indexPos;
			}

	   	}

		// now kick off any mails still on the queue for that actor, EXCEPT the static one, if there is one.

		// reinitialise values in light of possible changed size of queue, and restart sweep from the beginning
		queueSize = incomingMailQueue_.size();
		indexPos = 0;
		finished = ( indexPos >= queueSize );

		while( not finished )
		{
			ASSERT( indexPos < queueSize, "indexPos exceeded queue bounds!" );

			MachLogVoiceMail* pMail = incomingMailQueue_[indexPos];
			if( pMail->hasActorId() and pMail->actorId() == actorId and pMail != pStaticMail )
			{
				// that's one of ours - just boot it off the queue
				_DELETE(pMail);
				incomingMailQueue_.erase(incomingMailQueue_.begin() + indexPos);

				--queueSize;
				ASSERT( queueSize == incomingMailQueue_.size(), "Our tracking variable is out of synch with the actual queue size." ) ;
			}
			else
				// as the queue has not reduced in size under our feet, have to move on to the next entry ourselves
				++indexPos;

			finished = ( indexPos >= queueSize );
		}
	}
}

void MachLogVoiceMailManager::nMailSlots(size_t newNoOfSlots)
{
	CB_MachLogVoiceMailManager_DEPIMPL();

	savedMailslots_ = newNoOfSlots;
}

size_t MachLogVoiceMailManager::nMailSlots() const
{
	CB_MachLogVoiceMailManager_DEPIMPL();

	return savedMailslots_;
}

bool MachLogVoiceMailManager::currentMailHasActorId() const
{
	CB_MachLogVoiceMailManager_DEPIMPL();

	if(!savedMail_.empty())
	{
		ASSERT(currentMailMessage_ < savedMail_.size(), "Mail Error1");
		return(savedMail_[currentMailMessage_]->actorId());
	}

	return false;
}

UtlId MachLogVoiceMailManager::currentMailActorId() const
{
	CB_MachLogVoiceMailManager_DEPIMPL();

	ASSERT(currentMailMessage_ < savedMail_.size(), "Mail Error2");
	return(savedMail_[currentMailMessage_]->actorId());
}

bool MachLogVoiceMailManager::currentMailHasPosition() const
{
	CB_MachLogVoiceMailManager_DEPIMPL();

	if(!savedMail_.empty())
	{
		ASSERT(currentMailMessage_ < savedMail_.size(), "Mail Error3");
		return(savedMail_[currentMailMessage_]->hasPosition());
	}

	return false;
}

const MexPoint3d MachLogVoiceMailManager::currentMailPosition() const
{
	CB_MachLogVoiceMailManager_DEPIMPL();

	ASSERT(currentMailMessage_ < savedMail_.size(), "Mail Error4");
	return(savedMail_[currentMailMessage_]->position());
}

//static
bool MachLogVoiceMailManager::voiceMailsActivated()
{
	static bool haveVoicemail = getenv("CB_NOVMAIL" ) == NULL;
	return haveVoicemail;
}

void MachLogVoiceMailManager::registerVoiceMailIDs()
{
	CB_MachLogVoiceMailManager_DEPIMPL();

 	ASSERT( not definitionFileRead_, "Mail Error5" );

	// these are specified in vmman2.cpp
	assignMappingGroup1();
	assignMappingGroup2();

	// ********************************* REGISTERED VOICEMAIL TYPES ********************************************

	registeredTypes_.insert( "VM_FULL_FUNCTION", VM_FULL_FUNCTION );
	registeredTypes_.insert( "VM_PART_FUNCTION", VM_PART_FUNCTION );
	registeredTypes_.insert( "VM_TASK_AFFIRMATION", VM_TASK_AFFIRMATION );
	registeredTypes_.insert( "VM_SELECTION_AFFIRMATION", VM_SELECTION_AFFIRMATION );
	registeredTypes_.insert( "VM_WAIT_UNTIL_NOTHING_PLAYING", VM_WAIT_UNTIL_NOTHING_PLAYING );

	pAvailableVEMails_ = _NEW( MailInfoVector( VID_N_MAIL_IDS, NULL ) );
	MailInfoVector& availableVEMails_ = *pAvailableVEMails_;

	for ( MailInfoVector::iterator i = availableVEMails_.begin(); i != availableVEMails_.end(); ++i )
	{
		(*i) = NULL;
	}


	const SysPathName definitionFileName( "sounds/vemail/vemail.dat" );

    SysMetaFile metaFile( "mach1.met" );

    std::auto_ptr< std::istream > pIstream;

    if( SysMetaFile::useMetaFile() )
    {
        //pIstream = _NEW( SysMetaFileIstream( metaFile, definitionFileName, ios::text ) );
        pIstream = std::auto_ptr< std::istream > (
            _NEW( SysMetaFileIstream( metaFile, definitionFileName, std::ios::in ) ));
    }
    else
    {
        ASSERT_FILE_EXISTS( definitionFileName.c_str() );
        //pIstream = _NEW( ifstream( definitionFileName.c_str(), ios::text | ios::in ) );
        pIstream = std::auto_ptr< std::istream > (
            _NEW( std::ifstream( definitionFileName.c_str(), std::ios::in ) ));
    }

    UtlLineTokeniser parser( *pIstream, definitionFileName );
	while( not parser.finished() )
	{
        const UtlLineTokeniser::Tokens& tokens = parser.tokens();
		ASSERT( tokens.size() != 4, "VEmail Definition line size error. Your problem may be that a VEmail definition line must include PRELOAD or NO_PRELOAD as its final parameter." );
		ASSERT( tokens.size() == 5, "VEmail Definition line size error" );
		ASSERT_INFO( tokens[0] );
		VoiceMailID id = registeredIds_[ tokens[0] ];
		ASSERT_INFO( (int)id );
		ASSERT_INFO( _STATIC_CAST( int, VID_N_MAIL_IDS ) );
		ASSERT( id < VID_N_MAIL_IDS, "What the hell? An id larger than the size of the ids collection?" );
		ASSERT( availableVEMails_[ id ] == NULL, "Duplicate VEMail id");

		VoiceMailType type = registeredTypes_[ tokens[2] ];

		std::string fileName(tokens[1]);
		std::transform(fileName.begin(), fileName.end(), fileName.begin(), ::tolower);
//		ASSERT_DATA( SysPathName vemailFileName( tokens[1] ) );
		ASSERT_DATA( SysPathName vemailFileName( fileName ) );
		ASSERT_INFO( vemailFileName );
		ASSERT( vemailFileName.existsAsFile(), "Cannot locate vemail file.");

		availableVEMails_[ id ] =
			_NEW(MachLogVoiceMailInfo
//			( id, tokens[1], type, atoi(tokens[3].c_str())));
			( id, fileName, type, atoi(tokens[3].c_str())));


		ASSERT_INFO( tokens[4] );
		ASSERT( tokens[4] == "PRELOAD"
				or tokens[4] == "NO_PRELOAD",
				"VEmail definition line must have PRELOAD or NO_PRELOAD as its final parameter." );

		if( tokens[4] == "PRELOAD" )
//			SndMixer::instance().loadWaveform( SndWaveformId( tokens[1] ) );
			SndMixer::instance().loadWaveform( SndWaveformId( fileName ) );

	    parser.parseNextLine();
		definitionFileRead_ = true;
	}
	POST( definitionFileRead_ );
}

void MachLogVoiceMailManager::CLASS_INVARIANT
{
	CB_MachLogVoiceMailManager_DEPIMPL();

    INVARIANT( this != NULL );
}

const MachLogVoiceMailManager::VEmailIDMap& MachLogVoiceMailManager::veMailIDMap() const
{
	CB_MachLogVoiceMailManager_DEPIMPL();

	return registeredIds_;
}

MachLogVoiceMailManager::MailInfoVector* MachLogVoiceMailManager::pAvailableVEMails()
{
	CB_MachLogVoiceMailManager_DEPIMPL();

	return pAvailableVEMails_;
}

ostream& operator <<( ostream& o, const MachLogVoiceMailManager& t )
{

    o << "MachLogVoiceMailManager " << (void*)&t << " start" << std::endl;
    o << "MachLogVoiceMailManager " << (void*)&t << " end" << std::endl;

    return o;
}

void MachLogVoiceMailManager::acceptMailPostings( bool status )
{
	CB_MachLogVoiceMailManager_DEPIMPL();

	acceptMailPostings_ = status;
}

bool MachLogVoiceMailManager::acceptMailPostings() const
{
	CB_MachLogVoiceMailManager_DEPIMPL();

	return acceptMailPostings_;
}

void MachLogVoiceMailManager::clearMailQueue()
{
	CB_MachLogVoiceMailManager_DEPIMPL();

	while( not incomingMailQueue_.empty() )
	{
		MachLogVoiceMailManager::MailVector::iterator i = incomingMailQueue_.begin();
		MachLogVoiceMail* pMail = *i;

		if( pMail->hasStarted() )
		{
			if( pMail->isPlaying() )
			{
				// stop it (will automatically deallocate resources)
				pMail->stop();
			}
			else
			{
				// finished playing - need to deallocate its resources anyway.
				pMail->invalidateSample();
			}
		}

		_DELETE( pMail );
		incomingMailQueue_.erase( i );
	}

	// and ensure that all actors are registered as not playing any voicemails
	int totalPossibleActors = MachLogRaces::instance().maxActors();
	for( int actorLoop = 0; actorLoop < totalPossibleActors; ++actorLoop )
		voiceMailPlaying_[ actorLoop ] = false;

	podMailPlaying_ = false;
	noOfMailsPlaying_ = 0;
}

//Force recompile 01/02/99 CPS
/* End VMMAN.CPP ****************************************************/
