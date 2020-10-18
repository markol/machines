/*
 * M I S C A S M . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _MISCASM_HPP
#define _MISCASM_HPP

#include "base/base.hpp"

extern "C"
{
	void DevEnableVideo( void );
	void DevDisableVideo( void );

	void DevSetColour( uint8, uint8, uint8, uint8 );
	void DevSetPalette256( uint8 * );

	void DevWaitVBL( void );
	void DevWaitDisplay( void );
	void DevWaitRetrace( void );
}

#endif // _MISCASM_HPP
