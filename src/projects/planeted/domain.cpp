/*
 * D O M A I N . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "planeted/domain.hpp"
#include "planeted/portal.hpp"

#include "mathex/mathex.hpp"
#include "mathex/line2d.hpp"

#include "render/colour.hpp"

// For std::swap<>().
#include <algorithm>

#include <fstream>

PedDomain::PedDomain( const PolyVerticies& verticies, MATHEX_SCALAR z, bool selected )
: 	PedPolygon( verticies, z, selected )
{
   	PRE( verticies.size() > 2 );

    TEST_INVARIANT;
}

PedDomain::~PedDomain()
{
    TEST_INVARIANT;

}

void PedDomain::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const PedDomain& t )
{

    o << "PedDomain " << (void*)&t << " start" << std::endl;
    o << "PedDomain " << (void*)&t << " end" << std::endl;

    return o;
}

// virtual
RenColour PedDomain::getWireColour() const
{
	return RenColour::blue();
}

// virtual
void PedDomain::save( std::ofstream& out )
{
	//If the domain is rectangular then write out as aligned box
	bool writtenBox = false;
	if( verticies_.size() == 4 )
	{
		bool x_0_3 = verticies_[0].x() == verticies_[3].x();
		bool x_1_2 = verticies_[1].x() == verticies_[2].x();
		bool y_0_1 = verticies_[0].y() == verticies_[1].y();
		bool y_2_3 = verticies_[2].y() == verticies_[3].y();
		if( x_0_3 and x_1_2 and y_0_1 and y_2_3 )
		{
			size_t minX = std::min( verticies_[0].x(), verticies_[2].x() );
			size_t maxX = std::max( verticies_[0].x(), verticies_[2].x() );
			size_t minY = std::min( verticies_[0].y(), verticies_[2].y() );
			size_t maxY = std::max( verticies_[0].y(), verticies_[2].y() );
			out << "DOMAIN " << uniqueId_ << " " << minX << " " << minY << " " << maxX << " " << maxY << std::endl;
			writtenBox = true;
		}
	}

	if( not writtenBox )
	{
		out << "NEWDOMAIN " << uniqueId_ << " " << verticies_.size() << std::endl;

		for ( ctl_vector< MexPoint2d >::iterator verIter = verticies_.begin();
			  verIter != verticies_.end();
			  ++verIter )
		{
			out << (*verIter).x() << " " << (*verIter).y() << std::endl;
		}
		out << std::endl;
	}
}

bool PedDomain::intersectsPortal( const PedPortal& portal ) const
{
	PRE( portal.verticies().size() == 2 );

	TEST_INVARIANT;

/*
	bool retVal = false;

	size_t minX = min( verticies_[0].x(), verticies_[2].x() );
	size_t maxX = max( verticies_[0].x(), verticies_[2].x() );
	size_t minY = min( verticies_[0].y(), verticies_[2].y() );
	size_t maxY = max( verticies_[0].y(), verticies_[2].y() );

	if ( portal.verticies()[0].x() >= minX and
		 portal.verticies()[0].x() <= maxX and
		 portal.verticies()[0].y() >= minY and
		 portal.verticies()[0].y() <= maxY and
		 portal.verticies()[1].x() >= minX and
		 portal.verticies()[1].x() <= maxX and
		 portal.verticies()[1].y() >= minY and
		 portal.verticies()[1].y() <= maxY )
	{
		retVal = true;
	}
*/

	bool portalVertex1Intersects = false;
	bool portalVertex2Intersects = false;

	MexPoint2d firstPoint = verticies_.back();

	for ( 	ctl_vector< MexPoint2d >::const_iterator verIter = verticies_.begin();
			verIter != verticies_.end();
			++verIter )
	{
		MexLine2d domainEdge( firstPoint, (*verIter) );

		if ( domainEdge.isPointOnLine( portal.verticies()[0] ) )
		{
			portalVertex1Intersects = true;
		}
		if ( domainEdge.isPointOnLine( portal.verticies()[1] ) )
		{
			portalVertex2Intersects = true;
		}

		firstPoint = (*verIter);
	}

	TEST_INVARIANT;

	return portalVertex1Intersects and portalVertex2Intersects;
}

