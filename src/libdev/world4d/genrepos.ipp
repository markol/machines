/*
 * G E N R E P O S . I P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif
/*
_CODE_INLINE
void W4dGenericRepository::add( const string& key, W4dGeneric* pGeneric )
{
	pGeneric->attachTo(pRoot_);	
	simpleEntries_.push_back(SimpleEntry(key, pGeneric));
}

_CODE_INLINE
void W4dGenericRepository::add( const string& key, W4dGenericComposite* pGeneric )
{	
	pGeneric->attachTo(pRoot_);	
	compositeEntries_.push_back(CompositeEntry(key, pGeneric));
}
*/
/* End GENREPOS.IPP *************************************************/
