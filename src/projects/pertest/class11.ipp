/*
 * C L A S S 1 1 . I P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

// _CODE_INLINE
// void Class11::inlineFunction()
// {
// }

_CODE_INLINE
void perWrite( PerOstream& ostr, const Class11& c )
{
    ostr << c.i_;
}

_CODE_INLINE
void perRead( PerIstream& istr, Class11& c )
{
    istr >> c.i_;
}

/* End CLASS11.IPP **************************************************/
