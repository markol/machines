/*
 * C O N S D A T A . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysConstructionData
	This class is the base class for each construction type specific
	data class. This class contains all aspects common to ALL constructions.

*/

#ifndef _MACHPHYS_CONSDATA_HPP
#define _MACHPHYS_CONSDATA_HPP

#include "machphys/objdata.hpp"
#include "mathex/point2d.hpp"
#include "mathex/abox2d.hpp"
#include "mathex/line2d.hpp"
#include "ctl/vector.hpp"

#include "stdlib/string.hpp"

class MexTransform3d;
class MexConvexPolygon2d;
class MachPhysStations;
class MachPhysConstructionDataImpl;

class MachPhysConstructionData : public MachPhysObjectData
{
public:
    //ctor. Defaults to no entrances, pickup points etc.
    MachPhysConstructionData();
    //  Copy construction data and place it at a specific position
    MachPhysConstructionData( const MachPhysConstructionData&, const MexTransform3d& );
    MachPhysConstructionData& operator =( const MachPhysConstructionData& );
    virtual ~MachPhysConstructionData( void );

	//Get methods are public
	MachPhys::ResearchUnits hwResearchCost( void ) const;

    //Pair of points for an entrance
    struct EntranceData
    {
        MexConvexPolygon2d  interiorPolygon() const;
        MexConvexPolygon2d  exteriorPolygon() const;

        // Returns a polygon enclosing interior and exterior 
        // entrance areas
        MexConvexPolygon2d  totalPolygon() const;

        MexPoint2d externalPoint; //Approach point outside door
        MexPoint2d internalPoint; //Arrival point inside door
        MexLine2d doorLine; //ground level line conecting the two points
        MATHEX_SCALAR height;	//simple height

        string  doorEntityName;

    private:
        enum InteriorExterior { INTERIOR, EXTERIOR };

        void  polygonPoints( 
            InteriorExterior type,
            MexPoint2d* pPoint1,
            MexPoint2d* pPoint2 ) const;
    };

    typedef ctl_vector< EntranceData > Entrances;
    typedef ctl_vector< MexPoint2d > Locations;
    typedef ctl_vector< MexConvexPolygon2d* > Polygons;
    
    //Accessors
    const MexAlignedBox2d& localBoundary() const;
    const Entrances& entrances() const;
    const Locations& pickUpPoints() const;
    const Locations& putDownPoints() const;

    //  This vector of entrances has not been transformed into global coordinates.
    const Entrances& localEntrances() const;

    const MexAlignedBox2d&  interiorBoundary() const;
    const Polygons&         interiorObstacles() const;
    
    MachPhysStations&   stations();
    const MachPhysStations&   stations() const;

    MachPhys::ConstructionType constructionType() const;
    int subType() const;
    uint hwLevel() const;
	
	int accessSizeIndex() const;
    
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysConstructionData& t );

private:
	friend class MachPhysDataParser;
	friend class MachPhysHardwareLabData;
	friend class MachPhysSoftwareLabData;
	friend class MachPhysSmelterData;
	friend class MachPhysFactoryData;
	friend class MachPhysTurretData;
	friend class MachPhysGarrisonData;
	friend class MachPhysMineData;
	friend class MachPhysBeaconData;
    // Operations deliberately revoked
    bool operator ==( const MachPhysConstructionData& );

	//Set Methods are all private
    //Add entrance/pickup data
	void defineLocalBoundary( const MexAlignedBox2d& );

    void addEntrance(
        const MexPoint2d& externalPoint,
        const MexPoint2d& internalPoint,
        const MexPoint2d& doorPoint,
        const string& doorEntityName,
		const MachPhys::DoorType doorType );

    void addPickUpPoint( const MexPoint2d& point );
    void addPutDownPoint( const MexPoint2d& point );
	void hwResearchCost( const MachPhys::ResearchUnits& );

    void interiorBoundary( const MexAlignedBox2d& boundary );
    void addInteriorObstacle( const ctl_vector< MexPoint2d > points );
    // PRE( points must be defined clockwise );

    void constructionType( MachPhys::ConstructionType type );
    void subType( int type );
    void hwLevel( uint level );
	
	void accessSizeIndex( int size );

    //  Should only be called when the data has been completely set
    //  up. Checks for things like the interior boundary being
    //  entirely within the exterior boundary, the station positions
    //  being within the interior boundary etc. Returns true iff the
    //  data is self consistent.
    bool dataValid() const;
        
	MachPhys::ResearchUnits			hwResearchCost_;
    //Data members
    MexAlignedBox2d localBoundary_; //The footprint
    Entrances entrances_; //The entrance internal/external locations
    Locations pickUpPoints_; //The locations for pickup activity in local coords
    Locations putDownPoints_; //The locations for putdown activity in local coords

    MexAlignedBox2d interiorBoundary_;
    Polygons        interiorObstacles_;
    
    // This is a horribly hacked way of pimplising this class however it needs 
    // to be done like this to avoid massive recompiles.
    MachPhysConstructionDataImpl* pImpl_;

    MachPhys::ConstructionType constructionType_; //The construction major type
    int subType_; //minor type, if any
    uint hwLevel_; //hw level
	int accessSizeIndex_;
};

#endif

/* End CONSDATA.HPP *************************************************/
