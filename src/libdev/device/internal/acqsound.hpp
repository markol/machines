/*
 * A C Q S O U N D . H P P 
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */

/*
    This class exists to make sure that the AIL sound library is
    opened once and only once, and closed once and only once.
*/

#ifndef _ACQSOUND_HPP
#define _ACQSOUND_HPP

#include "base/base.hpp"

class AcquireAILSound

{
public:
    static  void    instance();
    
    ~AcquireAILSound();

private:
    // Operation deliberately revoked
    AcquireAILSound( const AcquireAILSound& );

    // Operation deliberately revoked
    const AcquireAILSound& operator =( const AcquireAILSound& );

    // Operation deliberately revoked
    bool operator ==( const AcquireAILSound& ) const;

    //  Singleton class
    AcquireAILSound();
};

#endif
