/*
 * V G A A S M . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _VGAASM_HPP
#define _VGAASM_HPP

#include "base/base.hpp"

extern "C"
{
	uint8 DevSeqRead( uint8 );
	void DevSeqSet( uint8, uint8 );
	uint8 DevCrtRead( uint8 );
	void DevCrtSet( uint8, uint8 );
	uint8 DevGfxRead( uint8 );
	void DevGfxSet( uint8, uint8 );

	void DevSetVideoMode( uint8 );
	void DevShowVideoPage( uint16 );
	void DevClearVideoPage( uint16, uint8 );
	void DevCopyLinearToPlanar( uint8 *, int32, int32 );
	void DevCopyPlanarToLinear( int32, uint8 *, int32 );
}

#endif // _VGAASM_HPP
