/*
 * P L A N S U R F . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysPlanetSurface

    Read in a planet surface file and set up MachPhysTerrainTiles
    corresponding to the planet's surface.
*/

#ifndef _PLANSURF_HPP
#define _PLANSURF_HPP

#include "stdlib/string.hpp"

#include "base/base.hpp"
#include "base/persist.hpp"
#include "system/pathname.hpp"
#include "mathex/mathex.hpp"
#include "mathex/point3d.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/vec2.hpp"
#include "mathex/abox2d.hpp"
#include "ctl/vector.hpp"
#include "ctl/utility.hpp"

class EnvPlanetEnvironment;
class MachPhysTerrainTile;
class UtlLineTokeniser;
class W4dEntity;
class W4dDomain;
class W4dSceneManager;
class RenTextureSet;
class MexAlignedBox3d;
class W4dLight;
class MexVec3;
class MachPhysDomainAssignor;
class MachPhysPlanetSurfaceImpl;
template <class T> class ctl_list;

class MachPhysPlanetSurface
{
public:
    MachPhysPlanetSurface( W4dSceneManager* pSceneManager, const SysPathName& fileName );
    // PRE( pParent != NULL );

    ~MachPhysPlanetSurface();

    //Export the planet surface exterior environment
    bool hasEnvironment() const;
    EnvPlanetEnvironment& environment() const;
    //PRE( hasEnvironment() );

	// Is the given point within the planet surface?
    bool contains(const MexPoint3d& position) const;

	// Returns true if the box is *entirely* contained in the planet, i.e,
	// both corners must be contained.
    bool contains(const MexAlignedBox3d& box) const;

	// PRE(contains(position));
    MachPhysTerrainTile* pTile( const MexPoint3d& position ) const;

    //Export the domain containing point.
	// PRE(contains(point));
    W4dDomain* domainAt( const MexPoint3d& point ) const;

	// This type is supposed to match W4dEntity::W4dDomains.  We could make one
	// a typedef for the other, but that would require including entity.hpp (bad).
    typedef ctl_list< W4dDomain* >  W4dDomains;

	// Export the domains which intersect an alligned rectangle.
	void domainsAt(W4dLight *) const;

    //Export the domain referenced by indicies
    W4dDomain* domainByXYIndex( size_t xIndex, size_t yIndex ) const;

    //Export the planet continent bounds
    MATHEX_SCALAR xMin() const;
    MATHEX_SCALAR xMax() const;
    MATHEX_SCALAR yMin() const;
    MATHEX_SCALAR yMax() const;

    //The number of tiles in x and y, and the spacing.
    //Tiles are located with the "top left" corner (in zenith view) at (0, 0)
    size_t nXTiles() const;
    size_t nYTiles() const;
    MATHEX_SCALAR xTileSpacing() const;
    MATHEX_SCALAR yTileSpacing() const;
	size_t nTileXVertices() const;
    size_t nTileYVertices() const;

    const MachPhysTerrainTile& tile( size_t xIndex, size_t yIndex ) const;
    // PRE( xIndex < nXTiles() );
    // PRE( yIndex < nYTiles() );

    CTL_PAIR( Floor, MexAlignedBox2d, area, MATHEX_SCALAR, height );
    typedef ctl_vector< Floor > Floors;

    //height of terrain at global coordinates (x, y) in planet space.
    //If pUnitNormal is not null a global normal vector to the terrain is returned in it.
    MATHEX_SCALAR terrainHeight( MATHEX_SCALAR x, MATHEX_SCALAR y ) const;
    MATHEX_SCALAR terrainHeight( MATHEX_SCALAR x, MATHEX_SCALAR y, MexVec3* pUnitNormal ) const;

    // As above but if the position is on any of the given floors the floor height
    // will be returned.
    MATHEX_SCALAR terrainHeight( MATHEX_SCALAR x, MATHEX_SCALAR y, const Floors& floors ) const;
    MATHEX_SCALAR terrainHeight( MATHEX_SCALAR x, MATHEX_SCALAR y, const Floors& floors, MexVec3* pUnitNormal ) const;

    typedef ctl_vector< MexTransform3d >    Profile;

    //  Get the profile of a path between a transform and an end point.
    //  The first transform in the path will be identical to startTransform.
    void pathProfile(
        const MexTransform3d& startTransform,
        const MexPoint3d& finishPoint,
        Profile* pProfile ) const;

    //  As above but if the path crosses any of the given floors its height will be
    //  set to the height of the floor.
    void pathProfile(
        const MexTransform3d& startTransform,
        const MexPoint3d& finishPoint,
        const Floors& floors,
        Profile* pProfile ) const;

