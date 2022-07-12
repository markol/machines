/*
 * T I M E R . H P P 
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */


#ifndef _TIMER_HPP
#define _TIMER_HPP

#include "base/base.hpp"

class DevTimer
{
public:

	enum Direction	{ COUNT_UP,	COUNT_DOWN };

    DevTimer( Direction direction = COUNT_UP );
    ~DevTimer();

	double	time () const;
    //  The "NoRecord" function is only suppied to allow library
    //  to work without making any non-repeatable recorded calls to the
    //  timers. It should not normally be used.
	double	timeNoRecord() const;
	void	time ( double newTime );
	void	timeNoRecord ( double newTime );

	void    pause();
	void    resume();

    bool    paused() const;
    
	Direction	direction () const;
	void        direction ( Direction newDirection );

    friend ostream& operator <<( ostream& o, const DevTimer& t );

private:
    // Operation deliberately revoked
    DevTimer( const DevTimer& );

    // Operation deliberately revoked
    const DevTimer& operator =( const DevTimer& );

    // Operation deliberately revoked
    bool operator ==( const DevTimer& ) const;

    bool        paused_;
	Direction	direction_;
	double		offset_;

};

#endif

/* End TIMER.HPP ****************************************************/
