/////////////////////////////////////////////////////////////////////////////
// Global.h
// Created: April 7, 2000 - 3:48am - Ryan McGrail
/////////////////////////////////////////////////////////////////////////////

#ifndef _GLOBAL_H
#define _GLOBAL_H

// D I S A B L E   W A R N I N G S //////////////////////////////////////////

#pragma warning( disable : 4244 ) // conversion from '__int32' to 'float'
#pragma warning( disable : 4098 ) // defaultlib "LIBC" conflicts w/other libs

// L I B A R I E S //////////////////////////////////////////////////////////

#ifdef _MSC_VER

// Windows sockets v2
#pragma comment( lib, "ws2_32.lib" )

// OpenGL
#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glu32.lib" )
//#pragma comment( lib, "glut.lib" )   // temp for quick model creation Jul 23, 2000
#pragma comment( lib, "glut32.lib" ) // temp for quick model creation Jul 23, 2000
#pragma comment( lib, "fmodvc.lib" ) // FMOD music + wav player
#endif // _MSC_VER

// D E F I N E ////////////////////////////////////////////////////////////////

#define VK_TILDE 0x0C0

#define WIN32_LEAN_AND_MEAN // windows.h

#define PI                  3.14159265358979323846
#define PIOVER180           0.01745329251994329576
#define ONE_EIGHTY_OVER_PI  57.2957795130823208768

// M A C R O S ////////////////////////////////////////////////////////////////

#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEYUP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } } // ~= free();
#define SAFE_DELARY(p)  { if(p) { delete [] (p);  (p)=NULL; } } // ~= free();
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } } // DX Release

// I N C L U D E //////////////////////////////////////////////////////////////
#include <winsock2.h> // <-- #include <windows.h>
#include <fstream>

//-----------------
// OpenGL
//-----------------

#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glpng.h>
#include <gl/glut.h>

//-----------------
// Local
//-----------------
#include "fmod.h" // sound + music
#include "Types.h"
#include "CError.h"

// P R O T O T Y P E S ////////////////////////////////////////////////////////
// These shouldn't be here, but they don't fit any where else...

char *strrlc( char *string, char check = ' ' );
char *strrtc( char *string, char check = ' ' );

///////////////////////////////////////////////////////////////////////////////

#endif // _GLOBAL_H

// E O F //////////////////////////////////////////////////////////////////////