/*
 * T E X S E T I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    RenTextureSetImpl

    A brief description of the class should go in here
*/

#ifndef _TEXSETI_HPP
#define _TEXSETI_HPP

#include "base/base.hpp"
#include "ctl/vector.hpp"
#include "render/texture.hpp"
#include "render/texset.hpp"


class RenTextureSetImpl
// Canonical form revoked
{
public:
    ~RenTextureSetImpl();

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const RenTextureSetImpl& t );
	friend class RenTextureSet;

    RenTextureSetImpl();
    RenTextureSetImpl( const SysPathName& directory );
	RenTextureSetImpl( const SysPathName& directory, BaseProgressReporter* pReporter );

    RenTextureSetImpl( const RenTextureSetImpl& );
    RenTextureSetImpl& operator =( const RenTextureSetImpl& );

	void load( const SysPathName&, BaseProgressReporter* pReporter = NULL );

	RenTextureSet::RenTextures textures_;
	bool isLoaded_;
};


#endif

/* End TEXSETI.HPP **************************************************/
