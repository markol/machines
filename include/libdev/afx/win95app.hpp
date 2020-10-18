#ifndef _FRAMEWORK_AfxWin95App_INCLUDED
#define _FRAMEWORK_AfxWin95App_INCLUDED

#include "afx/app.hpp"
#include "afx/slimwin.hpp"
//class ostream;

//  This is necessary to stop the compiler complaining
//  about different linkage specifications for the WinMain function.
extern "C" int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);

// An abstraction of an application running under Windows 95.
class AfxWin95App : public AfxApp
{
public:
	static AfxWin95App& win95Instance();
	HWND window()	{ return hWnd_; }

protected:
	virtual bool OSStartup ();
	virtual void OSShutdown();

	AfxWin95App();
	virtual ~AfxWin95App();

	virtual void finish();
	virtual bool isFinished() const;
	virtual void testPrint(const char*) const;

	void moveWindow(int ox, int oy);

private:
	// Allow initialisation by this fn only:
	friend WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
	void initialise(HINSTANCE hInstance, LPSTR lpCmdLine, int nCmdShow);
	virtual void coreLoop();

	// Called by the framework to determine the client's required window size.
	// Before the call, the values are initialised to sensible defaults.  If the
	// function doesn't set any of the parameters, then it will get a default value.
	virtual void getWindowSize(int& ox, int& oy, int& w, int& h);

	// Call by the framework when this app's window is activated.
	// Return true to indicate that the message was dealt with.
	virtual bool activate(WORD /* wParam */) { return false; }

	// Internal methods.
	void dispatchKeyMsg(bool, WPARAM, LPARAM, char=0);
	void dispatchMouseMsg(bool press, int button);
	void dispatchCharMsg( WPARAM, LPARAM );

	HWND		hWnd_;
	HINSTANCE	hInstance_;
	int			iCmdShow_;
	bool		finished_;
	bool		finishing_;
};

#endif // _FRAMEWORK_AfxWin95App_INCLUDEDtion when c
