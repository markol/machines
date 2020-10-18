/*
 * S E Q D A T A . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <stdlib.h>
#include <stdio.h>
#include "machlog/internal/seqdata.hpp"

//  TBD: Sort out a better way of doing this
static const size_t NO_ID = _STATIC_CAST( size_t, -1 );

MachLogMachineMotionSequencerData::MachLogMachineMotionSequencerData( int id )
: commandId_( NO_ID ),
  keepEntranceLocked_( false ),
  internalState_( MachLogMachineMotionSequencer::InternalState::INTERNAL_RESTING ),
  ignoringPolygons_( false ),
  logStreamOn_( false ),
  logStream_( str_, "" ),
  indentLevel_( 0 ),
  lastSetDomainTimeMessage_( 0 ),
  offsetValid_( false ),
  swerveRecursionCount_( 0 ),
  shuffleEndTime_( 0 )

#ifndef PRODUCTION
  ,reasonForStateChange_( "no change yet" )
#endif
{
    moveInfos_.reserve( 8 );
    motionChunkIds_.reserve( 8 );

    Diag::instance().addCrashStream( str_ );

    //  Set up the pathname for the log stream and clear it.
    char str[ 200 ];
    sprintf( str, "ms%06d", id );
    //logFilePathName_ = str + string( ".log" );
    logFilePathName_ = SysPathName( str + string( ".log" ) );

    remove( logFilePathName_.c_str() );

    string environmentVariable = "cb_";
    environmentVariable += str;
    environmentVariable += "_log";

    if( getenv( "cb_mseq_log" ) or getenv( environmentVariable.c_str() ) )
        toggleLogging();

    TEST_INVARIANT;
}

MachLogMachineMotionSequencerData::~MachLogMachineMotionSequencerData()
{
    TEST_INVARIANT;
    Diag::instance().removeCrashStream( str_ );
}

void MachLogMachineMotionSequencerData::commandId( size_t id )
{
    commandId_ = id;
}

bool MachLogMachineMotionSequencerData::commandIdSet() const
{
    return commandId_ != NO_ID;
}

void MachLogMachineMotionSequencerData::clearCommandId()
{
    commandId_ = NO_ID;

    POST( not commandIdSet() );
}

size_t MachLogMachineMotionSequencerData::commandId() const
{
    return commandId_;
}

void MachLogMachineMotionSequencerData::indent( int n )
{
    indentLevel_ += n;

    if( indentLevel_ < 0 )
        indentLevel_ = 0;

    logStream_.indentString( " ", indentLevel_ );
}

#ifndef PRODUCTION

void MachLogMachineMotionSequencerData::reasonForStateChange( const string& reason )
{
    reasonForStateChange_ = reason;
}

#endif

void MachLogMachineMotionSequencerData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogMachineMotionSequencerData& t )
{
#ifndef PRODUCTION
    o << "  Reason for last state change: " << t.reasonForStateChange_ << std::endl;
#endif

    return o;
}

bool MachLogMachineMotionSequencerData::keepEntranceLocked() const
{
    return keepEntranceLocked_;
}

void MachLogMachineMotionSequencerData::keepEntranceLocked( bool keep )
{
    keepEntranceLocked_ = keep;
}

void MachLogMachineMotionSequencerData::toggleLogging()
{
    if( logStreamOn_ )
    {
        logStreamOn_ = false;
        str_.close();
    }
    else
    {
        logStreamOn_ = true;
        str_.open( logFilePathName_.c_str(), std::ios::app );
    }
}

void MachLogMachineMotionSequencerData::calculateOffset( const MexVec2& directionOfTravel )
{
    if( groupMoveInfo_.valid() )
    {
        if( directionOfTravel.isZeroVector() )
            offsetValid_ = false;
        else if( groupMoveInfo_.offset().isZeroVector() )
        {
            offsetAngle_ = 0.0;
            offsetDistance_ = 0.0;
            offsetValid_ = true;
        }
        else
        {
            offsetAngle_ = directionOfTravel.angleBetween( groupMoveInfo_.offset() );
            offsetDistance_ = groupMoveInfo_.offset().modulus();
            offsetValid_ = true;
        }
    }
    else
    {
        offsetValid_ = false;
    }
}

/* End SEQDATA.CPP **************************************************/
