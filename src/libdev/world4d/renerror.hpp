/*
 * R E N E R R O R . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _RENDER_RENERROR_HPP
#define _RENDER_RENERROR_HPP

#include "world4d/dxerror.hpp"

// A DirectX error handler which takes Render library specific actions
// when errors occur.
class W4dDXErrorHandler : public DirectXErrorHandler
{
public:
    W4dDXErrorHandler();
    virtual ~W4dDXErrorHandler() {}

protected:
	// Handler actions for terminal and non-terminal errors, respectively.
	static Action shutdownDirectX(const DirectXErrorHandler*, HRESULT);
	static Action informManager  (const DirectXErrorHandler*, HRESULT);
};

extern W4dDXErrorHandler renErr;

#endif

/* End RENERROR.HPP *************************************************/
