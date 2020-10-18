/*
 * R A C E . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include "stdlib/string.hpp"
#include "planeted/race.hpp"

MachPhys::Race PedRace::parse( const string& race )
{
	// return race based on text string passed in
	MachPhys::Race returnRace;
   	if( race == "RED" )
		returnRace = MachPhys::RED;
	else if( race == "BLUE" )
		returnRace = MachPhys::BLUE;
	else if( race == "GREEN" )
		returnRace = MachPhys::GREEN;
	else if( race == "YELLOW" )
		returnRace = MachPhys::YELLOW;
	else
		returnRace = MachPhys::N_RACES;
	return returnRace;
}

MachPhys::Race PedRace::next( MachPhys::Race race )
{
	MachPhys::Race nextRace;
	if ( race == MachPhys::RED )
		nextRace = MachPhys::BLUE;
	else if ( race == MachPhys::BLUE)
		nextRace = MachPhys::GREEN;
	else if ( race == MachPhys::GREEN )
		nextRace = MachPhys::YELLOW;
	else if ( race == MachPhys::YELLOW )
		nextRace = MachPhys::RED;
	return nextRace;
}

RenColour PedRace::colour( MachPhys::Race race )
{
	RenColour colour;

	switch( race )
	{
		case( MachPhys::RED ) 	:	colour = RenColour::red(); break;
		case( MachPhys::BLUE ) 	:	colour = RenColour::blue(); break;
		case( MachPhys::GREEN ) :	colour = RenColour::green(); break;
		case( MachPhys::YELLOW ):	colour = RenColour::yellow(); break;
	}
	return colour;
}
