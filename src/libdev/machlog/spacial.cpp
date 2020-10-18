/*
 * S P A C I A L . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"

#include "ctl/vector.hpp"
#include "ctl/pvector.hpp"
#include "ctl/list.hpp"

#include "mathex/abox2d.hpp"
#include "mathex/abox3d.hpp"
#include "mathex/circle2d.hpp"
#include "mathex/point2d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/angle.hpp"
#include "mathex/vec2.hpp"
#include "mathex/vec3.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/eulerang.hpp"

#include "phys/motchunk.hpp"
#include "phys/cspace2.hpp"

#include "world4d/composit.hpp"

#include "machphys/mphydata.hpp"
#include "machphys/random.hpp"

#include "machlog/actor.hpp"
#include "machlog/entrance.hpp"
#include "machlog/constron.hpp"
#include "machlog/machine.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/planet.hpp"
#include "machlog/races.hpp"
#include "machlog/spacial.hpp"
#include "machlog/mapper.hpp"

/////////////////////////////////////////////////////////////////////////

//static
bool MachLogSpacialManipulation::inExtendedMexAlignedBox2d( const MexAlignedBox2d& box, const MexPoint3d& pos, MATHEX_SCALAR radius )
{
	MexAlignedBox2d inflatedBox( box.minCorner(), box.maxCorner(), radius );
	return inflatedBox.contains( pos );
}

/////////////////////////////////////////////////////////////////////////

//static
bool MachLogSpacialManipulation::getNearestFreeSpacePoint( const MexTransform3d& orTrans, const MATHEX_SCALAR innerRadius, MATHEX_SCALAR clearance, MexPoint2d* pResult, MATHEX_SCALAR distanceToOuterRadius, UseWhichPattern pattern )
{
	bool foundSpace = true;
	MATHEX_SCALAR useRadius = innerRadius;

	switch( pattern )
	{
		case USE_CLOCK_PATTERN:
		{
			while( not getNextInCircularPattern(  orTrans, useRadius, clearance, pResult )
				   and useRadius < (innerRadius + distanceToOuterRadius ) )
			{
				useRadius += 2.5;
			}
			break;
		}
		case USE_FLANKING_PATTERN:
		{
			while( not getNextInFlankingPattern(  orTrans, useRadius, clearance, pResult )
				   and useRadius < (innerRadius + distanceToOuterRadius ) )
			{
				useRadius += 2.5;
			}
			break;
		}
		DEFAULT_ASSERT_BAD_CASE( pattern );
	}


	if( not ( useRadius < (innerRadius + distanceToOuterRadius ) ) )
	{
		HAL_STREAM("MLSpacialManipulation::getNearestFreeSpacePoint from:\n " << orTrans << " innerRadius " << innerRadius << " clearance " << clearance << " use innerRadius " << useRadius << std::endl << "Has NOT found a point" << std::endl );
		foundSpace = false;
	}
	else
	{
		HAL_STREAM("MLSpacialManipulation::getNearestFreeSpacePoint from:\n " << orTrans << " innerRadius " << innerRadius << " clearance " << clearance << " use innerRadius " << useRadius << std::endl << "Has found point at " << *pResult << std::endl );
		foundSpace = true;
	}

	return foundSpace;
}

/////////////////////////////////////////////////////////////////////////

//static
bool MachLogSpacialManipulation::getNextInCircularPattern( const MexTransform3d& orTrans, const MATHEX_SCALAR radius, MATHEX_SCALAR clearance, MexPoint2d* pResult )
{
	//** this function has multiple exit points **
	int nSectors = 16;

	MexTransform3d trans( orTrans );
	MexTransform3d destTrans( orTrans );

	destTrans.translate( MexPoint3d( radius,0,0 ) );

	// check the "top of circle case first"

	if( pointIsFree( destTrans.position(), clearance ) )
	{
        *pResult = destTrans.position();
        return true;
    }

	// otherwise, we'll have to rattle through the alternatives.
	// the pattern is: check 1 o'clock, check 11 o'clock, check 2 o'clock, check 10 o'clock etc.
	// (although in fact there are nSectors points to check in total, not nec. 12, but you get the idea.)

	MexRadians angleChunk( MexRadians( ( 2 * Mathex::PI ) / nSectors ) );

	int addAngleModifier = 1;

	bool found = false;

	for( int multiplier = 1; not found and multiplier < nSectors; ++multiplier )
	{

		trans.rotate( MexEulerAngles( angleChunk * addAngleModifier * multiplier, 0, 0 ) );
		destTrans = trans;
		destTrans.translate( MexPoint3d( radius,0,0 ) );

		if( pointIsFree( destTrans.position(), clearance ) )
		{
			*pResult = destTrans.position();
			found = true;
		}
		else
			addAngleModifier *= -1;	// to ensure that we go back 4 sectors anticlockwise if we just went forward
									// 3 sectors clockwise etc.
	}

	return found;
}

/////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const MachLogSpacialManipulation& t )
{

    o << "MachLogSpacialManipulation " << (void*)&t << " start" << std::endl;
    o << "MachLogSpacialManipulation " << (void*)&t << " end" << std::endl;

    return o;
}

/////////////////////////////////////////////////////////////////////////

//static
void MachLogSpacialManipulation::getCollidingObjects( const MexAlignedBox2d& /*testingBox*/, ctl_vector< UtlId >* /*pCollidingObjectIds*/ )
{

}

