#include <iostream>
//#include "planeted/aggrdata.hpp"
#include "planeted/mphydata.hpp"


int main()
{
	std::cout << "main\n";
	std::cout << "Aggressor 1 1" << std::endl << MachPhysData::instance().aggressorData( 1, 1 );
	std::cout << "Administrator 1 1" << std::endl << MachPhysData::instance().administratorData( 1, 1 );
	std::cout << "AGG 4 5 NoWeapons " << MachPhysData::instance().aggressorData( 4, 5 ).numberOfWeapons() << std::endl;
	std::cout << "AGG 4 10 NoWeapons " << MachPhysData::instance().aggressorData( 4, 10 ).numberOfWeapons() << std::endl;
	std::cout << "AGG 5 4 NoWeapons " << MachPhysData::instance().aggressorData( 5 , 4 ).numberOfWeapons() << std::endl;
	std::cout << "AGG 10 4 NoWeapons " << MachPhysData::instance().aggressorData( 10, 4 ).numberOfWeapons() << std::endl;
	std::cout << "Mine 6 " << std::endl << MachPhysData::instance().mineData( 6 );
	std::cout << std::endl << std::endl;
	for( int i = 1; i < 11 ; ++i )
	{
		std::cout << "HARD " << i << std::endl << MachPhysData::instance().hardwareLabData( i ) << std::endl;
		std::cout << "SOFT " << i << std::endl << MachPhysData::instance().softwareLabData( i ) << std::endl;
		std::cout << "SMEL " << i << std::endl << MachPhysData::instance().smelterData( i ) << std::endl;
		std::cout << "FACT " << i << std::endl << MachPhysData::instance().factoryData( i ) << std::endl;
		std::cout << "TURR " << i << std::endl << MachPhysData::instance().turretData( i ) << std::endl;
		std::cout << "GARR " << i << std::endl << MachPhysData::instance().garrisonData( i ) << std::endl;
		std::cout << "MINE " << i << std::endl << MachPhysData::instance().mineData( i ) << std::endl;
		std::cout << "BEAC " << i << std::endl << MachPhysData::instance().beaconData( i ) << std::endl;
		for( int j = 1; j < 11; ++j )
		{
			std::cout << "AGG " << i << " " << j << std::endl << MachPhysData::instance().aggressorData( i, j ) << std::endl;
			std::cout << "ADM " << i << " " << j << std::endl << MachPhysData::instance().administratorData( i, j ) << std::endl;
			std::cout << "TRAN " << i << " " << j << std::endl << MachPhysData::instance().transporterData( i, j ) << std::endl;
			std::cout << "LOC " << i << " " << j << std::endl << MachPhysData::instance().locatorData( i, j ) << std::endl;
			std::cout << "TECH " << i << " " << j << std::endl << MachPhysData::instance().technicianData( i, j ) << std::endl;
			std::cout << "CON " << i << " " << j << std::endl << MachPhysData::instance().constructorData( i, j ) << std::endl;
		}
	}
	return 0;
}

