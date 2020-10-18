/*
 * C H K S U R F . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysPlanetSurfaceChecker

    Functions to perform debug checking on a planet's surface
*/

#ifndef _MACHPHYS_SURFCHK_HPP
#define _MACHPHYS_SURFCHK_HPP

#include "base/base.hpp"
#include "ctl/vector.hpp"
#include "ctl/utility.hpp"
#include "mathex/point3d.hpp"
#include "mathex/degrees.hpp"

class MachPhysPlanetSurface;
class PhysConfigSpace2d;
class MexTriangle2d;
class MexLine2d;

class MachPhysPlanetSurfaceChecker
// Canonical form revoked
{
public:
    MachPhysPlanetSurfaceChecker( const MachPhysPlanetSurface& );
    ~MachPhysPlanetSurfaceChecker( void );

    CTL_TRIPLE( Triangle, MexPoint3d, point1, MexPoint3d, point2, MexPoint3d, point3 );
    typedef ctl_vector< Triangle >  Triangles;

    //  Check that any triangles not inside an obstacle have gradients
    //  within acceptable limits - i.e. do not allow machines to attempt
    //  to move over surfaces that are too steep.
    //  Return the coordinates of any triangles whose gradients
    //  are too steep.
    void    checkGradients( const PhysConfigSpace2d&, Triangles* ) const;

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachPhysPlanetSurfaceChecker& t );

    MachPhysPlanetSurfaceChecker( const MachPhysPlanetSurfaceChecker& );
    MachPhysPlanetSurfaceChecker& operator =( const MachPhysPlanetSurfaceChecker& );

    //  Purely to allow template instantiation
    friend void MachDummyFunction1();

    //  These values must match those defined in machlog.hpp
    //  TBD: Move the machlog values somewhere where this
    //  code can access them without having to include bits
    //  of machlog. Bob 13 Oct 1998
    enum ObstacleFlag
    {
        OBSTACLE_NORMAL = 0,
        OBSTACLE_WATER = 1,
        OBSTACLE_LOW = 2,
        OBSTACLE_ALL = 3
    };


    void getTriangles( Triangles* pTriangles ) const;
    bool triangleIsVertical( const Triangle& triangle ) const;
    MexDegrees triangleGradient( const Triangle& triangle ) const;

    bool checkProjectedSegmentOK( const Triangle& triangle, MexDegrees gradient, const PhysConfigSpace2d& ) const;
    bool segmentInsideObstacle( const MexLine2d& triangle, ObstacleFlag flag, const PhysConfigSpace2d& ) const;

    bool checkTriangleOK( const Triangle& triangle, MexDegrees gradient, const PhysConfigSpace2d& ) const;
    bool triangleInsideObstacle( const MexTriangle2d& triangle, ObstacleFlag flag, const PhysConfigSpace2d& ) const;

    const MachPhysPlanetSurface& surface_;

    //  Order dependancy - leave these in the order they are defined here.
    const MexDegrees    maxNormalGradient_;
    const MexDegrees    maxLowObstacleGradient_;
    const MexDegrees    maxWaterGradient_;
    const MexDegrees    minMaxGradient_;
};


#endif

/* End SURFCHK.HPP **************************************************/