/*bool PedDomain::intersectsDomain( const PedDomain& rhs, MexPoint2d& point1, MexPoint2d& point2 ) const
{
	bool retVal = false;
	bool commonX = false;
	bool commonY = false;
	uint x1, x2, y1, y2;

	size_t minX = min( verticies_[0].x(), verticies_[2].x() );
	size_t maxX = max( verticies_[0].x(), verticies_[2].x() );
	size_t minY = min( verticies_[0].y(), verticies_[2].y() );
	size_t maxY = max( verticies_[0].y(), verticies_[2].y() );

	size_t rhsMinX = min( rhs.verticies_[0].x(), rhs.verticies_[2].x() );
	size_t rhsMaxX = max( rhs.verticies_[0].x(), rhs.verticies_[2].x() );
	size_t rhsMinY = min( rhs.verticies_[0].y(), rhs.verticies_[2].y() );
	size_t rhsMaxY = max( rhs.verticies_[0].y(), rhs.verticies_[2].y() );


	if( maxX == rhsMinX )
	{
		x1 = x2 = maxX;
		commonX = true;
	}
	else if( rhsMaxX == minX )
	{
		x1 = x2 = minX;
		commonX = true;
	}
	if( maxY == rhsMaxY )
	{
		y1 = y2 = maxY;
		commonY = true;
	}
	else if( rhsMaxY == minY )
	{
		y1 = y2 = minY;
		commonY = true;
	}
	if( commonX )
	{
		if( ( not ( rhsMaxY < minY ) ) and ( not ( rhsMinY > maxY ) ) )
		{
			y1 = min( maxY, rhsMaxY );
			y2 = max( minY, rhsMinY );
			retVal = true;
		}
	}
	if( commonY )
	{
		if( ( not ( rhsMaxX < minX ) ) and ( not ( rhsMinX > maxX ) ) )
		{
			x1 = min( maxX, rhsMaxX );
			x2 = max( minX, rhsMinX );
			retVal = true;
		}
	}
	point1 = MexPoint2d( x1, y1 );
	point2 = MexPoint2d( x2, y2 );

	if( point1 == point2 )
	// We have detected two domains intersecting at corners
	// which we are not interested in
	{
		retVal = false;
	}
	return retVal;
}*/

