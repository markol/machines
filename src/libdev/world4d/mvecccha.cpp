/*
 * M V E C C C H A . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "ctl/countptr.hpp"
#include "ctl/pvector.hpp"

#include "render/matmap.hpp"
#include "render/hsv.hpp"
#include "render/colour.hpp"

#include "world4d/mveccple.hpp"
#include "world4d/mvececha.hpp"
#include "world4d/mvecccha.hpp"
#include "world4d/composit.hpp"
#include "world4d/link.hpp"

W4dCompositeMaterialVecChanger::W4dCompositeMaterialVecChanger
(
    const W4dComposite& patternComposite,
    const RenMaterialMap& map,
    W4dMaterialVecPtrSet* pMaterialVecPtrSet
)
:   pCompositeChanger_( NULL ),
    pLinkIds_( NULL ),
    pLinkChangers_( NULL )
{
    //Attempt to create a changer for the composite itself.
    //If no overrides, delete it.
    pCompositeChanger_ = _NEW( W4dEntityMaterialVecChanger( patternComposite, map, pMaterialVecPtrSet ) );
    if( not pCompositeChanger_->hasAnyOverrides() )
    {
        _DELETE( pCompositeChanger_ );
        pCompositeChanger_ = NULL;
    }

    //Now do the same for each link. Keep a local collection of id/changer pairs,
    //and then copy them so we ensure we get the reserve size exact, which we can't
    //know in advance.
    const W4dComposite::W4dLinks& links = patternComposite.links();

    ctl_vector< W4dLinkId > linkIds;
    linkIds.reserve( links.size() );

    ctl_pvector< W4dEntityMaterialVecChanger > changers;
    changers.reserve( links.size() );

    for( W4dComposite::W4dLinks::const_iterator it = links.begin(); it != links.end(); ++it )
    {
        W4dLink* pLink = *it;

        //Create a changer for this link
        W4dEntityMaterialVecChanger* pChanger =
            _NEW( W4dEntityMaterialVecChanger( *pLink, map, pMaterialVecPtrSet ) );

        if( pChanger->hasAnyOverrides() )
        {
            //Add to our list
            linkIds.push_back( pLink->id() );
            changers.push_back( pChanger );
        }
        else
        {
            _DELETE( pChanger );
        }
    }

    //If we have any link changers set up the permanent vectors
    uint nLinkChangers = changers.size();
    if( nLinkChangers != 0 )
    {
        //Allocate the vectors and reserve exact space
        pLinkIds_ = _NEW( ctl_vector< W4dLinkId >() );
        pLinkIds_->reserve( nLinkChangers );

        pLinkChangers_ = _NEW( ctl_pvector< W4dEntityMaterialVecChanger >() );
        pLinkChangers_->reserve( nLinkChangers );

        //Copy the data element by element to ensure keep exact allocation size
        for( uint i = 0; i != nLinkChangers; ++i )
        {
            pLinkIds_->push_back( linkIds[i] );
            pLinkChangers_->push_back( changers[i] );
        }
    }

    TEST_INVARIANT;
}

W4dCompositeMaterialVecChanger::~W4dCompositeMaterialVecChanger()
{
    TEST_INVARIANT;

    _DELETE( pCompositeChanger_ );
    _DELETE( pLinkIds_ );

    if( pLinkChangers_ )
    {
        for( size_t i = 0; i < pLinkChangers_->size(); ++i )
        {
            _DELETE( (*pLinkChangers_)[ i ] );
        }
    }

    _DELETE( pLinkChangers_ );
}

void W4dCompositeMaterialVecChanger::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const W4dCompositeMaterialVecChanger& t )
{

    o << "W4dCompositeMaterialVecChanger " << (void*)&t << " start" << std::endl;
    o << "W4dCompositeMaterialVecChanger " << (void*)&t << " end" << std::endl;

    return o;
}

bool W4dCompositeMaterialVecChanger::hasAnyOverrides() const
{
    return pCompositeChanger_ != NULL  or  pLinkChangers_ != NULL ;
}

bool W4dCompositeMaterialVecChanger::applyOverrides( W4dComposite *pComposite ) const
{
    bool anyApplied = false;

    //Do the composite itself if defined
    if( pCompositeChanger_ )
    {
        pCompositeChanger_->applyOverrides( pComposite );
        anyApplied = true;
    }

    //Do the links
    if( pLinkChangers_ )
    {
        const W4dComposite::W4dLinks& links = pComposite->links();
        uint nLinks = links.size();
        uint nChangers = pLinkChangers_->size();

        for( size_t i = 0; i != nChangers; ++i )
        {
            //Check a link with id is present
            W4dLinkId id = (*pLinkIds_)[i];
            if( id < nLinks )
            {
                //Apply the overrides to the link
                (*pLinkChangers_)[i]->applyOverrides( links[id] );
                anyApplied = true;
            }
        }
    }

    return anyApplied;
}

bool W4dCompositeMaterialVecChanger::hasCompositeChanger() const
{
    return pCompositeChanger_ != NULL;
}

const W4dEntityMaterialVecChanger& W4dCompositeMaterialVecChanger::compositeChanger() const
{
    PRE( hasCompositeChanger() );
    return *pCompositeChanger_;
}

uint W4dCompositeMaterialVecChanger::nLinkChangers() const
{
    return (pLinkChangers_ ? pLinkChangers_->size() : 0);
}

W4dLinkId W4dCompositeMaterialVecChanger::linkId( size_t index ) const
{
    PRE( index < nLinkChangers() );

    return (*pLinkIds_)[index];
}

const W4dEntityMaterialVecChanger& W4dCompositeMaterialVecChanger::linkChanger
(
    size_t index
) const
{
    PRE( index < nLinkChangers() );

    return *((*pLinkChangers_)[index]);
}

//static
void W4dCompositeMaterialVecChanger::fillMaterialMaps
(
    const W4dComposite& composite, double fromHue, const ctl_vector< double >& toHues,
		const Textures& fromTextures, const TexturesVec& toTexturesVec,
    const ctl_pvector< RenMaterialMap >& maps
)
{
    PRE( toHues.size() == maps.size() );
    PRE( fromTextures.size() != 0 );
    PRE( toTexturesVec.size() != 0 );
    PRE( toTexturesVec[0].size() == maps.size() );

    //Construct a set of the materials used by the model
    RenMaterialSet materialSet;
    composite.addMaterials( &materialSet );

    //Iterate through this set of materials, adding map entries where appropriate
    uint nMaps = maps.size();
    for( RenMaterialSet::iterator it = materialSet.begin(); it != materialSet.end(); ++it )
    {
        //Check the hue of this material is defined and is close to red
        RenMaterial compositeMaterial = *it;

        RenColourHSV hsvColour( compositeMaterial.diffuse() );
        RenColourHSV hsvColourEmissive( compositeMaterial.emissive() );

		bool texTureMatch = false;
		uint idx;
		for(uint i=0; i<fromTextures.size(); ++i)
		{
			if( compositeMaterial.texture() == fromTextures[i] )
			{
				texTureMatch = true;
				idx = i;
			}
		}

		if( texTureMatch )
		{

            //This material needs conversion. Add a map entry to each map
            for( uint j = 0; j != nMaps; ++j )
            {
                //convert the material	texture
                RenMaterial entryMaterial( compositeMaterial );
                entryMaterial.texture( (toTexturesVec[idx])[j] );

                //Add the map entry
                maps[j]->insert( compositeMaterial, entryMaterial );
            }
		}

        else if( hsvColour.hueDefined() )
        {
            double compositeHue = hsvColour.hue();

            if( W4dEntityMaterialVecChanger::hueClose( compositeHue, fromHue ) )
            {
                //This material needs conversion. Add a map entry to each map
                for( uint j = 0; j != nMaps; ++j )
                {
                    //Convert the HSV colour
                    hsvColour.hue( compositeHue - fromHue + toHues[j] );

                    //Hence convert the material
                    RenMaterial entryMaterial( compositeMaterial );
                    entryMaterial.diffuse( hsvColour.rgb() );

                    //Add the map entry
                    maps[j]->insert( compositeMaterial, entryMaterial );
                }
            }
        }

        else if( hsvColourEmissive.hueDefined() )
        {
            double compositeHue = hsvColourEmissive.hue();

            if( W4dEntityMaterialVecChanger::hueClose( compositeHue, fromHue ) )
            {
                //This material needs conversion. Add a map entry to each map
                for( uint j = 0; j != nMaps; ++j )
                {
                    //Convert the HSV colour
                    hsvColourEmissive.hue( compositeHue - fromHue + toHues[j] );

                    //Hence convert the material
                    RenMaterial entryMaterial( compositeMaterial );
                    entryMaterial.emissive( hsvColourEmissive.rgb() );

                    //Add the map entry
                    maps[j]->insert( compositeMaterial, entryMaterial );
                }
            }
        }
    }
}

bool W4dCompositeMaterialVecChanger::hasLinkChanger( W4dLinkId linkId ) const
{
    ctl_vector< W4dLinkId >::const_iterator it = pLinkIds_->begin();
    while( it != pLinkIds_->end() and (*it) != linkId )
        ++it;

    return it != pLinkIds_->end();
}

const W4dEntityMaterialVecChanger& W4dCompositeMaterialVecChanger::linkIdChanger
(
    W4dLinkId linkId
) const
{
    PRE( hasLinkChanger( linkId ) );
    uint nChangers = pLinkIds_->size();

    W4dEntityMaterialVecChanger* pChanger = NULL;

    for( uint i = 0; pChanger == NULL and i != nChangers; ++i )
    {
        if( (*pLinkIds_)[i] == linkId )
            pChanger = (*pLinkChangers_)[i];
    }

    POST( pChanger != NULL );
    return *pChanger;
}
/* End MVECCCHA.CPP *************************************************/
