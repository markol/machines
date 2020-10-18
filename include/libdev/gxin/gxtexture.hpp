/*
 * G X T E X T U R E . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    GXTexture

    A brief description of the class should go in here
*/

#ifndef _GXTEXTURE_HPP
#define _GXTEXTURE_HPP

#include "base/base.hpp"

#include "gxin/gx.hpp"
#include "gxin/gxidpos.hpp"
#include "gxin/gxname.hpp"

class GXTexture
// Canonical form revoked
{
public:
    GXTexture( void );
    ~GXTexture( void );
    GXTexture( const GXTexture& );
    GXTexture& operator =( const GXTexture& );
    friend bool operator ==( const GXTexture& , const GXTexture& );
    friend bool operator < ( const GXTexture& , const GXTexture& );

    void CLASS_INVARIANT;

    const GXName& name( void ) const {
	  return name_;
	}
	void name(const GXName& newName) {
	  name_=newName;
	}
    const GXIdPos& id( void ) const {
      return id_;
    }
    void id(const GXIdPos& newId) {
      id_=newId;
    }
	   
    friend ostream& operator <<( ostream& o, const GXTexture& t );

private:
	GXIdPos id_;
	GXName name_;
	short transparancy_;
	bool isPaletted_;
	USHORT palette_;

};


#endif

/* End GXTEXTURE.HPP ************************************************/
