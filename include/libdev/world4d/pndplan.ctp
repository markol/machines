/*
 * P N D P L A N . C T P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline template methods

template< class X >
PLAN_TYPE< X >::PLAN_TYPE()
{

    TEST_INVARIANT;
}

template< class X >
PLAN_TYPE< X >::~PLAN_TYPE()
{
    TEST_INVARIANT;

}

template< class X >
void PLAN_TYPE< X >::CLASS_INVARIANT
{
}

ostream& operator <<( ostream& o, const PLAN_TYPE< X >& t )
{

    o << "PLAN_TYPE< X > " << (void*)&t << " start" << endl;
    o << "PLAN_TYPE< X > " << (void*)&t << " end" << endl;

    return o;
}

/* End PNDPLAN.CTP **************************************************/
