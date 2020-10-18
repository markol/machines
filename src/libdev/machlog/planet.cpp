/*
 * P L A N E T . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "stdlib/string.hpp"

#include "ctl/map.hpp"
#include "ctl/vector.hpp"
#include "ctl/algorith.hpp"

#include "device/timer.hpp"

#include "machlog/planet.hpp"
#include "machlog/internal/planeti.hpp"

#include "system/pathname.hpp"
#include "utility/linetok.hpp"

#include "mathex/point2d.hpp"
#include "mathex/abox2d.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/cvexpgon.hpp"
#include "phys/cspace2.hpp"
#include "world4d/domain.hpp"
#include "world4d/root.hpp"
#include "world4d/scenemgr.hpp"

#include "machphys/compmgr.hpp"
#include "machphys/plansurf.hpp"
#include "machphys/surfchk.hpp"
#include "machlog/machine.hpp"
#include "machlog/presspad.hpp"
#include "machlog/minesite.hpp"

#include "system/metafile.hpp"
#include "system/metaistr.hpp"
#include "profiler/profiler.hpp"
#include <memory>

#ifndef _INLINE
    #include "machlog/planet.ipp"
#endif

#define CB_MachLogPlanet_DEPIMPL()	\
	PRE( pImpl_ )	\
	CB_DEPIMPL( MachPhysPlanetSurface* 		,pSurface_) \
    CB_DEPIMPL( W4dEntity* 					,pWorld_) \
	CB_DEPIMPL( W4dRoot*	   				,pHiddenRoot_) \
	CB_DEPIMPL( W4dDomain*	   				,pHiddenRootDomain_) \
    CB_DEPIMPL( PhysConfigSpace2d* 			,pConfigSpace_) \
    CB_DEPIMPL( PhysConfigSpace2d* 			,pHiddenConfigSpace_) \
    CB_DEPIMPL( MachLogPressurePads* 		,pPressurePads_) \
	CB_DEPIMPL( int 						,idsGenerated_) \
	CB_DEPIMPL( MachLogPlanetImpl::SiteMap	,siteMap_) \
	CB_DEPIMPL( MachLogPlanet::Sites		,oreSites_ ) \
	CB_DEPIMPL( MachLogPlanet::DebrisSites	,debrisSites_ );


//////////////////////////////////////////////////////////////////////////////////////////

MachLogPlanet::MachLogPlanet( void )
:	pImpl_( _NEW( MachLogPlanetImpl ) )
{
	CB_MachLogPlanet_DEPIMPL();
	W4dTransform3d localTransform;
	pHiddenRootDomain_ = _NEW( W4dDomain( pHiddenRoot_, localTransform ) );
	oreSites_.reserve( 20 );
    debrisSites_.reserve( 100 );

    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

MachLogPlanet::~MachLogPlanet()
{
	CB_MachLogPlanet_DEPIMPL();
    TEST_INVARIANT;
	_DELETE( pHiddenConfigSpace_ );
	_DELETE( pHiddenRootDomain_ );
	_DELETE( pHiddenRoot_ );
    _DELETE( pConfigSpace_ );
    _DELETE( pPressurePads_ );
    _DELETE( pSurface_ );

	while( oreSites_.size() > 0 )
	{
		_DELETE( oreSites_.front() );
		oreSites_.erase( oreSites_.begin() );
	}

	// have to erase the contents of the map too. Note that we don't delete what's pointed to here,
	// as this should have already been deleted in the pvector erasure above (the map and pvector both
	// point at the same things).
	while( siteMap_.size() > 0 )
	{
		siteMap_.erase( siteMap_.begin() );
	}

	_DELETE( pImpl_ );
}

/*
void MachLogPlanet::removeSite( int id )
{
	// This appears rather complex because we have to do double-book-keeping for our list of sites stored
	// both in pvector form and in map form - blurgh!

	CB_MachLogPlanet_DEPIMPL();

	PRE( siteExists( id ) );

	MachLogMineralSite* pSite =  siteMap_[ id ];

	// remove from our pvector first
	Sites::iterator i = find( oreSites_.begin(), oreSites_.end(), pSite );
	ASSERT( i != oreSites_.end(), "Site not found in pvector - map and pvector must be in disagreement!" );

	_DELETE( *i );
	oreSites_.erase( i );
	bool mapErasedOK = siteMap_.erase( id );
	ASSERT( mapErasedOK, "Nothing erased from map!" );
}
*/

