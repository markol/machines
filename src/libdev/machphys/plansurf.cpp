/*
 * P L A N S U R F . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "base/base.hpp"
//#include <conio.h>
#include <ctype.h>

#include "stdlib/string.hpp"
#include "utility/linetok.hpp"
#include "mathex/quatern.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/epsilon.hpp"
#include "mathex/vec2.hpp"
#include "mathex/point2d.hpp"
#include "mathex/abox2d.hpp"
#include "mathex/abox3d.hpp"
#include "mathex/sphere3d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/quad3d.hpp"
#include "mathex/grid2d.hpp"
#include "mathex/line2d.hpp"
#include "phys/eulertx.hpp"
#include "phys/rampacce.hpp"
#include "ctl/list.hpp"
#include "render/texmgr.hpp"
#include "render/mesh.hpp"
#include "render/surfmgr.hpp"
#include "render/texset.hpp"
#include "world4d/domain.hpp"
#include "world4d/portal.hpp"
#include "world4d/scenemgr.hpp"
#include "world4d/root.hpp"
#include "world4d/soundman.hpp"
#include "world4d/light.hpp"
#include "envirnmt/planet.hpp"

#include "system/pathname.hpp"
#include "system/fileenum.hpp"

#include "machphys/plansurf.hpp"
#include "machphys/terrtile.hpp"
#include "machphys/tiledata.hpp"
#include "machphys/machexp.hpp"
#include "machphys/domasign.hpp"

#include "machphys/internal/plansuri.hpp"

#include "profiler/stktrace.hpp"

#ifndef _INLINE
    #include "machphys/plansurf.ipp"
#endif

PER_DEFINE_PERSISTENT( MachPhysPlanetSurface );

// I can't believe Iain had to write this !	(see meshload.cpp)
static void lowerString( string* pString )
{
	if (!pString || pString->length() == 0)
		return;

    char ch;
    for( size_t i = 0; (ch = (*pString)[i]) != '\0'; ++i )
        (*pString)[i] = (char)tolower( ch );
}

MachPhysPlanetSurface::MachPhysPlanetSurface( W4dSceneManager* pSceneManager, const SysPathName& pathname )
: pSceneManager_( pSceneManager ),
  psfFile_( pathname ) ,
  pEnvironment_( NULL ),
  pDomainAssign_(_NEW(MachPhysDomainAssignor(this))),
  pTextureSet_( NULL ),
  pImpl_( _NEW( MachPhysPlanetSurfaceImpl() ) )
{
    PRE( pSceneManager != NULL );

    //Construct the object which knows about the layout of domains on the planet surface
	pSceneManager_->domainAssignor(pDomainAssign_);

    //Add the planet's texture directory to the texture search path
    RenTexManager::PathNames oldSearchList = RenTexManager::instance().searchList();
    RenTexManager::PathNames tempSearchList( oldSearchList );

    SysPathName tempPath( pathname );
    tempPath.pop_back();
    string textureDirectory = tempPath.pathname();

	// hack: we check whether the actual list is set for a 2 or a
	// 4 Mbytes card and update the list accordingly
	bool directory4MbNotFound=true;
	bool directory2MbNotFound=true;
	bool noTexturePreload = true;

	for( RenTexManager::PathNames::const_iterator itDirectories=RenTexManager::instance().searchList().begin();
	     itDirectories!=RenTexManager::instance().searchList().end() and directory4MbNotFound and directory2MbNotFound;
	     ++itDirectories )
	{
	  const SysPathName::Components& components = (*itDirectories).components();
	  for( SysPathName::Components::const_iterator itComponents = components.begin();
	     itComponents != components.end() and directory4MbNotFound and directory2MbNotFound;
	     ++itComponents )
	  {
		 string componentAsLowerCase = (*itComponents);
		 lowerString( &componentAsLowerCase );
		 if( componentAsLowerCase == "texture4" )
		   directory4MbNotFound=false;
		 if( componentAsLowerCase == "texture2" )
		   directory2MbNotFound=false;
		 if( componentAsLowerCase == "preload" )
			noTexturePreload=false;
	  }
	}

	if( directory4MbNotFound )
	{
	  // by default 2 Mbytes configuration
	  RENDER_STREAM("Planet surface will load textures for a 2 Mbytes video card" << std::endl);
      textureDirectory += "/texture2";
	}
	else
	{
	  // 4 MBytes configuration
	  RENDER_STREAM("Planet surface will load textures for a 4 Mbytes video card" << std::endl);
      textureDirectory += "/texture4";
	}

	if( noTexturePreload )
	{
	    LIONEL_STREAM("setting up search path for run-time planet texture loading  " << std::endl);
	    tempSearchList.push_back( SysPathName( textureDirectory ) );
	    RenTexManager::instance().searchList( tempSearchList );
		LIONEL_STREAM("searchList " << tempSearchList);
	    LIONEL_STREAM("done setting up search path for run-time planet texture loading  " << std::endl);
	}
	else // do texture preload
	{
		LIONEL_STREAM("preloading planet texture bitmaps " << std::endl);
		// preload the textures, this overrrides the run time loading based on the search path
		pTextureSet_ = _NEW( RenTextureSet( textureDirectory ) );
		LIONEL_STREAM("done preloading planet texture bitmaps " << std::endl);
	}

    //Load the planet from surface or persistent file
    loadPlanet( pathname );

    //Load the environment
    SysPathName environmentPath( pathname );
    environmentPath.extension( "env" );

	//Create an environment for the sky etc
	pEnvironment_ = _NEW( EnvPlanetEnvironment( environmentPath, pSceneManager ) );

    //Make the scene manager use it
	pSceneManager->environment( pEnvironment_ );

    //Restore the texture search path
    RenTexManager::instance().searchList( oldSearchList );

    TEST_INVARIANT;
}

MachPhysPlanetSurface::~MachPhysPlanetSurface()
{
    TEST_INVARIANT;

    //Clear any outstanding sounds
    W4dSoundManager::instance().unloadAll();

    //Delete all the planet domains, which will in turn delete all the tiles and portals
    for( size_t yTile = 0; yTile < nTilesY_; ++yTile )
    {
        for( size_t xTile = 0; xTile < nTilesX_; ++xTile )
        {
            //Get the domain for this tile
            W4dDomain* pDomain = tileDomainArray_[yTile][xTile];
            _DELETE( pDomain );
        }
    }

    //Clear out any tile databases
    MachPhysTerrainTile::clearAll();

	if (pDomainAssign_ == pSceneManager_->domainAssignor())
		pSceneManager_->domainAssignor(NULL);

	_DELETE(pDomainAssign_);

	_DELETE(pTextureSet_);

    //Clear the background
    pSceneManager_->clearAllLights();
    pSceneManager_->useBackground( NULL );
    pSceneManager_->environment( NULL );
    _DELETE( pEnvironment_ );

	//Clear the unused meshes
	RenMesh::emptyCache();
	RenMesh::removeAllFromFactory();

    _DELETE( pImpl_ );
}

void MachPhysPlanetSurface::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}


// PLANET_SIZE	2	2
// TILE_SIZE		350
// TILE_CELLS 6 6
// TILE_BOUNDS -175 -175 175 175
// BACKGROUND_FILE sky.bg
//
// ROTATION	2
// HEIGHT	53.1
// TILE	bsets/mars/mrs0a2n0.lod
// ROTATION	0
// HEIGHT	0.0
// TILE	bsets/mars/mrs00an0.lod
// ROTATION	1
// HEIGHT	0.0
// TILE	bsets/mars/mrs01an0.lod
// ROTATION	2
// HEIGHT	0.0
// TILE	bsets/mars/mrs00110.lod

void MachPhysPlanetSurface::readPlanetSurfaceFile( const SysPathName& pathname )
{
    PRE( pSceneManager_ != NULL );
    CB_DEPIMPL( W4dRoot*, pPersistenceRoot_ );

    //Use the temporary root in the pimple as the parent, until we have saved the persistent file.
    W4dEntity* pParent = pPersistenceRoot_;

    UtlLineTokeniser   parser( pathname );

    parseHeader( &parser );

    for( size_t y = 0; y < nTilesY_; ++y )
    {
        tileArray_.push_back( TileRow() );
        tileDomainArray_.push_back( TileDomainRow() );

        for( size_t x = 0; x < nTilesX_; ++x )
        {
            //Construct transform for the domain
            MexPoint3d location( x * edgeLength_, y * edgeLength_, 0.0 );
            MexTransform3d domainTransform( location );

            //Construct the domain
            W4dDomain* pDomain = _NEW( W4dDomain( pParent, domainTransform ) );
            tileDomainArray_.back().push_back( pDomain );
            parseTile( SysPathName( pathname.directory() ), &parser, pDomain );

            //Name the domain
            char domainName[9] = "domainxy";
            domainName[6] = char('0' + (char)x);
            domainName[7] = char('0' + (char)y);
            pDomain->name( string( domainName ) );
        }
    }

	if ( not parser.finished() )
	    parser.parseNextLine();

    ASSERT( parser.finished(), "" );
}

void MachPhysPlanetSurface::parseHeader( UtlLineTokeniser* pParser )
{
    ASSERT_INFO( pParser->tokens()[ 0 ] );
    ASSERT( pParser->tokens()[ 0 ] == "PLANET_SIZE", "" );
    ASSERT( pParser->tokens().size() == 3, "" );

    nTilesX_ = atoi( pParser->tokens()[ 1 ].c_str() );
    nTilesY_ = atoi( pParser->tokens()[ 2 ].c_str() );

    pParser->parseNextLine();
    ASSERT_INFO( pParser->tokens()[ 0 ] );
    ASSERT( pParser->tokens()[ 0 ] == "TILE_SIZE", "" );
    ASSERT( pParser->tokens().size() == 2, "" );

    edgeLength_ = atof( pParser->tokens()[ 1 ].c_str() );
	recipEdgeLength_ = 1.0/edgeLength_;

    //Number of cells in x and y
    //TILE_CELLS 6 6
    pParser->parseNextLine();
    ASSERT_INFO( pParser->tokens()[ 0 ] );
    ASSERT( pParser->tokens()[ 0 ] == "TILE_CELLS", "" );
    ASSERT( pParser->tokens().size() == 3, "" );

    nTileXVertices_ = 1 + atoi( pParser->tokens()[ 1 ].c_str() );
    nTileYVertices_ = 1 + atoi( pParser->tokens()[ 2 ].c_str() );

    //Min and max tile coordinates relative to origin. Note that tile might be
    //slightly bigger than nominal size as stated in TILE_SIZE, due to the need
    //for overlap to eliminate gaps.
    //TILE_BOUNDS <xmin> <ymin> <xmax> <ymax>
    pParser->parseNextLine();
    ASSERT_INFO( pParser->tokens()[ 0 ] );
    ASSERT( pParser->tokens()[ 0 ] == "TILE_BOUNDS", "" );
    ASSERT( pParser->tokens().size() == 5, "" );

    tileXMin_ = atof( pParser->tokens()[ 1 ].c_str() );
    tileYMin_ = atof( pParser->tokens()[ 2 ].c_str() );
    tileXMax_ = atof( pParser->tokens()[ 3 ].c_str() );
    tileYMax_ = atof( pParser->tokens()[ 4 ].c_str() );
    ASSERT( tileXMin_ < tileXMax_, "");
    ASSERT( tileYMin_ < tileYMax_, "");

	// BACKGROUND_FILE <filename>
    pParser->parseNextLine();
    ASSERT_INFO( pParser->tokens()[ 0 ] );
    ASSERT( pParser->tokens()[ 0 ] == "BACKGROUND_FILE", "" );

	if (pParser->tokens().size() == 2 && pParser->tokens()[1] != "none")
    {
        backgroundFile_ = pParser->fileName().directory();
		backgroundFile_.combine( pParser->tokens()[1] );
    }

    // GRAVITY 9.81 (optional)
    pParser->parseNextLine();
    ASSERT_INFO( pParser->tokens()[ 0 ] );
    if( pParser->tokens()[ 0 ] == "GRAVITY" )
    {
        double  gravity = atof( pParser->tokens()[ 1 ].c_str() );

        //  TBD: This is a hack to get the gravity into MachPhysMachineExplosion
        //  without changing the size of MachPhysPlanetSurface.

        MachPhysMachineExplosion::acceleration().z( -gravity );
    }
    else
    {
        pParser->repeatLine();
    }

    // MIN_MACHINE_HEIGHT 0.0 (optional)
    pParser->parseNextLine();
    ASSERT_INFO( pParser->tokens()[ 0 ] );
    if( pParser->tokens()[ 0 ] == "MIN_MACHINE_HEIGHT" )
    {
        pImpl_->minMachineHeight_ = atof( pParser->tokens()[ 1 ].c_str() );
    }
    else
    {
        pParser->repeatLine();
    }
}

void MachPhysPlanetSurface::parseTile( const SysPathName& directoryname, UtlLineTokeniser* pParser, W4dEntity* pParent )
{
	if ( not pParser->finished() )
	    pParser->parseNextLine();

   	if ( 	pParser->finished() or
   			pParser->tokens()[ 0 ] != "ROTATION" )
	{
		// Setup default tile
    	double height = -0.065;

	    MexPoint3d		newPosition( -tileXMin_, -tileYMin_, height );
	    MexVec3         zAxis( 0.0, 0.0, 1.0 );
	    MexQuaternion  	rotation;

	    rotation.set( zAxis, 0 );

	    MexTransform3d  transform;
	    transform.position( newPosition );
	    transform.rotation( rotation );

	    SysPathName tilePath( directoryname );
	    tilePath.combine( SysPathName( "lod" ) );
		SysFileEnumerator finder( tilePath, SysPathName( "*.lod" ) );
		// Execute search
		finder.find();

		ASSERT( not finder.isEmpty(), tilePath.pathname().c_str() );

	    //Construct the tile and add to the collection
	    MachPhysTerrainTile* pTile =
	        _NEW( MachPhysTerrainTile( pParent, finder.files()[0].pathName(), transform,
	                                   nTileXVertices_, nTileYVertices_,
	                                   tileXMin_, tileXMax_, tileYMin_, tileYMax_ ) );
	    tileArray_.back().push_back( pTile );
		const MexAlignedBox3d& boundary = pTile->boundingVolume();
		if( ( boundary.maxCorner().x() - boundary.minCorner().x() > edgeLength_ ) or
		    ( boundary.maxCorner().y() - boundary.minCorner().y() > edgeLength_ ) )
		{
			PLANET_SURFACE_STREAM("Tile Problem detected (N): " << finder.files()[0].pathName() << std::endl );
			PATH_PROFILE_STREAM("Tile Problem Detected : " << finder.files()[0].pathName() << std::endl << pTile->tileData() << std::endl );
		}
	}
	else
	{
		ASSERT_INFO( pParser->tokens()[ 0 ] );
		ASSERT( pParser->tokens()[ 0 ] == "ROTATION", "" );
	    ASSERT( pParser->tokens().size() == 2, "" );

	    double rotationAngle = atof( pParser->tokens()[ 1 ].c_str() );

	    pParser->parseNextLine();
	    ASSERT_INFO( pParser->tokens()[ 0 ] );
	    ASSERT( pParser->tokens()[ 0 ] == "HEIGHT", "" );
	    ASSERT( pParser->tokens().size() == 2, "" );

	    double height = atof( pParser->tokens()[ 1 ].c_str() );

	    MexPoint3d   newPosition( -tileXMin_, -tileYMin_, height );
	    MexVec3         zAxis( 0.0, 0.0, 1.0 );
	    MexQuaternion  rotation;

	    //  The rotation stored in the file has positive going anti
	    //  clockwise, we use positive going clockwise internally
	    rotation.set( zAxis, -rotationAngle * 90.0 * 3.1415926535 / 180.0 );

	    MexTransform3d  transform;
	    transform.position( newPosition );
	    transform.rotation( rotation );

	    pParser->parseNextLine();
	    ASSERT_INFO( pParser->tokens()[ 0 ] );
	    ASSERT( pParser->tokens()[ 0 ] == "TILE", "" );
	    ASSERT( pParser->tokens().size() == 2, "" );

	    SysPathName tilePathname( directoryname );
	    tilePathname.combine( SysPathName( pParser->tokens()[ 1 ] ) );

	    //Construct the tile and add to the collection
	    MachPhysTerrainTile* pTile =
	        _NEW( MachPhysTerrainTile( pParent, tilePathname, transform,
	                                   nTileXVertices_, nTileYVertices_,
	                                   tileXMin_, tileXMax_, tileYMin_, tileYMax_ ) );
	    tileArray_.back().push_back( pTile );
		const MexAlignedBox3d& boundary = pTile->boundingVolume();
		if( ( boundary.maxCorner().x() - boundary.minCorner().x() > edgeLength_ ) or
			( boundary.maxCorner().y() - boundary.minCorner().y() > edgeLength_ ) )
		{
			PLANET_SURFACE_STREAM("Tile Problem detected (R): " << tilePathname << std::endl );
			PATH_PROFILE_STREAM("Tile Problem Detected : " << tilePathname << std::endl << pTile->tileData() << std::endl );
		}
	}
}

bool MachPhysPlanetSurface::save()
{
	std::ofstream psfFileStream( psfFile_.pathname().c_str() );
	bool saveSuccessful = psfFileStream.is_open();

	if ( saveSuccessful )
	{
        if( !backgroundFile_.existsAsFile() )
            std::ofstream outfile (backgroundFile_.c_str());
		psfFileStream << "PLANET_SIZE " << nTilesX_ << " " << nTilesY_ << std::endl;
		psfFileStream << "TILE_SIZE " << edgeLength_ << std::endl;
		psfFileStream << "TILE_CELLS " << nTileXVertices_ - 1 << " " << nTileYVertices_ - 1 << std::endl;
		psfFileStream << "TILE_BOUNDS " << tileXMin_ << " " << tileYMin_ << " " << tileXMax_ << " " << tileYMax_ << std::endl;
		psfFileStream << "BACKGROUND_FILE " << backgroundFile_.filename() << std::endl << std::endl;

	    for( size_t y = 0; y < nTilesY_; ++y )
	    {
        	for( size_t x = 0; x < nTilesX_; ++x )
	        {
				psfFileStream << "// Tile(" << x << "," << y << ")" << std::endl;

				MachPhysTerrainTile* pTile = tileArray_[y][x];
				const MexTransform3d& localTrans = pTile->localTransform();

				// Output rotation
				MexEulerAngles eulerAngle = localTrans.rotationAsEulerAngles();
				MexDegrees 	   degrees = eulerAngle.azimuth();

				int intRotation = 0;
				if ( degrees.asScalar() > 85.0 and degrees.asScalar() < 95.0 )
					intRotation = 3;
				else if ( degrees.asScalar() > -185.0 and degrees.asScalar() < -175.0 )
					intRotation = 2;
				else if ( degrees.asScalar() > 175.0 and degrees.asScalar() < 185.0 )
					intRotation = 2;
				else if ( degrees.asScalar() > -95.0 and degrees.asScalar() < -85.0 )
					intRotation = 1;

				psfFileStream << "ROTATION " << intRotation << std::endl;

				// Output height
				const MexPoint3d& localPos = localTrans.position();
				psfFileStream << "HEIGHT " << localPos.z() << std::endl;

			   	// Output .lod file used
				SysPathName shortLodPath( "lod" );
				if( !pTile->lodFileName().existsAsFile() )
                    std::ofstream outfile (pTile->lodFileName().c_str());
				shortLodPath.combine( pTile->lodFileName().filename() );

				psfFileStream << "TILE " <<  shortLodPath << std::endl << std::endl;
	    	}
	    }
		saveSuccessful = not psfFileStream.fail();
	}

	return saveSuccessful;
}

void MachPhysPlanetSurface::registerLoadedTiles() const
{
    //Loop through all the tiles
    for( size_t yTile = 0; yTile < nTilesY_; ++yTile )
        for( size_t xTile = 0; xTile < nTilesX_; ++xTile )
            tileArray_[yTile][xTile]->registerExemplar();
}

int MachPhysPlanetSurface::xIndexClipped(const MexPoint3d& position) const
{
    int result = xIndex(position);
    if( result >= nTilesX_ )
		result = nTilesX_ - 1;
	else if (result < 0)
		result = 0;

	POST(result >=0 && result < nTilesX_);
	return result;
}

int MachPhysPlanetSurface::yIndexClipped(const MexPoint3d& position) const
{
    int result = yIndex(position);
    if( result >= nTilesY_ )
		result = nTilesY_ - 1;
	else if (result < 0)
		result = 0;

	POST(result >=0 && result < nTilesY_);
	return result;
}

bool MachPhysPlanetSurface::contains(const MexPoint3d& position) const
{
    const MATHEX_SCALAR eps = MexEpsilon::instance();
    const MATHEX_SCALAR x = position.x();
    const MATHEX_SCALAR y = position.y();

    PRE_INFO( eps );
    PRE_INFO( edgeLength_ );
    PRE_INFO( nTilesX_ );
    PRE_INFO( nTilesY_ );

    return (x > -eps && x < eps + edgeLength_ * nTilesX_) &&
    	   (y > -eps && y < eps + edgeLength_ * nTilesY_);
}

bool MachPhysPlanetSurface::contains(const MexAlignedBox3d& box) const
{
	return contains(box.minCorner()) && contains(box.maxCorner());
}

MachPhysTerrainTile* MachPhysPlanetSurface::pTile( const MexPoint3d& position ) const
{
    PRE( edgeLength_ != 0.0 );
    PRE_INFO(position);
    PRE(contains(position));

    MachPhysTerrainTile* pTile = tileArray_[yIndexClipped(position)][xIndexClipped(position)];
    POST( pTile != NULL );

    return pTile;
}

ostream& operator <<( ostream& o, const MachPhysPlanetSurface& t )
{

    o << "MachPhysPlanetSurface " << (void*)&t << " start" << std::endl;
    o << "MachPhysPlanetSurface " << (void*)&t << " end" << std::endl;

    return o;
}

W4dDomain* MachPhysPlanetSurface::domainAt( const MexPoint3d& point ) const
{
    //Get the tile and return its parent as a domain
    PRE(contains(point));
    return pTile( point )->containingDomain();
}

MATHEX_SCALAR MachPhysPlanetSurface::xMin( void ) const
{
    return 0;
}

MATHEX_SCALAR MachPhysPlanetSurface::yMin( void ) const
{
    return 0;
}

MATHEX_SCALAR MachPhysPlanetSurface::xMax( void ) const
{
    return nTilesX_ * edgeLength_;
}

MATHEX_SCALAR MachPhysPlanetSurface::yMax( void ) const
{
    return nTilesY_ * edgeLength_;
}

MATHEX_SCALAR MachPhysPlanetSurface::terrainHeight( MATHEX_SCALAR x, MATHEX_SCALAR y ) const
{
    return terrainHeight( x, y, NULL );
}

MATHEX_SCALAR MachPhysPlanetSurface::terrainHeight( MATHEX_SCALAR x, MATHEX_SCALAR y, MexVec3* pUnitNormal ) const
{
    //Get the tile at these coordinates
    MexPoint3d pos( x, y, 0 );
    MachPhysTerrainTile* pXYTile = pTile( pos );

    //Get the inverse of its transform
    MexTransform3d tileInverseTransform;
    const MexTransform3d& tileTransform = pXYTile->globalTransform();
    tileTransform.invert( &tileInverseTransform );

    //Get the z location of the tile
    MexPoint3d tileLocation;
    tileTransform.position( &tileLocation );

    //Compute the point coordinates in the tile's local space
    tileInverseTransform.transform( &pos );

    //Get the tile boundary for this tile
    const MachPhysTileData& tileData = pXYTile->tileData();

    //Hence get the tile height
    MATHEX_SCALAR answer = tileLocation.z() + tileData.height( pos.x(), pos.y(), pUnitNormal );

    //If required, convert the normal to global system
    if( pUnitNormal )
        tileTransform.transform( pUnitNormal );

    return answer;
}

void MachPhysPlanetSurface::makePortals( void )
{
    MATHEX_SCALAR maxHeight = 30000.0;
    MATHEX_SCALAR x;
    MATHEX_SCALAR y;
    MATHEX_SCALAR height, minHeight;
    MATHEX_SCALAR eps = MexEpsilon::instance();
    MexTransform3d identityTransform;
  	//ofstream outputStream( "portals.log" );

    //Loop through all the tiles
    for( size_t yTile = 0; yTile < nTilesY_; ++yTile )
    {
        for( size_t xTile = 0; xTile < nTilesX_; ++xTile )
        {
            //Get the domain for this tile, and the tile itself
            W4dDomain* pOwnerDomain = tileDomainArray_[yTile][xTile];
            MachPhysTerrainTile* pTile = tileArray_[yTile][xTile];

            //If not at eastern edge of world do eastern portal
            if( xTile < nTilesX_ - 1 )
            {
                //Find minimum terrain height on eastern edge
                x = (xTile + 1) * edgeLength_;
                minHeight = maxHeight - 0.1;

                for( size_t i = 0; i < nTileYVertices_; ++i )
                {
                    y = (yTile + (double)i / (double)(nTileYVertices_ - 1)) * edgeLength_;
                    height = terrainHeight( x, y );
                    if( height < minHeight ) minHeight = height;
                }

                //TBD: The height is reduced here to allow for anomolies caused by including eg water surfaces
                //in the mesh, which are translucent. For E3 the distance between the water surface and basin
                //is approx 5 metres. Drop the base by 2x this to be safe. Needs to be resolved properly,
                //although I'm not sure how.
                minHeight -= 10.0;

                //Construct portal polygon
                MexPoint3d p1( edgeLength_, 0, minHeight);
                MexPoint3d p2( edgeLength_, edgeLength_, minHeight);
                MexPoint3d p3( edgeLength_, edgeLength_, maxHeight);
                MexPoint3d p4( edgeLength_, 0, maxHeight);
                MexQuad3d quad( p1, p2, p3, p4 );

                //Construct the portal
                pOwnerDomain->addPortal( tileDomainArray_[yTile][xTile + 1], quad,
                                         identityTransform );
                /*
                outputStream << "east side of ( " << xTile << ", " << yTile << " )" << std::endl;
                outputStream << p1 << std::endl;
                outputStream << p2 << std::endl;
                outputStream << p3 << std::endl;
                outputStream << p4 << std::endl;
                */
            }

            //If not at southern edge of world do south portal
            if( yTile < nTilesY_ - 1 )
            {
                //Find minimum terrain height on south edge
                y = (yTile + 1) * edgeLength_;
                minHeight = maxHeight - 0.1;

                for( size_t i = 0; i < nTileXVertices_; ++i )
                {
                    x = (xTile + (double)i / (double)(nTileXVertices_ - 1)) * edgeLength_;
                    height = terrainHeight( x, y );
                    if( height < minHeight ) minHeight = height;
                }

                //TBD: The height is reduced here to allow for anomolies caused by including eg water surfaces
                //in the mesh, which are translucent. For E3 the distance between the water surface and basin
                //is approx 5 metres. Drop the base by 2x this to be safe. Needs to be resolved properly,
                //although I'm not sure how.
                minHeight -= 10.0;

                //Construct portal polygon
                MexPoint3d p1( 0, edgeLength_, minHeight);
                MexPoint3d p2( edgeLength_, edgeLength_, minHeight);
                MexPoint3d p3( edgeLength_, edgeLength_, maxHeight);
                MexPoint3d p4( 0, edgeLength_, maxHeight);
                MexQuad3d quad( p1, p2, p3, p4 );

                //Construct the portal
                pOwnerDomain->addPortal( tileDomainArray_[yTile + 1][xTile], quad,
                                         identityTransform );

                /*
                outputStream << "south side of ( " << xTile << ", " << yTile << " )" << std::endl;
                outputStream << p1 << std::endl;
                outputStream << p2 << std::endl;
                outputStream << p3 << std::endl;
                outputStream << p4 << std::endl;
                */
            }
        }
    }
}


