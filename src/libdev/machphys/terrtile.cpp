/*
 * T E R R T I L E . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/terrtile.hpp"

#include "stdlib/string.hpp"

#include "system/pathname.hpp"

#include "ctl/map.hpp"
#include "ctl/vector.hpp"

#include "mathex/point3d.hpp"
#include "mathex/transf3d.hpp"

#include "render/mesh.hpp"
#include "render/matmap.hpp"
#include "render/meshinst.hpp"
#include "render/texture.hpp"
#include "render/texmgr.hpp"

#include "world4d/entity.hpp"
#include "world4d/root.hpp"
#include "world4d/matfrmpl.hpp"

#include "sim/manager.hpp"
#include "machphys/tiledata.hpp"
//#include "machphys/waterfal.hpp"

#include "stdlib/string.hpp"

PER_DEFINE_PERSISTENT( MachPhysTerrainTile );

//define the water animation textures
//static const ctl_vector<RenTexture>& waterAnimationTextures();

//A map used to facilitate loading terrain tile models only once.
typedef ctl_map< string, MachPhysTerrainTile*, std::less<string> > TerrainExemplarMap;
static TerrainExemplarMap& exemplarTileMap();

#include "profiler/stktrace.hpp"
MachPhysTerrainTile::MachPhysTerrainTile
(
    W4dEntity* pParent,
    const SysPathName& lodFileName,
    const W4dTransform3d& transform,
    size_t nXVertices, size_t nYVertices,
    MATHEX_SCALAR xMin, MATHEX_SCALAR xMax,
    MATHEX_SCALAR yMin, MATHEX_SCALAR yMax
)
:   W4dEntity( exemplar( lodFileName, nXVertices, nYVertices, xMin, xMax, yMin, yMax ),
               pParent, transform ),
    pTileDataPtr_( NULL ),
	tileLodFile_( lodFileName )
{
    //Copy the ref-counted pointer to the tile data
    pTileDataPtr_ =
        _NEW( MachPhysTileDataPtr( *(exemplar( lodFileName, nXVertices, nYVertices,
                                               xMin, xMax, yMin, yMax ).pTileDataPtr_)) );
/*
//E3 hack. Should be able to delete this soon
	if( hasMesh() )
	{
		const string& name = _CONST_CAST(const W4dEntity*, this)->mesh().mesh()->meshName();

		bool contains = ( name.substr(0, 4) == "anim" );

		if( contains )
		{
			//apply the water animation if there is water
    		const PhysAbsoluteTime now = SimManager::instance().currentTime();
			static const PhysRelativeTime oneYear = 31536000;

			static const ctl_vector<RenTexture>& textures = waterAnimationTextures();
			W4dMaterialFramePlan::makePlan(this, textures, now, oneYear, 3);
		}

		if( name.substr(0, 5) == "animW" )
        {
            _NEW( MachPhysWaterfall( this, MexTransform3d() ) );
        }
	}
*/
	TEST_INVARIANT;
}

MachPhysTerrainTile::MachPhysTerrainTile( PerConstructor con )
: W4dEntity( con )
{
}

// To be used only with examplar methods
MachPhysTerrainTile::MachPhysTerrainTile( const MachPhysTerrainTile& copyMe )
:   W4dEntity( copyMe, &exemplarRoot(), MexTransform3d()),
    pTileDataPtr_( _NEW( MachPhysTileDataPtr( *copyMe.pTileDataPtr_ ) ) ),
    tileLodFile_( copyMe.tileLodFile_ )
{
}

MachPhysTerrainTile::~MachPhysTerrainTile()
{
    TEST_INVARIANT;

    _DELETE( pTileDataPtr_ );
}

// virtual
void MachPhysTerrainTile::doDebugOutput( ostream& ostr )
{
    ostr << "Start MachPhysTerrainTile " << (void*)this << std::endl;
    ostr << "End   MachPhysTerrainTile " << (void*)this << std::endl;
}