int MachLogPlanet::nextSiteID()
{
	CB_MachLogPlanet_DEPIMPL();

	return idsGenerated_++;
}

bool MachLogPlanet::siteExists( int id )
{
	CB_MachLogPlanet_DEPIMPL();

	MachLogPlanetImpl::SiteMap::iterator i = siteMap_.find( id );

	return ( i != siteMap_.end() );
}

//////////////////////////////////////////////////////////////////////////////////////////

//static
MachLogPlanet& MachLogPlanet::instance( void )
{
    static MachLogPlanet instance_;
    return instance_;
}
//////////////////////////////////////////////////////////////////////////////////////////

//The reporter has between 20 and 50% in this function.
void MachLogPlanet::surface( W4dSceneManager* pSceneManager, const SysPathName& surfaceFilePath, BaseProgressReporter* pReporter )
{
	CB_MachLogPlanet_DEPIMPL();
    PRE( pSceneManager != NULL );
    PRE( not hasSurface() );

    //Store the world pointer
    pWorld_ = &(pSceneManager->root());

    //Construct the planet surface
    pSurface_ = _NEW( MachPhysPlanetSurface( pSceneManager, surfaceFilePath ) );

    //Generate the config space. Use file same as planet surface file pathname,
    //but with extension '.csp'
    SysPathName spaceFilePath( surfaceFilePath );
    spaceFilePath.extension( "csp" );
    initConfigSpace( spaceFilePath, pReporter );

    pImpl_->spaceFilePath_ = spaceFilePath;
    pImpl_->surfaceFilePath_ = surfaceFilePath;

	// provide the planet surface to the scene complexity manager
	MachPhysComplexityManager::instance().planetSurface( pSurface_ );
	//This call sets correctly the rendering parameters of the environment
	// attached to the surface
	MachPhysComplexityManager::instance().updateSceneParameters();

    if( CONFIG_SPACE_STREAM_ENABLED )
    {
        CONFIG_SPACE_STREAM( pImpl_->spaceFilePath_ << std::endl );
        CONFIG_SPACE_STREAM( pImpl_->surfaceFilePath_ << std::endl );

        pConfigSpace_->traceObstacles( Diag::instance().configSpaceStream() );
        CONFIG_SPACE_STREAM( std::endl );

        CONFIG_SPACE_STREAM( pImpl_->spaceFilePath_ << std::endl );
        CONFIG_SPACE_STREAM( pImpl_->surfaceFilePath_ << std::endl );

        CONFIG_SPACE_STREAM( *pConfigSpace_ );
        CONFIG_SPACE_STREAM( std::endl );
    }

    TEST_INVARIANT;
}

//////////////////////////////////////////////////////////////////////////////////////////

