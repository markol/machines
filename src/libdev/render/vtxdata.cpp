/*
 * V T X D A T A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "render/internal/ren_pch.hpp" // NB: pre-compiled header must come 1st
#include "render/internal/vtxdata.hpp"
#include "mathex/point3d.hpp"
#include "mathex/point2d.hpp"
#include "mathex/vec3.hpp"
#include "ctl/vector.hpp"
#include "render/internal/vtxmat.hpp"
#include "render/internal/intmap.hpp"
#include "render/internal/colpack.hpp"
#include <iomanip>

char* RenIVertexData::perPDerivedClass() const
{
    return const_cast<char*> (reinterpret_cast<const char*> (this));
}

const char* RenIVertexData::perClassName()
{
    return "RenIVertexData";
}

const char* RenIVertexData::perMostDerivedClassName() const
{
    return this->perClassName();
}

void perReadRenIVertexData(PerIstream& istr)
{
    istr >> RenIVertexData::perCreate();
}

void perWriteRenIVertexData( PerOstream& ostr, const void* pVoid )
{
    const RenIVertexData* pObject = static_cast<const RenIVertexData*> ( pVoid );
    ostr << *pObject;
}

PerOstream& operator<<(PerOstream& ostr, const RenIVertexData& ob)
{
    ASSERT_INFO( "w" );
    Persistence::instance().writeObjectPre( ostr, static_cast< const void*> ( &ob ), "RenIVertexData" );
    ASSERT_INFO( "x" );
    perWrite( ostr, ob );
    ASSERT_INFO( "y" );
    Persistence::instance().writeObjectPost( static_cast< const void*> ( &ob ), "RenIVertexData" );
    ASSERT_INFO( "z" );
    return ostr;
}

PerIstream& operator>>(PerIstream& istr, RenIVertexData& ob)
{
    Persistence::instance().readObjectPre( istr, static_cast< const void*> ( &ob ), "RenIVertexData" );
    perRead( istr, ob );
    Persistence::instance().readObjectPost( static_cast< const void*> ( &ob ), "RenIVertexData" );
    return istr;
}

PerOstream& operator<<(PerOstream& ostr, const RenIVertexData* pOb)
{
    void* pMostDerivedOb = pOb ? pOb->perPDerivedClass() : (void*) pOb;
    const char* mostDerivedClassName =  pOb ? pOb->perMostDerivedClassName() : nullptr;

    auto const result = Persistence::instance().writePointerPre(ostr, static_cast< const void*> ( pOb ), "RenIVertexData", pMostDerivedOb, mostDerivedClassName );

    if (result == Persistence::WRITE_OBJECT)
    {
        ostr << *pOb;
    }

    Persistence::instance().writePointerPost( ostr, static_cast< const void*> ( pOb ), "RenIVertexData", pMostDerivedOb );

    return ostr;
}

PerIstream& operator>>(PerIstream& istr, RenIVertexData*& pOb)
{
    auto const result = Persistence::instance().readPointerPre( istr, reinterpret_cast< void**> (&pOb), "RenIVertexData");

    if (result == Persistence::READ_OBJECT )
    {
        istr >> RenIVertexData::perCreate();
    }

    Persistence::instance().readPointerPost( istr, reinterpret_cast< void**> (&pOb), "RenIVertexData" );

    return istr;
}

RenIVertexData& RenIVertexData::perCreate()
{
    return *(new RenIVertexData(PERSISTENCE_CONSTRUCTOR));
}

RenIVertexData::RenIVertexData(size_t nVertices):
intensityMap_(nullptr),
materialMap_(nullptr)
{
    reserve(nVertices);
    persistAsRaw( true );
    normals_.persistAsRaw();
}

RenIVertexData::RenIVertexData(PerConstructor):
intensityMap_(nullptr),
materialMap_(nullptr)
{
    // Allow the base-class persistent read to allocate the memory.
    reserve(0);
    persistAsRaw( true );
    normals_.persistAsRaw();
}

// After this call, the lhs and rhs objects share pointers to the same intensity
// and material maps.  The vertex data, however, is copied.  So we have an odd
// mix of deep and shallow semantics.  This is the desired behaviour for texture
// UV animation which is currently the only client.

RenIVertexData::RenIVertexData(const RenIVertexData& copyMe) :
Base(copyMe.begin(), copyMe.end()),
intensityMap_(copyMe.intensityMap_),
materialMap_(copyMe.materialMap_),
normals_(copyMe.normals_)
{
    persistAsRaw( true );
    normals_.persistAsRaw();

    POST(size() == copyMe.size());
}

// After this call, the lhs and rhs objects share pointers to the same intensity
// and material maps.  The vertex data, however, is copied.  So we have an odd
// mix of deep and shallow semantics.  This is the desired behaviour for texture
// UV animation which is currently the only client.

RenIVertexData::RenIVertexData(const RenIVertexData& copyMe, size_t nElements) :
Base(copyMe.begin(), copyMe.begin() + nElements),
intensityMap_(copyMe.intensityMap_),
materialMap_(copyMe.materialMap_),
normals_(copyMe.normals_)
{
    PRE(nElements <= copyMe.size());

    persistAsRaw( true );
    normals_.persistAsRaw();

    POST(size() == nElements);
}

void RenIVertexData::createIntensityMap()
{
    // What is a good choice of initial size for the map?
    if (!intensityMap())
    {
        const size_t maxInitialSize = 10;
        intensityMap_ = new RenIVertexIntensities(std::min(maxInitialSize, size()));
    }

    POST(intensityMap());
}

void RenIVertexData::resetAllIntensities()
{
    intensityMap_ = nullptr;
}

void RenIVertexData::createMaterialMap()
{
    // What is a good choice of initial size for the map?
    if (!materialMap())
    {
        const size_t maxInitialSize = 10;
        materialMap_ = new RenIVertexMaterials(std::min(maxInitialSize, size()));
    }

    POST(materialMap());
}

void RenIVertexData::resetAllMaterials()
{
    materialMap_ = nullptr;
}

void RenIVertexData::eraseAll()
{
    resetAllIntensities();
    resetAllMaterials();
    normals_.erase(normals_.begin(), normals_.end());
    erase(begin(), end());
}

/* Old floating point version.
static size_t addOrFindNormal(const MexVec3& normal, RenIVertexData::Normals& normals)
{
    size_t result;

    // Search for an existing normal which matches the given one.  The
    // tollerance used is pretty large -- we're trading off space and time
    // savings against lighting accuracy.  This will be hellish slow, but
    // hopefully it will benifit run-time performance.
    static const MexDegrees tollerance = 5;
    RenIVertexData::Normals::const_iterator it = normals.begin();
    while (it != normals.end())
    {
        const MexVec3& extant = *it;

        if (extant.angleBetween(normal) <= tollerance)
        {
            result = it - normals.begin();
            break;
        }

        ++it;
    }

    if (it == normals.end())
    {
        result = normals.size();
        normals.push_back(normal);
    }

    POST(result < normals.size());
    return result;
}
 */