void MachPhysTerrainTile::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

const MachPhysTileData& MachPhysTerrainTile::tileData() const
{
    PRE( pTileDataPtr_ != NULL );
    return **pTileDataPtr_;
}

const SysPathName& MachPhysTerrainTile::lodFileName() const
{
	return tileLodFile_;
}

W4dRoot& MachPhysTerrainTile::exemplarRoot() const
{
    static W4dRoot theRoot( 88 );
    return theRoot;
}

bool MachPhysTerrainTile::intersectsLine
(
    const MexLine3d& testLine, MATHEX_SCALAR* pDistance, Accuracy
) const
{
  //JON_STREAM("MachPhysTerrainTile::intersectsLine entry " << testLine.end1() << " to " << testLine.end2() << std::endl;)
    //Get the global transform, and invert it so that we can produce a copy
    //of the line transformed into the local coordinate system of this entity.
    MexTransform3d inverseGlobal;
    globalTransform().invert( &inverseGlobal );
    MexLine3d localLine( testLine );
    localLine.transform( inverseGlobal );
    MATHEX_SCALAR localLength = localLine.length();

    //We need to clip the test line to that portion over the terrain tile.
    //We do this by getting the intersection between the test line and the
    //aligned box boundary.
    const MexAlignedBox3d& terrainBoundary = boundingVolume();

    MATHEX_SCALAR entryDistance, exitDistance;
    bool result;
    result = terrainBoundary.intersects( localLine, &entryDistance, &exitDistance );
  //JON_STREAM("  local line " << localLine.end1() << " to " << localLine.end2() << std::endl;)
  //JON_STREAM("  BV " << terrainBoundary << std::endl;)
  //JON_STREAM("  After bvint " << result << " entryDistance " << entryDistance << " exitDistance " << exitDistance << std::endl;)
    if( result )
    {
        //We're only interested in the part that includes the original line
        if( entryDistance < 0.0 )
            entryDistance = 0.0;

        if( exitDistance > localLength )
            exitDistance = localLength;

        //Ignore very short lines. Sometimes, a line can have one end within epsilon of the
        //the aligned box, and we even get an exit distance less than the entry distance.
        //Such lines should be ignored.
        MATHEX_SCALAR inLength = exitDistance - entryDistance;
        result = inLength > MexEpsilon::instance();

        if( result )
        {
            //If necessary, modify the local line
            if( entryDistance != 0.0  or  exitDistance != localLength )
            {
                MexPoint3d a = localLine.pointAtDistance( entryDistance );
                MexPoint3d b = localLine.pointAtDistance( exitDistance );
                localLine = MexLine3d( a, b, inLength );
            }

            //Now test the line against the tileData object
            result = (*pTileDataPtr_)->intersectsLine( localLine, pDistance );
        }

        //Remember to add the entry distance back in, as we've probably used a shorter line
        //starting at a different point
        if( result )
            *pDistance += entryDistance;
    }

  //JON_STREAM("MachPhysTerrainTile::intersectsLine exit " << result << " " << *pDistance << std::endl;)
    return result;
}

const MachPhysTerrainTile& MachPhysTerrainTile::exemplar
(
    const SysPathName& lodFileName,
    size_t nXVertices, size_t nYVertices,
    MATHEX_SCALAR xMin, MATHEX_SCALAR xMax,
    MATHEX_SCALAR yMin, MATHEX_SCALAR yMax
)
{
    //Have a local map from pathname to tile instance
    TerrainExemplarMap& theMap = exemplarTileMap();

    //See if the required file is already loaded
    MachPhysTerrainTile* pExemplar;
    TerrainExemplarMap::iterator it = theMap.find( lodFileName.pathname() );
    if( it != theMap.end() )
        pExemplar = (*it).second;
    else
    {
        //This is the first time. Create a new entity.
        pExemplar = _NEW( MachPhysTerrainTile( lodFileName, nXVertices, nYVertices,
                                               xMin, xMax, yMin, yMax ) );

        //Enter into map
        theMap.insert( TerrainExemplarMap::value_type( lodFileName.pathname(), pExemplar ) );
    }

    return *pExemplar;
}

