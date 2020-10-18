/*
 * R A T E . H P P
 * (c) Charybdis Limited, 1995. All Rights Reserved
 */

/*
    DevRate
    
    Calculate the rate at which one quantity (known here as 
    the numerator) is changing with respect to another (the
    denominator). E.g. If we were writing a file to disk and
    wanted to calculate bytes / second the numerator would be
    bytes, the denominator would be seconds
*/

#ifndef _DEVICE_RATE_HPP
#define _DEVICE_RATE_HPP

///////////////////////////////////

#include "base/base.hpp"
#include "ctl/utility.hpp"
#include "ctl/vector.hpp"

///////////////////////////////////

class DevRate
{
public:

    //  Define the interval over which the rate will be calculated.
	DevRate( double denominatorInterval );
	~DevRate();

    void update( double numerator, double denominator );

    //  The rate is not defined until two updates have been
    //  received
    bool    rateDefined() const;
	double rate() const;
    //  PRE( rateDefined() );

private:

	// copy-ctor, assignment and equivalence operations intentionally revoked
	DevRate( const DevRate& );
	const DevRate& operator =( const DevRate& );
	bool operator ==( const DevRate& ) const;

    //  This is a friend purely to allow template instantiation
    friend void devDummyFunction( void );

    enum { N_READINGS = 10 };

    CTL_PAIR( Reading, double, numerator, double, denominator );

    typedef ctl_vector< Reading >   Readings;
    
    uint lastIndex() const;
    const Reading& lastReading() const;
    //  PRE( readings_.size() > 0 );

    Readings    readings_;

    double  updateInterval_;
    uint    index_;
    double  rate_;
    
//     bool    rateDefined_;
//     bool    lastValuesSet_;
//     double  denominatorInterval_;
//     double  lastNumerator_;
//     double  lastDenominator_;
//     double  rate_;

};

///////////////////////////////////

#endif		/* 	_DevRate_HPP 	*/