bool PedDomain::intersectsDomain(const PedDomain& that, MexPoint2d* pPoint1, MexPoint2d* pPoint2) const
{
	PRE(pPoint1);
	PRE(pPoint2);

	bool result = false;

	// This method works on the following set of assumptions.
	// 1. The caller of this method will check that duplicate portals are not being made.
	// 2. Placing the points within epsilon of the vertex of a domain is exceptable.

	// Algorithm.
	// 1. Iterate through all edges of this domain against all edges of that domain.
	// 2. Check each vertex of an edge against the infinite line projected from the opposing edge.
	// - If all are within epsilon of the opposing edge a portal can be made.
	// 3. Obtain the distanceAlongLine() (see MexLine2d) of each vertex against the opposing edge.

	ASSERT(verticies().size() > 2, "This domain does not have enough vertices.");
	ASSERT(that.verticies().size() > 2, "That domain does not have enough vertices.");

	DANIEL_STREAM("nEdges == " << verticies().size() << std::endl);
	DANIEL_STREAM("nEdges == " << that.verticies().size() << std::endl);

	// I would normally use functors but for simplicity it's all here.
	PolyVerticies::const_iterator itThisVertex1 = verticies().begin();
	PolyVerticies::const_iterator itThisVertex2 = itThisVertex1 + 1;
	PolyVerticies::const_iterator itThisEnd = verticies().end();

	PolyVerticies::const_iterator itThatBase = that.verticies().begin();
	PolyVerticies::const_iterator itThatVertex1 = itThatBase;
	PolyVerticies::const_iterator itThatVertex2 = itThatVertex1 + 1;
	PolyVerticies::const_iterator itThatEnd = that.verticies().end();

	// Outer loop goes through this objects' vertices.
	for(
		int thisEdgeIndex =0;
		itThisVertex1 != itThisEnd and result != true;
		++thisEdgeIndex, ++itThisVertex1, ++itThisVertex2)
	{
		if(itThisVertex1 == itThisEnd - 1)
			itThisVertex2 = verticies().begin();

		// Inner loop goes through that objects' vertices.
		for(
			int thatEdgeIndex = 0;
			itThatVertex1 != itThatEnd and result != true;
			++thatEdgeIndex, ++itThatVertex1, ++itThatVertex2)
		{
			if(itThatVertex1 == itThatEnd - 1)
				itThatVertex2 = that.verticies().begin();

			// Create lines from the pairs of vertices.	We use the methods later.
			MexLine2d thisLine((*itThisVertex1), (*itThisVertex2));
			MexLine2d thatLine((*itThatVertex1), (*itThatVertex2));

			// PointX is the source point. PointD is the other point on the same line.
			// The other two refer to the points on the opposing line.
			MexPoint2d pointX((*itThisVertex1));
			MexPoint2d pointD((*itThisVertex2));
			MexPoint2d pointD1((*itThatVertex1));
			MexPoint2d pointD2((*itThatVertex2));

			DANIEL_STREAM("Compare [" << thisEdgeIndex << ", " << thatEdgeIndex << ']' << std::endl);
			DANIEL_INDENT(2);
			DANIEL_STREAM("thisEdge :" << thisLine << std::endl);
			DANIEL_STREAM("thatEdge :" << thatLine << std::endl);

			bool colinear =
				MexLine2d::side(thatLine.end1(), thatLine.end2(), thisLine.end1()) == Mathex::ONEDGE and
				MexLine2d::side(thatLine.end1(), thatLine.end2(), thisLine.end2()) == Mathex::ONEDGE and
				MexLine2d::side(thisLine.end1(), thisLine.end2(), thatLine.end1()) == Mathex::ONEDGE and
				MexLine2d::side(thisLine.end1(), thisLine.end2(), thatLine.end2()) == Mathex::ONEDGE;

			bool longEnough = thisLine.length() >= 1 and thatLine.length() >= 1;

			DANIEL_STREAM("colinear == " << colinear << std::endl);
			DANIEL_STREAM("longEnough == " << longEnough << std::endl);

			// This returns true if the lines are colinear.
			if(MexLine2d::side(thatLine.end1(), thatLine.end2(), thisLine.end1()) == Mathex::ONEDGE and
				MexLine2d::side(thatLine.end1(), thatLine.end2(), thisLine.end2()) == Mathex::ONEDGE and
				MexLine2d::side(thisLine.end1(), thisLine.end2(), thatLine.end1()) == Mathex::ONEDGE and
				MexLine2d::side(thisLine.end1(), thisLine.end2(), thatLine.end2()) == Mathex::ONEDGE and
				thisLine.length() >= 1 and
				thatLine.length() >= 1)
			{
				DANIEL_STREAM("Lines are colinear." << std::endl);
				// As pointX is at the origin of this it's displacement is 0. The names are
				// much terser than I would normally allow but the maths is quite hard so they help.
				MATHEX_SCALAR x = 0.0;
				MATHEX_SCALAR d = thisLine.distanceAlongLine(thisLine.end2());
				MATHEX_SCALAR d1 = thisLine.distanceAlongLine(thatLine.end1());
				MATHEX_SCALAR d2 = thisLine.distanceAlongLine(thatLine.end2());

				DANIEL_STREAM("Before adjustment:" << std::endl);
				DANIEL_STREAM("x  :" << x << std::endl);
				DANIEL_STREAM("d  :" << d << std::endl);
				DANIEL_STREAM("d1 :" << d1 << std::endl);
				DANIEL_STREAM("d2 :" << d2 << std::endl);

				// The following two if statements help me to reduce the number of admissible
				// portal states that I have to check for.

				// I want x to be less than d. Halves the admissible portal state space.
				if(d < 0)
				{
					DANIEL_STREAM("Swapping x and d." << std::endl);
					std::swap(x, d);
					std::swap(pointX, pointD);
				}

				// I want d1 to be < d2. The halves again the state space of admissible portals.
				if(d2 < d1)
				{
					DANIEL_STREAM("Swapping d1 and d2");
					std::swap(d1, d2);
					std::swap(pointD1, pointD2);
				}

				// Now that I have reduced the state space of eight admissible portals
				// I can get to the job of finding whether there is one. By some
				// sneeky use of relational ops I can further reduce the state space.
				// This sneekyness forces the guard in the first if statement.
				if(not ((d1 <= x and d2 <= x) or (d1 >= d and d2 >= d)))
				{
					DANIEL_STREAM("The lines intersect." << std::endl);

					if(d1 <= x)
					{
						if(d2 > d)
						{
							DANIEL_STREAM("First" << std::endl);

							(*pPoint1) = pointX;
							(*pPoint2) = pointD;

							result = true;
						}
						else
						{
							DANIEL_STREAM("Second" << std::endl);

							(*pPoint1) = pointX;
							(*pPoint2) = pointD2;

							result = true;
						}
					}
					else if(d1 > x)
					{
						if(d2 > d)
						{
							DANIEL_STREAM("Third" << std::endl);

							(*pPoint1) = pointD1;
							(*pPoint2) = pointD;

							result = true;
						}
						else
						{
							DANIEL_STREAM("Fourth" << std::endl);

							(*pPoint1) = pointD1;
							(*pPoint2) = pointD2;

							result = true;
						}
					}
					else
					{
						ASSERT_INFO(thisLine);
						ASSERT_INFO(thatLine);
						ASSERT_INFO(x);
						ASSERT_INFO(d);
						ASSERT_INFO(d1);
						ASSERT_INFO(d2);
						ASSERT_FAIL("Something went very wrong.");
					}
				}
			}

			DANIEL_INDENT(-2);
		}

		itThatVertex1 = that.verticies().begin();
		itThatVertex2 = itThatVertex1 + 1;
	}

	return result;
}

/* End DOMAIN.CPP *************************************************/
