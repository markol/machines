/*
 * P L A N E T . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachLogPlanet

    Class defining the planet
*/

#ifndef _PLANET_HPP
#define _PLANET_HPP

#include "base/base.hpp"
#include "base/istrrep.hpp"
#include "machlog/machlog.hpp"

//Forward declarations
class MexTransform3d;
class MexPoint3d;
class MexRadians;
class MachPhysPlanetSurface;
class MachPhysTerrainTile;
class MachLogRace;
class W4dEntity;
class W4dDomain;
class W4dRoot;
class SysPathName;
class MachLogPressurePads;
class MachLogMineralSite;
class MachLogDebris;
class W4dSceneManager;
class PhysConfigSpace2d;
class UtlLineTokeniser;
class MachLogPlanetImpl;
template < class X > class ctl_pvector;

//singleton
class MachLogPlanet
{
public:	

    //The singleton instance
    static MachLogPlanet& instance( void );
    ~MachLogPlanet( void );

    //construct the planet surface using the psf file specified.
    //To be displayed in window represented by pSceneManager.
    void surface( W4dSceneManager* pSceneManager, const SysPathName& surfaceFilePath, BaseProgressReporter* );
    //PRE( not hasSurface() )

    //True if a surface is defined
    bool hasSurface( void ) const;

    //Export the planet surface
    MachPhysPlanetSurface* surface( void ) const;
    //PRE( hasSurface() )

    //Clear out the current planet surface and any associated data structures. After this call
    //it should be possible to use the surface() method again to load a new surface model.
    void clear();
    //POST( not hasSurface() )

    //True iff the world entity is defined
    bool hasWorld( void ) const;

    //The world entity
    W4dEntity* pWorld( void ) const;

    //Exports the planet's configuration space
    PhysConfigSpace2d& configSpace();

    MachLogPressurePads& pressurePads();
        
	// hidden W4dRoot which never gets rendered - allows physical objects to switch 
	// into the interiors of APCs and the like
	W4dEntity& hiddenRoot();
	const W4dEntity& hiddenRoot() const;

    //Exports the planet's hidden configuration space
    PhysConfigSpace2d& hiddenConfigSpace() const;

    void CLASS_INVARIANT;

	typedef ctl_pvector< MachLogMineralSite >		Sites;
	typedef ctl_pvector< MachLogDebris >			DebrisSites;

	const Sites& sites() const;
	const DebrisSites& debrisSites() const;
	friend ostream& operator<<( ostream& o, const MachLogPlanet& t );
	
	void newSite( MachLogMineralSite* pSite );
	void newDebrisSite( MachLogDebris* pDebris );
	void removeDebrisSite( MachLogDebris* pDebris );
	
	bool siteExists( int id );
	MachLogMineralSite* pSite( int id );
	// PRE( siteExists( id ) );
	
//	void removeSite( int id );
	// PRE( siteExists( id ) );
	
	// returns a safely-clipped (if necessary) version of the parameter co-ordinate guaranteed to
	// lie within the world's boundaries
	MexPoint3d safeWorldCoordinate( const MexPoint3d& potentiallyUnsafeCoordinate ) const;

    #ifndef PRODUCTION
        //  Check for errors such as vertical polygons not in obstacles
        //  and points on the planet surface not being in obstacles or
        //  domains.
        void checkPlanet( ostream& ) const;
    #endif

private:
    //ctor
    MachLogPlanet( void );
	friend class MachLogPlanetImpl;

    // Operations deliberately revoked
    MachLogPlanet( const MachLogPlanet& );
    MachLogPlanet& operator =( const MachLogPlanet& );
    bool operator ==( const MachLogPlanet& );

	int nextSiteID();
	
    //Constants
    enum
    {
        DOMAIN_CLEARANCE = 14 //Nominal clearance(m) used for config space 
    };

    //Create and initialise the config space
    typedef uint32  ObstacleFlags;
    void initConfigSpace( const SysPathName& spaceFilePath, BaseProgressReporter* );
    void readPlanetConfigSpaceFile( const SysPathName& spaceFilePath, BaseProgressReporter* );
    void readObstacleFlags( UtlLineTokeniser* pParser, ObstacleFlags* pObstacleFlags );

    #ifndef PRODUCTION
        bool planetSurfaceOK( ostream& ) const;
    #endif

   //Data members
	MachLogPlanetImpl*		pImpl_;
};

#ifdef _INLINE
    #include "machlog/planet.ipp"
#endif


#endif

/* End PLANET.HPP ***************************************************/
