/////////////////////////////////////////////////////////////////////////////
// CDisplay.h
// Created: April 14, 2000 - 4:30pm - Ryan McGrail
/////////////////////////////////////////////////////////////////////////////

#ifndef _CDISPLAY_H
#define _CDISPLAY_H

// I N C L U D E ////////////////////////////////////////////////////////////

#include "Global.h"
#include "resource.h"

// T Y P E D E F / S T R U C T / C L A S S //////////////////////////////////

struct SDisplay
{

  byte  fullscreen;
  u16   colorDepth;
  u16   xPos, yPos;
  u16   width, height;
  char  wndTitle[128];
  float fov, aspect;

//-----------------
// Windows params
//-----------------
  
  HDC hDC;
  HWND hWnd;
  HGLRC hRC;
  WNDPROC wndProc;

};

class CDisplay
{
public:

  CDisplay();
  ~CDisplay();

  bool init( SDisplay &displayStruct, long exWndParams = 0, u8 initGL = 1 );  
  bool shutdown();

  // Public get
  bool hasFocus()     { return focus;         }
  HDC  &getHDC()      { return params.hDC;    }
  HWND &getHandle()   { return params.hWnd;   }
  u16  getWidth()     { return params.width;  }
  u16  getHeight()    { return params.height; }

  // Public set
  bool setOrtho();
  bool setProspective();
  void setFocus( bool f ) { focus = f; };

private:

  bool focus;
  SDisplay params;
  
  // Private setup
  bool setupWindow( long extraWndParams = 0 );
  bool setupOpenGL();
  bool displayMode();
};

/////////////////////////////////////////////////////////////////////////////

#endif // _CAUDIO_H

// E O F ////////////////////////////////////////////////////////////////////