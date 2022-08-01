/*
 * S E R I P M A P . C T P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline template methods

#ifndef _INLINE
    #include "ftl/seripmap.itp"
#endif

//////////////////////////////////////////////////////////////////////////////////////////
template< class REF >
//virtual 
void FtlSerialPMap< REF >::add( const FtlPair< FtlSerialId, Ref* >& addMe )
{
    PRE( not contains( addMe.first ) );

    //get index and current highest entry + 1
    size_t index = addMe.first.asScalar();
    size_t n = refs_.size();

    //Add the pointer to the vector
    if( index < n )
        refs_[index] = addMe.second;
    else
    {
        //If necessary, push some nulls into the vector
        n = index - n;
        while( n-- != 0 )
            refs_.push_back( NULL );

        refs_.push_back( addMe.second );
    }
}
//////////////////////////////////////////////////////////////////////////////////////////
template< class X >
void FtlSerialPMap< X >::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}
//////////////////////////////////////////////////////////////////////////////////////////
/* End SERIPMAP.CTP *************************************************/