void MachLogPlanet::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogPlanet::initConfigSpace( const SysPathName& spaceFilePath, BaseProgressReporter* pReporter )
{
	CB_MachLogPlanet_DEPIMPL();
    PRE( hasSurface() );

    //Get the surface and its dimensions
    MachPhysPlanetSurface* pSurface = surface();

    //Construct the space itself
    MexPoint2d minBound( pSurface->xMin() - 1, pSurface->yMin() - 1 );
    MexPoint2d maxBound( pSurface->xMax() + 1, pSurface->yMax() + 1 );
    pConfigSpace_ = _NEW( PhysConfigSpace2d( minBound, maxBound,
                                             PhysConfigSpace2d::SUBTRACTIVE,
                                             DOMAIN_CLEARANCE, MachLogMachine::minHighClearance(),
                                             MachLogMachine::maxHighClearance(),
                                             MachLog::OBSTACLE_ALL ) );
	//HAL_STREAM(" creating pHiddenConfigSpace_ \n" );
    pHiddenConfigSpace_ = _NEW( PhysConfigSpace2d( minBound, maxBound,
                                             PhysConfigSpace2d::SUBTRACTIVE,
                                             DOMAIN_CLEARANCE, MachLogMachine::minHighClearance(),
                                             MachLogMachine::maxHighClearance(),
                                             MachLog::OBSTACLE_ALL ) );

	pHiddenConfigSpace_->addDomain( MexAlignedBox2d( minBound, maxBound ) );

    //Read the surface obstacle data.
    //Buffer polygon updates while doing so.
    pConfigSpace_->bufferPolygonPortalUpdates( true );
    readPlanetConfigSpaceFile( spaceFilePath, pReporter );
    pConfigSpace_->bufferPolygonPortalUpdates( false );


    pPressurePads_ = _NEW( MachLogPressurePads( minBound, maxBound ) );
}
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogPlanet::readPlanetConfigSpaceFile( const SysPathName& spaceFilePath, BaseProgressReporter* pReporter )
{
	CB_MachLogPlanet_DEPIMPL();

    SysMetaFile metaFile( "mach1.met" );

    std::auto_ptr< std::istream > pIstream;

    if( SysMetaFile::useMetaFile() )
    {
        //pIstream = _NEW( SysMetaFileIstream( metaFile, spaceFilePath, ios::text ) );
        pIstream = std::auto_ptr< std::istream >(
            _NEW( SysMetaFileIstream( metaFile, spaceFilePath, std::ios::in ) ));
    }
    else
    {
        ASSERT_FILE_EXISTS( spaceFilePath.c_str() );
        //pIstream = _NEW( ifstream( spaceFilePath.c_str(), ios::text | ios::in ) );
        pIstream = std::auto_ptr< std::istream > (
            _NEW( std::ifstream( spaceFilePath.c_str(), std::ios::in ) ));
    }

	pIstream->seekg( 0, std::ios::end );
    long nBytesInFile = pIstream->tellg();
    pIstream->seekg( 0, std::ios::beg );


    UtlLineTokeniser parser( *pIstream, spaceFilePath );

    ctl_vector< PhysConfigSpace2d::DomainId > domainIds;
    domainIds.reserve( 64 );

    //Read definitions until finished
    while( not parser.finished() )
    {
		long currentPosition = pIstream->tellg();
		//This function must report on section 20% to 50% - so scale % read of file accordingly.
		long reportAmount = 20 + ( currentPosition / _STATIC_CAST( float, nBytesInFile ) ) * 30;
		pReporter->report( reportAmount, 100 );
        ASSERT( parser.tokens().size() != 0, "" );

        if( parser.tokens()[ 0 ] == "OBSTACLE" )
        {
            //Must have line like "OBSTACLE 4 35.2", indicating obstacle with
            //4 points and height 35.2
            ASSERT( parser.tokens().size() == 3, "" );

            size_t nPoints = atoi( parser.tokens()[ 1 ].c_str() );
            ASSERT( nPoints > 2, "" );

            MATHEX_SCALAR height = atof( parser.tokens()[ 2 ].c_str() );
            ASSERT( height > 0, "" );
            parser.parseNextLine();

            //  This line might contain flags indicating the nature of the obstacle

            ObstacleFlags    obstacleFlags;
            readObstacleFlags( &parser, &obstacleFlags );

            //Read each point, pushing a MexPoint2d onto a vector
            MexConvexPolygon2d::Points* pPoints = _NEW( MexConvexPolygon2d::Points );
            pPoints->reserve( nPoints );
            while( nPoints-- != 0 )
            {
                ASSERT( parser.tokens().size() == 2, "" );
                MATHEX_SCALAR x = atof( parser.tokens()[ 0 ].c_str() );
                MATHEX_SCALAR y = atof( parser.tokens()[ 1 ].c_str() );
                pPoints->push_back( MexPoint2d( x, y ) );
                parser.parseNextLine();
            }

            //Construct a new polygon from the points, and add to the config space
            std::auto_ptr< MexConvexPolygon2d::Points > pointsAPtr( pPoints );
            MexConvexPolygon2d* pPolygon = _NEW( MexConvexPolygon2d( pointsAPtr ) );
            std::auto_ptr< MexPolygon2d > polygonAPtr( pPolygon );
            pConfigSpace_->add( polygonAPtr, height, obstacleFlags, PhysConfigSpace2d::PERMANENT );
        }
        else if( parser.tokens()[ 0 ] == "NEWDOMAIN" )
        {
           	//Must have line like 	"NEWDOMAIN <uniqueId> <number of verticies>
          	//						<x1> <y1>
          	//						<x2> <y2>
          	//						<x3> <y3>"

            ASSERT( parser.tokens().size() == 3, "" );

            //Check domain index in sequence
            ASSERT( atoi( parser.tokens()[ 1 ].c_str() ) == domainIds.size(), "" );

			size_t nPoints = atoi( parser.tokens()[ 2 ].c_str() );
            ASSERT( nPoints > 2, "" );

            parser.parseNextLine();

            //Read each point, pushing a MexPoint2d onto a vector
            MexConvexPolygon2d::Points* pPoints = _NEW( MexConvexPolygon2d::Points );
            pPoints->reserve( nPoints );
            while( nPoints-- != 0 )
            {
                ASSERT( parser.tokens().size() == 2, "" );
                MATHEX_SCALAR x = atof( parser.tokens()[ 0 ].c_str() );
                MATHEX_SCALAR y = atof( parser.tokens()[ 1 ].c_str() );
                pPoints->push_back( MexPoint2d( x, y ) );
                parser.parseNextLine();
            }

            //Construct a new polygon from the points, and add to the config space
            std::auto_ptr< MexConvexPolygon2d::Points > pointsAPtr( pPoints );
            MexConvexPolygon2d* pPolygon = _NEW( MexConvexPolygon2d( pointsAPtr ) );
            std::auto_ptr< MexPolygon2d > polygonAPtr( pPolygon );

            //Add the domain
			MexAlignedBox2d domainBoundary;
			pPolygon->boundary( &domainBoundary );

            PhysConfigSpace2d::DomainId domainId = pConfigSpace_->addDomain( domainBoundary, polygonAPtr );
			domainIds.push_back( domainId );
        }
        else if( parser.tokens()[ 0 ] == "DOMAIN" )
        {
            //Must have line like "DOMAIN 4 88 88 262 291"
            //ie DOMAIN <index> <xmin> <ymin> <xmax> <ymax>
            ASSERT( parser.tokens().size() == 6, "" );

            //Check domain index in sequence
            ASSERT( atoi( parser.tokens()[ 1 ].c_str() ) == domainIds.size(), "" );

            //Read the boundary data
            MATHEX_SCALAR xMin = atof( parser.tokens()[ 2 ].c_str() );
            MATHEX_SCALAR yMin = atof( parser.tokens()[ 3 ].c_str() );
            MATHEX_SCALAR xMax = atof( parser.tokens()[ 4 ].c_str() );
            MATHEX_SCALAR yMax = atof( parser.tokens()[ 5 ].c_str() );
            ASSERT( xMin < xMax, "" );
            ASSERT( yMin < yMax, "" );

            parser.parseNextLine();

            //Add the domain
            PhysConfigSpace2d::DomainId domainId =
                pConfigSpace_->addDomain( MexAlignedBox2d( xMin, yMin, xMax, yMax) );
            domainIds.push_back( domainId );
        }
        else if( parser.tokens()[ 0 ] == "PORTAL" )
        {
            //Must have line like "PORTAL 4 7 88 88 262 291"
            //ie PORTAL <domain index> <domain index> <x1> <y1> <x2> <y2>
            ASSERT( parser.tokens().size() == 7, "" );

            //Get the two domain ids and check defined
            size_t index1 = atoi( parser.tokens()[ 1 ].c_str() );
            size_t index2 = atoi( parser.tokens()[ 2 ].c_str() );
            ASSERT( index1 < domainIds.size(), "" );
            ASSERT( index2 < domainIds.size(), "" );

            //Get the endpoint coordinates
            MATHEX_SCALAR x1 = atof( parser.tokens()[ 3 ].c_str() );
            MATHEX_SCALAR y1 = atof( parser.tokens()[ 4 ].c_str() );
            MATHEX_SCALAR x2 = atof( parser.tokens()[ 5 ].c_str() );
            MATHEX_SCALAR y2 = atof( parser.tokens()[ 6 ].c_str() );

            parser.parseNextLine();

            //Add the portal
            pConfigSpace_->addPortal( domainIds[ index1 ], domainIds[ index2 ],
                                      MexPoint2d( x1, y1 ), MexPoint2d( x2, y2 ) );
        }
        else
        {
            ASSERT_FAIL( "" );
        }
    }
}

