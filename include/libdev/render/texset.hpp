/*
 * T E X S E T . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    RenTextureSet

    A brief description of the class should go in here
*/

#ifndef _TEXSET_HPP
#define _TEXSET_HPP

#include "base/base.hpp"
#include "base/istrrep.hpp"

template <class T> class ctl_vector;
class RenTexture;
class RenTextureSetImpl;
class SysPathName;

class RenTextureSet
// Canonical form revoked
{
public:
    RenTextureSet( void );
	// POST( not isLoaded() );
    RenTextureSet( const SysPathName& sysPathName );
	// POST( isLoaded() );
	RenTextureSet( const SysPathName& sysPathName, BaseProgressReporter* pReporter );
	// PRE( pReporter );
	// POST( isLoaded() );
    ~RenTextureSet( void );

    void CLASS_INVARIANT;

	// This function can only be called if the texture set has
	// not been loaded (i.e. the instance has been created by the default 
	// constructor) 
	void load( const SysPathName& );
	// PRE(not isLoaded() ); 
	void load( const SysPathName&, BaseProgressReporter* pReporter );
	// PRE(not isLoaded() );
	// PRE( pReporter );
	
	bool isLoaded( void ) const;

    typedef ctl_vector< RenTexture > RenTextures;

private:
    friend ostream& operator <<( ostream& o, const RenTextureSet& t );

    RenTextureSet( const RenTextureSet& );
    RenTextureSet& operator =( const RenTextureSet& );

	RenTextureSetImpl* pImpl_;
};


#endif

/* End TEXSET.HPP ***************************************************/
