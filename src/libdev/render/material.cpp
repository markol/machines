/*
 * M A T E R I A L . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "render/material.hpp"
#include "render/colour.hpp"
#include "render/texture.hpp"
#include "render/internal/matbody.hpp"
#include "render/internal/matmgr.hpp"

// If we are to hide the implementation details of RenIMatBody, we cannot
// have an inline method in both the handle and body.  In practice this means
// that RenIMatBody has inline methods because the efficiency needs of
// internal clients are more important.  Private methods which use the body
// class can be inlined, but *not* in a publicly availbe file, hence, we have
// inlines in this file.
#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
	#include "render/material.ipp"
#endif

PER_DEFINE_PERSISTENT( RenMaterial );

RenMaterial::RenMaterial():
	body_(RenIMatManager::instance().defaultBody())
{
	PRE(Ren::initialised());
	body_->incRefCount();
	POST(body_->sharable());
}

// A default non-sharable material.  Make a unique copy of a default body.
RenMaterial::RenMaterial(NonSharable):
	body_(_NEW(RenIMatBody))
{
	PRE(Ren::initialised());
	body_->makeNonSharable();
	body_->incRefCount();
	POST(body_->refCount() == 1);
	POST(!body_->sharable());
}

RenMaterial::RenMaterial(const RenColour& dif):
	body_(RenIMatManager::instance().createBody(dif))
{
	PRE(Ren::initialised());
	body_->incRefCount();
	POST(body_->diffuse() == dif);
	POST(body_->sharable());
}

RenMaterial::RenMaterial( const RenMaterial& copyMe ):
	body_(copyMe.body_)
{
	body_->incRefCount();
	POST(*body_ == *(copyMe.body_));
}

RenMaterial::~RenMaterial()
{
	body_->decRefCount();
}

RenMaterial& RenMaterial::operator=(const RenMaterial& copyMe)
{
	if (body_ != copyMe.body_)
	{
		body_->decRefCount();
		body_ = copyMe.body_;
		body_->incRefCount();
	}

	return *this;
}

void RenMaterial::makeNonSharable()
{
	PRE(body_->refCount() >= 1);

	if (body_->refCount() == 1)
	{
		body_->makeNonSharable();
	}
	else
	{
		// This material has a shared body.  Give this handle a unique body
		// and detach it from the original so that the other handles are not
		// affected by the change.
		RenIMatBody* modifiedBody = _NEW(RenIMatBody(*body_));
		body_->decRefCount();
		ASSERT(body_->refCount() >= 1, logic_error());

		modifiedBody->makeNonSharable();
		body_ = RenIMatManager::instance().addPossiblyNewBody(modifiedBody);
		body_->incRefCount();

		// As the new copy is non-sharable, the material manager should *not*
		// return a pointer to a duplicate body.
		ASSERT(body_ == modifiedBody, "Material manager shared a non-sharable body.");
	}
}

// The material manager class ensures that duplicate bodies are never
// created, so comparing pointers to bodies suffices as an equality test.
// What semantics are required for the sharable flag?  If two materials
// differ only in the sharable flag, this test returns false.  As the required
// behaviour is not obvious, this is implmeneted because it is fast.
bool RenMaterial::operator==(const RenMaterial& compareMe)
{
	return body_ == compareMe.body_;
}

bool RenMaterial::areColoursEqual(const RenMaterial& m) const
{
	return	body_->diffuse  () == m.body_->diffuse  () &&
			body_->emissive () == m.body_->emissive ();
}

bool RenMaterial::needsLighting() const
{
	const RenColour& blk = RenColour::black();
	return body_->diffuse() != blk;
}

// static
void RenMaterial::globalTransform(const RenMaterialTransform* xform)
{
	RenIMatManager::instance().globalMaterialTransform(xform);
}

// static
const RenMaterialTransform* RenMaterial::globalTransform()
{
	return RenIMatManager::instance().globalMaterialTransform();
}

// virtual
RenMaterialTransform::~RenMaterialTransform()
{
	// do nothing
}

ostream& operator <<( ostream& o, const RenMaterial& t )
{
	o << *t.body_;
    return o;
}

// static
void RenMaterial::printAll(ostream& os)
{
	os << RenIMatManager::instance();
}

void perWrite( PerOstream& ostr, const RenMaterial& mat )
{
    ostr << mat.body_;
}

void perRead( PerIstream& istr, RenMaterial& mat )
{
	mat.body_->decRefCount();

    istr >> mat.body_;

    //  Make sure that the material manager is aware of this body
	RenIMatBody* pBody = RenIMatManager::instance().addPossiblyNewBody( mat.body_ );

    //  If there was already an identical material body use that
    //  rather than our new body
    if( pBody != mat.body_ )
    {
        //The body read via persistence is not going to be used. Hence it needs to be deleted,
        //but not yet, in case any more materials in the persistent file reference it.
        //We increment its reference count the first time it is encountered, and add it to the
        //material manager's list of unused bodies. This list is cleared in RenDevice::start3d.
        //Hence we must only add it if its reference count is currently zero.
        if( mat.body_->refCount() == 0 )
        {
            mat.body_->incRefCount();
            RenIMatManager::instance().addUnusedMatBody( mat.body_ );
        }

        mat.body_ = pBody;
    }

    mat.body_->incRefCount();
}

// Most methods are delegated to RenIMatBody using these macros.
#define MAT_FORWARD_GET(TYPE, METHOD)			\
TYPE RenMaterial::METHOD() const				\
{												\
	return body_->METHOD();						\
}

// Modifying a material is complicated by the needs of copy on write and not
// duplicating materials.  There are special cases which the body sharing
// must deal with:
// 1) if the body's ref count is 1, we can simply modify without copying
// 2) having modified the body, the material manager may determine that the
//    new material exists already in which case we don't need the new version
// 3) if the client has made a material non-sharable, then simply apply the
//    modification to the body and don't search for duplicates.
#define MAT_FORWARD_SET(TYPE, METHOD)										\
void RenMaterial::METHOD(TYPE v)		  									\
{																			\
	PRE(body_->refCount() >= 1);											\
																			\
	if (!body_->sharable())													\
	{																		\
		body_->METHOD(v);							 						\
		return;																\
	}																		\
																			\
	if (body_->refCount() == 1)			  									\
	{																		\
		body_->METHOD(v);							 						\
																			\
		RenIMatBody* copy = RenIMatManager::instance().findDuplicate(body_);\
		ASSERT(implies(copy, copy != body_), logic_error());				\
		ASSERT(implies(copy, copy->sharable()), "Material sharing error.");	\
																			\
		if (copy)															\
		{																	\
			body_->decRefCount();											\
			body_ = copy;													\
			body_->incRefCount();											\
		}																	\
	}																		\
	else																	\
	{																		\
		RenIMatBody* modifiedBody = _NEW(RenIMatBody(*body_));				\
		body_->decRefCount();			 			 						\
		ASSERT(body_->refCount() >= 1, "Material reference counting error");\
		ASSERT(modifiedBody->sharable(), "Material sharing error.");		\
																			\
		modifiedBody->METHOD(v);					 						\
		body_ = RenIMatManager::instance().addPossiblyNewBody(modifiedBody);\
		body_->incRefCount();		 				 						\
		ASSERT(body_->sharable(), "Material sharing error.");				\
																			\
		if (body_ != modifiedBody)											\
			_DELETE(modifiedBody);		 			 						\
	}																		\
}

#define MAT_FORWARD_PROPERTY(TYPE, METHOD)									\
MAT_FORWARD_GET(TYPE, METHOD)			  									\
MAT_FORWARD_SET(TYPE, METHOD)

// For the sake of using the macro, this is not quite as efficient as it could
// be: the argument to the set method would be more efficient if it were a
// const RenTexture&, however, the return value of the get method cannot be a
// reference, it must be a texture object.
MAT_FORWARD_PROPERTY(const RenTexture&,	texture)
MAT_FORWARD_PROPERTY(const RenColour&,	diffuse)
MAT_FORWARD_PROPERTY(const RenColour&,	specular)
MAT_FORWARD_PROPERTY(const RenColour&,	ambient)
MAT_FORWARD_PROPERTY(const RenColour&,	emissive)
MAT_FORWARD_PROPERTY(double,			shininess)
MAT_FORWARD_PROPERTY(short,				coplanarPriority)
MAT_FORWARD_PROPERTY(short,				alphaPriority)
MAT_FORWARD_PROPERTY(ushort,			meshId)
MAT_FORWARD_PROPERTY(float,				fogMultiplier)
MAT_FORWARD_PROPERTY(bool,				interMeshCoplanar)
MAT_FORWARD_PROPERTY(bool,				absoluteAlphaPriority)
MAT_FORWARD_PROPERTY(bool,				intraMeshAlphaPriority)

MAT_FORWARD_GET(short,		coplanarDepth)
MAT_FORWARD_GET(bool,		hasAlphaTransparency)
MAT_FORWARD_GET(bool,		hasFlatTransparency)
MAT_FORWARD_GET(bool,		usesBilinear)
MAT_FORWARD_GET(bool,		hasFogMultiplier)

/* End MATERIAL.CPP *************************************************/
