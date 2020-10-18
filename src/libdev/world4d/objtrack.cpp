/*
 * O B J T R A C K . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/objtrack.hpp"
#include "world4d/manager.hpp"
#include "world4d/entity.hpp"

#ifndef _INLINE
    #include "world4d/objtrack.ipp"
#endif

PER_DEFINE_PERSISTENT( W4dObjectTracker );

W4dObjectTracker::W4dObjectTracker
(
    const W4dEntity& parent, const MexPoint3d& localOrigin,
    const W4dEntity& target, const MexPoint3d& targetOffset,
    MATHEX_SCALAR maxAbsYawSine, MATHEX_SCALAR maxAbsPitchSine,
    MATHEX_SCALAR minLength, MATHEX_SCALAR maxLength,
    MATHEX_SCALAR defaultLength
)
:   pParent_( &parent ),
    localOrigin_( localOrigin ),
    pTarget_( &target ),
    targetOffset_( targetOffset ),
    maxAbsYawSine_( maxAbsYawSine ),
    maxAbsPitchSine_( maxAbsPitchSine ),
    yawDeltaSine_( 0.0 ),
    yawDeltaCosine_( 1.0 ),
    minLength_( minLength ),
    maxLength_( maxLength ),
    defaultLength_( defaultLength ),
    lastUpdateTime_( -1.0 )
{
    PRE( maxAbsYawSine >= 0.0  and  maxAbsYawSine <= 1.0 );
    PRE( maxAbsPitchSine >= 0.0  and  maxAbsPitchSine <= 1.0 );
    PRE( minLength <= maxLength );
    PRE( defaultLength >=  minLength and defaultLength <= maxLength );
    TEST_INVARIANT;
}

W4dObjectTracker::W4dObjectTracker( PerConstructor )
:   lastUpdateTime_( -1.0 )
{
}

W4dObjectTracker::~W4dObjectTracker()
{
    TEST_INVARIANT;
}

void W4dObjectTracker::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const W4dObjectTracker& t )
{

    o << "W4dObjectTracker " << (void*)&t << " start" << std::endl;
    o << "W4dObjectTracker " << (void*)&t << " end" << std::endl;

    return o;
}

MATHEX_SCALAR W4dObjectTracker::track( MexTransform3d* pTrackTransform ) const
{
    PRE( pTrackTransform != NULL );

    //Get current time and see if already done it
    PhysAbsoluteTime now = W4dManager::instance().time();
    if( now != lastUpdateTime_ )
    {
        //Update the cached data
        W4dObjectTracker* nonConstThis = _CONST_CAST( W4dObjectTracker*, this );
        nonConstThis->lastUpdateTime_ = now;

        //Get the current global position of the target point
        MexPoint3d nowTarget( targetOffset_ );
        pTarget_->globalTransform().transform( &nowTarget );

        //Produce unmodified global transform for the tracker
        MexTransform3d translateTransform( localOrigin_ );
        pParent_->globalTransform().transform( translateTransform,
                                               &nonConstThis->trackerTransform_ );

        //Hence compute the target coordinates relative to this unmodified frame
        trackerTransform_.transformInverse( &nowTarget );

        //Compute the extra rotation transform, and get distance from target
        MexTransform3d rotationTransform;
        MATHEX_SCALAR targetDistance =
            track( nowTarget, &rotationTransform,
                   maxAbsYawSine_, maxAbsPitchSine_,
                   yawDeltaSine_, yawDeltaCosine_ );

        //Combine the translation and rotation parts of the final transform
        translateTransform.transform( rotationTransform, &nonConstThis->trackerTransform_ );

        //Set the length within limits
        nonConstThis->trackerLength_ = (targetDistance > maxLength_ ? maxLength_ :
                                            (targetDistance < minLength_ ? minLength_ :
                                                targetDistance) );
    }

    //return the results
    *pTrackTransform = trackerTransform_;
    return trackerLength_;
}

//static
MATHEX_SCALAR W4dObjectTracker::track
(
    const MexPoint3d& target, MexTransform3d* pTrackTransform,
    MATHEX_SCALAR maxAbsYawSine, MATHEX_SCALAR maxAbsPitchSine,
    MATHEX_SCALAR yawDeltaSine, MATHEX_SCALAR yawDeltaCosine
)
{
    //Compute horizontal distance to target
    MATHEX_SCALAR qx = target.x();
    MATHEX_SCALAR qy = target.y();
    MATHEX_SCALAR qz = target.z();
    MATHEX_SCALAR hSqrd = qx*qx + qy*qy;
    MATHEX_SCALAR h = sqrt( hSqrd );

    //Hence compute the yaw angle
    MATHEX_SCALAR eps = MexEpsilon::instance();
    MATHEX_SCALAR yawSine, yawCosine;

    if( h > eps  and  maxAbsYawSine > 0.0)
    {
        yawSine = qy / h;
        if( fabs( yawSine ) <= maxAbsYawSine )
        {
            yawCosine = qx / h;
        }
        else
        {
            yawSine = (yawSine >= 0.0 ? maxAbsYawSine : -maxAbsYawSine);
            yawCosine = sqrt( 1.0 - yawSine * yawSine );
        }
    }
    else
    {
        yawSine = 0.0;
        yawCosine = 1.0;
    }

    //Amend if required using the delta angle
    if( yawDeltaSine != 0.0  or  yawDeltaCosine != 1.0 )
    {
        MATHEX_SCALAR temp = yawSine*yawDeltaCosine + yawCosine*yawDeltaSine;
        yawCosine = yawCosine*yawDeltaCosine - yawSine*yawDeltaSine;
        yawSine = temp;
    }

    //Now compute distance to target
    MATHEX_SCALAR l = sqrt( hSqrd + qz*qz );

    //Now the pitch angle
    MATHEX_SCALAR pitchSine, pitchCosine;

    if( l > eps  and  maxAbsPitchSine > 0.0 )
    {
        pitchSine = -qz / l;
        if( fabs( pitchSine ) <= maxAbsPitchSine )
        {
            pitchCosine = h / l;
        }
        else
        {
            pitchSine = (pitchSine >= 0.0 ? maxAbsPitchSine : -maxAbsPitchSine);
            pitchCosine = sqrt( 1.0 - pitchSine * pitchSine );
        }
    }
    else
    {
        pitchSine = 0.0;
        pitchCosine = 1.0;
    }

    //Compute the rotation transform
    MexVec3 xBasis( yawCosine*pitchCosine, yawSine*pitchCosine, -pitchSine );
    MexVec3 yBasis( -yawSine, yawCosine, 0.0 );
    MexVec3 zBasis( yawCosine*pitchSine, yawSine*pitchSine, pitchCosine );
    *pTrackTransform = MexTransform3d( xBasis, yBasis, zBasis, MexPoint3d() );

    return l;
}

void perWrite( PerOstream& ostr, const W4dObjectTracker& tracker )
{
    ostr << tracker.pParent_;
    ostr << tracker.localOrigin_;
    ostr << tracker.pTarget_;
    ostr << tracker.targetOffset_;
    ostr << tracker.maxAbsYawSine_;
    ostr << tracker.maxAbsPitchSine_;
    ostr << tracker.yawDeltaSine_;
    ostr << tracker.yawDeltaCosine_;
    ostr << tracker.minLength_;
    ostr << tracker.maxLength_;
    ostr << tracker.defaultLength_;
}

void perRead( PerIstream& istr, W4dObjectTracker& tracker )
{
    PER_READ_CONST_OBJECT( istr, W4dEntity*, tracker.pParent_ );
    istr >> tracker.localOrigin_;
    PER_READ_CONST_OBJECT( istr, W4dEntity*, tracker.pTarget_ );
    istr >> tracker.targetOffset_;
    istr >> tracker.maxAbsYawSine_;
    istr >> tracker.maxAbsPitchSine_;
    istr >> tracker.yawDeltaSine_;
    istr >> tracker.yawDeltaCosine_;
    istr >> tracker.minLength_;
    istr >> tracker.maxLength_;
    istr >> tracker.defaultLength_;
}

/* End OBJTRACK.CPP *************************************************/
