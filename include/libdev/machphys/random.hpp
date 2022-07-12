/*
 * R A N D O M . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysRandom

    A brief description of the class should go in here
*/

#ifndef _MACHPHYS_RANDOM_HPP
#define _MACHPHYS_RANDOM_HPP

#include "base/base.hpp"

#include "mathex/mathex.hpp"

class MachPhysRandom
{
public:
    MachPhysRandom();
    ~MachPhysRandom();

    static  double randomDouble( MATHEX_SCALAR lowerLimit, MATHEX_SCALAR upperLimit );
    //  PRE( lowerLimit <= upperLimit );
    
    static  int randomInt( int lowerLimit, int upperLimit );
    //  PRE( lowerLimit < upperLimit );
    //  POST( lowerLimit <= result and result < upperLimit );

    static  size_t randomInt( size_t lowerLimit, size_t upperLimit );
    //  PRE( lowerLimit <= upperLimit );
    //  POST( lowerLimit <= result and result < upperLimit );

    static  size_t randomInt( size_t upperLimit );
    //  POST( 0 <= result and result < upperLimit );

    void CLASS_INVARIANT;

private:
    // Operation deliberately revoked
    MachPhysRandom( const MachPhysRandom& );

    // Operation deliberately revoked
    MachPhysRandom& operator =( const MachPhysRandom& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysRandom& );

};


#endif

/* End RANDOM.HPP ***************************************************/
