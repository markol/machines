/*
 * M E S H I N S T . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */


#include "render/meshinst.hpp"

#include "mathex/point3d.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/epsilon.hpp"

#include "render/mesh.hpp"
#include "render/matvec.hpp"
#include "render/matmap.hpp"
#include "render/uvanim.hpp"
#include "render/scale.hpp"

PER_DEFINE_PERSISTENT( RenMeshInstance );

RenMeshInstance::RenMeshInstance(const Ren::ConstMeshPtr& mesh)
: pMesh_( mesh ),
  scale_( unityScale() )
{
	PRE(Ren::initialised());
	PRE(mesh.isDefined());
}

RenMeshInstance::RenMeshInstance( const RenMeshInstance& copyMe )
: pMesh_( copyMe.pMesh_ ),
  materialVecPtr_( copyMe.materialVecPtr_ ),
  texAnimationPtr_( copyMe.texAnimationPtr_ )
{
	// There's no point in copying the untiyScale object; just copy the
	// pointer.  However, other parts of the code *must not* delete it.
	if (copyMe.scale_ == unityScale())
		scale_ = unityScale();
	else
		scale_ = copyMe.scale_->clone();
}

RenMeshInstance::~RenMeshInstance()
{
    TEST_INVARIANT;

	// Do not delete the default scale because it's not allocated with new.
	if (scale_ != unityScale())
		_DELETE(scale_);
}

// static
RenMeshInstance* RenMeshInstance::createShared
(
	const SysPathName& path,
	const std::string& mesh,
	double scale
)
{
	Ren::ConstMeshPtr ptr = RenMesh::createShared(path, mesh);

    RenMeshInstance* pInstance;

	if (ptr.isDefined())
    {
        pInstance = _NEW( RenMeshInstance( ptr ) );

        //Use the unity scale if scale is 1
        if( Mathex::abs( scale - 1.0 ) < MexEpsilon::instance() )
            pInstance->scale(*unityScale());
        else
            pInstance->scale(RenUniformScale(scale));
    }
	else
        pInstance = NULL;

    return pInstance;
}

Ren::ConstMeshPtr RenMeshInstance::mesh() const
{
    return pMesh_;
}

void RenMeshInstance::render( const MexTransform3d& transform ) const
{
    RenMaterialVec* pMaterialVec = (materialVecPtr_.isDefined() ? &(*materialVecPtr_) : NULL);
    RenUVTransform* pTexAnimation = (texAnimationPtr_.isDefined() ? &(*texAnimationPtr_) : NULL );

	if (pTexAnimation)
	{
	    if( pMaterialVec )
	    {
    	    pMesh_->render( transform, pMaterialVec, *pTexAnimation, *scale_ );
	    }
	    else
	    {
    	    pMesh_->render( transform, *pTexAnimation, *scale_ );
	    }
	}
	else
	{
	    if( pMaterialVec )
	    {
    	    pMesh_->render( transform, pMaterialVec, *scale_ );
	    }
	    else
	    {
    	    pMesh_->render( transform, *scale_ );
	    }
	}
}

void    RenMeshInstance::addMaterials( RenMaterialSet* pSet ) const
{
    PRE( pSet != NULL );

    std::auto_ptr<RenMaterialVec> materialSetAPtr (pMesh_->materialVec());

    for( RenMaterialVec::iterator i = materialSetAPtr->begin();
      i != materialSetAPtr->end(); ++i )
    {
        pSet->insert( *i );
    }
}

void    RenMeshInstance::materialMap( const RenMaterialMap& map )
{
    //Clear any existing override
    if( materialVecPtr_.isDefined() )
        materialVecPtr_.clear();

    //  get the materials currently in use
    std::auto_ptr<RenMaterialVec> materialSetAPtr (pMesh_->materialVec());
    RenMaterialVec& materialVec = *materialSetAPtr;

    bool    useMappedSet = false;

    size_t nMaterials = materialVec.size();
    for( size_t i = 0; i != nMaterials; ++i )
    {
        //  If the material is in our list of override materials
        //  set up the mapping to something new
        const RenMaterial& oldMaterial = materialVec[ i ];
        const RenMaterialMap::const_iterator itEntry = map.find( oldMaterial );
        if( itEntry != map.end() )
        {
            materialVec[ i ] = (*itEntry).second;
            useMappedSet = true;
        }
    }

    //If we defined an override, save the material vector using our counted ptr
    if( useMappedSet )
    {
        materialVecPtr_ = Ren::MaterialVecPtr( materialSetAPtr.release() );
    }
}

const MexAlignedBox3d& RenMeshInstance::boundingVolume() const
{
    return pMesh_->boundingVolume();
}