/////////////////////////////////////////////////////////////////////////

//static
bool MachLogSpacialManipulation::targetBehindCover( const MachActor& actor, const MachActor& target )
{
	PhysConfigSpace2d::PolygonId junk;
	bool result = false;
	HAL_STREAM("(" << actor.id() <<") targetbehindCover for object (" << target.id() << ") " << std::endl );
	if( target.objectIsMachine() )
	{
		MachLogMachine& mlm = _CONST_CAST( MachLogMachine&, target.asMachine() );
		//HAL_STREAM(" target is a machine checking for inside a building\n" );
		if( mlm.insideBuilding() )
		{
			//HAL_STREAM(" target inside building using interiorConfigSpace\n" );
			PhysConfigSpace2d& cSpace = mlm.insideWhichBuilding().interiorConfigSpace();
			result = not cSpace.contains( actor.position(), target.position(), mlm.obstacleFlags(), &junk, PhysConfigSpace2d::USE_PERMANENT_ONLY );
			//HAL_STREAM(" config space check gives " << result << std::endl );
			if( junk == mlm.insideWhichBuilding().entrance( 0 ).interiorPolygonId() )
			{
				//HAL_STREAM(" hits interior polygon id - result will be false\n" );
				result = false;
			}
		}
		else
		{
			HAL_STREAM(" not inside a building checking against planet CSpace2d\n" );
			PhysConfigSpace2d& cSpace = MachLogPlanet::instance().configSpace();
			result = not cSpace.contains( actor.position(), target.position(), mlm.obstacleFlags(), &junk, PhysConfigSpace2d::USE_PERMANENT_ONLY );
		}
	}
	else
	{
		//HAL_STREAM(" target is a construction checking against planet CSpace2d\n" );
		PhysConfigSpace2d& cSpace = MachLogPlanet::instance().configSpace();
		bool result = not cSpace.contains( actor.position(), target.position(), MachLog::OBSTACLE_NORMAL, &junk, PhysConfigSpace2d::USE_PERMANENT_ONLY );
		//HAL_STREAM(" result " << result << std::endl );
		//HAL_STREAM(" junk polygon " << junk << std::endl );
		if( target.objectIsConstruction() and
			target.asConstruction().obstaclePolygonId() == junk )
		{
			//HAL_STREAM( " junk == obstacle polygon id so changing to false\n" );
			result = false;
		}
	}
//	if( result = true )
//		if( cSpace.polygon( junk ).height() <=0 )
//			result = false;
	return result;
}

/////////////////////////////////////////////////////////////////////////

