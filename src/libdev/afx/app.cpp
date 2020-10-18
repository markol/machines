#include "afx/app.hpp"

AfxApp::AfxApp()
{
}

// virtual 
AfxApp::~AfxApp()
{
}

AfxApp::ExitStatus AfxApp::run()
{
	if (startup())
	{
		coreLoop();
		shutdown();
	}

	return exitStatus();
}

// virtual 
bool AfxApp::startup()
{
	if (not OSStartup())
		return false;
		
	if (not clientStartup())
	{
		OSShutdown();
		return false;
	}
	
	clientStarted();
	
	return true;
}

// virtual 
void AfxApp::shutdown()
{
	clientShutdown();
	OSShutdown();
}

AfxApp::ExitStatus AfxApp::exitStatus() const
{
	return exitStatus_;
}

void AfxApp::exitStatus(AfxApp::ExitStatus es)
{
	exitStatus_ = es;
}

void AfxApp::setArgs(const AfxInvokeArgs& a)
{
	args_ = a;
}

// virtual
const string& AfxApp::name() const
{
	static string n = "Way Cool Charybdis Game";
	return n;
}
