/*
 * G X D A T A . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */
#ifndef _GXDATA_HPP
#define _GXDATA_HPP

#include <iostream.h>
#include "GXError.hpp"
#include "GXId.hpp"

class GXData {

public:
	virtual GXError read(const ifstream &inputfile) = 0;

protected:
	GXError err_;
	GXTag tag_;
	GXId id_;

};

#endif
	