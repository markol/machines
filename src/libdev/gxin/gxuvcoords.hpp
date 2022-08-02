/*
 * G X U V C O O R D S . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    GXUVCoords

    A brief description of the class should go in here
*/

#ifndef _GXUVCOORDS_HPP
#define _GXUVCOORDS_HPP

#include "base/base.hpp"

class GXUVCoords
// Canonical form revoked
{
public:
    GXUVCoords();
    ~GXUVCoords();
    GXUVCoords( const GXUVCoords& );
    GXUVCoords& operator =( const GXUVCoords& );
    friend bool operator ==( const GXUVCoords&, const GXUVCoords& );
    friend bool operator < ( const GXUVCoords&, const GXUVCoords& );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const GXUVCoords& t );

	float u() const {
	  return u_;
	}
	void u(float newU) {
	  u_=newU;
	}
    float v() const {
	  return v_;
	}
    void v(float newV) {
	  v_=newV;
	}
private:

	float u_;
	float v_;
};


#endif

/* End GXUVCOORDS.HPP ***********************************************/
