// Slimline version of <windows.h>. Defines things required by header files to avoid
// including the whole of <windows.h>.

//#ifndef _WINDOWS_ // Shared with <windows.h>
#ifndef _WINDEF_H
// Don't #define _WINDOWS_ otherwise any subsiquent #include <windows.h> won't work.
/*
#ifdef STRICT
typedef void *HANDLE;
#define DECLARE_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name
#else
typedef void *HANDLE;
#define DECLARE_HANDLE(name) typedef HANDLE name
#endif

#ifndef VOID
#define VOID void
typedef char CHAR;
typedef short SHORT;
typedef long LONG;
#endif

typedef CHAR *LPSTR, *PSTR;

typedef unsigned int        UINT;

typedef UINT WPARAM;
typedef LONG LPARAM;
typedef LONG LRESULT;
typedef unsigned short      WORD;

DECLARE_HANDLE(HWND);
DECLARE_HANDLE(HINSTANCE);

#if (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED)
#define CALLBACK    __stdcall
#define WINAPI      __stdcall
#define WINAPIV     __cdecl
#define APIENTRY    WINAPI
#define APIPRIVATE  __stdcall
#define PASCAL      __stdcall
#else
#define CALLBACK
#define WINAPI
#define WINAPIV
#define APIENTRY    WINAPI
#define APIPRIVATE
#define PASCAL      pascal
#endif
*/
#endif
