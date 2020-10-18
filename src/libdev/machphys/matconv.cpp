/*
 * M A T C O N V . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/matconv.hpp"

#include "ctl/countptr.hpp"
#include "render/hsv.hpp"
#include "render/colour.hpp"
#include "render/matmap.hpp"
#include "render/matvec.hpp"
#include "render/material.hpp"
#include "world4d/composit.hpp"
#include "world4d/mveccple.hpp"
#include "world4d/mvecccha.hpp"

MachPhysMaterialConverter::MachPhysMaterialConverter( MachPhys::Race from, MachPhys::Race to )
: fromHue_( hue( from ) ),
  toHue_( hue( to ) )
{
    TEST_INVARIANT;
}

MachPhysMaterialConverter::~MachPhysMaterialConverter()
{
    TEST_INVARIANT;

}

bool MachPhysMaterialConverter::convert( RenMaterial* pMaterial ) const
{
    RenColourHSV    colour( pMaterial->diffuse() );
    bool    converted = false;

    if( colour.hueDefined() and close( colour.hue(), fromHue_ ) )
    {
        colour.hue( colour.hue() + toHue_ - fromHue_ );

        pMaterial->diffuse( colour.rgb() );
        converted = true;
    }
    else
        converted = false;

    return converted;
}

// Apply this material conversion to all materials in a given composite.
void MachPhysMaterialConverter::convert( W4dComposite* comp ) const
{
	PRE(comp);

    RenMaterialSet    materials;
    comp->addMaterials( &materials );

    RenMaterialMap  newMaterialMap;

    for( RenMaterialSet::iterator i = materials.begin(); i != materials.end(); ++i )
    {
        RenMaterial material = *i;

        if( convert( &material ) )
            newMaterialMap.insert( *i, material );
    }

    //comp->materialMap( newMaterialMap );
    //Test the new material changer classes
    W4dMaterialVecPtrSet tempSet;
    W4dCompositeMaterialVecChanger testChanger( *comp, newMaterialMap, &tempSet );
    testChanger.applyOverrides( comp );
}

bool MachPhysMaterialConverter::close( double hue1, double hue2 ) const
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

double MachPhysMaterialConverter::hue( MachPhys::Race race ) const
{
    double result = 0;

    switch( race )
    {
        case MachPhys::RED:
            result = 0.0;
            break;
        case MachPhys::BLUE:
            result = 223.0;
            break;

        case MachPhys::GREEN:
            result = 120.0;
            break;
        case MachPhys::YELLOW:
            result = 60.0;
            break;
    }

    return result;
}

void MachPhysMaterialConverter::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}


/* End MATCONV.CPP **************************************************/
