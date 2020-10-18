/*
 * G X M A T . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    GXMat

    A brief description of the class should go in here
*/

#ifndef _GXMAT_HPP
#define _GXMAT_HPP

#include "base/base.hpp"

#include "gxcolor.hpp"
#include "gxidpos.hpp"

class GXMat
// Canonical form 
{
public:
    GXMat( void );
    ~GXMat( void );
    GXMat( const GXMat& );
    GXMat& operator =( const GXMat& );
    friend bool operator ==( const GXMat& , const GXMat& );
    friend bool operator  <( const GXMat& , const GXMat& );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const GXMat& t );

	GXIdPos textureId( void ) const {
	  return textureId_;
	}
	void textureId(GXIdPos newTextureId) {
	  textureId_=newTextureId;
	}

    // true if textureId_ refers to a valid texture name in the mesh
    bool hasValidTexture() {
      return hasValidTexture_;
    }

    void hasValidTexture(bool valid) {
      hasValidTexture_=valid;
    }

	short transparancy( void ) const { return transparancy_; }
	void transparancy(short newTransparancy) { transparancy_=newTransparancy; }

    void hasOneColor(bool newHasOneColor )  { hasOneColor_=newHasOneColor; }
    bool hasOneColor() const { return hasOneColor_; }
	const GXColor& diffuseColor() const {
	  PRE(hasOneColor());
	  return diffuseColor_;
	}
	void diffuseColor(const GXColor& newDiffuseColor)
	{
	  diffuseColor_=newDiffuseColor;
	  hasOneColor_=true;
	}
private:
    
	// does the textureId_ refer to a valid texture name ?
	bool hasValidTexture_;
	GXIdPos textureId_;   

	short transparancy_; 

   // true if all vertices have same color
	bool hasOneColor_;
	GXColor diffuseColor_;

};


#endif

/* End GXMAT.HPP ****************************************************/
