/*
 * C O N S D A T A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/cvexpgon.hpp"
#include "mathex/transf3d.hpp"

#include "machphys/consdata.hpp"
#include "machphys/stations.hpp"
#include "machphys/internal/consdati.hpp"


MachPhysConstructionData::MachPhysConstructionData()
: accessSizeIndex_( -1 )
{
    pImpl_ = _NEW( MachPhysConstructionDataImpl() );

    entrances_.reserve( 2 );
    pickUpPoints_.reserve( 2 );
    putDownPoints_.reserve( 2 );
    pImpl_->localEntrances_.reserve( 2 );

    TEST_INVARIANT;
}

MachPhysConstructionData::MachPhysConstructionData(
    const MachPhysConstructionData& copyMe,
    const MexTransform3d& newTransform )
: MachPhysObjectData( copyMe ),
  hwResearchCost_( copyMe.hwResearchCost_ ),
  localBoundary_( copyMe.localBoundary_ ),
  interiorBoundary_( copyMe.interiorBoundary_ ),
  pickUpPoints_( copyMe.pickUpPoints_ ),
  putDownPoints_( copyMe.putDownPoints_ ),
  entrances_( copyMe.entrances_ ),
  constructionType_( copyMe.constructionType_ ),
  subType_( copyMe.subType_ ),
  hwLevel_( copyMe.hwLevel_ ),
  accessSizeIndex_( copyMe.accessSizeIndex_ )
{
    pImpl_ = _NEW( MachPhysConstructionDataImpl() );

    pImpl_->stations_ = MachPhysStations( copyMe.pImpl_->stations_, newTransform );
    pImpl_->localEntrances_ = copyMe.pImpl_->localEntrances_;

    for( Locations::iterator i = pickUpPoints_.begin(); i != pickUpPoints_.end(); ++i )
        newTransform.transform( &(*i) );

    for( Locations::iterator i = putDownPoints_.begin(); i != putDownPoints_.end(); ++i )
        newTransform.transform( &(*i) );

    for( Entrances::iterator i = entrances_.begin(); i != entrances_.end(); ++i )
    {
        newTransform.transform( &(*i).externalPoint );
        newTransform.transform( &(*i).internalPoint );
        newTransform.transform( &(*i).doorLine );
    }

    for( Polygons::const_iterator i = copyMe.interiorObstacles_.begin(); i != copyMe.interiorObstacles_.end(); ++i )
    {
        interiorObstacles_.push_back( _NEW( MexConvexPolygon2d( *(*i) ) ) );
        interiorObstacles_.back()->transform( newTransform );
    }
}

MachPhysConstructionData& MachPhysConstructionData::operator =( const MachPhysConstructionData& copyMe )
{
	if( &copyMe != this )
	{
		hwResearchCost_ = copyMe.hwResearchCost_;
	    localBoundary_ = copyMe.localBoundary_;
	    entrances_ = copyMe.entrances_;
	    pickUpPoints_ = copyMe.pickUpPoints_;
	    putDownPoints_ = copyMe.putDownPoints_;
	    interiorBoundary_ = copyMe.interiorBoundary_;

	    interiorObstacles_.erase( interiorObstacles_.begin(), interiorObstacles_.end() );

	    for( Polygons::const_iterator i = copyMe.interiorObstacles_.begin(); i != copyMe.interiorObstacles_.end(); ++i )
	    {
	        interiorObstacles_.push_back( _NEW( MexConvexPolygon2d( *(*i) ) ) );
	    }

        pImpl_->localEntrances_ = copyMe.pImpl_->localEntrances_;
        pImpl_->stations_ = copyMe.pImpl_->stations_;

	    constructionType_ = copyMe.constructionType_;
	    subType_ = copyMe.subType_;
	    hwLevel_ = copyMe.hwLevel_;
	}

    return *this;
}

MachPhysConstructionData::~MachPhysConstructionData()
{
    TEST_INVARIANT;

    for( Polygons::iterator i = interiorObstacles_.begin(); i != interiorObstacles_.end(); ++i )
        _DELETE( (*i) );

    ASSERT( pImpl_ != NULL, "" );
    _DELETE( pImpl_ );
}

MachPhys::ResearchUnits MachPhysConstructionData::hwResearchCost() const
{
	return hwResearchCost_;
}

void MachPhysConstructionData::hwResearchCost( const MachPhys::ResearchUnits& newSwResearchCost )
{
	hwResearchCost_ = newSwResearchCost;
}

void MachPhysConstructionData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void MachPhysConstructionData::defineLocalBoundary( const MexAlignedBox2d& localBoundary )
{
   localBoundary_ = localBoundary;
}

void MachPhysConstructionData::accessSizeIndex( int size )
{
	accessSizeIndex_ = size;
}

int MachPhysConstructionData::accessSizeIndex() const
{
	return accessSizeIndex_;
}

void MachPhysConstructionData::addEntrance
(
    const MexPoint2d& externalPoint,
    const MexPoint2d& internalPoint,
    const MexPoint2d& doorPoint,
    const string& doorEntityName,
	const MachPhys::DoorType doorType
)
{
	static const MATHEX_SCALAR DOOR_HEIGHT_0 = 4.6;
	static const MATHEX_SCALAR DOOR_WIDTH_0  = 6.9;

	static const MATHEX_SCALAR DOOR_HEIGHT_1 = 8.0;
	static const MATHEX_SCALAR DOOR_WIDTH_1  = 11.9;

	MATHEX_SCALAR width = 0;
	MATHEX_SCALAR height = 0;

	switch( doorType )
	{
		case 0:
			width = DOOR_WIDTH_0;
			height = DOOR_HEIGHT_0;
			break;
		case 1:
			width = DOOR_WIDTH_1;
			height = DOOR_HEIGHT_1;
			break;
		default:;
			ASSERT( false, "DOOR_SIZE for construction was not specified before ENTRANCE line was parsed." );
	}

    //  Calculate the end points of the door
    MexVec2 vectorThroughDoor( externalPoint, internalPoint );
    MexVec2 vectorAlongDoor( vectorThroughDoor.y(), -vectorThroughDoor.x() );

    vectorAlongDoor.makeUnitVector();

    vectorAlongDoor.setVector( vectorAlongDoor.x() * width / 2.0, vectorAlongDoor.y() * width / 2.0 );

    MexPoint2d  end1( doorPoint );
    MexPoint2d  end2( doorPoint );

    end1 += vectorAlongDoor;
    end2 -= vectorAlongDoor;

    MexLine2d    doorLine( end1, end2 );

    EntranceData entrance;
    entrance.externalPoint = externalPoint;
    entrance.internalPoint = internalPoint;
    entrance.doorLine = doorLine;
    entrance.height = height;
    entrance.doorEntityName = doorEntityName;

    entrances_.push_back( entrance );
    pImpl_->localEntrances_.push_back( entrance );
}
//////////////////////////////////////////////////////////////////////////////////////////
// void MachPhysConstructionData::addDoor
// (
//     const MexPoint2d& externalPoint, const MexPoint2d& internalPoint, const MachPhys::DoorType& type
// )
// {
// 	static const SMALL_DOOR_HEIGHT = 4.6;
// //	static const SMALL_DOOR_WIDTH  = 6.9;
//
//     //  Changed by Bob purely to fit old smelter model better for ECTS demo
// 	static const SMALL_DOOR_WIDTH  = 4.25;
// 	static const LARGE_DOOR_HEIGHT = 8.0;
// 	static const LARGE_DOOR_WIDTH  = 11.9;
//
//
// 	MexPoint2d p( ( externalPoint.x() + internalPoint.x() ) / 2,
// 				  ( externalPoint.y() + internalPoint.y() ) / 2 );
//
//
// 	MATHEX_SCALAR width;
// 	MATHEX_SCALAR height;
//
// 	MexPoint2d minC = p;
// 	MexPoint2d maxC = p;
//
// 	switch( type )
// 	{
// 		case MachPhys::SMALL_DOOR:
// 			width = SMALL_DOOR_WIDTH;
// 			height = SMALL_DOOR_HEIGHT;
// 			break;
// 		case MachPhys::LARGE_DOOR:
// 			width = LARGE_DOOR_WIDTH;
// 			height = LARGE_DOOR_HEIGHT;
// 			break;
// 		default:;
// 			ASSERT_BAD_CASE;
// 	}
// 	//If buildings are every defined without being axis aligned then we will have to compute the gradient
// 	//from ext point and int point and apply with correct scalars to coords.
// 	if( p.y() == externalPoint.y() )
// 	{
// 		minC.y( minC.y() - ( width / 2 ) );
// 		maxC.y( maxC.y() + ( width / 2 ) );
// 	}
// 	else
// 	{
// 		minC.x( minC.x() - ( width / 2 ) );
// 		maxC.x( maxC.x() + ( width / 2 ) );
// 	}
//
// 	MexLine2d l( minC, maxC );
//     DoorData door;
// 	door.doorLine = l;
// 	door.height = height;
//
//     doors_.push_back( door );
// }
//////////////////////////////////////////////////////////////////////////////////////////

void MachPhysConstructionData::addPickUpPoint( const MexPoint2d& point )
{
    pickUpPoints_.push_back( point );
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachPhysConstructionData::addPutDownPoint( const MexPoint2d& point )
{
    putDownPoints_.push_back( point );
}
//////////////////////////////////////////////////////////////////////////////////////////

const MexAlignedBox2d& MachPhysConstructionData::localBoundary() const
{
    return localBoundary_;
}
//////////////////////////////////////////////////////////////////////////////////////////
const MachPhysConstructionData::Entrances& MachPhysConstructionData::entrances() const
{
    return entrances_;
}
//////////////////////////////////////////////////////////////////////////////////////////
const MachPhysConstructionData::Locations& MachPhysConstructionData::pickUpPoints() const
{
    return pickUpPoints_;
}
//////////////////////////////////////////////////////////////////////////////////////////
const MachPhysConstructionData::Locations& MachPhysConstructionData::putDownPoints() const
{
    return putDownPoints_;
}

const MexAlignedBox2d&  MachPhysConstructionData::interiorBoundary() const
{
    return interiorBoundary_;
}

const MachPhysConstructionData::Polygons&         MachPhysConstructionData::interiorObstacles() const
{
    return interiorObstacles_;
}

void MachPhysConstructionData::interiorBoundary( const MexAlignedBox2d& boundary )
{
    interiorBoundary_ = boundary;
}

void MachPhysConstructionData::addInteriorObstacle( const ctl_vector< MexPoint2d > points )
{
    interiorObstacles_.push_back( _NEW( MexConvexPolygon2d( points ) ) );
}

ostream& operator <<( ostream& o, const MachPhysConstructionData& t )
{

    o << "hwResearch cost " << t.hwResearchCost() << std::endl;
	o << "nEntrances " << t.entrances().size() << std::endl;
	for( int i = 0; i < t.entrances().size(); ++i )
	{
		MachPhysConstructionData::EntranceData e = t.entrances()[ i ];
		o << " " << e.internalPoint << " " << e.externalPoint;
        o << "  door line " << e.doorLine.end1() << " - " << e.doorLine.end2() << " height " << e.height;
        o << "  door entity name " << e.doorEntityName << std::endl;
	}
	o << "nPickUpPoints " << t.pickUpPoints().size() << std::endl;
	o << "nPutDownPoints " << t.putDownPoints().size() << std::endl;

    o << "interiorBoundary " << t.interiorBoundary() << std::endl;
    o << "interior obstacles " << std::endl;
	for( int i = 0; i < t.interiorObstacles().size(); ++i )
	{
        o << *t.interiorObstacles()[ i ] << std::endl;
    }

    return o;
}

MachPhysStations&   MachPhysConstructionData::stations()
{
    return pImpl_->stations_;
}

const MachPhysStations&   MachPhysConstructionData::stations() const
{
    return pImpl_->stations_;
}

MachPhys::ConstructionType MachPhysConstructionData::constructionType() const
{
    return constructionType_;
}

int MachPhysConstructionData::subType() const
{
    return subType_;
}

uint MachPhysConstructionData::hwLevel() const
{
    return hwLevel_;
}

void MachPhysConstructionData::constructionType( MachPhys::ConstructionType type )
{
    constructionType_ = type;
}

void MachPhysConstructionData::subType( int type )
{
    subType_ = type;
}

void MachPhysConstructionData::hwLevel( uint level )
{
    hwLevel_ = level;
}

bool MachPhysConstructionData::dataValid() const
{
    bool    result = true;

    if( not localBoundary().contains( interiorBoundary() ) )
    {
        ASSERT_INFO( "Interior not contained in exterior" );
        ASSERT_INFO( localBoundary() );
        ASSERT_INFO( interiorBoundary() );

        result = false;
    }

    for( size_t i = 0; i < stations().nStations(); ++i )
    {
        const MachPhysStation& station = stations().station( i );

        if( not interiorBoundary().contains( station.position() ) )
        {
            ASSERT_INFO( "Station not contained in interior" );
            ASSERT_INFO( interiorBoundary() );
            ASSERT_INFO( station.position() );

            result = false;
        }
    }

    return result;
}

MexConvexPolygon2d  MachPhysConstructionData::EntranceData::interiorPolygon() const
{
    MexPoint2d point1;
    MexPoint2d point2;

    polygonPoints( INTERIOR, &point1, &point2 );

    ctl_vector< MexPoint2d >   interiorPoints;
	interiorPoints.reserve( 10 );
    interiorPoints.push_back( doorLine.end1() );
    interiorPoints.push_back( point1 );
    interiorPoints.push_back( point2 );
    interiorPoints.push_back( doorLine.end2() );

    MexConvexPolygon2d  interiorPoly( interiorPoints );

    return interiorPoly;
}

MexConvexPolygon2d  MachPhysConstructionData::EntranceData::exteriorPolygon() const
{
    MexPoint2d point1;
    MexPoint2d point2;

    polygonPoints( EXTERIOR, &point1, &point2 );

    ctl_vector< MexPoint2d >   exteriorPoints;
	exteriorPoints.reserve( 10 );
    exteriorPoints.push_back( doorLine.end1() );
    exteriorPoints.push_back( doorLine.end2() );
    exteriorPoints.push_back( point2 );
    exteriorPoints.push_back( point1 );

    MexConvexPolygon2d  exteriorPoly( exteriorPoints );

    return exteriorPoly;
}

MexConvexPolygon2d  MachPhysConstructionData::EntranceData::totalPolygon() const
{
    MexPoint2d interior1;
    MexPoint2d interior2;
    MexPoint2d exterior1;
    MexPoint2d exterior2;

    polygonPoints( EXTERIOR, &exterior1, &exterior2 );
    polygonPoints( INTERIOR, &interior1, &interior2 );

    ctl_vector< MexPoint2d >   points;
	points.reserve( 10 );
    points.push_back( interior1 );
    points.push_back( interior2 );
    points.push_back( exterior2 );
    points.push_back( exterior1 );

    MexConvexPolygon2d  poly( points );

    return poly;
}

//  Utility function for the functions that calcualte a polygon. Return the additional
//  points that are used to define one of the internal or external polygons
void  MachPhysConstructionData::EntranceData::polygonPoints(
    InteriorExterior type,
    MexPoint2d* pPoint1,
    MexPoint2d* pPoint2 ) const
{
    PRE( pPoint1 != NULL );
    PRE( pPoint2 != NULL );

    const MexVec2 v( doorLine.end1(), doorLine.end2() );
    const MATHEX_SCALAR   doorWidth = v.modulus();

    MexVec2 perpendicular;
    MexPoint2d point;

    if( type == INTERIOR )
    {
        perpendicular = MexVec2( v.y(), -v.x() );
        point = internalPoint;
    }
    else
    {
        perpendicular = MexVec2( -v.y(), +v.x() );
        point = externalPoint;
    }

    perpendicular.makeUnitVector();

    //  Make sure the pads extend far enough to include the interior
    //  and exterior entrance target points

    const MATHEX_SCALAR internalDistance =
      doorLine.end1().euclidianDistance( point ) + doorWidth / 2;

    perpendicular *= internalDistance;

    *pPoint1 = doorLine.end1();
    *pPoint1 += perpendicular;
    *pPoint2 = doorLine.end2();
    *pPoint2 += perpendicular;
}

const MachPhysConstructionData::Entrances& MachPhysConstructionData::localEntrances() const
{
    return pImpl_->localEntrances_;
}

//////////////////////////////////////////////////////////////////////////////////////////

/* End CONSDATA.CPP *************************************************/
