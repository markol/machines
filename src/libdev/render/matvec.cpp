/*
 * M A T V E C . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include <iostream>
#include "render/matvec.hpp"

PER_DEFINE_PERSISTENT( RenMaterialVec );

RenMaterialVec::RenMaterialVec(size_t nMats): Base(0)
{
	reserve(nMats);
    TEST_INVARIANT;
}

RenMaterialVec::~RenMaterialVec()
{
    TEST_INVARIANT;
}

void RenMaterialVec::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const RenMaterialVec& t )
{
    o << "RenMaterialVec of " << t.size() << " materials";
	o << ((t.size() > 0)? ":": ".") << "\n";

	for (RenMaterialVec::const_iterator it=t.begin(); it!=t.end(); ++it)
		o << "  " << *it << "\n";

    return o;
}

void perWrite( PerOstream& ostr, const RenMaterialVec& vec )
{
    const RenMaterialVec::Base& base = vec;
    ostr << base;
}

void perRead( PerIstream& istr, RenMaterialVec& vec )
{
    RenMaterialVec::Base& base = vec;
    istr >> base;
}

RenMaterialVec::RenMaterialVec( PerConstructor )
: Base( 0 )
{
}

/* End MATVEC.CPP ***************************************************/