MachPhysTerrainTile::MachPhysTerrainTile
(
    const SysPathName& lodFileName,
    size_t nXVertices, size_t nYVertices,
    MATHEX_SCALAR xMin, MATHEX_SCALAR xMax,
    MATHEX_SCALAR yMin, MATHEX_SCALAR yMax
)
:   W4dEntity( &exemplarRoot(), MexTransform3d(), W4dEntity::SOLID ),
    pTileDataPtr_( NULL )
{
    //Read the mesh data
    readLODFile( lodFileName );
    ASSERT( hasMesh( W4dLOD( 0 ) ), "" );

    //Construct the tile data
    MachPhysTileData* pData =
        _NEW( MachPhysTileData( nXVertices, nYVertices,
                                MexAlignedBox2d( xMin, yMin, xMax, yMax ),
                                *(_CONST_CAST( const W4dEntity&, _STATIC_CAST(W4dEntity&, *this)).mesh( W4dLOD( 0 ) ).mesh() )) );
                                //*(_CONST_CAST( const W4dEntity&, *this).mesh( W4dLOD( 0 ) ).mesh() )) );

    //...and hence the ref-counted pointer to it
    pTileDataPtr_ = _NEW( MachPhysTileDataPtr( pData ) );
}

/*
//static
const ctl_vector<RenTexture>& waterAnimationTextures()
{
	static ctl_vector<RenTexture> textures;
			textures.reserve(9);

    static firstTime = true;

    if( firstTime )
    {
        firstTime = false;

        //Load the textures
        char* names[] =
        {
            "wat00_b.bmp",
            "wat01_b.bmp",
            "wat02_b.bmp",
            "wat03_b.bmp",
            "wat04_b.bmp",
            "wat05_b.bmp",
            "wat06_b.bmp",
            "wat07_b.bmp",
            "wat08_b.bmp"
        };

        for( int i = 0; i != 9; ++i )
            textures.push_back( RenTexManager::instance().createTexture( names[i] ) );
    }

    return textures;
}

static void MachPhysTerrainTile::preloadWaterTextures()
{
	waterAnimationTextures();
}
*/
void MachPhysTerrainTile::registerExemplar() const
{
    TerrainExemplarMap& theMap = exemplarTileMap();

    //See if the required file is already loaded
    TerrainExemplarMap::iterator it = theMap.find( lodFileName().pathname() );
    if( it == theMap.end() )
    {
        //This is the first time. Copy a new entity.
        MachPhysTerrainTile* pExemplar = _NEW( MachPhysTerrainTile( *this ) );

        //Enter into map
        theMap.insert( TerrainExemplarMap::value_type( lodFileName().pathname(), pExemplar ) );
    }
}

//static
TerrainExemplarMap& exemplarTileMap()
{
    static TerrainExemplarMap theMap;
    return theMap;
}

//static
void MachPhysTerrainTile::clearAll()
{
    TerrainExemplarMap& theMap = exemplarTileMap();

    //Iterate through the map, deleting all the entries
    while( theMap.size() != 0 )
    {
        TerrainExemplarMap::iterator it = theMap.begin();
        _DELETE( (*it).second );
        theMap.erase( it );
    }
}

void perWrite( PerOstream& ostr, const MachPhysTerrainTile& tile )
{
    const W4dEntity& base = tile;

    ostr << base;

    ostr << tile.pTileDataPtr_;
    ostr << tile.tileLodFile_;
}

void perRead( PerIstream& istr, MachPhysTerrainTile& tile )
{
    W4dEntity& base = tile;

    istr >> base;

    istr >> tile.pTileDataPtr_;
    istr >> tile.tileLodFile_;
}

/* End TERRTILE.CPP *************************************************/
