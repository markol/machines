/*
 * G R P M O V E . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/grpmove.hpp"
#include "machlog/internal/grpmovei.hpp"

MachLogGroupSimpleMove::MachLogGroupSimpleMove(
    const Actors& actors,
    const Points& points,
    size_t commandId,
    string* pReason,
    PhysPathFindingPriority pathFindingPriority )
: pImpl_( _NEW( MachLogGroupSimpleMoveImplementation( actors, points, commandId, pReason, pathFindingPriority ) ) )
{

    TEST_INVARIANT;
}

MachLogGroupSimpleMove::~MachLogGroupSimpleMove()
{
    TEST_INVARIANT;

    _DELETE( pImpl_ );
}

bool    MachLogGroupSimpleMove::moveOK() const
{
    PRE( pImpl_ != NULL );
    
    return pImpl_->moveOK();
}

void MachLogGroupSimpleMove::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogGroupSimpleMove& t )
{

    o << "MachLogGroupSimpleMove " << (void*)&t << " start" << std::endl;
    o << "MachLogGroupSimpleMove " << (void*)&t << " end" << std::endl;

    return o;
}

/* End GRPMOVE.CPP **************************************************/

