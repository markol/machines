/*
 * C H K S U R F . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/surfchk.hpp"
#include "machphys/plansurf.hpp"
#include "machphys/terrtile.hpp"
#include "machphys/tiledata.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/genedata.hpp"
#include "phys/cspace2.hpp"
#include "mathex/plane3d.hpp"
#include "mathex/tri2d.hpp"
#include "mathex/line2d.hpp"
#include "mathex/epsilon.hpp"

MachPhysPlanetSurfaceChecker::MachPhysPlanetSurfaceChecker( const MachPhysPlanetSurface& surface )
: surface_( surface ),
  maxNormalGradient_( MachPhysData::instance().generalData().maxMoveGradient() ),
  maxLowObstacleGradient_( 89.0 ),
  maxWaterGradient_( MachPhysData::instance().generalData().maxMoveGradient() ),
  minMaxGradient_( std::min(
      maxNormalGradient_.asScalar(), std::min(
      maxLowObstacleGradient_.asScalar(),
      maxWaterGradient_.asScalar() ) ) )
{

    TEST_INVARIANT;
}

MachPhysPlanetSurfaceChecker::~MachPhysPlanetSurfaceChecker()
{
    TEST_INVARIANT;

}

void    MachPhysPlanetSurfaceChecker::checkGradients( const PhysConfigSpace2d& configSpace, Triangles* pTriangles ) const
{
    Triangles   allTriangles;

    getTriangles( &allTriangles );

    BOB_INSPECT( MexDegrees( maxNormalGradient_ ) );
    BOB_INSPECT( MexDegrees( maxLowObstacleGradient_ ) );
    BOB_INSPECT( MexDegrees( maxWaterGradient_ ) );
    BOB_INSPECT( MexDegrees( minMaxGradient_ ) );

    for( size_t i = 0; i < allTriangles.size(); ++i )
    {
        MexDegrees  gradient = triangleGradient( allTriangles[ i ] );

        BOB_STREAM( allTriangles[ i ].point1() << " " );
        BOB_STREAM( allTriangles[ i ].point2() << " " );
        BOB_STREAM( allTriangles[ i ].point3() << " " );

        BOB_STREAM( gradient << std::endl );

        if( gradient > minMaxGradient_ )
        {
			if( triangleIsVertical( allTriangles[ i ] )	)
			{
	            if( checkProjectedSegmentOK( allTriangles[ i ], gradient, configSpace ) )
    	        {
        		        BOB_STREAM( "Triangle OK" << std::endl );
            	}
            	else
            	{
                	pTriangles->push_back( allTriangles[ i ] );
                	BOB_STREAM( "Triangle not in obstacle" << std::endl );
            	}
			}
			else
			{
	            if( checkTriangleOK( allTriangles[ i ], gradient, configSpace ) )
    	        {
        	        BOB_STREAM( "Triangle OK" << std::endl );
            	}
	            else
    	        {
        	        pTriangles->push_back( allTriangles[ i ] );
            	    BOB_STREAM( "Triangle not in obstacle" << std::endl );
            	}
			}
        }
    }
}

static void createProjectedSegment( const MachPhysPlanetSurfaceChecker::Triangle& triangle3d, MexLine2d* segment )
{
	PRE( segment );

    MexTriangle2d triangleProjection(
      triangle3d.point1(),
      triangle3d.point2(),
      triangle3d.point3() );

	MexPoint2d p1( triangleProjection.vertex( 0 ) );
	MexPoint2d p2( triangleProjection.vertex( 1 ) );

	if( p1.sqrEuclidianDistance( p2 ) < p1.sqrEuclidianDistance( triangleProjection.vertex( 2 ) ) )
	{
		p2 = triangleProjection.vertex( 2 );
	}
	if( p2.sqrEuclidianDistance( p1 ) < p2.sqrEuclidianDistance( triangleProjection.vertex( 1 ) ) )
	{
		p1 = triangleProjection.vertex( 1 );
	}
	*segment = MexLine2d( p1, p2 );
}

bool MachPhysPlanetSurfaceChecker::checkProjectedSegmentOK( const Triangle& triangle, MexDegrees gradient, const PhysConfigSpace2d& configSpace ) const
{
    //  This triangle is too steep for at least some machines.
    //  We need to check that it is inside an obstacle.
	PRE( triangleIsVertical( triangle ) );

	MexLine2d segment2d;
	createProjectedSegment( triangle, &segment2d );
    LIONEL_STREAM( "Degenerated triangle " );
    LIONEL_STREAM( triangle.point1() << " " );
    LIONEL_STREAM( triangle.point2() << " " );
    LIONEL_STREAM( triangle.point3() << " " );

    LIONEL_STREAM( segment2d.end1() << " " );
    LIONEL_STREAM( segment2d.end2() << " " );

    bool    result = false;

    if( segmentInsideObstacle( segment2d, OBSTACLE_ALL, configSpace ) )
    {
        result = true;
    }
    else if( segmentInsideObstacle( segment2d, OBSTACLE_WATER, configSpace ) and gradient <= maxWaterGradient_ )
    {
        result = true;
    }
    else if( segmentInsideObstacle( segment2d, OBSTACLE_LOW, configSpace ) and gradient <= maxLowObstacleGradient_ )
    {
        result = true;
    }

    return result;
}

bool MachPhysPlanetSurfaceChecker::segmentInsideObstacle( const MexLine2d& segment, ObstacleFlag flags, const PhysConfigSpace2d& configSpace ) const
{
    //  The flags value we have passed in is the flags value for the obstacles
    //  we are looking to check. The flags value we pass to the contains
    //  functions is the obstacles we wish to ignore. We therefore need
    //  to invert the flags value.

    //flags ^= OBSTACLE_ALL;
    flags = _STATIC_CAST(MachPhysPlanetSurfaceChecker::ObstacleFlag, flags ^ OBSTACLE_ALL );

    PhysConfigSpace2d::PolygonId id;

    bool    result = true;

    if( configSpace.contains( segment.end1(), segment.end2(), flags, &id ) )
    {
        //  The triangle is entirely outside the obstacles

        result = false;
    }
    else if( configSpace.contains( segment.end1(), flags, &id ) )
    {
        result = false;
    }
    else if( configSpace.contains( segment.end2(), flags, &id ) )
    {
        result = false;
    }
    else
        result = true;

    return result;
}

bool MachPhysPlanetSurfaceChecker::checkTriangleOK( const Triangle& triangle, MexDegrees gradient, const PhysConfigSpace2d& configSpace ) const
{
    //  This triangle is too steep for at least some machines.
    //  We need to check that it is inside an obstacle.

    MexTriangle2d triangle2d(
      triangle.point1(),
      triangle.point2(),
      triangle.point3() );

    bool    result = false;

    if( triangleInsideObstacle( triangle2d, OBSTACLE_ALL, configSpace ) )
    {
        result = true;
    }
    else if( triangleInsideObstacle( triangle2d, OBSTACLE_WATER, configSpace ) and gradient <= maxWaterGradient_ )
    {
        result = true;
    }
    else if( triangleInsideObstacle( triangle2d, OBSTACLE_LOW, configSpace ) and gradient <= maxLowObstacleGradient_ )
    {
        result = true;
    }

    return result;
}

bool MachPhysPlanetSurfaceChecker::triangleInsideObstacle( const MexTriangle2d& triangle, ObstacleFlag flags, const PhysConfigSpace2d& configSpace ) const
{
    //  The flags value we have passed in is the flags value for the obstacles
    //  we are looking to check. The flags value we pass to the contains
    //  functions is the obstacles we wish to ignore. We therefore need
    //  to invert the flags value.

    //flags ^= OBSTACLE_ALL;
    flags = _STATIC_CAST(MachPhysPlanetSurfaceChecker::ObstacleFlag, flags ^ OBSTACLE_ALL );

    PhysConfigSpace2d::PolygonId id;

    bool    result = true;

    if( configSpace.contains( triangle, flags, &id ) )
    {
        //  The triangle is entirely outside the obstacles

        result = false;
    }
    else if( configSpace.contains( triangle.vertex( 0 ), flags, &id ) )
    {
        result = false;
    }
    else if( configSpace.contains( triangle.vertex( 1 ), flags, &id ) )
    {
        result = false;
    }
    else if( configSpace.contains( triangle.vertex( 2 ), flags, &id ) )
    {
        result = false;
    }
    else
        result = true;

    return result;
}

void MachPhysPlanetSurfaceChecker::getTriangles( Triangles* pTriangles ) const
{
    for( size_t i = 0; i < surface_.nXTiles(); ++i )
    {
        for( size_t j = 0; j < surface_.nYTiles(); ++j )
        {
            const MachPhysTerrainTile& tile = surface_.tile( i, j );
            const MachPhysTileData& tileData = tile.tileData();

            for( size_t k = 0; k < tileData.nTriangles(); ++k )
            {
                MexPoint3d  p1;
                MexPoint3d  p2;
                MexPoint3d  p3;

                tileData.triangle( k, &p1, &p2, &p3 );

                tile.globalTransform().transform( &p1 );
                tile.globalTransform().transform( &p2 );
                tile.globalTransform().transform( &p3 );

                pTriangles->push_back( Triangle( p1, p2, p3 ) );
            }
        }
    }
}

bool MachPhysPlanetSurfaceChecker::triangleIsVertical( const Triangle& triangle ) const
{
    MexDegrees  gradient = triangleGradient( triangle );
	bool result = Mathex::abs( gradient.asScalar() - 90.0 ) < MexEpsilon::instance();
	return result;
}

MexDegrees MachPhysPlanetSurfaceChecker::triangleGradient( const Triangle& triangle ) const
{
    MexPlane3d  plane( triangle.point1(), triangle.point2(), triangle.point3() );

    //  The angle between the normal and the Z axis is the same as the angle that
    //  the surface of the triangle makes with the ground.

    MexVec3 zAxis( 0.0, 0.0, 1.0 );

    MexDegrees result = zAxis.angleBetween( plane.normal() );

    //  Make sure that any angles above 90 degrees get translated to the correcponding angle in the range 0 - 90
    if( result.asScalar() > 90.0 )
        result = 180.0 - result.asScalar();

    POST_INFO( result );
    POST( result.asScalar() >= 0.0 );
    POST( result.asScalar() <= 90.0 );

    return result;
}

void MachPhysPlanetSurfaceChecker::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysPlanetSurfaceChecker& t )
{

    o << "MachPhysPlanetSurfaceChecker " << (void*)&t << " start" << std::endl;
    o << "MachPhysPlanetSurfaceChecker " << (void*)&t << " end" << std::endl;

    return o;
}

/* End CHKSURF.CPP **************************************************/
