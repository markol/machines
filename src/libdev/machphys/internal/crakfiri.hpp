/*
 * C R A K F I R I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysCrackFireImpl

    the implementation class of MachPhysCrackFire
*/

#ifndef _MACHPHYS_CRAKFIRI_HPP
#define _MACHPHYS_CRAKFIRI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "mathex/mathex.hpp"
#include "world4d/link.hpp"
#include "ctl/vector.hpp"

class MachPhysCrackFire;

class MachPhysCrackFireImpl
// Canonical form revoked
{
public:


    PER_MEMBER_PERSISTENT( MachPhysCrackFireImpl );
    PER_FRIEND_READ_WRITE( MachPhysCrackFireImpl );		  
        
private:
    MachPhysCrackFireImpl( const MATHEX_SCALAR& length );
    MachPhysCrackFireImpl();
    ~MachPhysCrackFireImpl();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysCrackFireImpl& t );
	friend void perRead( PerIstream& istr, MachPhysCrackFire& crackFire );

    MachPhysCrackFireImpl( const MachPhysCrackFireImpl& );
    MachPhysCrackFireImpl& operator =( const MachPhysCrackFireImpl& );

	friend class MachPhysCrackFire;

	W4dLink* pCrack_;
	W4dLink* pFire_;
	MATHEX_SCALAR length_;
	ctl_vector < W4dLink* > debris_;
};

PER_DECLARE_PERSISTENT( MachPhysCrackFireImpl );

#endif

/* End CRAKFIRI.HPP *************************************************/
