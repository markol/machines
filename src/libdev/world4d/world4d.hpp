/*
 * W O R L D 4 D . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _WORLD4D_WORLD4D_HPP
#define _WORLD4D_WORLD4D_HPP

#include "base/base.hpp"
#include "mathex/mathex.hpp"
#include "utility/id.hpp"

class MexDouble;
class MexTransform3d;

//A link identifier
typedef uint W4dLinkId;

//An entity id
typedef UtlId W4dId;

//A level of detail id
typedef int W4dLOD;
    
typedef MexTransform3d  W4dTransform3d;

//Namespace declarations
class W4d
{
public:
    //The explicit values allow enumeration objects to be used to index into
    //vectors, points etc, and are guaranteed not to change.
    enum Axis{ X_AXIS = 0, Y_AXIS = 1, Z_AXIS = 2 };
};

#endif