/*
 * M V E C E C H A . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "ctl/countptr.hpp"
#include "world4d/mvececha.hpp"
#include "world4d/mveccple.hpp"

#include "render/matmap.hpp"
#include "render/mesh.hpp"
#include "render/meshinst.hpp"
#include "world4d/entity.hpp"

#include "ctl/vector.hpp"

W4dEntityMaterialVecChanger::W4dEntityMaterialVecChanger
(
    const W4dEntity& patternEntity, const RenMaterialMap& map,
    W4dMaterialVecPtrSet* pMaterialVecPtrSet
)
:   pMaterialVecPtrs_( NULL )
{
    //Keep track of whether any overrides are defined
    bool anyOverrides = false;

    //Get the number of LOD meshes the pattern entity has
    uint nMeshes = patternEntity.nLODs();
    if( nMeshes != 0 )
    {
        //Construct a vector with enough space for the required overrides
        pMaterialVecPtrs_ = _NEW( W4dMaterialVecPtrs() );
        pMaterialVecPtrs_->reserve( nMeshes );

        //Deal with each lod
        for( uint lodId = 0; lodId != nMeshes ; ++lodId )
        {
            //Check a mesh is defined at this LOD
            Ren::MaterialVecPtr materialVecPtr;

            if( patternEntity.hasMesh( lodId ) )
            {
                //Get the mesh
                const RenMesh& mesh = *(patternEntity.mesh( lodId ).mesh());

                //Get the pattern material vector for the mesh
                std::auto_ptr<RenMaterialVec> patternVecAPtr = mesh.materialVec();
                RenMaterialVec& materialVec = *patternVecAPtr;

                //replace any materials as defined by the map, keeping track if any changes
                bool usedMappedSet = false;

                size_t nMaterials = materialVec.size();
                for( size_t i = 0; i != nMaterials; ++i )
                {
                    //  If the material is in our list of override materials
                    //  set up the mapping to something new
                    const RenMaterialMap::const_iterator itEntry = map.find( materialVec[ i ] );
                    if( itEntry != map.end() )
                    {
                        materialVec[ i ] = (*itEntry).second;
                        usedMappedSet = true;
                    }
                }

                //If we defined an override...
                if( usedMappedSet )
                {
                    //Set flag so we know at least one mesh has an override
                    anyOverrides = true;

                    //Store the material vector using a counted ptr
                    Ren::MaterialVecPtr tempMaterialVecPtr = patternVecAPtr.release();

                    //Attempt to add this material vector to the set, getting back an
                    //iterator for the insertion. If the material already exists, this
                    //will reference the existing entry. Use the iterator directly to
                    //get the actual material vec counetd pointer to use.
                    materialVecPtr = *(pMaterialVecPtrSet->insert( tempMaterialVecPtr ).first);
                }
            }

            //Add the (possibly null) override
            pMaterialVecPtrs_->push_back( materialVecPtr );
        }
    }

    //If no overrides were defined, delete the vector
    if( not anyOverrides )
    {
        _DELETE( pMaterialVecPtrs_ );
        pMaterialVecPtrs_ = NULL;
    }

    TEST_INVARIANT;
}

W4dEntityMaterialVecChanger::~W4dEntityMaterialVecChanger()
{
    TEST_INVARIANT;
    _DELETE( pMaterialVecPtrs_ );
}

void W4dEntityMaterialVecChanger::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const W4dEntityMaterialVecChanger& t )
{

    o << "W4dEntityMaterialVecChanger " << (void*)&t << " start" << std::endl;
    o << "W4dEntityMaterialVecChanger " << (void*)&t << " end" << std::endl;

    return o;
}

bool W4dEntityMaterialVecChanger::hasAnyOverrides() const
{
    return pMaterialVecPtrs_ != NULL;
}

bool W4dEntityMaterialVecChanger::applyOverrides( W4dEntity *pEntity ) const
{
    bool appliedAny = false;

    //Check we have some
    if( pMaterialVecPtrs_ != NULL )
    {
        //Apply override for each lod mesh
        const W4dMaterialVecPtrs& materialVecPtrs = *pMaterialVecPtrs_;
        uint nOverrides = materialVecPtrs.size();
        for( uint lodId = 0; lodId != nOverrides; ++lodId )
        {
            //Check we have an override at this level, and a mesh
            const Ren::MaterialVecPtr& materialVecPtr = materialVecPtrs[lodId];
            if( materialVecPtr.isDefined() and pEntity->hasMesh( lodId ) )
            {
                //Get the mesh instance
                RenMeshInstance& meshInstance = pEntity->mesh( lodId );
                ASSERT( meshInstance.mesh()->nMaterials() <= materialVecPtr->size(), "" );

                //Apply the override
                meshInstance.materialVec( materialVecPtr );
                appliedAny = true;
            }
        }
    }

    return appliedAny;
}

bool W4dEntityMaterialVecChanger::hasOverride( W4dLOD lodId ) const
{
    return pMaterialVecPtrs_ != NULL  and
           lodId < pMaterialVecPtrs_->size()  and
           (*pMaterialVecPtrs_)[ lodId ].isDefined();
}

const Ren::MaterialVecPtr& W4dEntityMaterialVecChanger::override( W4dLOD lodId ) const
{
    PRE( hasOverride( lodId ) );
    return (*pMaterialVecPtrs_)[ lodId ];
}

//static
bool W4dEntityMaterialVecChanger::hueClose( double hue1, double hue2 )
{
    const double epsilon = 10.0;

    bool    result;

    //Return true if hue1 is within epsilon of hue2
    double diff = hue2 - hue1;

    if( diff < 0.0 )
        diff = -diff;

    while( diff >= 360.0 )
        diff -= 360.0;

    result = diff <= epsilon  or  diff >= (360.0 - epsilon);

    return result;
}

/* End MVECECHA.CPP *************************************************/