void MachLogPlanet::readObstacleFlags( UtlLineTokeniser* pParser, ObstacleFlags* pObstacleFlags )
{
    *pObstacleFlags = 0;

    bool    finished = false;

    while( not finished )
    {
        for( size_t i = 0; i < pParser->tokens().size(); ++i )
        {
            if( pParser->tokens()[ i ] == "WATER" )
            {
                (*pObstacleFlags) |= MachLog::OBSTACLE_WATER;
            }
            else if( pParser->tokens()[ i ] == "LOW_OBSTACLE" )
            {
                (*pObstacleFlags) |= MachLog::OBSTACLE_LOW;
            }
            else
                finished = true;
        }

        if( not finished )
            pParser->parseNextLine();
    }
}

void MachLogPlanet::newSite( MachLogMineralSite* pSite )
{
	CB_MachLogPlanet_DEPIMPL();

	int newSiteId = nextSiteID();
	siteMap_.insert( newSiteId, pSite );
	pSite->id( newSiteId );

	oreSites_.push_back( pSite );
}

MachLogMineralSite* MachLogPlanet::pSite( int id )
{
	CB_MachLogPlanet_DEPIMPL();

	PRE( siteExists( id ) );

	return siteMap_[ id ];
}


/*
MachLogPlanet::Sites&
MachLogPlanet::sites()
{
	CB_MachLogPlanet_DEPIMPL();
	return oreSites_;
}
*/