    //  Get the profile of a path between the two given points. The
    //  first transform in the path will be pointing in the direction of movement.
    void pathProfile(
        const MexPoint3d& startPoint,
        const MexPoint3d& finishPoint,
        Profile* pProfile ) const;

    //  As above but if the path crosses any of the given floors its height will be
    //  set to the height of the floor.
    void pathProfile(
        const MexPoint3d& startPoint,
        const MexPoint3d& finishPoint,
        const Floors& floors,
        Profile* pProfile ) const;

	const SysPathName& backgroundFile() const;

	// Change a tile. Used by PlanetEditor. Returns new tile.
	MachPhysTerrainTile* replaceTile( size_t xPos, size_t yPos, const SysPathName& lodPath );
	// PRE( lodPath.existsAsFile() );

	// Used by PlanetEditor
	bool save();
	void registerLoadedTiles() const;

	void addLocalLight( W4dLight* pLight, const MexAlignedBox3d& lightCoveredBox );

	MATHEX_SCALAR minMachineHeight() const;

    void CLASS_INVARIANT;

    friend class MachPhysMachineExplosion;
    friend ostream& operator <<( ostream& o, const MachPhysPlanetSurface& t );

    PER_MEMBER_PERSISTENT( MachPhysPlanetSurface );
    PER_FRIEND_READ_WRITE( MachPhysPlanetSurface );

private:
    // Operation deliberately revoked
    MachPhysPlanetSurface( const MachPhysPlanetSurface& );

    // Operation deliberately revoked
    MachPhysPlanetSurface& operator =( const MachPhysPlanetSurface& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysPlanetSurface& );

    //Load the planet surface details from source or persistent file
    void loadPlanet( const SysPathName& pathname );

    //Attach all the domains to the scene manager root.
    void switchRoot();

    void readPlanetSurfaceFile( const SysPathName& pathname );
    void parseHeader( UtlLineTokeniser* pParser );
    void parseTile( const SysPathName& pathname, UtlLineTokeniser* pParser, W4dEntity* pParent );

    //Construct the inter-domain portals
    void makePortals();

    MATHEX_SCALAR   heightOffset() const;

	// Return the tile indices given a real-world co-ordinate.  Unlike their
	// "clipped" bretheren below, there are no bounds on the return values.
	int xIndex(const MexPoint3d& position) const;
	int yIndex(const MexPoint3d& position) const;

	// POST(result >=0 && result < nTilesX_); and likewise for Y.
	int xIndexClipped(const MexPoint3d& position) const;
	int yIndexClipped(const MexPoint3d& position) const;

    void    optimiseProfile( Profile* pProfile ) const;

    //  Adjust the profile to take account of floors.
    void    handleFloors(
        const Floors& floors,
        Profile* pProfile ) const;
    //  PRE( floors.size() != 0 );

    bool    pointsInLine( const MexPoint3d& p1, const MexPoint3d& p2, const MexPoint3d& p3 ) const;

    typedef ctl_vector< MachPhysTerrainTile* >  TileRow;
    typedef ctl_vector< TileRow >                   TileArray2d;

    typedef ctl_vector< W4dDomain* > TileDomainRow;
    typedef ctl_vector< TileDomainRow > TileDomainArray2d;

    size_t      nTilesX_;
    size_t      nTilesY_;
    size_t      nTileXVertices_; //Count for tile grid per tile in X
    size_t      nTileYVertices_; //Count for tile grid per tile in Y
    MATHEX_SCALAR tileXMin_; //min x coordinate per tile
    MATHEX_SCALAR tileXMax_; //max x coordinate per tile
    MATHEX_SCALAR tileYMin_; //min y coordinate per tile
    MATHEX_SCALAR tileYMax_; //max y coordinate per tile
    TileArray2d tileArray_;
    TileDomainArray2d tileDomainArray_;
	SysPathName psfFile_;

    double      			edgeLength_, recipEdgeLength_;
    W4dSceneManager*		pSceneManager_; //The scene manager in which the planet is being displayed
	SysPathName				backgroundFile_;
    EnvPlanetEnvironment*	pEnvironment_; //The environment used by the planet exterior surface
	MachPhysDomainAssignor*	pDomainAssign_;
	RenTextureSet*			pTextureSet_;

    MachPhysPlanetSurfaceImpl*  pImpl_;
};

PER_DECLARE_PERSISTENT( MachPhysPlanetSurface );


#ifdef _INLINE
    #include "machphys/plansurf.ipp"
#endif

#endif

/* End PLANSURF.HPP *************************************************/
