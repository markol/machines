/*
 * E N T S C A L E . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dEntityScale

    Holds the information related to any scale override associated with a W4dEntity.
*/

#ifndef _WORLD4D_ENTSCALE_HPP
#define _WORLD4D_ENTSCALE_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

//Forward refs
class MexPoint3d;
class RenScale;
class RenUniformScale;
class RenNonUniformScale;

//orthodox canonical (revoked)
class W4dEntityScale
{
public:
    //ctor.
    W4dEntityScale();

    //dtor
    ~W4dEntityScale();

    //True iff a local mesh scale attribute is defined
    bool hasMeshScale() const;

    //The current mesh scale attribute
    const RenScale& meshScale() const;
    //PRE( hasMeshScale() )

    //Set current scale attribute
    void meshScale( const RenScale& scale );
    void meshScale( const RenUniformScale& scale );
    void meshScale( const RenNonUniformScale& scale );

    //Set/get the scale transform parent flag
    void scaleTransformFromParent( bool doScale );
    bool scaleTransformFromParent() const;

    //Store the unscaled position
    void unscaledTransformPosition( const MexPoint3d& p );
    //PRE( scaleTransformFromParent() )

    //The unscaled position
    const MexPoint3d& unscaledTransformPosition() const;
    //PRE( scaleTransformFromParent() )

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const W4dEntityScale& t );

    PER_MEMBER_PERSISTENT_DEFAULT( W4dEntityScale );
    PER_FRIEND_READ_WRITE( W4dEntityScale );
    
private:
    // Operations deliberately revoked
    W4dEntityScale( const W4dEntityScale& );
    W4dEntityScale& operator =( const W4dEntityScale& );
    bool operator ==( const W4dEntityScale& );

    //data members
    MexPoint3d* pUnscaledTransformPosition_; //The translation component of the entity's
                                             //transform before applying parent's scale.
                                             //Presence of this object iff applying parent scale.
    RenScale* pScale_; //The entity's mesh scaling attribute.
                       //Exists iff scaling attribute is not uniform 1.
};

PER_DECLARE_PERSISTENT( W4dEntityScale );

#ifdef _INLINE
    #include "world4d/entscale.ipp"
#endif


#endif

/* End ENTSCALE.HPP *************************************************/
