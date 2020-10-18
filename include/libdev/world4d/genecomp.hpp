/*
 * G E N E C O M P . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    W4dGenericComposite

    Generic class derived from W4dComposite
*/

#ifndef _GENECOMP_HPP
#define _GENECOMP_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "world4d/composit.hpp"

//forward refs
class W4dEntity;
class SysPathName;

//orthodox canonical (revoked)
class W4dGenericComposite : public W4dComposite
{
public:
    //Construct the composite using the cdf file pathname.
    //Attach it to pParent at position localTransform
    W4dGenericComposite( W4dEntity* pParent, const W4dTransform3d& localTransform,
                         const SysPathName& pathname, Solidity solidity = SOLID );

    //copy ctor.
    W4dGenericComposite( const W4dGenericComposite& copyMe,
        W4dEntity* pParent, const W4dTransform3d& localTransform );

    //Special copy ctor. copyMe may be any derived class of W4dComposite.
    //However, only the composite level information will be copied, so the
    //new object will be a generic composite, not any previous subclass.
    W4dGenericComposite( const W4dComposite& copyMe,
        W4dEntity* pParent, const W4dTransform3d& localTransform );

    //dtor
    ~W4dGenericComposite( void );

    //Inherited from W4dEntity
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                             Accuracy accuracy ) const;

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_VIRTUAL( W4dGenericComposite );
    PER_FRIEND_READ_WRITE( W4dGenericComposite );

private:
    // Operations deliberately revoked
    W4dGenericComposite( const W4dGenericComposite& );
    W4dGenericComposite& operator =( const W4dGenericComposite& );
    bool operator ==( const W4dGenericComposite& );

};

PER_DECLARE_PERSISTENT( W4dGenericComposite );

#endif

/* End GENECOMP.HPP *************************************************/