const MachLogPlanet::Sites&
MachLogPlanet::sites() const
{
	CB_MachLogPlanet_DEPIMPL();
	return oreSites_;
}

void MachLogPlanet::newDebrisSite( MachLogDebris* pDebris )
{
	CB_MachLogPlanet_DEPIMPL();

	debrisSites_.push_back( pDebris );
}

void MachLogPlanet::removeDebrisSite( MachLogDebris* pDebris )
{
	CB_MachLogPlanet_DEPIMPL();

	DebrisSites::iterator i = find( debrisSites_.begin(), debrisSites_.end(), pDebris );

	ASSERT( i != debrisSites_.end(), "Debris site was not found in collection!" );

	debrisSites_.erase( i );
}

const MachLogPlanet::DebrisSites&
MachLogPlanet::debrisSites() const
{
	CB_MachLogPlanet_DEPIMPL();
	return debrisSites_;
}

W4dEntity& MachLogPlanet::hiddenRoot()
{
	CB_MachLogPlanet_DEPIMPL();
	return *pHiddenRootDomain_;
}

const W4dEntity& MachLogPlanet::hiddenRoot() const
{
	CB_MachLogPlanet_DEPIMPL();
	return *pHiddenRootDomain_;
}

PhysConfigSpace2d& MachLogPlanet::hiddenConfigSpace() const
{
	CB_MachLogPlanet_DEPIMPL();
	return *pHiddenConfigSpace_;
}

ostream& operator<<( ostream& o, const MachLogPlanet& t )
{
	o << "MLPlanet:: Mineral Sites number " << t.sites().size() << "\n";
	for( MachLogPlanet::Sites::const_iterator i = t.sites().begin(); i != t.sites().end(); ++i)
		o << (**i) << std::endl;
	return o;

}

void MachLogPlanet::clear()
{
    //Delete the physical surface model
	CB_MachLogPlanet_DEPIMPL();
    _DELETE( pSurface_ );
    pSurface_ = NULL;
    pWorld_ = NULL;

    //Delete the planet exterior config space, and the hidden one
    _DELETE( pConfigSpace_ );
    pConfigSpace_ = NULL;

    _DELETE( pHiddenConfigSpace_ );
    pHiddenConfigSpace_ = NULL;

    //Delete the pressure pads object
    _DELETE( pPressurePads_ );
    pPressurePads_ = NULL;

    //Remove any mineral sites
    while( oreSites_.size() != 0 )
    {
        MachLogMineralSite* pSite = oreSites_.back();
        oreSites_.pop_back();
        _DELETE( pSite );
    }

	// have to erase the contents of the map too. Note that we don't delete what's pointed to here,
	// as this should have already been deleted in the pvector erasure above (the map and pvector both
	// point at the same things).
	siteMap_.erase( siteMap_.begin(), siteMap_.end() );

	// clear the planet surface in scene complexity manager
	MachPhysComplexityManager::instance().planetSurface( NULL );

    POST( not hasSurface() );
}

