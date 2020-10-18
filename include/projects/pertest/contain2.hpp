/*
 * C O N T A I N 2 . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    Containers2

    Test persistence for the different container types
*/

#ifndef _PERSIST_CONTAIN2_HPP
#define _PERSIST_CONTAIN2_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "ctl/vector.hpp"
#include "ctl/fixedvec.hpp"
#include "ctl/mmvector.hpp"
#include "ctl/list.hpp"
#include "ctl/pvector.hpp"

class Containers2
// Canonical form revoked
{
public:
    Containers2( void );
    ~Containers2( void );

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_DEFAULT( Containers2 );
    PER_FRIEND_READ_WRITE( Containers2 );

    friend ostream& operator <<( ostream& o, const Containers2& t );

private:
    Containers2( const Containers2& );
    Containers2& operator =( const Containers2& );

    typedef ctl_pvector< int >   PVectorInt;
    PVectorInt   pvi1_;
    PVectorInt*  pPVi1_;
    PVectorInt*  pPVi2_;

    friend bool operator ==( const Containers2&, const Containers2& );
};


PER_DECLARE_PERSISTENT( Containers2 );

#endif

/* End CONTAIN.HPP **************************************************/
