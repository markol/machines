/*
 * S P A C I A L . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachLogSpacialManipulation

    A few utility functions for manipulations used in MachLog but not belonging
	to any particular type of entity.
*/

#ifndef _MACHLOG_SpacialManipulation_HPP
#define _MACHLOG_SpacialManipulation_HPP

#include "base/base.hpp"
#include "utility/id.hpp"
#include "mathex/mathex.hpp"

template< class T> class ctl_vector;
template< class T> class ctl_pvector;

class MachActor;
class MexPoint3d;
class MexPoint2d;
class MachLogMachine;
class MexAlignedBox2d;
class MexTransform3d;
class MexRadians;
class MachPhysConstructionData;
class PhysConfigSpace2d;

class MachLogSpacialManipulation
{
public:

	enum UseWhichEnd { SOURCE_END_IS_CENTRE, DESTINATION_END_IS_CENTRE };
	enum UseWhichPattern { USE_CLOCK_PATTERN, USE_FLANKING_PATTERN };

	static bool inExtendedMexAlignedBox2d( const MexAlignedBox2d& box, const MexPoint3d& pos, MATHEX_SCALAR radius );
	
	static bool getNearestFreeSpacePoint( const MexTransform3d& orTrans, 
										  const MATHEX_SCALAR innerRadius, 
										  MATHEX_SCALAR clearance, 
										  MexPoint2d* pResult, 
										  MATHEX_SCALAR distanceToOuterRadius = 20, 
										  UseWhichPattern pattern = USE_CLOCK_PATTERN );
										  
	static bool pointIsFree( const MexPoint2d& testPoint, MATHEX_SCALAR clearance );
	static bool pointInsideConfigSpaceIsFree( const MexPoint2d& testPoint, MATHEX_SCALAR clearance, const PhysConfigSpace2d& cs );
	static void getCollidingObjects( const MexAlignedBox2d& testingBox, ctl_vector< UtlId >* pCollidingObjectIds );
	static bool targetBehindCover( const MachActor& actor, const MachActor& target );
	static MexRadians angleToTurnToFace( const MexTransform3d& fromTransform, const MexPoint3d& toPosition );
	static MexRadians angleToTurnToFace( const MachActor& actor, const MexPoint3d& pos );

	static MexTransform3d sourceToDestination3dTransform( const MexPoint2d& sourcePosition, const MexPoint2d& destinationPosition, UseWhichEnd );
	
	static bool withinRangeOfMexAlignedBox2d( const MexAlignedBox2d& box, const MexPoint2d& pos, MATHEX_SCALAR range );

    ~MachLogSpacialManipulation( void );

    void CLASS_INVARIANT;
	
	static void genericCheckForIntersections( const MexPoint3d& epicentre, 
												MATHEX_SCALAR range, 
												ctl_pvector< MachActor>* pActors );
												
	//return a modified global point with the same (x,y) coords as location but z adjusted
    //to lie at the terrain height (plus a bit to overcome coplanar problems)
    static MexPoint3d heightAdjustedLocation( const MexPoint3d& location );

	// return a modified global point with the same (x,y) coords as transform but
    // adjusted so that the building type specified by type, subType and hwLevel
    // will sit at the correct height on the terrain.
    static MexPoint3d heightAdjustedLocation(
        MachLog::ObjectType type, int subType, int hwLevel,
        const MexTransform3d& transform );
    static MexPoint3d heightAdjustedLocation(
        const MachPhysConstructionData& constructionData,
        const MexTransform3d& transform );

    // return the range of heights found for the "key points" of the given
    // building when sitting on the terrain at the given position.
    static void heightRange(
        MachLog::ObjectType type, int subType, int hwLevel,
        const MexTransform3d& transform,
        MATHEX_SCALAR* pMinHeight, MATHEX_SCALAR* pMaxHeight );
    static void heightRange(
        const MachPhysConstructionData& constructionData,
        const MexTransform3d& transform,
        MATHEX_SCALAR* pMinHeight, MATHEX_SCALAR* pMaxHeight );

    //  Return true iff the given box intersects with
    //  any actors other than debris or ore holographs
    static bool intersectsWithActor( const MexAlignedBox2d& box );
    //  Return true iff the given box intersects with
    //  any motion chunks
    static bool intersectsWithMotionChunks( const MexAlignedBox2d& box );

private:
    friend ostream& operator <<( ostream& o, const MachLogSpacialManipulation& t );
    // Operation deliberately revoked
    MachLogSpacialManipulation( void );
    MachLogSpacialManipulation( const MachLogSpacialManipulation& );

    // Operation deliberately revoked
    MachLogSpacialManipulation& operator =( const MachLogSpacialManipulation& );

    // Operation deliberately revoked
    bool operator ==( const MachLogSpacialManipulation& );

	// Tests positions in what can be conceived of as a 12 o'clock position, then an 11, then a 1, a 10 etc.
	static bool getNextInCircularPattern( const MexTransform3d& orTrans, const MATHEX_SCALAR radius, MATHEX_SCALAR clearance, MexPoint2d* pResult );
	
	// tries to find an approach position first trying points at a roughly 45 degree bearing from the transform facing.
	// This was designed for machines to effect flanking approaches, particularly useful for aggressives with
	// easily-rotatable upper torsos, approaching obliquely rather than always head-on.
	// There is a random element to which side is tried first, and also the size of the angle offset.
	static bool getNextInFlankingPattern( const MexTransform3d& orTrans, const MATHEX_SCALAR radius, MATHEX_SCALAR clearance, MexPoint2d* pResult );	
};


#endif

/* End SpacialManipulation.HPP **************************************************/
