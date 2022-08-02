/*
 * C O N T A I N . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    Containers

    Test persistence for the different container types
*/

#ifndef _PERSIST_CONTAIN_HPP
#define _PERSIST_CONTAIN_HPP

// #include "pertest/pvector.hpp"

#include "base/base.hpp"
#include "base/persist.hpp"

#include "ctl/vector.hpp"
#include "ctl/fixedvec.hpp"
#include "ctl/mmvector.hpp"
#include "ctl/list.hpp"
// #include "ctl/pvector.hpp"

class Containers
// Canonical form revoked
{
public:
    Containers();
    ~Containers();

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_DEFAULT( Containers );
    PER_FRIEND_READ_WRITE( Containers );

    friend ostream& operator <<( ostream& o, const Containers& t );

private:
    Containers( const Containers& );
    Containers& operator =( const Containers& );

    typedef ctl_vector< int >   VectorInt;
    VectorInt   vi1_;
    VectorInt*  pVi1_;
    VectorInt*  pVi2_;

    typedef ctl_min_memory_vector< int >   MinMemVectorInt;
    MinMemVectorInt   mmvi1_;
    MinMemVectorInt*  pMmvi1_;
    MinMemVectorInt*  pMmvi2_;

    typedef ctl_fixed_vector< int >   FixedVectorInt;
    FixedVectorInt   fvi1_;
    FixedVectorInt*  pFvi1_;
    FixedVectorInt*  pFvi2_;

    typedef ctl_list< int >   ListInt;
    ListInt   li1_;
    ListInt*  pLi1_;
    ListInt*  pLi2_;

//     typedef ctl_pvector< int >   PVectorInt;
//     PVectorInt   pvi1_;
//     PVectorInt*  pPVi1_;
//     PVectorInt*  pPVi2_;

    friend bool operator ==( const Containers&, const Containers& );
};


PER_DECLARE_PERSISTENT( Containers );

#endif

/* End CONTAIN.HPP **************************************************/