MachPhysTerrainTile *MachPhysPlanetSurface::replaceTile( size_t xPos, size_t yPos, const SysPathName& lodPath )
{
 	PRE( lodPath.existsAsFile() );

	MachPhysTerrainTile* pTileToReplace = tileArray_[yPos][xPos];

	MachPhysTerrainTile* pNewTile = _NEW( MachPhysTerrainTile( 	pTileToReplace->pParent(),
																lodPath,
																pTileToReplace->localTransform(),
																nTileXVertices_,
																nTileYVertices_,
																tileXMin_, tileXMax_,
																tileYMin_, tileYMax_ ) );


	tileArray_[yPos][xPos] = pNewTile;

	_DELETE( pTileToReplace );

	return pNewTile;
}

void MachPhysPlanetSurface::pathProfile(
  const MexPoint3d& startPoint,
  const MexPoint3d& finishPoint,
  Profile* pProfile ) const
{
    pathProfile( startPoint, finishPoint, Floors(), pProfile );
}

void MachPhysPlanetSurface::pathProfile(
  const MexTransform3d& startTransform,
  const MexPoint3d& finishPoint,
  Profile* pProfile ) const
{
    pathProfile( startTransform, finishPoint, Floors(), pProfile );
}

void MachPhysPlanetSurface::pathProfile(
  const MexTransform3d& startTransform,
  const MexPoint3d& finishPoint,
  const Floors& floors,
  Profile* pProfile ) const
{
    PATH_PROFILE_STREAM( "----------------------------------------------------------------------------" << std::endl );
    PATH_PROFILE_STREAM( "Start find profile from " << startTransform.position() << " to " << finishPoint << std::endl );

    PRE_INFO( startTransform );
    PRE_INFO( finishPoint );
    PRE( startTransform.position().x() >= xMin() );
    PRE( startTransform.position().x() <= xMax() );
    PRE( startTransform.position().y() >= yMin() );
    PRE( startTransform.position().y() <= yMax() );
    PRE( finishPoint.x() >= xMin() );
    PRE( finishPoint.x() <= xMax() );
    PRE( finishPoint.y() >= yMin() );
    PRE( finishPoint.y() <= yMax() );

    pathProfile( startTransform.position(), finishPoint, floors, pProfile );

    //  Now force the first position on the profile to be identical
    //  to the start position that was passed in. This is to allow
    //  for the start position being "in the air" as a result of
    //  the machine having stopped on a slope transition.

    (*pProfile)[ 0 ] = startTransform;

    ASSERT( pProfile->size() >= 2, "" );

    PATH_PROFILE_STREAM( "Finish find profile from " << startTransform.position() << " to " << finishPoint << std::endl );
    PATH_PROFILE_STREAM( "----------------------------------------------------------------------------" << std::endl );
}

