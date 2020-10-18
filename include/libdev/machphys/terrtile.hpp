/*
 * T E R R T I L E . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysTerrainTile

    A brief description of the class should go in here
*/

#ifndef _TERRTILE_HPP
#define _TERRTILE_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "system/pathname.hpp"

#include "world4d/entity.hpp"

#include "stdlib/string.hpp"

//Forward declarations
class SysPathName;
class MachPhysTileData;
typedef CtlCountedPtr< MachPhysTileData > MachPhysTileDataPtr;

class MachPhysTerrainTile : public W4dEntity
{
public:
    //Construct instance of tile owned by pParent from lodFileName. Will copy
    //an existing tile previously loaded using this name, sharing mesh and boundary data.
    //Set up boundary data using a rectangular grid of nXVertices by nYVertices points,
    //with boundary limits defined by xMin, yMin, xMax and yMax.
    MachPhysTerrainTile( W4dEntity* pParent, const SysPathName& lodFileName,
                         const W4dTransform3d& transform,
                         size_t nXVertices, size_t nYVertices,
                         MATHEX_SCALAR xMin, MATHEX_SCALAR xMax,
                         MATHEX_SCALAR yMin, MATHEX_SCALAR yMax );

    ~MachPhysTerrainTile( void );

    //The tile boundary
    const MachPhysTileData& tileData() const;

    //Inherited from W4dEntity
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                             Accuracy accuracy ) const;

	const SysPathName& lodFileName() const;

    //Adds tile to database of previously loaded tile items, used by planet editor.
    void registerExemplar() const;

    //Clears any database of previously loaded tile items.
    static void clearAll();

	//static void preloadWaterTextures();

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT( MachPhysTerrainTile );
    PER_FRIEND_READ_WRITE( MachPhysTerrainTile );
private:
    // Operations deliberately revoked
    MachPhysTerrainTile( const MachPhysTerrainTile& );
    MachPhysTerrainTile& operator =( const MachPhysTerrainTile& );
    bool operator ==( const MachPhysTerrainTile& );

    //Constructor used by the exemplar() method
    MachPhysTerrainTile( const SysPathName& lodFileName,
                         size_t nXVertices, size_t nYVertices,
                         MATHEX_SCALAR xMin, MATHEX_SCALAR xMax,
                         MATHEX_SCALAR yMin, MATHEX_SCALAR yMax );

    //Ensures a copy of the terrain tile denoted by lodFileName is loaded, and
    //returns its ref.
    //The vertex count and boundary info is needed for the first call for a given
    //tile filename to construct the data object.
    const MachPhysTerrainTile& exemplar( const SysPathName& lodFileName,
                                         size_t nXVertices, size_t nYVertices,
                                         MATHEX_SCALAR xMin, MATHEX_SCALAR xMax,
                                         MATHEX_SCALAR yMin, MATHEX_SCALAR yMax );

    //A root entity used for owning the exemplar tiles.
    //Will be destroyed at Armageddon.
    W4dRoot& exemplarRoot() const;

    virtual void doDebugOutput( ostream& ostr );
    // Write out the details of 'this' for debugging purposes.

    //data members
    MachPhysTileDataPtr* pTileDataPtr_; //Ptr to ref-counted ptr to tile data defining boundary
	SysPathName tileLodFile_; // Store the lod file used (Used by PlanetEditor )
};

PER_DECLARE_PERSISTENT( MachPhysTerrainTile );

#endif

/* End TERRTILE.HPP *************************************************/
