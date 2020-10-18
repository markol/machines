/*
 * E N V I R N M T . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _REN_ENVIRNMT_HPP
#define _REN_ENVIRNMT_HPP

#include "base/base.hpp"

class SysPathName;

template <class T> class ctl_pvector;

class RenDevice;
class RenDirectionalLight;
class RenPointLight;
class RenUniformLight;

// Reads the given environment file.  Sets the appropriate sky colours and fog
// parameters in the device.  Creates directional or point lights as indicated
// by the file, adds them to the device and returns them in the given lists.
// If the given file does not exist, it attempts "default.env".  If there is no
// default file, then the function sets sensible defaults and creates one 
// directional light.
// PRE(dev);
void renReadEnvironment
(
	const SysPathName& envFile,
	RenDevice* dev,
	ctl_pvector<RenDirectionalLight>* 	dirs     = NULL,
	ctl_pvector<RenPointLight>* 		points   = NULL,
	ctl_pvector<RenUniformLight>*		uniforms = NULL
);

#endif

/* End ENVIRNMT.HPP ****************************************************/