const RenScale& RenMeshInstance::scale() const
{
    return *scale_;
}

void RenMeshInstance::scale( const RenScale& newScale )
{
	// Do not delete the default scale because it's not allocated with new.
    RenScale* pUnityScale = unityScale();
	if (scale_ != pUnityScale)
		_DELETE(scale_);

	scale_ = (&newScale == pUnityScale ? pUnityScale : newScale.clone());
}

void RenMeshInstance::scale(MATHEX_SCALAR uniform)
{
	// Do not delete the default scale because it's not allocated with new.
	if (scale_ != unityScale())
		_DELETE(scale_);

	scale_ = _NEW(RenUniformScale(uniform));
}

// This is the default value for scale_, if a client doesn't set it.
// static
RenScale* RenMeshInstance::unityScale()
{
	static RenUnityScale unity;
	return &unity;
}

void RenMeshInstance::animateTextureCoords(const Ren::UVTransformPtr& animPtr)
{
    texAnimationPtr_ = animPtr;
}

void RenMeshInstance::CLASS_INVARIANT
{
	INVARIANT(pMesh_.isDefined());
}

void RenMeshInstance::materialVec( const Ren::MaterialVecPtr& materialVecPtr )
{
    materialVecPtr_ = materialVecPtr;
}

void RenMeshInstance::render( const MexTransform3d& transform, const RenScale& extraScale ) const
{
	//Cache the existing RenScale pointer, since we intend to temporarily
    //replace it with one allocated on the stack in this function
    RenScale& baseScale = *scale_;
    RenMeshInstance* nonConstThis = _CONST_CAST( RenMeshInstance*, this );

    //Check various combinations of base and extra scale types
    if( baseScale.isUnity() )
    {
        //No base scale, so just use the extra one
        nonConstThis->scale_ = _CONST_CAST( RenScale*, &extraScale );
        render( transform );
    }
    else if( extraScale.isUniform() )
    {
        if( baseScale.isUniform() )
        {
            //Two uniform scales. Construct one that combines the 2
            RenUniformScale temp( baseScale.asUniform(), extraScale.asUniform() );
            nonConstThis->scale_ = &temp;
            render( transform );
        }
        else
        {
            ASSERT( baseScale.isNonUniform(), "Invalid RenScale" );
            //Combine to get a single non-uniform scale
            RenNonUniformScale temp( baseScale.asNonUniform(), extraScale.asUniform() );
            nonConstThis->scale_ = &temp;
            render( transform );
        }
    }
    else if( extraScale.isNonUniform() )
    {
        if( baseScale.isUniform() )
        {
            //Combine to get a single non-uniform scale
            RenNonUniformScale temp( extraScale.asNonUniform(), baseScale.asUniform() );
            nonConstThis->scale_ = &temp;
            render( transform );
        }
        else
        {
            ASSERT( baseScale.isNonUniform(), "Invalid RenScale" );
            //Combine to get a single non-uniform scale
            RenNonUniformScale temp( baseScale.asNonUniform(), extraScale.asNonUniform() );
            nonConstThis->scale_ = &temp;
            render( transform );
        }
    }
    else
    {
        //Daft client has called this function with an extra scale of unity
        render( transform );
    }

    //Restore the original scale
    nonConstThis->scale_ = &baseScale;
}

const Ren::MaterialVecPtr& RenMeshInstance::materialVec() const
{
	return materialVecPtr_;
}

void perWrite( PerOstream& ostr, const RenMeshInstance& instance )
{
    ostr << instance.pMesh_;
    ostr << instance.materialVecPtr_;
    ostr << instance.texAnimationPtr_;

    //If the mesh instance has effectively no scaling (ie uniform scale of 1), the RenScale*
    //may be to a common local static RenUnityScale returned by unityScale().
    //This mustn't be written via persistence.
    ostr << (instance.scale_ == RenMeshInstance::unityScale() ? NULL : instance.scale_);
}

void perRead( PerIstream& istr, RenMeshInstance& instance )
{
    istr >> instance.pMesh_;
    istr >> instance.materialVecPtr_;
    istr >> instance.texAnimationPtr_;
    istr >> instance.scale_;

    //Check for null scale, and use unity if so
    if( instance.scale_ == NULL )
        instance.scale_ = RenMeshInstance::unityScale();
}

RenMeshInstance::RenMeshInstance( PerConstructor )
{
}

ostream& operator <<( ostream& o, const RenMeshInstance& t )
{
    if( t.pMesh_.isDefined() )
        o << t.pMesh_;

    if( t.scale_ )
        o << *t.scale_;

    return o;
}


/* End MESHINST.CPP *************************************************/