void MachPhysPlanetSurface::pathProfile(
  const MexPoint3d& startPoint,
  const MexPoint3d& finishPoint,
  const Floors& floors,
  Profile* pProfile ) const
{
    PRE_INFO( startPoint );
    PRE_INFO( finishPoint );
    PRE( startPoint.x() >= xMin() );
    PRE( startPoint.x() <= xMax() );
    PRE( startPoint.y() >= yMin() );
    PRE( startPoint.y() <= yMax() );
    PRE( finishPoint.x() >= xMin() );
    PRE( finishPoint.x() <= xMax() );
    PRE( finishPoint.y() >= yMin() );
    PRE( finishPoint.y() <= yMax() );

    PATH_PROFILE_STREAM( "----------------------------------------------------------------------------" << std::endl );
    PATH_PROFILE_STREAM( "Start find profile from " << startPoint << " to " << finishPoint << std::endl );
    PATH_PROFILE_STREAM( ProStackTracer() );

    // Find out which terrain tiles we are crossing

    MexGrid2d   grid( nXTiles(), nYTiles(), xTileSpacing(), yTileSpacing(), MexPoint2d( xMin(), yMin() ) );

    PATH_PROFILE_INSPECT( grid );

    uint nToReserve = 16;
    MexGrid2d::Cells    cells;
    cells.reserve( nToReserve );
    MexGrid2d::Points   points;
    points.reserve( nToReserve );

    MexLine2d   line( MexPoint2d( startPoint.x(), startPoint.y() ), MexPoint2d( finishPoint.x(), finishPoint.y() ) );

    grid.intersect( line, &cells, &points );

    PATH_PROFILE_INSPECT( line );
    PATH_PROFILE_STREAM( "Cells:\n" );

    for( MexGrid2d::Cells::const_iterator i = cells.begin(); i != cells.end(); ++i )
        PATH_PROFILE_STREAM( (*i).xIndex() << "  " << (*i).yIndex() << std::endl );

    PATH_PROFILE_STREAM( "Points:\n" << points );

    MexGrid2d::Points::const_iterator pointIterator = points.begin();

    for( MexGrid2d::Cells::const_iterator i = cells.begin(); i != cells.end(); ++i, ++pointIterator )
    {
        MachPhysTerrainTile* pTile = tileArray_[ (*i).yIndex() ][ (*i).xIndex() ];
        const MachPhysTileData& tileData = pTile->tileData();

        //Get the inverse of its transform
        MexTransform3d tileInverseTransform;
        const MexTransform3d& tileTransform = pTile->globalTransform();
        tileTransform.invert( &tileInverseTransform );

        //Compute the line's coordinates in the tile's local space
        MexPoint3d tileStartPoint( *pointIterator );

        PATH_PROFILE_INSPECT( tileStartPoint );
        tileInverseTransform.transform( &tileStartPoint );

        MexPoint3d tileFinishPoint( *( pointIterator + 1 ) );

        PATH_PROFILE_INSPECT( tileFinishPoint );
        tileInverseTransform.transform( &tileFinishPoint );

        size_t  profileStartIndex = pProfile->size();

        tileData.pathProfile( tileStartPoint, tileFinishPoint, pProfile );

        //  Unless we're dealing with the very last cell get rid of the very
        //  last profile position because this will be duplicated by the first
        //  position of the next cell.
        MexGrid2d::Cells::const_iterator j = i;
        ++j;

        if( j != cells.end() and pProfile->size() != 0 )
            pProfile->pop_back();

        //  get all of the profile points converted to the planet's coordinate system

        for( size_t j = profileStartIndex; j < pProfile->size(); ++j )
        {
            MexTransform3d& tx = (*pProfile)[ j ];
            tx.preTransform( tileTransform );

            if( tx.position().z() < pImpl_->minMachineHeight_ )
            {
                tx.position( MexPoint3d(
                  tx.position().x(),
                  tx.position().y(),
                  pImpl_->minMachineHeight_ ) );
            }
        }
    }

    if( floors.size() != 0 )
    {
        PATH_PROFILE_STREAM( "Profile pre floor adjustment:\n" << *pProfile );

        handleFloors( floors, pProfile );
    }

    PATH_PROFILE_STREAM( "Profile pre optimisation:\n" << *pProfile );

    optimiseProfile( pProfile );

    PATH_PROFILE_STREAM( "Profile post optimisation:\n" << *pProfile );

    ASSERT( pProfile->size() >= 2, "" );

    PATH_PROFILE_STREAM( "Finish find profile from " << startPoint << " to " << finishPoint << std::endl );
    PATH_PROFILE_STREAM( "----------------------------------------------------------------------------" << std::endl );
}

