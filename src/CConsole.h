/////////////////////////////////////////////////////////////////////////////
// CConsole.h
// Created: April 11, 2000 - 6:08pm - Ryan McGrail
/////////////////////////////////////////////////////////////////////////////

#ifndef _CONSOLE_H
#define _CONSOLE_H

// D E F I N E ////////////////////////////////////////////////////////////////

#define CONSOLE_WINDLG 1
#define CONSOLE_OPENGL 2
#define CONSOLE_DRCTX8 3

// I N C L U D E //////////////////////////////////////////////////////////////
// More includes below "class CConsole"

#include "Global.h"

// T Y P E D E F / S T R U C T / C L A S S ////////////////////////////////////
// CConsole is a friend to client and server.

class CServer; // included below
class CClient; // included below

class CConsole
{
  public:
    CConsole( CClient *c, CServer *s );
    virtual ~CConsole();

    // I/O
    virtual void draw() = 0;
    virtual void advanceLine() = 0;
    virtual void print( int send, char *outStr, ... ) = 0;
    virtual void getCommand( WPARAM wParam, LPARAM lParam ) = 0;

    // Command Execution
    virtual bool executeCommand( char *cmdString ) = 0;

    // State
    virtual bool splash( int value = (-1) ) = 0; // OpenGL ONLY
    virtual bool active() = 0;            // OpenGL ONLY
    virtual void activate() = 0;          // OpenGL ONLY
    virtual void deactivate() = 0;        // OpenGL ONLY

  protected:
    CClient *client;
    CServer *server;

  private:
    // Command Functions, should these be private?
    virtual bool CmdClear(char *szCmdParam) = 0; // Base Command
    virtual bool CmdCrash(char *szCmdParam) = 0; // Base Command
    virtual bool CmdBind (char *szCmdParam) = 0; // Base Command
    virtual bool CmdEcho (char *szCmdParam) = 0; // Base Command
    virtual bool CmdExec (char *szCmdParam) = 0; // Base Command
    virtual bool CmdExit (char *szCmdParam) = 0; // Base Command
    virtual bool CmdList (char *szCmdParam) = 0; // Base Command
    virtual bool CmdQuit (char *szCmdParam) = 0; // Base Command
    virtual bool CmdTrack(char *szCmdParam) = 0; // Base Command
    virtual bool CmdShip (char *szCmdParam) = 0; // Base Command

} extern *console;  // extern in Global.h

// I N C L U D E //////////////////////////////////////////////////////////////

#include "CClient.h"
#include "CServer.h"

#include "CConsoleDlg.h"
#include "CConsoleOGL.h"

///////////////////////////////////////////////////////////////////////////////

#endif // _CONSOLE_H

// E O F //////////////////////////////////////////////////////////////////////