/*
 * O F A C T O R Y . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysObjectFactory

    Factory for physical objects that have to be attached to a parent
    when they are created. The factory also assumes that the object id
    will be a size_t. This allows it to
    use a C style array rather than a map making it more efficient than
    a UtlFactory would be for this case. It is intended to be used to
    instantiate buildings and machines.

    This class provides
    its own dummy root for attaching the saved objects to. This also
    avoids problems whereby the root and the factory both try and
    delete an object.

    MachPhysSubTypedObjectFactory

    As MachPhysObjectFactory except that the part has a subtype which
    is used to construct it and to find its index in the internal list.
*/

#ifndef _MACHPHYS_OFACTORY_HPP
#define _MACHPHYS_OFACTORY_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "ctl/fixedvec.hpp"

#include "world4d/root.hpp"

template< class ID, class PART > class MachPhysObjectFactory;
template< class ID, class PART >
void perWrite( PerOstream& ostr, const MachPhysObjectFactory< ID, PART >& factory );
template< class ID, class PART >
void perRead( PerIstream& ostr, MachPhysObjectFactory< ID, PART >& factory );

template< class ID, class PART >
class MachPhysObjectFactory
{
public:
    MachPhysObjectFactory( size_t nEntries );
    virtual ~MachPhysObjectFactory( void );

    PART& part( const ID& id, size_t index );

    PER_MEMBER_PERSISTENT( MachPhysObjectFactory );
    PER_FRIEND_READ_WRITE( MachPhysObjectFactory );

    friend void perWrite < >( PerOstream& ostr, const MachPhysObjectFactory< ID, PART > & factory );
    friend void perRead < >( PerIstream& istr, MachPhysObjectFactory< ID, PART > & factory );


protected:

    void CLASS_INVARIANT;

private:
    // Operation deliberately revoked
    MachPhysObjectFactory( const MachPhysObjectFactory< ID, PART >& );

    // Operation deliberately revoked
    MachPhysObjectFactory< ID, PART >& operator =( const MachPhysObjectFactory< ID, PART >& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysObjectFactory< ID, PART >& );

    W4dRoot root_;
    ctl_fixed_vector< PART* >   entries_;


};

PER_DECLARE_PERSISTENT_T2( MachPhysObjectFactory );

//#ifdef _INSTANTIATE_TEMPLATE_CLASSES
    #include "machphys/ofactory.ctp"
//#endif

//#ifdef _INSTANTIATE_TEMPLATE_FUNCTIONS
    #include "machphys/ofactory.ctf"
//#endif


#endif

/* End OFACTORY.HPP **************************************************/
