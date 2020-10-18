/*
 * R E C O R D E R . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "recorder/recorder.hpp"
#include "recorder/private/recpriv.hpp"
#include "recorder/internal/recimpl.hpp"

// static
RecRecorder& RecRecorder::instance()
{
    static RecRecorder instance_;
    return instance_;
}

RecRecorder::RecRecorder()
{
    RecRecorderImplementation::instance();
    RecRecorderPrivate::instance();

    TEST_INVARIANT;
}

RecRecorder::~RecRecorder()
{
    TEST_INVARIANT;

}

RecRecorder::State RecRecorder::state() const
{
    return RecRecorderImplementation::instance().state();
}

void RecRecorder::recordingAllowed( bool val )
{
    RecRecorderImplementation::instance().recordingAllowed( val );
}

void RecRecorder::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const RecRecorder& )
{
    o << RecRecorderImplementation::instance();

    return o;
}

double RecRecorder::percentageComplete() const
{
	PRE( state() == PLAYING );

    double result = RecRecorderImplementation::instance().percentageComplete();

	POST( result >= 0.0 );
	POST( result <= 100.0 );

	return result;
}

/* End RECORDER.CPP *************************************************/