void    MachPhysPlanetSurface::optimiseProfile( Profile* pProfile ) const
{
    //  Remove duplicates and points that make up a straight line

    Profile newProfile;
    newProfile.reserve( pProfile->size() + 1 );

    for( size_t i = 0; i < pProfile->size(); ++i )
    {
        PATH_PROFILE_WHERE;
        //  First check for duplicates
        if( i == 0 or (*pProfile)[ i ] != (*pProfile)[ i - 1 ] )
        {
            PATH_PROFILE_WHERE;
            //  Now check for the points being in a straight line

            if( newProfile.size() > 1 and pointsInLine(
              newProfile[ newProfile.size() - 2 ].position(),
              newProfile[ newProfile.size() - 1 ].position(),
              (*pProfile)[ i ].position() ) )
            {

                PATH_PROFILE_INSPECT( newProfile[ newProfile.size() - 2 ].position() );
                PATH_PROFILE_INSPECT( newProfile[ newProfile.size() - 1 ].position() );
                PATH_PROFILE_INSPECT( (*pProfile)[ i ].position() );

                newProfile.pop_back();
            }

            PATH_PROFILE_WHERE;
            newProfile.push_back( (*pProfile)[ i ] );
        }
    }

    for( size_t i = 0; i < newProfile.size(); ++i )
    {
        //  raise the profile by the appropriate height offset
        MexPoint3d  position( newProfile[ i ].position() );
        position += MexVec3( 0.0, 0.0, heightOffset() );
        newProfile[ i ].position( position );
        PATH_PROFILE_WHERE;
    }

    *pProfile = newProfile;
}

