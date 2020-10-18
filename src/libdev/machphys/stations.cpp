/*
 * S T A T I O N S . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/stations.hpp"

#include "mathex/epsilon.hpp"

MachPhysStations::MachPhysStations()
{
    stations_.reserve( 4 );

    TEST_INVARIANT;
}

MachPhysStations::MachPhysStations( const MachPhysStations& copyMe, const MexTransform3d& transform )
{
    stations_.reserve( copyMe.stations_.size() );

    for( Stations::const_iterator i = copyMe.stations_.begin();
        i != copyMe.stations_.end(); ++i )
    {
        stations_.push_back( MachPhysStation( (*i), transform ) );
    }
}

MachPhysStations::MachPhysStations( const MachPhysStations& copyMe )
: stations_( copyMe.stations_ )
{
}

MachPhysStations::~MachPhysStations()
{
    TEST_INVARIANT;

}

MachPhysStations& MachPhysStations::operator =( const MachPhysStations& copyMe )
{
    stations_ = copyMe.stations_;

    return *this;
}


size_t  MachPhysStations::nStations() const
{
    return stations_.size();
}

bool   MachPhysStations::freeStation( MachPhysStation::Type type, MachPhysStation** ppStation )
{
    bool found = false;

    for( Stations::iterator i = stations_.begin(); i != stations_.end() and not found; ++i )
    {
        if( (*i).type() == type and ( not (*i).locked() ) )
        {
            *ppStation = &(*i);
            found = true;
        }
    }


    POST( implies( found, not (*ppStation)->locked() ) );
    POST( implies( found, (*ppStation)->type() == type ) );

    return found;
}

void    MachPhysStations::addStation( const MexPoint2d& position, MachPhysStation::Type type )
{
    stations_.push_back( MachPhysStation( position, type ) );
}

void MachPhysStations::CLASS_INVARIANT
{
}

ostream& operator <<( ostream& o, const MachPhysStations& t )
{

    o << "MachPhysStations " << (void*)&t << " start" << std::endl;
	o << " Number of Stations " << t.nStations() << std::endl;
    for( MachPhysStations::Stations::const_iterator i = t.stations_.begin(); i != t.stations_.end(); ++i )
    {
		o << (*i);
    }
    o << "MachPhysStations " << (void*)&t << " end" << std::endl;

    return o;
}

bool MachPhysStations::nearStation( const MexPoint2d& position, MachPhysStation** ppStation )
{
    bool found = false;
    for( Stations::iterator i = stations_.begin(); i != stations_.end() and not found; ++i )
    {
        if( position.sqrEuclidianDistance( (*i).position() ) < MexEpsilon::sqrInstance() )
        {
            found = true;
            *ppStation = &(*i);
        }
    }

    return found;
}

const MachPhysStation& MachPhysStations::station( size_t i ) const
{
    PRE( i < nStations() );

    return stations_[ i ];
}


/* End STATIONS.CPP *************************************************/
