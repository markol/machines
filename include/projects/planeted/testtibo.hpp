/*
 * T E S T T I B O . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysTestTileBoundary

    Tests the MachPhysTileBoundary class
*/

#ifndef _TESTTIBO_HPP
#define _TESTTIBO_HPP

#include "base/base.hpp"

class MachPhysTestTileBoundary
{
public:
    MachPhysTestTileBoundary();
    ~MachPhysTestTileBoundary();

    void CLASS_INVARIANT;

private:
    // Operation deliberately revoked
    MachPhysTestTileBoundary( const MachPhysTestTileBoundary& );
    MachPhysTestTileBoundary& operator =( const MachPhysTestTileBoundary& );
    bool operator ==( const MachPhysTestTileBoundary& );
};


#endif

/* End TESTTIBO.HPP *************************************************/
