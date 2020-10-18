/*
 * S U B I D . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysSubTypeId

    A convenient package for holding the id information
    needed to define subtyped objects.
*/

#ifndef _MACHPHYS_SUBID_HPP
#define _MACHPHYS_SUBID_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

template< class SUBTYPE >
class MachPhysSubTypeId
{
public:
    MachPhysSubTypeId()
    {}
    
    MachPhysSubTypeId( SUBTYPE subType, size_t level )
    : subType_( subType ),
      level_( level )
    {}
    
    MachPhysSubTypeId( const MachPhysSubTypeId< SUBTYPE >& copyMe )
    : subType_( copyMe.subType_ ),
      level_( copyMe.level_ )
    {}

    MachPhysSubTypeId< SUBTYPE >& operator =( const MachPhysSubTypeId< SUBTYPE >& copyMe )
    {
        subType_ = copyMe.subType_;
        level_ = copyMe.level_;
        return *this;
    }

    ~MachPhysSubTypeId( void )
    {}
    
    SUBTYPE subType_;
    size_t  level_;

    PER_MEMBER_PERSISTENT_DEFAULT_INLINE( MachPhysSubTypeId );
    
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysSubTypeId< SUBTYPE >& t );

private:
};

template< class SUBTYPE >
bool operator ==( const MachPhysSubTypeId< SUBTYPE >& a, const MachPhysSubTypeId< SUBTYPE >& b )
{
    return a.subType_ == b.subType_ and a.level_ == b.level_;
}

template< class SUBTYPE >
bool operator <( const MachPhysSubTypeId< SUBTYPE >& a, const MachPhysSubTypeId< SUBTYPE >& b )
{
    if( a.subType_ < b.subType_ )
        return true;
    if ( b.subType_ < a.subType_ )
        return false;

    return a.level_ < b.level_;
}

template< class SUBTYPE >
void perWrite( PerOstream& ostr, const MachPhysSubTypeId< SUBTYPE >& id )
{
    ostr << id.subType_;
    ostr << id.level_;
}

template< class SUBTYPE >
void perRead( PerIstream& istr, MachPhysSubTypeId< SUBTYPE >& id )
{
    istr >> id.subType_;
    istr >> id.level_;
}

#endif

/* End SUBID.HPP ****************************************************/
