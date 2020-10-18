/*
 * R E N E R R O R . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "render/renerror.hpp"
// #include "world4d/manager.hpp"

RenDXErrorHandler::RenDXErrorHandler()
{
	// Use these different default handlers.
	changeDefaultNonTerminal(informManager);
	changeDefaultTerminal(shutdownDirectX);
}

// static 
RenDXErrorHandler::Action RenDXErrorHandler::shutdownDirectX
(
	const DirectXErrorHandler* handler, 
	HRESULT hresult
)
{
	printTerminalMsg(handler, hresult);
//	RenManager::instance().emergencyShutdown();
	return TERMINAL;
}

// static 
RenDXErrorHandler::Action RenDXErrorHandler::informManager
(
	const DirectXErrorHandler* handler, 
	HRESULT hresult
)
{
	printNonTerminalMsg(handler, hresult);
//	RenManager::instance().renderingSuspended();
	return NON_TERMINAL;
}

//static
RenDXErrorHandler& RenDXErrorHandler::defaultHandler()
{
    static  RenDXErrorHandler renErr;
    
    return renErr;
}

/* End RENERROR.CPP *************************************************/
