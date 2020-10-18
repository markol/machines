/*
 * S E Q D E B U G . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
	Macros for helping debug the motion sequencer
*/

#ifndef _MACHLOG_SEQDEBUG_HPP
#define _MACHLOG_SEQDEBUG_HPP

#include "base/base.hpp"

#ifdef PRODUCTION
    #define CHANGE_STATE( STATE, REASON )   internalState_ = STATE
    #define LOG_STREAM( exp )
    #define LOG_STREAM2( exp, pThis )
    #define LOG_INSPECT( exp )
    #define LOG_WHERE
    #define LOG_ENTER( exp )
    #define LOG_EXIT( exp )
    #define LOG_INDENT( n )
    #define LOG_INDENT2( n, pThis )
    #define LOG_ENABLED false
#else
    #define LOG_STREAM( exp )       \
        MOTSEQ_STREAM( exp ),       \
        pImpl_->logStreamOn_ ? ( pImpl_->logStream_ << exp, 0 ) : 1

    #define LOG_STREAM2( exp, pThis )       \
        MOTSEQ_STREAM( exp ),       \
        pThis->pImpl_->logStreamOn_ ? ( pThis->pImpl_->logStream_ << exp, 0 ) : 1

    #define LOG_INDENT( n )                 \
     ( MOTSEQ_INDENT( n ),                  \
     pImpl_->logStreamOn_ ? ( pImpl_->indent( n ), 0 ) : 1 )

    #define LOG_INDENT2( n, pThis )         \
     ( MOTSEQ_INDENT( n ),                  \
      pThis->pImpl_->logStreamOn_ ? ( pThis->pImpl_->indent( n ), 0 ) : 1 )

    #define LOG_INSPECT( exp ) MOTSEQ_INSPECT( exp ),       \
        pImpl_->logStreamOn_ ? ( INSPECT_ON( pImpl_->logStream_, exp ), 0 ) : 1

    #define LOG_WHERE   LOG_STREAM( WHERE_STR << std::endl )

    #define CHANGE_STATE( STATE, REASON )                       \
        LOG_STREAM( SimManager::instance().currentTime() << "  " ),    \
        LOG_STREAM( internalState_ << " -> " << STATE << "   " << REASON << std::endl ),    \
        internalState_ = STATE,                                 \
        pImpl_->reasonForStateChange( REASON )

    #define LOG_ENTER( fn ) MachLogMotionSequencerEntryExit entryExit( fn, this, pImpl_->pLogMobile_->id() )

    #define LOG_ENABLED ( MOTSEQ_STREAM_ENABLED or pImpl_->logStreamOn_ )

#endif

class MachLogMotionSequencerEntryExit
{
public:
	MachLogMotionSequencerEntryExit( const char* functionName, const MachLogMachineMotionSequencer* pThis, W4dId id )
	: functionName_( functionName ),
	  pThis_( pThis ),
	  id_( id )
	{
	    LOG_STREAM2( "Enter MLMachineMotionSequencer::" << functionName_, pThis_ );
		LOG_STREAM2( "  " << (void*)pThis_ << "  " << id_ << std::endl, pThis_ );
	    LOG_INDENT2( 2, pThis_ );
	}

	~MachLogMotionSequencerEntryExit()
	{
	    LOG_INDENT2( -2, pThis_ );
	    LOG_STREAM2( "Exit MLMachineMotionSequencer::" << functionName_, pThis_ );
		LOG_STREAM2( "  " << (void*)pThis_ << "  " << id_ << std::endl, pThis_ );
	}
private:
	const char*     functionName_;
	const MachLogMachineMotionSequencer*		pThis_;
	const W4dId		id_;
};


#endif

/* End SEQDEBUG.HPP **************************************************/