//static
MexRadians MachLogSpacialManipulation::angleToTurnToFace( const MexTransform3d& fromTransform, const MexPoint3d& toPosition )
{
	MexPoint2d fromPos( fromTransform.position() );

	MexVec2 bearingToTarget( toPosition.x() - fromPos.x(), toPosition.y() - fromPos.y() );

	MexVec3 xBasis3;
	fromTransform.xBasis( &xBasis3 );

	MexVec2 xBasis2( xBasis3 );
	return xBasis2.angleBetween( bearingToTarget );

}

/////////////////////////////////////////////////////////////////////////

//static
MexRadians MachLogSpacialManipulation::angleToTurnToFace( const MachActor& actor, const MexPoint3d& pos )
{
	MexPoint2d actorPos( actor.position() );

	MexVec2 bearingToTarget( pos.x() - actorPos.x(), pos.y() - actorPos.y() );

	MexVec3 xBasis3;
	actor.globalTransform().xBasis( &xBasis3 );

	MexVec2 xBasis2( xBasis3 );
	return xBasis2.angleBetween( bearingToTarget );

}

//static
void MachLogSpacialManipulation::genericCheckForIntersections( const MexPoint3d& epicentre, MATHEX_SCALAR range, ctl_pvector< MachActor>* pActors )
{
	MATHEX_SCALAR useRange = range * range;
	for( MachLogRaces::Objects::iterator i = MachLogRaces::instance().objects().begin();
		i != MachLogRaces::instance().objects().end(); ++i )
	{
		//construct a temporary bounding box
		MexAlignedBox3d& box = _CONST_CAST( MexAlignedBox3d&, (*i)->physObject().boundingVolume() );
		if( (*i)->physObject().isComposite() )
			box = _CONST_CAST( MexAlignedBox3d&, (*i)->physObject().asComposite().compositeBoundingVolume() );

		MexPoint3d tempPosition( epicentre );
		//get explosion position in local space.
		(*i)->globalTransform().transformInverse( &tempPosition );
        MachActor& iActor = *(*i);
		//now point is in same coord system as box we can look for closest approach.
		if( box.sqrEuclidianDistance( tempPosition ) < useRange )
			pActors->push_back( *i );
	}
}

//static
MexPoint3d MachLogSpacialManipulation::heightAdjustedLocation( const MexPoint3d& location )
{
    MATHEX_SCALAR height = MachLogPlanetDomains::terrainHeight( location );
    return MexPoint3d( location.x(), location.y(), height + 0.065 );
}

//static
MexPoint3d MachLogSpacialManipulation::heightAdjustedLocation(
    MachLog::ObjectType type, int subType, int hwLevel,
    const MexTransform3d& transform )
{
    MATHEX_SCALAR minHeight;
    MATHEX_SCALAR maxHeight;

    heightRange( type, subType, hwLevel, transform, &minHeight, &maxHeight );

    MexPoint3d result = transform.position();
    result.z( minHeight );

    return result;
}

//static
MexPoint3d MachLogSpacialManipulation::heightAdjustedLocation(
    const MachPhysConstructionData& constructionData,
    const MexTransform3d& transform )
{
    MATHEX_SCALAR minHeight;
    MATHEX_SCALAR maxHeight;

    heightRange( constructionData, transform, &minHeight, &maxHeight );

    MexPoint3d result = transform.position();
    result.z( minHeight );

    return result;
}

//static
void MachLogSpacialManipulation::heightRange(
    MachLog::ObjectType type, int subType, int hwLevel,
    const MexTransform3d& transform,
    MATHEX_SCALAR* pMinHeight, MATHEX_SCALAR* pMaxHeight )
{
    PRE_INFO( type );
    PRE( MachLogMapper::isConstruction( type ) );

    const MachPhysConstructionData& constructionData =
      MachPhysData::instance().constructionData(
      MachLogMapper::mapToPhysConstruction( type ),
      subType, hwLevel );

    heightRange( constructionData, transform, pMinHeight, pMaxHeight );
}