bool MachLogPlanet::hasSurface( void ) const
{
	CB_MachLogPlanet_DEPIMPL();
	return pSurface_ != NULL;
}
//////////////////////////////////////////////////////////////////////////////////////////

MachPhysPlanetSurface* MachLogPlanet::surface( void ) const
{
	CB_MachLogPlanet_DEPIMPL();
	PRE( hasSurface() );
	return pSurface_;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool MachLogPlanet::hasWorld( void ) const
{
	CB_MachLogPlanet_DEPIMPL();
	return pWorld_ != NULL;
}
//////////////////////////////////////////////////////////////////////////////////////////

W4dEntity* MachLogPlanet::pWorld( void ) const
{
	CB_MachLogPlanet_DEPIMPL();
	return pWorld_;
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysConfigSpace2d& MachLogPlanet::configSpace()
{
	CB_MachLogPlanet_DEPIMPL();
    PRE( pConfigSpace_ != NULL )
    return *pConfigSpace_;
}
//////////////////////////////////////////////////////////////////////////////////////////

MachLogPressurePads& MachLogPlanet::pressurePads()
{
	CB_MachLogPlanet_DEPIMPL();
    PRE( pPressurePads_ != NULL )
    return *pPressurePads_;
}

//////////////////////////////////////////////////////////////////////////////////////////

MexPoint3d MachLogPlanet::safeWorldCoordinate( const MexPoint3d& potentiallyUnsafeCoordinate ) const
{
	MATHEX_SCALAR xVal = potentiallyUnsafeCoordinate.x();
	MATHEX_SCALAR yVal = potentiallyUnsafeCoordinate.y();
	//MATHEX_SCALAR zVal = potentiallyUnsafeCoordinate.z();

	const MachPhysPlanetSurface& planetSurface = *surface();

	if( xVal < planetSurface.xMin() )
		xVal = 0;
	else if( xVal > planetSurface.xMax() )
		xVal = planetSurface.xMax();

	if( yVal < planetSurface.yMin() )
		yVal = 0;
	else if( yVal > planetSurface.yMax() )
		yVal = planetSurface.yMax();

	return MexPoint3d( xVal, yVal, potentiallyUnsafeCoordinate.z() );
}

//////////////////////////////////////////////////////////////////////////////////////////

#ifndef PRODUCTION

bool    MachLogPlanet::planetSurfaceOK( ostream& os ) const
{
	CB_MachLogPlanet_DEPIMPL();

    MachPhysPlanetSurfaceChecker    checker( *pSurface_ );
    MachPhysPlanetSurfaceChecker::Triangles triangles;

    checker.checkGradients( *pConfigSpace_, &triangles );

    os << "Failed triangles: " << std::endl;
    for( size_t i = 0; i < triangles.size(); ++i )
    {
        os << triangles[ i ].point1() << " ";
        os << triangles[ i ].point2() << " ";
        os << triangles[ i ].point3() << std::endl;

        ASSERT_INFO(
          triangles[ i ].point1() << " " <<
          triangles[ i ].point2() << " " <<
          triangles[ i ].point3() );
    }

    os << "End of failed triangles" << std::endl;

//    return triangles.size() == 0;
    return true;
}

void MachLogPlanet::checkPlanet( ostream& os ) const
{
	CB_MachLogPlanet_DEPIMPL();

    DevTimer    timer;

    os << std::endl << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
    os << std::endl << std::endl;

    os << pImpl_->spaceFilePath_ << std::endl;
    os << pImpl_->surfaceFilePath_ << std::endl;

    pConfigSpace_->traceObstacles( os );
    os << std::endl;

    os << "Time to trace obstacles " << timer.time() << std::endl << std::endl;
    timer.time( 0.0 );

    pConfigSpace_->traceIllegalPoints( os );
    os << std::endl;

    os << "Time to trace illegal points " << timer.time() << std::endl << std::endl;
    timer.time( 0.0 );

    const bool planetOK = planetSurfaceOK( os );

    os << "Time to check triangles " << timer.time() << std::endl << std::endl;

    //  Commented out because it takes up lots of space
    //  and isn't particularly useful

//     os << pImpl_->spaceFilePath_ << std::endl;
//     os << pImpl_->surfaceFilePath_ << std::endl;
//
//     os << *pConfigSpace_;
//     os << std::endl;
}

#endif

/* End PLANET.CPP ***************************************************/
