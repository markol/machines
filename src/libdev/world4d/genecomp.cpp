/*
 * G E N E C O M P . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/genecomp.hpp"

//////////////////////////////////////////////////////////////////////////////////////////

PER_DEFINE_PERSISTENT( W4dGenericComposite );

W4dGenericComposite::W4dGenericComposite( W4dEntity* pParent,
                                          const W4dTransform3d& localTransform,
                                          const SysPathName& pathname,
                                          Solidity solidity )

:   W4dComposite( pParent, localTransform, solidity )
{
    //Checks
    PRE( pParent != NULL )

    readCompositeFile( pathname );

    TEST_INVARIANT;
}

W4dGenericComposite::W4dGenericComposite( const W4dGenericComposite& copyMe,
    W4dEntity* pParent, const W4dTransform3d& localTransform )
: W4dComposite( copyMe, pParent, localTransform )
{
}


//////////////////////////////////////////////////////////////////////////////////////////

W4dGenericComposite::~W4dGenericComposite()
{
    TEST_INVARIANT;

}
//////////////////////////////////////////////////////////////////////////////////////////

void W4dGenericComposite::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}
//////////////////////////////////////////////////////////////////////////////////////////

//virtual
bool W4dGenericComposite::intersectsLine
(
    const MexLine3d& line, MATHEX_SCALAR* pDistance, Accuracy accuracy
) const
{
    //Use the default
    return defaultCompositeIntersectsLine( line, pDistance, accuracy );
}
//////////////////////////////////////////////////////////////////////////////////////////


W4dGenericComposite::W4dGenericComposite( const W4dComposite& copyMe,
    W4dEntity* pParent, const W4dTransform3d& localTransform )
: W4dComposite( copyMe, pParent, localTransform )
{
}

void perWrite( PerOstream& ostr, const W4dGenericComposite& generic )
{
    const W4dComposite& base = generic;

    ostr << base;
}

void perRead( PerIstream& istr, W4dGenericComposite& generic )
{
    W4dComposite& base = generic;

    istr >> base;
}

W4dGenericComposite::W4dGenericComposite( PerConstructor con )
: W4dComposite( con )
{
}

/* End GENECOMP.CPP *************************************************/
