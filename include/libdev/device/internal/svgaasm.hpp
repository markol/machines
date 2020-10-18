/*
 * S V G A A S M . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _SVGAASM_HPP
#define _SVGAASM_HPP

#include "base/base.hpp"

extern "C"
{
	void DevSetPageGranularity( uint16 );
	void DevCopyLinearToPlanar640x400x256( uint8 * );
	void DevCopyLinearToPlanar640x480x256( uint8 * );
}

#endif // _SVGAASM_HPP
