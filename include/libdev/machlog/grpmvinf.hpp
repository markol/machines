/*
 * G R P M V I N F . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogGroupMoveInfo

    Information to assist with a group move. It gives the motion sequencer 
    some idea of where an individual machine is relative to the rest of the group.
*/

#ifndef _MACHLOG_GRPMVINF_HPP
#define _MACHLOG_GRPMVINF_HPP

#include "base/base.hpp"
#include "mathex/vec2.hpp"

class MachLogGroupMoveInfo
// Memberwise canonical
{
public:
    MachLogGroupMoveInfo();
    //  POST( not valid() );
    MachLogGroupMoveInfo( const MexVec2& offset, MATHEX_SCALAR groupRadius );
    //  POST( valid() );

    ~MachLogGroupMoveInfo();

    bool valid() const;
    const MexVec2& offset() const;
    MATHEX_SCALAR groupRadius() const;

    void CLASS_INVARIANT;

	PER_MEMBER_PERSISTENT_DEFAULT( MachLogGroupMoveInfo );
	PER_FRIEND_READ_WRITE( MachLogGroupMoveInfo );

private:
    friend ostream& operator <<( ostream& o, const MachLogGroupMoveInfo& t );

    bool            valid_;
    MexVec2         offset_;
    MATHEX_SCALAR   groupRadius_;
};

PER_DECLARE_PERSISTENT( MachLogGroupMoveInfo );

#ifdef _INLINE
    #include "machlog/grpmvinf.ipp"
#endif


#endif

/* End GRPMVINF.HPP *************************************************/
