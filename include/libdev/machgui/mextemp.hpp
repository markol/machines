/*
 * M E X T E M P . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MexTemp

    All these functions should be moved to mathex at a convenient point -
    put in here temporarily to save several hours recompile.
*/

#ifndef _MACHGUI_MEXTEMP_HPP
#define _MACHGUI_MEXTEMP_HPP

#include "base/base.hpp"

class MexAlignedBox2d;

class MexTemp
{
public:
    MexTemp( void );
    ~MexTemp( void );

    //Rotate the boundary of pBox by n times 90 degrees
    static void quarterRotate( MexAlignedBox2d* pBox, int n );

private:
    // Operation deliberately revoked
    MexTemp( const MexTemp& );

    // Operation deliberately revoked
    MexTemp& operator =( const MexTemp& );

    // Operation deliberately revoked
    bool operator ==( const MexTemp& );

};


#endif

/* End MEXTEMP.HPP **************************************************/
