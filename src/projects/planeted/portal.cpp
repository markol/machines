/*
 * P O R T A L . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "planeted/portal.hpp"

#include "render/colour.hpp"

#include <fstream>

PedPortal::PedPortal( const PolyVerticies& verticies, MATHEX_SCALAR z, bool selected )
: 	PedPolygon( verticies, z, selected )
{
   	PRE( verticies.size() == 2 );

    TEST_INVARIANT;
}

PedPortal::~PedPortal()
{
    TEST_INVARIANT;

}

void PedPortal::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const PedPortal& t )
{
	o << "PORTAL " << t.domainOne_ << " " << t.domainTwo_ << " "
					 << t.verticies()[0].x() << " " << t.verticies()[0].y() << " "
					 << t.verticies()[1].x() << " " << t.verticies()[1].y() << std::endl;
    return o;
}

// virtual
RenColour PedPortal::getWireColour() const
{
	return RenColour::cyan();
}

// virtual
void PedPortal::save( std::ofstream& out )
{
	PRE( verticies().size() == 2)

	out << "PORTAL " << domainOne_ << " " << domainTwo_ << " "
					 << verticies()[0].x() << " " << verticies()[0].y() << " "
					 << verticies()[1].x() << " " << verticies()[1].y() << std::endl;
}

// virtual
size_t PedPortal::height() const
{
	return 101;
}

size_t PedPortal::domainOne() const
{
	return domainOne_;
}

size_t PedPortal::domainTwo() const
{
	return domainTwo_;
}

void PedPortal::setDomains(size_t domainOne, size_t domainTwo )
{
	domainOne_ = domainOne;
	domainTwo_ = domainTwo;
}


/* End PORTAL.CPP *************************************************/
