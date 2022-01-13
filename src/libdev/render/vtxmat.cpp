/*
 * V T X M A T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include "render/internal/vtxmat.hpp"
#include "ctl/algorith.hpp"
#include "render/mesh.hpp"

PER_DEFINE_PERSISTENT( RenIVtxMaterial );
PER_DEFINE_PERSISTENT( RenIVertexMaterials );

RenIVertexMaterials::RenIVertexMaterials(size_t nElements) :
Base(0)
{
    reserve(nElements);
}

RenIVertexMaterials::RenIVertexMaterials(const RenIVertexMaterials& copyMe) :
Base(copyMe) { }

RenIVertexMaterials::~RenIVertexMaterials() { }

bool RenIVertexMaterials::contains(Ren::VertexIdx i) const
{
    const_iterator it = findIter(i);
    return it != end();
}

const RenMaterial& RenIVertexMaterials::find(Ren::VertexIdx i) const
{
    bool const contain = contains(i);

    PRE(contain);
    const_iterator it = findIter(i);
    ASSERT(it != end(), "Didn't find per-vertex material in material map.");
    return (*it).mat_;
}

void RenIVertexMaterials::insert(Ren::VertexIdx i, RenMaterial newValue)
{
    iterator it = findIter(i);

    if (it != end())
        (*it).mat_ = newValue;
    else
    {
        RenIVtxMaterial newElement;
        newElement.index_ = i;
        newElement.mat_   = newValue;
        Base::push_back(newElement);
    }

    POST(contains(i));
}

void RenIVertexMaterials::erase(Ren::VertexIdx i)
{
    iterator it = findIter(i);

    if (it != end())
        Base::erase(it);

    POST(!contains(i));
}

ctl_vector<RenIVtxMaterial>::iterator RenIVertexMaterials::findIter(Ren::VertexIdx i)
{
    RenIVtxMaterial target;
    target.index_ = i;

    // There appears to be a Watcom bug here.  The compiler fails to recognise
    // that begin and end can refer to overloaded functions which return a
    // iterator rather than a const_iterator.
#ifdef NDEBUG
    iterator it = std::find(begin(), end(), target);
    //	const_iterator cit = ::find(begin(), end(), target);
    //	iterator it = const_cast<iterator>( cit);
#else
    iterator it = std::find(begin(), end(), target);
#endif

    return it;
}

ctl_vector<RenIVtxMaterial>::const_iterator RenIVertexMaterials::findIter(Ren::VertexIdx i) const
{
    return const_cast<RenIVertexMaterials*> ( this)->findIter(i);
}

RenIVertexMaterials::RenIVertexMaterials( PerConstructor con )
: Base( con ) { }

void perWrite( PerOstream& ostr, const RenIVertexMaterials& t )
{
    const RenIVertexMaterials::Base& base = t;

    ostr << base;
}

void perRead( PerIstream& istr, RenIVertexMaterials& t )
{
    RenIVertexMaterials::Base& base = t;

    istr >> base;
}

void perWrite( PerOstream& ostr, const RenIVtxMaterial& t )
{
    ostr << t.index_;
    ostr << t.mat_;
}

void perRead( PerIstream& istr, RenIVtxMaterial& t )
{
    istr >> t.index_;
    istr >> t.mat_;
}

/* End VTXMAT.CPP ***************************************************/
