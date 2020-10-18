/*
 * G R I D 2 D . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MexGrid2d

    Represents a 2D rectangular grid and allows useful calculations to
    be done on that grid.
*/

#ifndef _MATHEX_GRID2D_HPP
#define _MATHEX_GRID2D_HPP

#include "base/base.hpp"
#include "mathex/point2d.hpp"
#include "ctl/utility.hpp"
#include "ctl/vector.hpp"

class MexLine2d;

class MexGrid2d
// Canonical form revoked
{
public:
    //  The origin is the coordinates of the corner of the cell (0, 0)
    MexGrid2d( size_t nXCells, size_t nYCells,
      MATHEX_SCALAR xCellSize, MATHEX_SCALAR yCellSize,
      const MexPoint2d& origin );

    ~MexGrid2d( void );

    CTL_PAIR( CellIndex, size_t, xIndex, size_t, yIndex );
    typedef ctl_vector< CellIndex > Cells;
    
    typedef ctl_vector< MexPoint2d >    Points;

    //  Find out which cells a line intersects with and the
    //  coordinates of the intersections    
    void intersect( const MexLine2d& line, Cells* pCells, Points* pPoints );
    //  PRE( pCells->size() == 0 );
    //  PRE( pPoints->size() == 0 );
    //  POST( pCells->size() == pPoints->size() - 1 );

    MATHEX_SCALAR xMin() const;
    MATHEX_SCALAR xMax() const;
    MATHEX_SCALAR yMin() const;
    MATHEX_SCALAR yMax() const;
    
    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MexGrid2d& t );

    MexGrid2d( const MexGrid2d& );
    MexGrid2d& operator =( const MexGrid2d& );

    bool valid( const Cells& cells, const MexLine2d& line ) const;
    bool valid( const Points& points, const MexLine2d& line ) const;
    bool inRange( const MexPoint2d& point ) const;

    size_t          nXCells_;
    size_t          nYCells_;
    MexPoint2d      origin_;
    MATHEX_SCALAR   xCellSize_;
    MATHEX_SCALAR   yCellSize_;

    typedef ctl_vector< MATHEX_SCALAR >   Scalars;

    void getIntegersBetween( MATHEX_SCALAR end1, MATHEX_SCALAR end2, Scalars* );
};

#endif

/* End GRID2D.HPP ***************************************************/
