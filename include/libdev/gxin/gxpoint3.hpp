/*
 * G X P O I N T 3 . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#ifndef _GXPOINT3_HPP
#define _GXPOINT3_HPP

#include "base/base.hpp"

class GXPoint3 {
public:
    GXPoint3( void );
    ~GXPoint3( void );
    GXPoint3( const GXPoint3& );
	// POST(*this==copy);
    GXPoint3& operator =( const GXPoint3& );
	// POST(*this==copy);
    friend bool operator ==( const GXPoint3&, const GXPoint3& );

    float x() const {
	  return x_;
	}
	float y() const {
	  return y_;
	}
	float z() const {
	  return z_;
	}

    void x(float newX) {
	  x_=newX;
	}
	void y(float newY) {
	  y_=newY;
	}
	void z(float newZ) {
	  z_=newZ;
	}
	
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const GXPoint3& t );
	
private:
	float x_;
	float y_;
	float z_;
};

#endif
