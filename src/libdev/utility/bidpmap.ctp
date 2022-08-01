/*
 * B I D P M A P . C T P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline template methods

#ifndef _INLINE
    #include "utility/bidpmap.itp"
#endif

template< class X >
UtlBoundedIdPMap< X >::UtlBoundedIdPMap( UtlId upperBound )
:   UtlBoundedIdVoidPMap( upperBound )
{

    TEST_INVARIANT;
}

template< class X >
UtlBoundedIdPMap< X >::~UtlBoundedIdPMap()
{
    TEST_INVARIANT;
    //Intentionally empty
}

template< class X >
void UtlBoundedIdPMap< X >::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

template< class X >
ostream& operator <<( ostream& o, const UtlBoundedIdPMap< X >& t )
{

    o << "UtlBoundedIdPMap< X > " << (void*)&t << " start" << std::endl;
    o << "UtlBoundedIdPMap< X > " << (void*)&t << " end" << std::endl;

    return o;
}

/* End BIDPMAP.CTP **************************************************/
