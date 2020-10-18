/*
 * M V E C E C H A . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dEntityMaterialVecChanger

    Stores a vector of counted pointers to RenMaterialVecs.
    The idea is that it can be used to change the material vec overrides
    for each level of detail of a particular set of entities.
    It is a client responsibility that any given entity to which the overrides
    are applied is appropriate.
    This is useful, for example, to change the colours of instances of
    a particular subclass of W4dEntity.
*/

#ifndef _WORLD4D_MVECECHA_HPP
#define _WORLD4D_MVECECHA_HPP

#include "base/base.hpp"
#include "world4d/world4d.hpp"
#include "world4d/fwmateri.hpp"

//forward refs
class W4dEntity;

//Canonical form revoked
class W4dEntityMaterialVecChanger
{
public:
    //ctor. Sets up the material overrides for patternEntity, for each lod which has
    //materials in the map. The material overrides will replace each such material
    //with the corresponding map entry.
    //Any material vectors resident in pMaterialVecPtrSet are to be reused. New ones
    //are to be added.
    W4dEntityMaterialVecChanger( const W4dEntity& patternEntity, const RenMaterialMap& map,
                                 W4dMaterialVecPtrSet* pMaterialVecPtrSet );

    //dtor.
    ~W4dEntityMaterialVecChanger();

    //True iff any overrides were created in the constructor
    bool hasAnyOverrides() const;

    //Apply the stored overrides to the LOD mesh instances of pEntity
    //The client must ensure that each LOD of pEntity has the same arity material
    //vector as the equivalent LOD in the constructor patternEntity.
    //This should be the case of pEntity is a copy of patternEntity.
    bool applyOverrides( W4dEntity *pEntity ) const;

    //True iff an override is defined for level of detail lodId
    bool hasOverride( W4dLOD lodId ) const;

    //The override for level of detail lodId
    const Ren::MaterialVecPtr& override( W4dLOD lodId ) const;
    //PRE( hasOverride( lodId ) );

    //true if hue1 and hue2 are within 10 degrees on the HSV scale
    static bool hueClose( double hue1, double hue2 );
    
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const W4dEntityMaterialVecChanger& t );

private:
    W4dEntityMaterialVecChanger( const W4dEntityMaterialVecChanger& );
    W4dEntityMaterialVecChanger& operator =( const W4dEntityMaterialVecChanger& );
    bool operator ==( const W4dEntityMaterialVecChanger& );

    //data members
    W4dMaterialVecPtrs* pMaterialVecPtrs_; //Pointer to vector of counted pointers to
                                           //material vectors. Using pointer to prevent
                                           //dreadful hpp include set.
};


#endif

/* End MVECECHA.HPP *************************************************/
