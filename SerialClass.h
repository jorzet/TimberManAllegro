
#ifndef SERIALCLASS_H_INCLUDED
#define SERIALCLASS_H_INCLUDED

#define ARDUINO_WAIT_TIME 2000

/*
#ifndef _WINDOWS_H
#define _WINDOWS_H
#if __GNUC__ >=3
#pragma GCC system_header
#endif
#if defined(__i686__) && !defined(_M_IX86)
#define _M_IX86 600
#elif defined(__i586__) && !defined(_M_IX86)
#define _M_IX86 500
#elif defined(__i486__) && !defined(_M_IX86)
#define _M_IX86 400
#elif defined(__i386__) && !defined(_M_IX86)
#define _M_IX86 300
#endif
#if defined(_M_IX86) && !defined(_X86_)
#define _X86_
#elif defined(_M_ALPHA) && !defined(_ALPHA_)
#define _ALPHA_
#elif defined(_M_PPC) && !defined(_PPC_)
#define _PPC_
#elif defined(_M_MRX000) && !defined(_MIPS_)
#define _MIPS_
#elif defined(_M_M68K) && !defined(_68K_)
#define _68K_
#endif

#ifdef RC_INVOKED
#include <winresrc.h>
#else

#include <stdarg.h>
#include <windef.h>
#include <wincon.h>
#include <winbase.h>
#if !(defined NOGDI || defined  _WINGDI_H)
//#include <wingdi.h>
#endif
#ifndef _WINUSER_H
//#include <winuser.h>
#endif
#ifndef _WINNLS_H
#include <winnls.h>
#endif
#ifndef _WINVER_H
#include <winver.h>
#endif
#ifndef _WINNETWK_H
#include <winnetwk.h>
#endif
#ifndef _WINREG_H
#include <winreg.h>
#endif
#ifndef _WINSVC_H
#include <winsvc.h>
#endif

#ifndef WIN32_LEAN_AND_MEAN
#include <cderr.h>
#include <dde.h>
#include <ddeml.h>
#include <dlgs.h>
//#include <imm.h>
#include <lzexpand.h>
#include <mmsystem.h>
#include <nb30.h>
#include <rpc.h>
#include <shellapi.h>
#include <winperf.h>
#ifndef NOGDI
//#include <commdlg.h>
//#include <winspool.h>
#endif
#if defined(Win32_Winsock)
#warning "The  Win32_Winsock macro name is deprecated.\
    Please use __USE_W32_SOCKETS instead"
#ifndef __USE_W32_SOCKETS
#define __USE_W32_SOCKETS
#endif
#endif
#if defined(__USE_W32_SOCKETS) || !(defined(__CYGWIN__) || defined(__MSYS__) || defined(_UWIN))
#if (_WIN32_WINNT >= 0x0400)
#include <winsock2.h>
#else
#include <winsock.h>
#endif 
#endif
#ifndef NOGDI

#if !defined (_OBJC_NO_COM) 
#if (__GNUC__ >= 3) || defined (__WATCOMC__)
//#include <ole2.h>
#endif
#endif
#endif

#endif 

#endif 

#ifdef __OBJC__
#undef BOOL
#endif

#endif


*/


//#include <windows.h>
//#include <stdio.h>
//#include <stdlib.h>

class Serial
{
    private:
        //Serial comm handler
        HANDLE hSerial;
        //Connection status
        bool connected;
        //Get various information about the connection
        COMSTAT status;
        //Keep track of last error
        DWORD errors;

    public:
        //Initialize Serial communication with the given COM port
        Serial(char *portName);
        //Close the connection
        ~Serial();
        //Read data in a buffer, if nbChar is greater than the
        //maximum number of bytes available, it will return only the
        //bytes available. The function return -1 when nothing could
        //be read, the number of bytes actually read.
        int ReadData(char *buffer, unsigned int nbChar);
        //Writes data from a buffer through the Serial connection
        //return true on success.
        bool WriteData(char *buffer, unsigned int nbChar);
        //Check if we are actually connected
        bool IsConnected();


};

#endif // SERIALCLASS_H_INCLUDED