void    MachPhysPlanetSurface::handleFloors(
  const Floors& floors,
  Profile* pProfile ) const
{
    PRE( floors.size() != 0 );
    // PRE( overlapping floors should have the same height );

    bool pathOverlapsFloor = false;
    bool lastPointRepositioned = false;

    for( size_t i = 0; i < pProfile->size(); ++i )
    {
        MexTransform3d& transform = (*pProfile)[ i ];
        const bool lastPoint = ( i == pProfile->size() - 1 );

        bool finished = false;

        for( size_t j = 0; j < floors.size() and not finished; ++j )
        {
            const MexAlignedBox2d& floorArea = floors[ j ].area();

            if( floorArea.contains( transform.position() ) )
            {
                const MexPoint3d newPosition(
                  transform.position().x(),
                  transform.position().y(),
                  floors[ j ].height() );

                transform.position( newPosition );
                pathOverlapsFloor = true;
                finished = true;

                if( lastPoint )
                    lastPointRepositioned = true;
            }
        }
    }

    if( pathOverlapsFloor )
    {
        //  Reset the transforms to handle the newly repositioned profile

        for( size_t i = 0; i < pProfile->size(); ++i )
        {
            MexTransform3d& transform = (*pProfile)[ i ];
            const bool lastPoint = ( i == pProfile->size() - 1 );

            if( lastPoint )
            {
                //  If the last point was not repositioned we don't want
                //  to change its orientation at all.
                if( lastPointRepositioned )
                {
                    ASSERT( i != 0, "" );

                    //  Use the orientation from the previous transform and
                    //  the position from this one.
                    MexTransform3d newTransform( (*pProfile)[ i - 1 ] );
                    newTransform.position( transform.position() );
                    transform = newTransform;
                }
            }
            else
            {
                const MexPoint3d& nextPosition = (*pProfile)[ i + 1 ].position();
                MexTransform3d newTransform(
                    MexTransform3d::X_XZ,
                    MexVec3( transform.position(), nextPosition ),
                    MexVec3( 0.0, 0.0, 1.0 ),
                    transform.position() );

                transform = newTransform;
            }
        }
    }
}