// Search the vector of normals for the given normal.  If it exists already,
// simply return its index, otherwise add it and return the new index.

static size_t addOrFindNormal(const MexVec3& normal, RenIVertexData::Normals& normals)
{
    size_t result;
    const RenIVec3FixPtS0_7 fixNorm = normal;

    // Search for an existing normal which matches the given one.  The
    // tollerance used should be pretty large -- we're trading off space and
    // time savings against lighting accuracy.  Rely on the inaccuracy of
    // the fixed-point rep. to give a low resolution test.
    // TBD: it may be advantagous to use fewer fraction bits for the comparison.
    RenIVertexData::Normals::const_iterator it = normals.begin();
    while (it != normals.end())
    {
        const RenIVec3FixPtS0_7& extant = *it;

        if (extant == fixNorm)
        {
            result = it - normals.begin();
            break;
        }

        ++it;
    }

    if (it == normals.end())
    {
        result = normals.size();
        normals.push_back(fixNorm);
    }

    POST(result < normals.size());
    return result;
}

void RenIVertexData::addVertex(const MexPoint3d& pt, const MexVec3& normal, const MexPoint2d& uv)
{
    PRE(!normal.isZeroVector());

    RenIVertex newVtx;
    newVtx.x = pt.x();
    newVtx.y = pt.y();
    newVtx.z = pt.z();
    newVtx.tu = uv.x();
    newVtx.tv = uv.y();

    // Ensure that all the specular values are initialised to black.
    // This allows the unlit vertices to be memcpy'd to the lit
    // vertices by the illuminator.
    static const uint32_t black = packColourChecked(0, 0, 0);
    newVtx.specular = black;
    newVtx.normalIndex = addOrFindNormal(normal, normals_);

    push_back(newVtx);
}

