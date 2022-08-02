/*
 * D O M A I N . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PedDomain

    A brief description of the class should go in here
*/

#ifndef _PLANETED_DOMAIN_HPP
#define _PLANETED_DOMAIN_HPP

#include "base/base.hpp"

#include "mathex/point2d.hpp"

#include "planeted/polygon.hpp"

class PedPortal;

class PedDomain : public PedPolygon
{
public:
    PedDomain( const PolyVerticies& , MATHEX_SCALAR height, bool selected );
    ~PedDomain();

    void CLASS_INVARIANT;

	virtual void save( std::ofstream& );

	virtual RenColour getWireColour() const;

	bool intersectsPortal( const PedPortal& ) const;
	// PRE( portal.verticies().size() == 2 );

	bool intersectsDomain( const PedDomain&, MexPoint2d* pPoint1, MexPoint2d* pPoint2 ) const;

 private:
    // Operations deliberately revoked
    PedDomain( const PedDomain& );
    PedDomain& operator =( const PedDomain& );
    bool operator ==( const PedDomain& );

	friend ostream& operator <<( ostream& o, const PedDomain& t );
};

#endif

/* End DOMAIN.HPP ***************************************************/
