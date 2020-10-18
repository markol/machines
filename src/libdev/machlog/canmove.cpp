/*
 * C A N M O V E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved.
 */
 
#include "machlog/canmove.hpp"



/* //////////////////////////////////////////////////////////////// */
PER_DEFINE_PERSISTENT_ABSTRACT( MachLogCanTurn );
PER_DEFINE_PERSISTENT( MachLogCanMove );

void perWrite( PerOstream& , const MachLogCanMove& )
{
}

void perRead( PerIstream& , MachLogCanMove& )
{
}

void perWrite( PerOstream& , const MachLogCanTurn& )
{
}

void perRead( PerIstream& , MachLogCanTurn& )
{
}

/* //////////////////////////////////////////////////////////////// */

/* End CANMOVE.CPP *****************************************************/

