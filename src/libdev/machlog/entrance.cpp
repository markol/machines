/*
 * E N T R A N C E . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/constron.hpp"
#include "machlog/entrance.hpp"
#include "machlog/presspad.hpp"
#include "machlog/planet.hpp"
#include "phys/cspace2.hpp"
#include "mathex/cvexpgon.hpp"
#include "machphys/consdata.hpp"


PER_DEFINE_PERSISTENT( MachLogEntrance );

MachLogEntrance::MachLogEntrance(
    const MachPhysConstructionData::EntranceData& entranceData,
    size_t  entranceNum,
    MachLogConstruction* pConstruction,
    PhysConfigSpace2d* pInteriorConfigSpace )
: locked_( false ),
  pConstruction_( pConstruction )
{
    const MexConvexPolygon2d padPolygon = entranceData.totalPolygon();

    ASSERT_INFO( padPolygon.nVertices() );
    ASSERT( padPolygon.nVertices() == 4, "" );

    //  Add the appropriate pressure pad data.
    // NB, all the pressure pads thus added are removed by the MachLogConstruction destructor.
    MachLogPlanet::instance().pressurePads().add(
      padPolygon.vertex( 0 ),
      padPolygon.vertex( 1 ),
      padPolygon.vertex( 2 ),
      padPolygon.vertex( 3 ),
      pConstruction, entranceNum );

    //  Add the polygons to the internal and external configuration
    //  spaces to allow the entrance to be locked

    const MATHEX_SCALAR   height = 20.0;

    std::auto_ptr< MexPolygon2d > interiorPolyAPtr (
      _NEW( MexConvexPolygon2d( entranceData.interiorPolygon() ) ) );

    interiorPolygonId_ =
      pInteriorConfigSpace->add( interiorPolyAPtr, height, MachLog::OBSTACLE_NORMAL, PhysConfigSpace2d::PERMANENT );

    std::auto_ptr< MexPolygon2d > exteriorPolyAPtr (
      _NEW( MexConvexPolygon2d( entranceData.exteriorPolygon() ) ) );

    exteriorPolygonId_ =
      MachLogPlanet::instance().configSpace().add(
      exteriorPolyAPtr, height, MachLog::OBSTACLE_NORMAL, PhysConfigSpace2d::PERMANENT );


    TEST_INVARIANT;
}

MachLogEntrance::~MachLogEntrance()
{
	//we need to explicitly remove a reference on the exterior polyon
	HAL_STREAM("MLEntrance::DTOR remove from planet cspace " << exteriorPolygonId_ << std::endl );
	MachLogPlanet::instance().configSpace().remove( exteriorPolygonId_ );
    TEST_INVARIANT;

}

void MachLogEntrance::CLASS_INVARIANT
{
	INVARIANT( this );
}

ostream& operator <<( ostream& o, const MachLogEntrance& t )
{

    o << "MachLogEntrance " << (void*)&t << " start" << std::endl;
    o << "MachLogEntrance " << (void*)&t << " end" << std::endl;

    return o;
}

bool    MachLogEntrance::lock()
{
    bool    result;

    if( locked() )
        result = false;
    else
    {
        result = true;
        locked_ = true;
    }

    POST( locked() );

    return result;
}

void    MachLogEntrance::unlock()
{
    PRE( locked() );

    locked_ = false;

    POST( not locked() );
}

bool    MachLogEntrance::locked() const
{
	// ALWAYS return true if the building isn't actually complete yet, i.e. you're always banned
	// from entering an incomplete building
	if( pConstruction_->isComplete() )
		return locked_;
	else
    	return true;
}

PhysConfigSpace2d::PolygonId    MachLogEntrance::interiorPolygonId() const
{
    return interiorPolygonId_;
}

PhysConfigSpace2d::PolygonId    MachLogEntrance::exteriorPolygonId() const
{
    return exteriorPolygonId_;
}

void perWrite( PerOstream& ostr, const MachLogEntrance& entrance )
{
	ostr << entrance.locked_;
}

void perRead( PerIstream& istr, MachLogEntrance& entrance )
{
	istr >> entrance.locked_;
}

MachLogEntrance::MachLogEntrance( PerConstructor )
{
}

MachLogConstruction& MachLogEntrance::construction() const
{
    return *pConstruction_;
}

/* End ENTRANCE.CPP *************************************************/