//static
void MachLogSpacialManipulation::heightRange(
    const MachPhysConstructionData& constructionData,
    const MexTransform3d& transform,
    MATHEX_SCALAR* pMinHeight, MATHEX_SCALAR* pMaxHeight )
{
    ctl_vector< MexPoint3d >    points;
    points.reserve( 20 );

    const MexAlignedBox2d& boundary = constructionData.localBoundary();

    points.push_back( MexPoint3d( boundary.minCorner().x(), boundary.minCorner().y(), 0.0 ) );
    points.push_back( MexPoint3d( boundary.minCorner().x(), boundary.maxCorner().y(), 0.0 ) );
    points.push_back( MexPoint3d( boundary.maxCorner().x(), boundary.minCorner().y(), 0.0 ) );
    points.push_back( MexPoint3d( boundary.maxCorner().x(), boundary.maxCorner().y(), 0.0 ) );

    const MachPhysConstructionData::Entrances& entrances = constructionData.entrances();

    for( size_t i = 0; i < entrances.size(); ++i )
    {
        points.push_back( entrances[ i ].externalPoint );
    }

    bool    first = true;

    for( size_t i = 0; i < points.size(); ++i )
    {
        transform.transform( &points[ i ] );

        if( MachLogPlanetDomains::onPlanet(  MexPoint2d( points[ i ] ) ) )
        {
            MATHEX_SCALAR height = MachLogPlanetDomains::terrainHeight( points[ i ] );

            if( first )
            {
                *pMinHeight = height;
                *pMaxHeight = height;

                first = false;
            }
            else
            {
                *pMinHeight = std::min( *pMinHeight, height );
                *pMaxHeight = std::max( *pMaxHeight, height );
            }
        }
    }

    if( first )
    {
        //All our points were off the planet, so use zero
        *pMinHeight = *pMaxHeight = 0.0;
    }
}

//static
MexTransform3d MachLogSpacialManipulation::sourceToDestination3dTransform( const MexPoint2d& sourcePosition, const MexPoint2d& destinationPosition, UseWhichEnd whichEnd )
{
    PRE_INFO( sourcePosition );
    PRE_INFO( destinationPosition );
    PRE( sourcePosition != destinationPosition );

	//Construct a transform in which the z axis is vertical, the x axis is pointing from me to
    //him, and the origin is at my origin.
    MexVec3 xBasis( destinationPosition.x() - sourcePosition.x(), destinationPosition.y() - sourcePosition.y(), 0.0 );
    MATHEX_SCALAR separation = xBasis.modulus();

    ASSERT_INFO( separation );
    ASSERT( separation > MexEpsilon::instance(), "" );

    xBasis *= (1.0 / separation);
    MexVec3 zBasis( 0.0, 0.0, 1.0 );
    MexVec3 yBasis = MexVec3::crossProduct( zBasis, xBasis );

	MexPoint2d centreOfTransform;
	if( whichEnd == SOURCE_END_IS_CENTRE )
		centreOfTransform = sourcePosition;
	else
		centreOfTransform = destinationPosition;

    MexTransform3d resultingTransform( xBasis, yBasis, zBasis, centreOfTransform );

	return resultingTransform;
}


//static
bool MachLogSpacialManipulation::pointIsFree( const MexPoint2d& testPoint, MATHEX_SCALAR clearance )
{
	bool result = false;

	PhysConfigSpace2d& cs = MachLogPlanet::instance().configSpace();
	MexCircle2d testCircle( testPoint, clearance * 1.01 );

	PhysConfigSpace2d::PolygonId dummyId;
	PhysConfigSpace2d::DomainId dummyDomainId;
    if( cs.contains( testCircle, MachLog::OBSTACLE_NORMAL, &dummyId, PhysConfigSpace2d::USE_ALL ) and cs.domain( testPoint, &dummyDomainId ) )
		result = true;

	return result;
}

