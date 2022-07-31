/*
 * S P I R A L . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogSquareSpiral

    Generate a spiral based on a square grid. Start off with
    a point in the centre, then each application of the ++ operator 
    will produce the next point on the grid working outwards in a
    series of squares.
*/

#ifndef _MACHLOG_SPIRAL_HPP
#define _MACHLOG_SPIRAL_HPP

#include "base/base.hpp"

class MachLogSquareSpiral
// Canonical form revoked
{
public:
    MachLogSquareSpiral();
    ~MachLogSquareSpiral();

    int x() const;
    int y() const;
    
    void operator ++( PREFIX );
    void operator ++( POSTFIX );    

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachLogSquareSpiral& t );

    MachLogSquareSpiral( const MachLogSquareSpiral& );
    MachLogSquareSpiral& operator =( const MachLogSquareSpiral& );

    int x_;
    int y_;
    int edgeLength_;
    int gridIndex_;
};


#endif

/* End SPIRAL.HPP ***************************************************/
