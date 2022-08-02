/*
 * G X V E C T 4 . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    GXVect4

    A brief description of the class should go in here
*/

#ifndef _GXVECT4_HPP
#define _GXVECT4_HPP

#include "base/base.hpp"

class GXVect4
// Canonical form revoked
{
public:
    GXVect4();
    ~GXVect4();
    GXVect4( const GXVect4& );
	// POST(*this==copy);
    GXVect4& operator =( const GXVect4& );
	// POST(*this==copy);
    friend bool operator ==(const GXVect4&, const GXVect4& );

    float x() const { return x_; }
	float y() const { return y_; }
	float z() const { return z_; }
	float h() const { return h_; }

    void x(float newX) { x_=newX; }
	void y(float newY) { y_=newY; }
	void z(float newZ) { z_=newZ; }
	void h(float newH) { h_=newH; }

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const GXVect4& t );

private:
	float x_;
	float y_;
	float z_;
	float h_;
};

#endif

/* End GXVECT3.HPP **************************************************/
