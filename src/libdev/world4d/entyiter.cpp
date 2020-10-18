/*
 * E N T Y I T E R . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/entyiter.hpp"
#include "world4d/composit.hpp"
#include "world4d/link.hpp"
#include "ctl/list.hpp"

W4dEntityIter::W4dEntityIter( W4dEntity* pEntity )
{
    //Set a reasonable size for the stack
    stack_.reserve( 128 );

    //Push the first entity on
    if( pEntity )
        stack_.push_back( pEntity );

    TEST_INVARIANT;
}

W4dEntityIter::~W4dEntityIter()
{
    TEST_INVARIANT;
}

void W4dEntityIter::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const W4dEntityIter& t )
{

    o << "W4dEntityIter " << (void*)&t << " start" << std::endl;
    for( int i = 0; i != t.stack_.size() ; ++i )
        o << "  " << (void*)t.stack_[i] << std::endl;
    o << "W4dEntityIter " << (void*)&t << " end" << std::endl;

    return o;
}

bool W4dEntityIter::isFinished() const
{
    return stack_.size() == 0;
}

W4dEntityIter& W4dEntityIter::operator++ ()
{
    PRE( not isFinished() );

    //Pop the top item off the stack
    W4dEntity& top = *(stack_.back());
    stack_.pop_back();

    //Check for top item being a composite
    if( top.isComposite() )
    {
        W4dComposite& topComposite = top.asComposite();

        //Push all its links onto the stack
        const W4dComposite::W4dLinks& links = topComposite.links();
        for( W4dComposite::W4dLinks::const_iterator it = links.begin();
             it != links.end(); ++it )
        {
            stack_.push_back( *it );
        }
    }

    //Push all its children on the stack
    const W4dEntity::W4dEntities& children = top.children();
    for( W4dEntity::W4dEntities::const_iterator it = children.begin();
         it != children.end(); ++it )
    {
        stack_.push_back( *it );
    }

    return *this;
}

W4dEntity& W4dEntityIter::operator* ()
{
    PRE( not isFinished() );
    return *(stack_.back());
}

/* End ENTYITER.CPP *************************************************/
