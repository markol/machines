/*
 * S T A T I O N . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/station.hpp"

#include "mathex/transf3d.hpp"

MachPhysStation::MachPhysStation( const MexPoint2d& position, Type type )
: position_( position ),
  type_( type ),
  locked_( false )
{
}

MachPhysStation::MachPhysStation( const MachPhysStation& copyMe )
: position_( copyMe.position_ ),
  type_( copyMe.type_ ),
  locked_( false )
{
}
MachPhysStation::MachPhysStation( const MachPhysStation& copyMe, const MexTransform3d& transform )
: position_( copyMe.position_ ),
  type_( copyMe.type_ ),
  locked_( false )
{
    transform.transform( &position_ );
}

MachPhysStation& MachPhysStation::operator =( const MachPhysStation& copyMe )
{
    position_ = copyMe.position_;
    type_ = copyMe.type_;
    locked_ = false;

    return *this;
}

MachPhysStation::~MachPhysStation()
{
    TEST_INVARIANT;

}

const MexPoint2d& MachPhysStation::position() const
{
    return position_;
}

MachPhysStation::Type    MachPhysStation::type() const
{
    return type_;
}

bool    MachPhysStation::locked() const
{
    return locked_;
}

void    MachPhysStation::lock( bool isLocked )
{
    locked_ = isLocked;
}


void MachPhysStation::CLASS_INVARIANT
{
}

ostream& operator <<( ostream& o, const MachPhysStation& t )
{
	o << "Type : ";
	switch (t.type_)
	{
		case MachPhysStation::PARKING_BAY:
			o << "PARKING_BAY ";
			break;
		case MachPhysStation::RESEARCH_BAY:
			o << "RESEARCH_BAY ";
			break;
		default:
			o << "Unknown type (int)type " << (int)t.type_ << std::endl;
	}
	o << "Position " << t.position_;
	o << " Locked " << t.locked_ << std::endl;

    return o;
}

/* End STATION.CPP **************************************************/
