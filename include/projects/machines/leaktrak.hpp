/*
 * L E A K T R A K . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    LeakTracker

    Debugging class used to help track memory leaks
*/

#ifndef _LEAKTRAK_HPP
#define _LEAKTRAK_HPP

#include "base/base.hpp"
#include "device/keyboard.hpp"

//static methods only
class LeakTracker
{
public:
    enum Action { INLOOP, SAMPLE, TERMINATE };
	enum SampleRate { NONE = 0, FAST = 10, MEDIUM = 25, SLOW = 200 };

    static void update( Action action, SampleRate );

private:
    // Operations deliberately revoked
    LeakTracker( void );
    ~LeakTracker( void );
    LeakTracker( const LeakTracker& );
    LeakTracker& operator =( const LeakTracker& );
    bool operator ==( const LeakTracker& );

};


#endif

/* End LEAKTRAK.HPP *************************************************/
