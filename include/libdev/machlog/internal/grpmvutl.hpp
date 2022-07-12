/*
 * G R P M V U T L . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogGroupMoverUtility

    Useful utility functions to help out with the group move code.
    Split a set of actors up into clumps
*/

#ifndef _MACHLOG_GRPMVUTL_HPP
#define _MACHLOG_GRPMVUTL_HPP

#include "base/base.hpp"
#include "ctl/vector.hpp"
#include "ctl/utility.hpp"
#include "mathex/mathex.hpp"
#include "mathex/point2d.hpp"

class MachLogMachine;

class MachLogGroupMoverUtility
// Canonical form revoked
{
public:
    typedef ctl_vector< MachLogMachine* > Machines;    	

    //  Split the actors up into clumps where each actor in a
    //  clump is within distance of at least one other actor.
    MachLogGroupMoverUtility( const Machines& actors, MATHEX_SCALAR distance );
    // PRE( actors.size() != 0 );
    
    ~MachLogGroupMoverUtility();

    uint nClumps() const;
    
    // Return the index to the largest clump
    uint largestClumpIndex() const;
    
    CTL_TRIPLE( Clump,
        Machines,       machines,
        MexPoint2d,     centroid,
        MATHEX_SCALAR,  radius );

    const Clump& clump( uint index ) const;
    // PRE( index < nClumps() );

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachLogGroupMoverUtility& t );

    MachLogGroupMoverUtility( const MachLogGroupMoverUtility& );
    MachLogGroupMoverUtility& operator =( const MachLogGroupMoverUtility& );

    void createClumps( const Machines& actors );
    size_t addToClump( Machines* pClump, Machines* pMachines );
    void calculateCentroidAndRadius( 
      const Machines& actors,
      MexPoint2d* pCentroid,
      MATHEX_SCALAR* pRadius ) const;

    ctl_vector< Clump >    clumps_;
    uint    largestClumpIndex_;
    MATHEX_SCALAR   distance_;
};


#endif

/* End GRPMVUTL.HPP *************************************************/
