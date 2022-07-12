/*
 * O F A C T O R Y . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysSubTypedObjectFactory

    Factory for physical objects that have to be attached to a parent
    when they are created. The objects are also sub typed (see
    MachPhysObjectFactory for the non subtyped version )
    he factory also assumes that the object id
    will be a size_t. This allows it to
    use a C style array rather than a map making it more efficient than
    a UtlFactory would be for this case. It is intended to be used to
    instantiate buildings and machines.

    This class provides
    its own dummy root for attaching the saved objects to. This also
    avoids problems whereby the root and the factory both try and
    delete an object.
*/

#ifndef _MACHPHYS_OSTFACT_HPP
#define _MACHPHYS_OSTFACT_HPP

#include "base/base.hpp"

#include "world4d/root.hpp"
#include "ctl/fixedvec.hpp"

template< class SUB_TYPE, class PART >
class MachPhysSubTypedObjectFactory
{
public:
    MachPhysObjectFactory( size_t nEntries );
    virtual ~MachPhysObjectFactory();

    PART& part( SUB_TYPE subType, size_t hardwareLevel )

protected:

    void CLASS_INVARIANT;

private:
    // Operation deliberately revoked
    MachPhysObjectFactory( const MachPhysObjectFactory< SUB_TYPE, PART >& );

    // Operation deliberately revoked
    MachPhysObjectFactory< ID, PART >& operator =( const MachPhysObjectFactory< SUB_TYPE, PART >& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysObjectFactory< SUB_TYPE, PART >& );

    W4dRoot root_;
    ctl_fixed_vector< PART* >   entries_;
};

//#ifdef _INSTANTIATE_TEMPLATE_CLASSES
    #include "machphys/ostfact.ctp"
//#endif


#endif

/* End OFACTORY.HPP **************************************************/
