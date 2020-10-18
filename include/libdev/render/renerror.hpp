/*
 * R E N E R R O R . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _REN_RENERROR_HPP
#define _REN_RENERROR_HPP

#include "render/dxerror.hpp"

// A DirectX error handler which takes Render library specific actions
// when errors occur.
class RenDXErrorHandler : public DirectXErrorHandler
{
public:
    RenDXErrorHandler();
    virtual ~RenDXErrorHandler() {}

    static  RenDXErrorHandler& defaultHandler();

protected:
	// Handler actions for terminal and non-terminal errors, respectively.
	static Action shutdownDirectX(const DirectXErrorHandler*, HRESULT);
	static Action informManager  (const DirectXErrorHandler*, HRESULT);
};

#endif

#define TRYRENDX( dx_call ) TRYDX( RenDXErrorHandler::defaultHandler(), dx_call )

/* End RENERROR.HPP *************************************************/
