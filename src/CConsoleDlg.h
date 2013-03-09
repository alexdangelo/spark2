// CConWin.h ////////////////////////////////////////////////////////////////

#ifndef _WINCON_H
#define _WINCON_H

// I N C L U D E ////////////////////////////////////////////////////////////

#include "Global.h"
#include "CConsole.h"
#include <stdlib.h>	
#include <stdio.h>  // sprintf

// D E F I N E //////////////////////////////////////////////////////////////

#define TO_SERV_WND 1
#define TO_GAME_WND 2

#define HISTORY_LENGTH 2

// T Y P E D E F / S T R U C T / C L A S S //////////////////////////////////

struct SConsoleDlg
{
  HWND hWnd;
  HWND hWndI;
  HWND hWndO_game;
  HWND hWndO_serv;
};

class CConsoleDlg : public CConsole
{
  private:
    void construct( SConsoleDlg *scw );
  public:
    CConsoleDlg( SConsoleDlg *scw, CClient *c, CServer *s ) : CConsole(c,s) 
      { construct( scw ); };
    ~CConsoleDlg() {}; // TODO: build this, lots of mem deallocation
    
  
    // I/O
    virtual void draw() {};
    virtual void advanceLine() {};
    virtual void print( int send, char *outStr, ... );
    virtual void getCommand( WPARAM wParam, LPARAM lParam ) {};

    // Command Execution
    virtual bool executeCommand( char *cmdString );

    // State, doesn't apply to this mode.
    virtual bool splash( int value = (-1) ) { return false; };
    virtual bool active()     { return(true); }; // this mode is always active.
    virtual void activate()   { error.msg( "WARNING: Cannot change console state in this mode." ); };
    virtual void deactivate() { error.msg( "WARNING: Cannot change console state in this mode." ); };

  protected:

    SConsoleDlg *hWnds;

//-------------------
// Command histroy
//-------------------

    struct SCmdHistory
    {
      char *cmdString;
      SCmdHistory *next;
      SCmdHistory *prev;

    } *historyIndex;

    virtual void addToHistory( char *cmdString );

//-------------------
// Command handling
//-------------------

    struct SCmdHandler
    {
      bool (CConsoleDlg::*function)(char *string);
      char *cmdString;
      SCmdHandler *next;
      SCmdHandler *prev;

    } *cmdIndex;

    // Sytem operations
    virtual bool addCommand( bool (CConsoleDlg::*function)(char *string), char *cmdString );
    virtual bool remCommand( char *cmdString );

    // Command Functions
    virtual bool CmdClear (char *cmdParam); // CConsoleDlg_BaseCmds.cpp
    virtual bool CmdCrash (char *cmdParam); // CConsoleDlg_BaseCmds.cpp
    virtual bool CmdBind  (char *cmdParam); // CConsoleDlg_BaseCmds.cpp
    virtual bool CmdEcho  (char *cmdParam); // CConsoleDlg_BaseCmds.cpp
    virtual bool CmdExec  (char *cmdParam); // CConsoleDlg_BaseCmds.cpp
    virtual bool CmdExit  (char *cmdParam); // CConsoleDlg_BaseCmds.cpp
    virtual bool CmdList  (char *cmdParam); // CConsoleDlg_BaseCmds.cpp
    virtual bool CmdQuit  (char *cmdParam); // CConsoleDlg_BaseCmds.cpp
    virtual bool CmdTrack (char *cmdParam); // CConsoleDlg_BaseCmds.cpp
    virtual bool CmdShip  (char *cmdParam); // CConsoleDlg_BaseCmds.cpp
};

//extern CConsoleDlg *tempCon; // TEMP CONSOLE

///////////////////////////////////////////////////////////////////////////////

#endif // _WINCONSOLE_H

// E O F //////////////////////////////////////////////////////////////////////