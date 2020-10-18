/*
 * G R P M V I N F . I P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

#include "profiler/profiler.hpp"

_CODE_INLINE
MachLogGroupMoveInfo::MachLogGroupMoveInfo()
: valid_( false )
{
    TEST_INVARIANT;
    POST( not valid() );
}

_CODE_INLINE
MachLogGroupMoveInfo::MachLogGroupMoveInfo( const MexVec2& offset, MATHEX_SCALAR groupRadius )
: valid_( true ),
  offset_( offset ),
  groupRadius_( groupRadius )
{
    POST( valid() );
}

_CODE_INLINE
MachLogGroupMoveInfo::~MachLogGroupMoveInfo()
{
    TEST_INVARIANT;

}

_CODE_INLINE
bool MachLogGroupMoveInfo::valid() const
{
    return valid_;
}

_CODE_INLINE
const MexVec2& MachLogGroupMoveInfo::offset() const
{
    return offset_;
}

_CODE_INLINE
MATHEX_SCALAR MachLogGroupMoveInfo::groupRadius() const
{
    return groupRadius_;
}

/* End GRPMVINF.IPP *************************************************/
