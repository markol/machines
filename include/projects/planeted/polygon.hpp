/*
 * P O L Y G O N . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PedPolygon

    A brief description of the class should go in here
*/

#ifndef _PLANETED_POLYGON_HPP
#define _PLANETED_POLYGON_HPP

#include "base/base.hpp"

#include "mathex/point2d.hpp"

#include "world4d/custom.hpp"

#include "ctl/vector.hpp"

class ofstream;

typedef ctl_vector< MexPoint2d > PolyVerticies;

class PedPolygon : public W4dCustom
{
public:
    PedPolygon( const PolyVerticies& , MATHEX_SCALAR z, bool selected );
    ~PedPolygon( void );

    void CLASS_INVARIANT;

	PolyVerticies& verticies( void );
	const PolyVerticies& verticies() const;

	bool isConvex() const;

	void refreshMesh( void );

	virtual void select( bool selected );

	virtual void save( std::ofstream& ) = 0;

	virtual RenColour getWireColour() const = 0;

	void hide( bool hide );

	bool hidden( void );

	virtual size_t height() const;

	virtual void height( size_t );

	void flatten( bool );

	void uniqueId( size_t );

	size_t uniqueId() const;

protected:
	//Data...
	PolyVerticies verticies_;
	MATHEX_SCALAR z_;
	bool selected_;
	bool hidden_;
	bool flatten_;
	size_t uniqueId_;

private:
    // Operations deliberately revoked
    PedPolygon( const PedPolygon& );
    PedPolygon& operator =( const PedPolygon& );
    bool operator ==( const PedPolygon& );

	friend ostream& operator <<( ostream& o, const PedPolygon& t );
};

#endif

/* End POLYGON.HPP **************************************************/
