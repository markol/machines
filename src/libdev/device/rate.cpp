/*
 * R A T E . C P P
 * (c) Charybdis Limited, 1995. All Rights Reserved
 */

#include "device/rate.hpp"

///////////////////////////////////

DevRate::DevRate( double denominatorInterval )
: updateInterval_( denominatorInterval / N_READINGS ),
  index_( 0 )
{
    readings_.reserve( N_READINGS );
}

DevRate::~DevRate()
{
	/* intentionally Empty */
}

void DevRate::update( double numerator, double denominator )
{
    if( readings_.size() == 0 )
    {
        readings_.push_back( Reading( numerator, denominator ) );
    }
    else
    {
        Reading last = lastReading();
        
        if( denominator > last.denominator() + updateInterval_ )
        {
            Reading reading( numerator, denominator );

            index_ = ( index_ + 1 ) % N_READINGS;

            if( readings_.size() == N_READINGS )
            {
                readings_[ index_ ] == reading;
            }
            else
            {
                readings_.push_back( reading );
            }

            rate_ = ( numerator - last.numerator() ) /
              ( denominator - last.denominator() );
        }
    }
}

bool DevRate::rateDefined() const
{
	return readings_.size() >= 2;
}

double DevRate::rate() const
{
    PRE( rateDefined() );

	return rate_;
}

const DevRate::Reading& DevRate::lastReading() const
{
    PRE( readings_.size() > 0 );

    return readings_[ index_ ];
}