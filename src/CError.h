
#ifndef _CERROR_H
#define _CERROR_H

// D E F I N E ////////////////////////////////////////////////////////////////

#define MAX_LENGTH 1024

// I N C L U D E //////////////////////////////////////////////////////////////
#include <windows.h>
#include <fstream>
#include <stdio.h> // sprintf
#include <time.h>  // _strtime

//#include "CConsole.h" // <--- Spark specific

#ifndef __GL_H__
# include <gl/gl.h>
# ifdef _MSC_VER
#   pragma warning( disable : 4089 ) // references to "OPENGL32.dll" discarded
#   pragma comment( lib, "opengl32.lib" )
# endif
#endif

using namespace std;

// T Y P E D E F / S T R U C T / C L A S S ////////////////////////////////////

class CError
{
public:
  CError();
  void setPath( char *newPath ) { strcpy_s( path, newPath ); };

  bool isFirstRun() { return firstRun; };

  __forceinline void kill();
  __forceinline void crash();

  void abort( HWND hWnd ) { abort(hWnd); };
  void exit ( HWND hWnd ) { SendMessage(hWnd, WM_CLOSE, 0, 0); };
  
  void msg( char *string, ... );

  //long getLast( char *errorString = NULL );
  //long setLast( long lastError, char *errorString = NULL );
  
  bool getGLErrors();

protected:
/*
  struct SError
  {
    long error;
    char *text;
  }current;
*/
  bool firstRun;
  long tickLast;
  long tickCurr;
  char path[512];

} extern error;

///////////////////////////////////////////////////////////////////////////////

#endif // _CERROR_H

// E O F //////////////////////////////////////////////////////////////////////