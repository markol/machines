/*
 * L L L I S T . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    W4dLocalLightList

    A brief description of the class should go in here
*/

#ifndef _WORLD4D_LLLIST_HPP
#define _WORLD4D_LLLIST_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

template <class T> class ctl_pvector;
class W4dLight;
class W4dEntity;

class W4dLocalLightList
{
public:
    W4dLocalLightList();
    ~W4dLocalLightList();

	void removeLight(W4dLight* pLightItem);
	void addLight(W4dLight* pLightItem);

	void turnOnAll();
	void turnOffAll();

	//Disassociate all lights from pEntity
	void clearAllLights( W4dEntity* pEntity );
	//PRE( pEntity must be the owner of the light list )

	bool isInSceneMgrsList(W4dLight* pLightItem) const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const W4dLocalLightList& t );

    PER_MEMBER_PERSISTENT_DEFAULT( W4dLocalLightList );
    PER_FRIEND_READ_WRITE( W4dLocalLightList );
    
private:
    // Operation deliberately revoked
    W4dLocalLightList( const W4dLocalLightList& );

    // Operation deliberately revoked
    W4dLocalLightList& operator =( const W4dLocalLightList& );

    // Operation deliberately revoked
    bool operator ==( const W4dLocalLightList& );

	typedef ctl_pvector<W4dLight> Lights;
	Lights*	pLights_;
};

PER_DECLARE_PERSISTENT( W4dLocalLightList );

#endif

/* End LLLIST.HPP ***************************************************/
