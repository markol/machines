/*
 * P O I N T S . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#ifndef _RENDER_POINTS_HPP
#define _RENDER_POINTS_HPP

#include "base/base.hpp"

class RenIPoints;
class MexPoint3d;
class RenColour;
class MexTransform3d;
class SysPathName;

// A list of 3D vertices each with its own colour.
class RenPoints
{
public:
    RenPoints(size_t initial=0);
    ~RenPoints();
	
	size_t nPoints() const;
	void point(size_t idx, const MexPoint3d& p, const RenColour& c);
	void addPoint(const MexPoint3d&, const RenColour&);
	void render(const MexTransform3d& world) const;
	void read(const SysPathName&);

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const RenPoints& t );

private:
	RenIPoints*	points_;
	
    RenPoints( const RenPoints& );
    RenPoints& operator =( const RenPoints& );
    bool operator ==( const RenPoints& );
};

#endif

/* End POINTS.HPP ***************************************************/
