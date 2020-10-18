/*
 * E U L E R A N G . C P P
 * (c) Charybdis Limited, 1995 - 1997. All Rights Reserved
 */

/////////////////////////////////////////////////////////

#include "base/base.hpp"

#include "mathex/mathex.hpp"
#include "mathex/eulerang.hpp"

#ifndef  _INLINE
    #include "mathex/eulerang.ipp"
#endif
    

MexEulerAngles::MexEulerAngles( const MexRadians& azimuth, const MexRadians& elevation, const MexRadians& roll )
:   azimuth_( azimuth ),
    elevation_( elevation ),
    roll_( roll )
{
    /* intentionally Empty */    
}


MexEulerAngles::MexEulerAngles( const MexEulerAngles& v )
:   azimuth_( v.azimuth_ ),
    elevation_( v.elevation_ ),
    roll_( v.roll_ )
{
    /* intentionally Empty */
}


MexEulerAngles::~MexEulerAngles()
{
    /* intentionally Empty */
}


const MexEulerAngles& MexEulerAngles::operator =( const MexEulerAngles& rhs )
{
    if( this != &rhs )
    {
        azimuth_ = rhs.azimuth_;
        elevation_ = rhs.elevation_;
        roll_ = rhs.roll_;
    }

    return *this;
}


bool  MexEulerAngles::operator ==( const MexEulerAngles& v ) const
{
    return azimuth_ == v.azimuth_ and elevation_ == v.elevation_ and roll_ == v.roll_;
}

ostream& operator <<( ostream& o, const MexEulerAngles& t )
{
    o << "( " << t.azimuth_ << " " << t.elevation_ << " " << t.roll_ << " )";
    
    return o;
}


PER_DEFINE_PERSISTENT( MexEulerAngles );

void perWrite( PerOstream& str, const MexEulerAngles& t )
{
    str << t.azimuth_;
    str << t.elevation_;
    str << t.roll_;
}

void perRead( PerIstream& str, MexEulerAngles& t )
{
    str >> t.azimuth_;
    str >> t.elevation_;
    str >> t.roll_;
}

