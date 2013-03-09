/////////////////////////////////////////////////////////////////////////////
// CServer.h
// Created: April 11, 2000 - 6:17pm - Ryan McGrail
// The Server Class, bascially just encapselates a bunch of other classes;
/////////////////////////////////////////////////////////////////////////////

#ifndef _CSERVER_H
#define _CSERVER_H

// I N C L U D E ////////////////////////////////////////////////////////////

#include "Global.h"
#include "CConsole.h"
#include "CDisplay.h"
#include "CNetwork.h"

// T Y P E D E F / S T R U C T / C L A S S //////////////////////////////////

struct SChildWnd
{
  int id;
  HWND hWnd;
  SChildWnd();
};

class CServer : public CNetwork
{
public:
  friend class CConsole;
  friend class CConsoleDlg;
  friend class CConsoleOGL;

  CServer( bool dedicatedServer );
  bool OpenWindow( WNDPROC proc, char *title );

  bool update();
  void getCommand( WPARAM wParam, LPARAM lParam );

  bool isDedicated() { return (dedicated); };

  CDisplay display;

  SChildWnd wndIn;
  SChildWnd wndOutGame;
  SChildWnd wndOutServ;

protected:

private:
  bool dedicated;

  SDisplay ds;
} extern *server;

/////////////////////////////////////////////////////////////////////////////

#endif // _CSERVER_H

// E O F ////////////////////////////////////////////////////////////////////