bool    MachPhysPlanetSurface::pointsInLine(
  const MexPoint3d& p1, const MexPoint3d& p2, const MexPoint3d& p3 ) const
{
    PATH_PROFILE_WHERE;
    PATH_PROFILE_INSPECT( p1 );
    PATH_PROFILE_INSPECT( p2 );
    PATH_PROFILE_INSPECT( p3 );
    PATH_PROFILE_WHERE;

    return MexLine3d( p1, p3 ).isPointOnLine( p2 );
}

//  TBD: This value should be read in from somewhere rather than
//  being hard coded. It is the offset we use for the planet surface
//  so that shadows appear on top of the planet surface.

MATHEX_SCALAR   MachPhysPlanetSurface::heightOffset() const
{
    return 0.065;
}

void MachPhysPlanetSurface::addLocalLight( W4dLight* pLight, const MexAlignedBox3d& lightCoveredBox )
{

	MexAlignedBox2d box2D( MexPoint2d( lightCoveredBox.minCorner().x(), lightCoveredBox.minCorner().y() ),
						   MexPoint2d( lightCoveredBox.maxCorner().x(), lightCoveredBox.maxCorner().y() )
						 );

    for( size_t yTile = 0; yTile < nTilesY_; ++yTile )
    {
		MATHEX_SCALAR y0 = yTile * edgeLength_;
		MATHEX_SCALAR y1 = y0 + edgeLength_;

        for( size_t xTile = 0; xTile < nTilesX_; ++xTile )
        {
            //Get the domain for this tile
            W4dDomain* pDomain = tileDomainArray_[yTile][xTile];

			MATHEX_SCALAR x0 = xTile * edgeLength_;
			MATHEX_SCALAR x1 = x0 + edgeLength_;

            MexAlignedBox2d boundingVolume2D( MexPoint2d(x0, y0), MexPoint2d(x1, y1));
			if( box2D.intersects( boundingVolume2D  ) )
			{
				pLight->illuminate( tileDomainArray_[yTile][xTile] );
			}
        }
    }

}

