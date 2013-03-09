///////////////////////////////////////////////////////////////////////////////
// CClient.h
// Created: April 7, 2000 - 3:03am - Ryan McGrail
// The Client Class, basically just encapsulates a bunch of other classes;
// keeps data for the LOCAL PLAYER organized.
///////////////////////////////////////////////////////////////////////////////

#ifndef _CCLIENT_H
#define _CCLIENT_H

// I N C L U D E //////////////////////////////////////////////////////////////

#include <stdlib.h>

#include "Global.h"
#include "CError.h"
#include "CServer.h"  // Check for local server.
#include "CNetwork.h"

#include "CMenu.h"
#include "CConsole.h"
#include "CDisplay.h"

#include "CScene.h"
#include "CPlayer.h"
#include "CCamera.h"

#include "CAi.h"

#include "Sys_time.h"

/*
#include "CAudio.h"
#include "CControls.h"
*/

// T Y P E D E F / S T R U C T / C L A S S ////////////////////////////////////

class CClient : public CNetwork
{
public:
  friend class CConsole;
  friend class CConsoleDlg;
  friend class CConsoleOGL;

//-----------------
// functions
//-----------------

  CClient( CServer *pServer = NULL );
  bool OpenWindow( WNDPROC proc, char *title );
  bool init();

  bool isServer()  { return ( (localServer == NULL) ? false : true ); };

  u16 update();
  u16 shutdown();
  u16 getInput( u8 vkey = NULL );

//-----------------
// encapsulation
//-----------------

  CDisplay display;
  
  CScene  scene;    // the scenery, track, etc
  CPlayer player;   // the player's stats and ship
  CAi     ai;       // the computer-controlled ships
  CCamera worldcam; // worldcam -- can either move freely or follow a model / ship
  
  CTimer  timer;    // sys_time.cpp/h from COTD at Flipcode

protected:
  CServer *localServer;   // ptr to the srvr, ! NULL then it's a local server.

  u16 lastError;
  bool loadConfigEx( SDisplay &dispStruct, bool applyChanges, char *path = NULL );
  u16 setError( u16 err ) { return (lastError = err); }

/*
  u16 addNetPlayer(playerInfo *pPI);
  u16 removeNetPlayer(PLAYER *currNetPlayer);
  u16 removeAllPlayers();
*/

} extern *client;

///////////////////////////////////////////////////////////////////////////////

#endif // _CCLIENT_H

// E O F //////////////////////////////////////////////////////////////////////