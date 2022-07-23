#ifndef _FRAMEWORK_DOSAPP_INCLUDED
#define _FRAMEWORK_DOSAPP_INCLUDED

#include "afx/app.hpp"

class AfxDosApp : public AfxApp
{
protected:
	AfxDosApp()					{}
	virtual ~AfxDosApp()			{}

	virtual bool OSStartup ();
	virtual void OSShutdown();
	virtual void finish();
	virtual bool isFinished() const;
	virtual void testPrint(const char*) const;

	static AfxDosApp& dosInstance();

private:
	friend int main(int, char*[]);
	void initialise(int argc, char* argv[]);
	virtual void coreLoop();
	virtual void clientStarted();

	bool		finished_;
};

typedef AfxDosApp MessDogApp;	// :-)

#endif // _FRAMEWORK_DOSAPP_INCLUDED