void RenIVertexData::addVertices
(
    const ctl_vector<MexPoint3d>& pts,
    const MexVec3& normal,
    const ctl_vector<MexPoint2d>& uvs,
    size_t nPts
    )
{
    PRE(implies(nPts == 0, pts.size() == uvs.size()));
    PRE(implies(nPts != 0, pts.size() >= nPts && uvs.size() >= nPts));
    PRE(!normal.isZeroVector());

    // If nPts is zero, that implies we should add all the given points.
    if (nPts == 0)
        nPts = pts.size();

    // Ensure that all the specular values are initialised to black.
    // This allows the unlit vertices to be memcpy'd to the lit
    // vertices by the illuminator.
    static const uint32_t black = packColourChecked(0, 0, 0);
    RenIVertex newVtx;
    newVtx.specular = black;
    newVtx.normalIndex = addOrFindNormal(normal, normals_);

    ctl_vector<MexPoint3d>::const_iterator ptIt = pts.begin();
    ctl_vector<MexPoint2d>::const_iterator uvIt = uvs.begin();

    for (size_t i = 0; i != nPts; ++i)
    {
        ASSERT(ptIt != pts.end(), "Vector too small");
        ASSERT(uvIt != uvs.end(), "Vector too small");

        newVtx.x = (*ptIt).x();
        newVtx.y = (*ptIt).y();
        newVtx.z = (*ptIt).z();
        newVtx.tu = (*uvIt).x();
        newVtx.tv = (*uvIt).y();

        push_back(newVtx);
        ++ptIt;
        ++uvIt;
    }
}

const RenIVec3FixPtS0_7& RenIVertexData::normal(Base::const_iterator it) const
{
    PRE(it >= begin());
    PRE(it < end());
    PRE((*it).normalIndex < normals_.size());
    return normals_[(*it).normalIndex];
}

const RenIVec3FixPtS0_7& RenIVertexData::normal(const RenIVertex& v) const
{
    PRE(v.normalIndex < normals_.size());
    return normals_[v.normalIndex];
}

RenIVec3FixPtS0_7& RenIVertexData::normal(const RenIVertex& v)
{
    PRE(v.normalIndex < normals_.size());
    return normals_[v.normalIndex];
}

void perRead( PerIstream& istr, RenIVertexData& data )
{
    RenIVertexData::Base& base = data;

    istr >> base;
    istr >> data.normals_;

    // Currently the persistence for counted pointers chokes on a private base.
    // TBD: stream these member vars.
    //	istr >> data.intensityMap_;
    istr >> data.materialMap_;
}

void perWrite( PerOstream& ostr, const RenIVertexData& data )
{
    const RenIVertexData::Base& base = data;

    ostr << base;
    ostr << data.normals_;

    // Currently the persistence for counted pointers chokes on a private base.
    // TBD: stream these member vars.
    //	ostr << data.intensityMap_;
    ostr << data.materialMap_;
}

std::ostream& operator <<(std::ostream& o, const RenIVertexData& data )
{
    const RenIVertexData::Base& base = data;

    o << base;

    return o;
}

std::ostream& operator<<(std::ostream& os, const RenIVertex& vtx)
{
    os << std::setprecision(1);
    os << "D3DVERTEX: (" << vtx.x  << "," << vtx.y  << "," << vtx.z  << ") "
        << "ni=" << vtx.normalIndex
        << " uv (" << vtx.tu << "," << vtx.tv << ") ";
    return os;
}

PerOstream& operator <<( PerOstream& os, const RenIVertex& vtx )
{
    PER_WRITE_RAW_DATA( os, &vtx, sizeof ( vtx ) );

    return os;
}

PerIstream& operator >>( PerIstream& is, RenIVertex& vtx )
{
    PER_READ_RAW_DATA( is, &vtx, sizeof ( vtx ) );

    return is;
}

/* End VTXDATA.CPP **************************************************/