//static
bool MachLogSpacialManipulation::pointInsideConfigSpaceIsFree( const MexPoint2d& testPoint, MATHEX_SCALAR clearance, const PhysConfigSpace2d& cs )
{
	bool result = false;

	MexCircle2d testCircle( testPoint, clearance * 1.01 );

	PhysConfigSpace2d::PolygonId dummyId;
	PhysConfigSpace2d::DomainId dummyDomainId;
    if( cs.contains( testCircle, MachLog::OBSTACLE_NORMAL, &dummyId, PhysConfigSpace2d::USE_ALL ) and cs.domain( testPoint, &dummyDomainId ) )
		result = true;

	return result;
}


//static
bool MachLogSpacialManipulation::getNextInFlankingPattern( const MexTransform3d& orTrans, const MATHEX_SCALAR radius, MATHEX_SCALAR clearance, MexPoint2d* pResult )
{
	static int nSectors = 32;

	static int quarterSectors = nSectors / 4;

	// this simply sets sense to either 1 or -1 (clockwise/anticlockwise) with equal probability
	int sense = 1 - ( 2 * MachPhysRandom::randomInt( 0, 2 ) );

	// basically starts somewhere between -66 and -12 degrees
	// (as at least one rotation is done in the main loop before testing, the
	// smallest starting angle is actually at -quartersectors + 2, not -quartersectors + 1)
	int progression = -(quarterSectors) + 1 + MachPhysRandom::randomInt( 0, 5 );

	MexTransform3d trans( orTrans );
	MexTransform3d destTrans( orTrans );

	MexRadians angleChunk( MexRadians( ( 2 * Mathex::PI ) / nSectors ) );

	// rotate around to the start position
	// note the use of 'sense' - can effectively make the whole findspace a mirror-image search if set at -1.
	trans.rotate( MexEulerAngles( angleChunk * progression * sense, 0, 0 ) );

	bool found = false;

	// the search will basically try from the initial angle defined by "progression", right around
	// to 90 degrees on the other side of the normal.
	for( ; not found and progression < quarterSectors; ++progression )
	{
		// note the use of 'sense' - can effectively make the whole findspace a mirror-image search if set at -1.
		trans.rotate( MexEulerAngles( angleChunk * sense, 0, 0 ) );
		destTrans = trans;
		destTrans.translate( MexPoint3d( radius,0,0 ) );

		if( pointIsFree( destTrans.position(), clearance ) )
		{
			*pResult = destTrans.position();
			found = true;
		}
	}

	return found;
}

//static
bool MachLogSpacialManipulation::withinRangeOfMexAlignedBox2d( const MexAlignedBox2d& box, const MexPoint2d& pos, MATHEX_SCALAR range )
{
	bool result = false;

	if( box.contains( pos ) )
		result = true;
	else
	{
		MexPoint2d nearestPoint = box.nearestPointToExternalPoint( pos );

		result = pos.sqrEuclidianDistance( nearestPoint ) <= sqr( range );
	}

	return result;
}


//static
bool MachLogSpacialManipulation::intersectsWithActor( const MexAlignedBox2d& box )
{
	// TBD: This check really needs to do a sqrEuclidean distance check
    // onto the Aligned Box 2d using the clearance of the actor

	bool intersects = false;

	for( MachLogRaces::Objects::iterator i = MachLogRaces::instance().objects().begin();
		not intersects and i !=  MachLogRaces::instance().objects().end();
		++i )
	{
		MachLog::ObjectType type = (*i)->objectType();
		//only bother checking if not holograph or debris.
		if( type != MachLog::DEBRIS and type != MachLog::ORE_HOLOGRAPH )
		{
			HAL_STREAM( "Checking actor position: " << (*i)->position() << std::endl );
			if( box.contains( (*i)->position() ) )
				intersects = true;
		}
	}

    return intersects;
}

//static
bool MachLogSpacialManipulation::intersectsWithMotionChunks( const MexAlignedBox2d& box )
{
    PhysConfigSpace2d::ChunkIntersectionsNoTime chunkIntersections;
    MachLogPlanet::instance().configSpace().findIntersections( box, &chunkIntersections );

    return chunkIntersections.size() != 0;
}

/////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////

/* End SpacialManipulation.CPP **************************************************/