void MachPhysPlanetSurface::domainsAt( W4dLight* light) const
{

	PRE(light);

	const MexSphere3d& sphere = light->boundingSphere();
	const MexPoint3d& center = sphere.center();
	const MATHEX_SCALAR rad  = sphere.radius();
	MexPoint3d minCorner(center.x() - rad, center.y() - rad, center.z() - rad);
	MexPoint3d maxCorner(center.x() + rad, center.y() + rad, center.z() + rad);

	MexAlignedBox3d box(minCorner, maxCorner);

	// Get the containing domain of the light
	W4dDomain* conDom = NULL;
	bool lightHasContainingDomain = light->hasContainingDomain(	&conDom );

	const int xIdx1 = xIndex(box.minCorner());
	const int xIdx2 = xIndex(box.maxCorner());
	const int yIdx1 = yIndex(box.minCorner());
	const int yIdx2 = yIndex(box.maxCorner());

	// It should be a property of a box that the min corner's co-ordinates
	// are less than the max corner's.
	ASSERT(xIdx1 <= xIdx2, "box corners are the wrong way around");
	ASSERT(yIdx1 <= yIdx2, "box corners are the wrong way around");

	if(not light->hasIntersectingDomains() )
	{
		for (int x=xIdx1; x <= xIdx2; ++x)
		{
			for (int y=yIdx1; y <= yIdx2; ++y)
			{
				if (x >= 0 && x < nTilesX_ && y >= 0 && y < nTilesY_)
				{
			    	W4dDomain* pTile = tileArray_[y][x]->containingDomain();
					// only push back if is not containing domain
					if( conDom != pTile )
					{
						if( not light->hasIntersectingDomains() )
							light->createIntersectingDomains();
						light->addIntersectingDomain( pTile );
					}
				}
			}
		}
	}
	else
	{
		light->eraseIntersectingDomains();

		for (int x=xIdx1; x <= xIdx2; ++x)
		{
			for (int y=yIdx1; y <= yIdx2; ++y)
			{
				if (x >= 0 && x < nTilesX_ && y >= 0 && y < nTilesY_)
				{
			    	W4dDomain* pTile = tileArray_[y][x]->containingDomain();
					// only push back if is not containing domain
					if( conDom != pTile )
					{
						light->addIntersectingDomain( pTile );
					}
				}
			}
		}

		if( light->intersectingDomains().size() == 0 )
			light->deleteIntersectingDomains();
	}


}

