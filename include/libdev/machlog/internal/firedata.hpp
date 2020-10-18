#ifndef _MACHLOG_FIREDATA_HPP
#define _MACHLOG_FIREDATA_HPP

#include "base/base.hpp"
#include "mathex/mathex.hpp"

class MachLogFireData
{
	public:	
		MachLogFireData() : 
		burstSeed( -1 ), missed( false ), firstDrift( 0.0 ), secondDrift( 0.0 ) {};
		
		MATHEX_SCALAR burstSeed;	//fnarr fnarr
		bool missed;	
		MATHEX_SCALAR firstDrift;
		MATHEX_SCALAR secondDrift;
};


#endif