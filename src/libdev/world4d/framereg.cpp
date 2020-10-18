
/*
 * F R A M E R E G . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/framereg.hpp"
#include "world4d/manager.hpp"

#ifndef _INLINE
    #include "world4d/framereg.ipp"
#endif

PER_DEFINE_PERSISTENT( W4dFrameRegulator );

W4dFrameRegulator::W4dFrameRegulator
(
    const PhysAbsoluteTime& startTime, const PhysRelativeTime& idealDuration,
    const PhysRelativeTime& maxDuration, size_t nFrames
)
:   startTime_( startTime ),
    idealDuration_( idealDuration ),
    maxDuration_( maxDuration ),
    startFrame_( 0 ),
    nFrames_( nFrames )
{

    TEST_INVARIANT;
}

W4dFrameRegulator::W4dFrameRegulator( PerConstructor )
{
}

W4dFrameRegulator::~W4dFrameRegulator()
{
    TEST_INVARIANT;

}

void W4dFrameRegulator::CLASS_INVARIANT
{
    INVARIANT( startTime_ >= 0.0 );
    //INVARIANT( maxDuration_ >= idealDuration_ );
}

ostream& operator <<( ostream& o, const W4dFrameRegulator& t )
{

    o << "W4dFrameRegulator " << (void*)&t << " start" << std::endl;
    o << "  startTime " << t.startTime_ << std::endl;
    o << "  idealDuration " << t.idealDuration_ << std::endl;
    o << "  maxDuration " << t.maxDuration_ << std::endl;
    o << "  startFrame " << t.startFrame_ << std::endl;
    o << "  nFrames " << t.nFrames_ << std::endl;
    o << "W4dFrameRegulator " << (void*)&t << " end" << std::endl;

    return o;
}

PhysAbsoluteTime W4dFrameRegulator::effectiveTime() const
{
    TEST_INVARIANT;

    //Get current time
    PhysAbsoluteTime timeNow = W4dManager::instance().time();
    PhysAbsoluteTime result;

    //Check for past start of animation
    if( timeNow >= startTime_ )
    {
        if( timeNow >= startTime_ + maxDuration_ )
        {
            //Past the end of the allowed time for the animation.
            //Return theoretical ideal end time
            result = startTime_ + idealDuration_;
        }
        else
        {
            //Get current frame, and check we have started the animation
            size_t simulationFrame = W4dManager::instance().frameNumber();
            if( startFrame_ == 0 )
            {
                //This is the first frame of the animation.
                //Store the starting frame, and return the start time.
                W4dFrameRegulator* nonConstThis = _CONST_CAST( W4dFrameRegulator*, this );
                nonConstThis->startFrame_ = simulationFrame;
                nonConstThis->startTime_ = timeNow;
                result = startTime_;
            }
            else
            {
                //Compute the effective frame offset. We mustn't go past the number
                //of frames in the animation.
                //Hence compute the effective frame time
                size_t frameOffset = simulationFrame - startFrame_;
                if( frameOffset >= nFrames_ )
                    result = startTime_ + idealDuration_;
                else
                    result = startTime_ +
                             idealDuration_ * (_STATIC_CAST( double, frameOffset ) /
                                               _STATIC_CAST( double, nFrames_));
            }
        }
    }
    else
    {
        //Before start of animation, so return current time
        result = timeNow;
    }

    return result;
}

void perWrite( PerOstream& ostr, const W4dFrameRegulator& regulator )
{
    ostr << regulator.startTime_;
    ostr << regulator.idealDuration_;
    ostr << regulator.maxDuration_;
    ostr << regulator.startFrame_;
    ostr << regulator.nFrames_;
}

void perRead( PerIstream& istr, W4dFrameRegulator& regulator )
{
    istr >> regulator.startTime_;
    istr >> regulator.idealDuration_;
    istr >> regulator.maxDuration_;
    istr >> regulator.startFrame_;
    istr >> regulator.nFrames_;
}

/* End FRAMEREG.CPP *************************************************/
