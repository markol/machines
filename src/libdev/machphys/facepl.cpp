/*
 * P R O T G E N . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/facepl.hpp"

#include "world4d/root.hpp"

#include "mathex/transf3d.hpp"

PER_DEFINE_PERSISTENT( MachPhysFacePlate );

MachPhysFacePlate::MachPhysFacePlate( W4dEntity* pParent, size_t level )
: W4dGeneric( pParent, W4dTransform3d() )
{
    loadLODFile( compositeFileName( level ) );

    TEST_INVARIANT;
}

MachPhysFacePlate::MachPhysFacePlate( PerConstructor con )
: W4dGeneric( con )
{
}

MachPhysFacePlate::~MachPhysFacePlate()
{
    TEST_INVARIANT;

}

SysPathName MachPhysFacePlate::compositeFileName( size_t brainLevel ) const
{
    PRE( 1 <= brainLevel and brainLevel <= 10 );
    
    SysPathName result;
    
    switch( brainLevel )
    {
        case 1:
            result = "models/facepl/facepl1.lod";
            break;
        case 2:
            result = "models/facepl/facepl1.lod";
            break;
        case 3:
            result = "models/facepl/facepl1.lod";
            break;
        case 4:
            result = "models/facepl/facepl1.lod";
            break;
        case 5:
            result = "models/facepl/facepl1.lod";
            break;
        case 6:
            result = "models/facepl/facepl1.lod";
            break;
        case 7:
            result = "models/facepl/facepl1.lod";
            break;
        case 8:
            result = "models/facepl/facepl1.lod";
            break;
        case 9:
            result = "models/facepl/facepl1.lod";
            break;
        case 10:
            result = "models/facepl/facepl1.lod";
            break;
    }
    
    return result;    
}


void MachPhysFacePlate::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void perWrite( PerOstream& ostr, const MachPhysFacePlate& facePlate )
{
    const W4dGeneric& base = facePlate;
    
    ostr << base;
    
    ASSERT_FAIL( "Can't write out face plates yet" );
}

void perRead( PerIstream& istr, MachPhysFacePlate& facePlate )
{
    W4dGeneric& base = facePlate;
    
    istr >> base;
    
    ASSERT_FAIL( "Can't read in face plates yet" );
}

/* End PROTGEN.CPP **************************************************/
