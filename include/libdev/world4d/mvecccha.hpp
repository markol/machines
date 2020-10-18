/*
 * M V E C C C H A . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dCompositeMaterialVecChanger

    Stores a set of W4dEntityMaterialVecChangers, for the composite itself and some of
    its links. See that class for details.
*/

#ifndef _WORLD4D_MVECCCHA_HPP
#define _WORLD4D_MVECCCHA_HPP

#include "base/base.hpp"
#include "world4d/world4d.hpp"
#include "world4d/fwmateri.hpp"
#include "render/texture.hpp"
#include "ctl/vector.hpp"

//forward refs
class W4dComposite;
class W4dEntityMaterialVecChanger;

template< class T > class ctl_vector;
template< class T > class ctl_pvector;

class W4dCompositeMaterialVecChanger
// Canonical form revoked
{
public:
    //ctor. Sets up the material overrides for patternComposite, for each lod which has
    //materials in the map. The material overrides will replace each such material
    //with the corresponding map entry.
    //Any material vectors resident in pMaterialVecPtrSet are to be reused. New ones
    //are to be added
    W4dCompositeMaterialVecChanger( const W4dComposite& patternComposite,
                                    const RenMaterialMap& map,
                                    W4dMaterialVecPtrSet* pMaterialVecPtrSet );

    //dtor    
    ~W4dCompositeMaterialVecChanger();

    //True iff any overrides were created in the constructor
    bool hasAnyOverrides() const;

    //Apply the stored overrides to the LOD mesh instances of pComposite.
    //The client must ensure that each LOD of link and the composite has the same arity material
    //vector as the equivalent LOD in the constructor patternComposite.
    //This should be the case if pComposite is a copy of patternComposite.
    bool applyOverrides( W4dComposite *pComposite ) const;

    //True iff a changer is defined for the composite itself
    bool hasCompositeChanger() const;

    //The compsite's changer
    const W4dEntityMaterialVecChanger& compositeChanger() const;
    //PRE( hasCompositeChanger() );

    //The number of changers defined for links
    uint nLinkChangers() const;

    //True if has a changer defined for the link with id linkId
    bool hasLinkChanger( W4dLinkId linkId ) const;

    //The entity changer defined for link with id linkId
    const W4dEntityMaterialVecChanger& linkIdChanger( W4dLinkId linkId ) const;
    //PRE( hasLinkChanger( linkId ) );

    //The link id for the index'th defined link changer
    W4dLinkId linkId( size_t index ) const;
    //PRE( index < nLinkChangers() );

    //The index'th defined link changer
    const W4dEntityMaterialVecChanger& linkChanger( size_t index ) const;
    //PRE( index < nLinkChangers() );

    //Plave an entry in each of the material maps referenced by maps for each
    //material used by composite whose diffuse colour hue is close to fromHue/whose texture == from texture
    //Each entry maps from that material to a material with diffuse colour hue changed
    //to correspond to the entry in toHues/a corresponding texture in toTextures.
	typedef  ctl_vector< RenTexture > Textures;
	typedef  ctl_vector< Textures > TexturesVec;

    static void fillMaterialMaps
    (
        const W4dComposite& composite, double fromHue, const ctl_vector< double >& toHues,
		const Textures& fromTextures, const TexturesVec& toTexturesVec,
        const ctl_pvector< RenMaterialMap >& maps
    );
    //PRE( toHues.size() == maps.size() );
    //PRE( toTexture.size() == maps.size() );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const W4dCompositeMaterialVecChanger& t );

private:
    W4dCompositeMaterialVecChanger( const W4dCompositeMaterialVecChanger& );
    W4dCompositeMaterialVecChanger& operator =( const W4dCompositeMaterialVecChanger& );
    bool operator ==( const W4dCompositeMaterialVecChanger& );

    //data members
    W4dEntityMaterialVecChanger* pCompositeChanger_; //The one for the composite itself if defined
    ctl_vector< W4dLinkId >* pLinkIds_; //the id of a link to be overridden
    ctl_pvector< W4dEntityMaterialVecChanger >* pLinkChangers_; //Corresponding changers for that link
};


#endif

/* End MVECCCHA.HPP *************************************************/
