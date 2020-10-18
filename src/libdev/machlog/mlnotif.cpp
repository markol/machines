/*
 * M L N O T I F . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    //  Definitions of non-inline non-template methods and global functions
*/

#include "machlog/mlnotif.hpp"

#include "machlog/races.hpp"	

//////////////////////////////////////////////////////////////////////////////

MachLogNotifiable::MachLogNotifiable( MachPhys::Race r )
:
race_( r )
{ 	
	TEST_INVARIANT;		
}

//////////////////////////////////////////////////////////////////////////////
  
MachLogNotifiable::~MachLogNotifiable ( void ) 
{
	TEST_INVARIANT;
}	   
    
//////////////////////////////////////////////////////////////////////////////

void MachLogNotifiable::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

void MachLogNotifiable::changeRace( MachPhys::Race r )
{
	race_ = r;
}

//////////////////////////////////////////////////////////////////////////////
/* End MLNOTIF.CPP **************************************************/
	