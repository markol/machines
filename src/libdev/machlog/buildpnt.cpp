/*
 * B U I L D P N T . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/buildpnt.hpp"

PER_DEFINE_PERSISTENT( MachLogBuildPoint )

MachLogBuildPoint::MachLogBuildPoint( const MexPoint2d& position )
:	position_( position ),
	reserved_( false )
{
    TEST_INVARIANT;
}

MachLogBuildPoint::MachLogBuildPoint()
:	position_( MexPoint2d( 0,0 ) ),
	reserved_( false )
{
	//ASSERT( false, "This default constructor should never be used." );

    TEST_INVARIANT;
}


MachLogBuildPoint::~MachLogBuildPoint()
{
    TEST_INVARIANT;
}

MexPoint2d MachLogBuildPoint::position() const
{
	return position_;
}

bool MachLogBuildPoint::isReserved() const
{
	return reserved_;
}

bool MachLogBuildPoint::reserve( MachPhys::ConstructorSubType subTypeOfReserver )
{
	bool result = false; 

	if( not reserved_ )
	{
		reserved_= true;
		subTypeOfReserver_ = subTypeOfReserver;
		result = true;	
	}

	return result;
}

MachPhys::ConstructorSubType MachLogBuildPoint::subTypeOfReserver() const
{
	PRE( isReserved() );

	return subTypeOfReserver_; 
}	 

void MachLogBuildPoint::unReserve()
{
	PRE( reserved_ );
	
	reserved_ = false;	
}
        
void MachLogBuildPoint::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}


bool operator <( const MachLogBuildPoint& lhs, const MachLogBuildPoint& rhs )
{
    return lhs.position_ < rhs.position_;
}

bool operator ==( const MachLogBuildPoint& lhs, const MachLogBuildPoint& rhs )
{
    return lhs.position_ == rhs.position_;
}

ostream& operator <<( ostream& o, const MachLogBuildPoint& t )
{

    o << "MachLogBuildPoint " << (void*)&t << " start" << std::endl;
    o << "MachLogBuildPoint " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogBuildPoint& buildPoint )
{
    ostr << buildPoint.position_;
    ostr << buildPoint.reserved_;
	ostr << buildPoint.subTypeOfReserver_;
}

void perRead( PerIstream& istr, MachLogBuildPoint& buildPoint )
{
    istr >> buildPoint.position_;
    istr >> buildPoint.reserved_;
	istr >> buildPoint.subTypeOfReserver_;
}


/* End BUILDPNT.CPP *************************************************/
