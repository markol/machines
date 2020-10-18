/*
 * S U B I D . C T P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline template methods

template< class X >
MachPhysSubTypeId< X >::MachPhysSubTypeId()
{

    TEST_INVARIANT;
}

template< class X >
MachPhysSubTypeId< X >::~MachPhysSubTypeId()
{
    TEST_INVARIANT;

}

template< class X >
void MachPhysSubTypeId< X >::CLASS_INVARIANT
{
}

template< class X >
ostream& operator <<( ostream& o, const MachPhysSubTypeId< X >& t )
{

    o << "MachPhysSubTypeId< X > " << (void*)&t << " start" << endl;
    o << "MachPhysSubTypeId< X > " << (void*)&t << " end" << endl;

    return o;
}

/* End SUBID.CTP ****************************************************/