//Export the domain referenced by indicies
W4dDomain*
MachPhysPlanetSurface::domainByXYIndex( size_t xIndex, size_t yIndex ) const
{
	PRE_INFO( xIndex );
	PRE_INFO( yIndex );
	PRE( xIndex < nTilesX_ );
	PRE( yIndex < nTilesY_ );
	return tileArray_[yIndex][xIndex]->containingDomain();
}

const MachPhysTerrainTile& MachPhysPlanetSurface::tile( size_t xIndex, size_t yIndex ) const
{
    PRE( xIndex < nXTiles() );
    PRE( yIndex < nYTiles() );

	return *tileArray_[yIndex][xIndex];
}

void MachPhysPlanetSurface::switchRoot()
{
    PRE( pSceneManager_ != NULL );

    //Get the world entity from the scene manager
    W4dEntity* pParent = &(pSceneManager_->root());

    //Iterate through the domains, attaching them to the scene manager root
    for( size_t j = 0; j != nTilesY_; ++j )
    {
        for( size_t i = 0; i != nTilesX_; ++i )
        {
            W4dDomain* pDomain = tileDomainArray_[j][i];
            pDomain->attachTo( pParent );
        }
    }
}

MachPhysPlanetSurface::MachPhysPlanetSurface( PerConstructor )
{
    ASSERT_FAIL( "MachPhysPlanetSurface should never be constructed by persistence" );
}

void perWrite( PerOstream& ostr, const MachPhysPlanetSurface& t )
{
    ostr << t.nTilesX_;
    ostr << t.nTilesY_;
    ostr << t.nTileXVertices_;
    ostr << t.nTileYVertices_;
    ostr << t.tileXMin_;
    ostr << t.tileXMax_;
    ostr << t.tileYMin_;
    ostr << t.tileYMax_;
    ostr << t.tileArray_;
    ostr << t.tileDomainArray_;
    ostr << t.psfFile_;
    ostr << t.edgeLength_;
    ostr << t.recipEdgeLength_;
    ostr << t.backgroundFile_;
    ostr << t.pImpl_;

    //Not persisted
    //pSceneManager_
    //pEnvironment_
    //pDomainAssign_
    //pTextureSet_
}

void perRead( PerIstream& istr, MachPhysPlanetSurface& t )
{
    _DELETE( t.pImpl_ );

    istr >> t.nTilesX_;
    istr >> t.nTilesY_;
    istr >> t.nTileXVertices_;
    istr >> t.nTileYVertices_;
    istr >> t.tileXMin_;
    istr >> t.tileXMax_;
    istr >> t.tileYMin_;
    istr >> t.tileYMax_;
    istr >> t.tileArray_;
    istr >> t.tileDomainArray_;
    istr >> t.psfFile_;
    istr >> t.edgeLength_;
    istr >> t.recipEdgeLength_;
    istr >> t.backgroundFile_;
    istr >> t.pImpl_;

	//Do a quick validation loop around y then x for size of tiles.
	//Any invlaid tiles will be streamed out
	//Any problems at all will result in an assertion at the end of both loops
	bool tileSizeProblemFound = false;
	for( int i = 0; i < t.nTilesY_; ++i )
		for( int j = 0; j < t.nTilesX_; ++j )
		{
			MachPhysTerrainTile* pTile = t.tileArray_[i][j];
			const MexAlignedBox3d& boundary = pTile->boundingVolume();
			if( ( boundary.maxCorner().x() - boundary.minCorner().x() > t.edgeLength_ ) or
			    ( boundary.maxCorner().y() - boundary.minCorner().y() > t.edgeLength_ ) )
			{
				PLANET_SURFACE_STREAM("Tile Problem detected : AT x " << j << " , y " << i << std::endl );
				PATH_PROFILE_STREAM("Tile Problem Detected : " << pTile->tileData() << std::endl );
				tileSizeProblemFound = true;
			}
		}
//	POST( not tileSizeProblemFound );
}

void MachPhysPlanetSurface::loadPlanet( const SysPathName& pathname )
{
    //Check for the existence of a persisted file for the planet
    string path = "pdata\\" + pathname.components().back();
    SysPathName persistentPathname( path );
    persistentPathname.extension( "psb" );

    if( persistentPathname.existsAsFile() )
    {
    	std::ifstream ifstr( persistentPathname.c_str(), std::ios::binary );
    	PerIstream istr( ifstr );

    	istr >> (*this);
    }
    else
    {
        //Load the planet as defined in the surface file
        ASSERT_FILE_EXISTS( pathname.c_str() );
        readPlanetSurfaceFile( pathname );

        //Construct the portals
        makePortals();

        //save the planet in persistent form
    	std::ofstream ofstr( persistentPathname.c_str(), std::ios::binary );
    	PerOstream ostr( ofstr );

    	ostr << (*this);
    }

    //Swap the domains across to the scene manager's root.
    switchRoot();
}

MATHEX_SCALAR MachPhysPlanetSurface::minMachineHeight() const
{
	return pImpl_->minMachineHeight_;
}

MATHEX_SCALAR MachPhysPlanetSurface::terrainHeight(
    MATHEX_SCALAR x,
    MATHEX_SCALAR y,
    const Floors& floors ) const
{
    return terrainHeight( x, y, floors, NULL );
}

MATHEX_SCALAR MachPhysPlanetSurface::terrainHeight(
    MATHEX_SCALAR x,
    MATHEX_SCALAR y,
    const Floors& floors,
    MexVec3* pUnitNormal ) const
{
    //  PRE( any overlapping floors have the same height );

    bool intersectsWithFloor = false;
    MATHEX_SCALAR result;

    for( size_t i = 0; i < floors.size() and not intersectsWithFloor; ++i )
    {
        const MexAlignedBox2d& floorArea = floors[ i ].area();

        if( floorArea.contains( MexPoint2d( x, y ) ) )
        {
            intersectsWithFloor = true;
            result = floors[ i ].height();

            if( pUnitNormal )
                *pUnitNormal = MexVec3( 0.0, 0.0, 1.0 );
        }
    }

    if( not intersectsWithFloor )
    {
        result = terrainHeight( x, y, pUnitNormal );
    }

    return result;
}

/* End PLANSURF.CPP *************************************************/

