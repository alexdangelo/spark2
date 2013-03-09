// I N C L U D E //////////////////////////////////////////////////////////////

#include "Global.h"
#include "CConsoleDlg.h"

// F U N C T I O N S //////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// Func: ConCmdClear
// Desc: Clear the text in the console output
// Parm: (game/server/""), clears specified output window,
//                         nothing specified clears both.
//-----------------------------------------------------------------------------

bool CConsoleDlg::CmdClear(char *cmdParam)
{
  int flag = 0;
  if ( (strcmp( cmdParam, "" ) == 0 ) || cmdParam == 0 )
  {
    flag = 0;
  }
  else
  {
    char skip[] = {" ,;\"\'"};
    cmdParam = strtok( cmdParam, skip );

    if ( strcmp( _strlwr(cmdParam) , "game" ) == 0 )
      flag = 1;
    else if ( strcmp( _strlwr(cmdParam) , "server" ) == 0 )
      flag = 2;
    else
      flag = 0;
  }

  switch ( flag )
  {
    case 0: SendMessage( hWnds->hWndO_serv, LB_RESETCONTENT, (WPARAM) 0, (LPARAM) 0 );
    case 1: SendMessage( hWnds->hWndO_game, LB_RESETCONTENT, (WPARAM) 0, (LPARAM) 0 );
      break;
    case 2: SendMessage( hWnds->hWndO_serv, LB_RESETCONTENT, (WPARAM) 0, (LPARAM) 0 );
      break;
    default: {};
  }

  return (true);
}

//-----------------------------------------------------------------------------
// Func: ConCmdBind (bind)
// Desc: Bind a key to a command (or string of commands)
// More: PARAM: string to echo
// Rtrn: Success = 1
//-----------------------------------------------------------------------------

bool CConsoleDlg::CmdBind(char *cmdParam)
{
  print( TO_SERV_WND, "TODO: bind" );
  return (true);
}

//-----------------------------------------------------------------------------
// Func: ConCmdCrash
// Desc: Crash... or really reboot.
//-----------------------------------------------------------------------------

bool CConsoleDlg::CmdCrash(char *cmdParam)
{
  // error.crash();
  return (true); // even though there will be no returning, heheh
}

//-----------------------------------------------------------------------------
// Func: ConCmdEcho (echo)
// Desc: Echo a string to the console
// More: PARAM: string to echo
// Rtrn: Success = 1
//-----------------------------------------------------------------------------

bool CConsoleDlg::CmdEcho(char *cmdParam)
{
  print( TO_SERV_WND, cmdParam );
  return (true);
}

//-----------------------------------------------------------------------------
// Func: ConCmdExec (exec)
// Desc: Executes a file
// More: PARAM: file to execute
// Rtrn: Success = 1, Failure = 0
//-----------------------------------------------------------------------------

bool CConsoleDlg::CmdExec(char *cmdParam)
{
  print( TO_SERV_WND, "TODO: Exec File" ); 
  return (true);
}

//-----------------------------------------------------------------------------
// Func: CmdExit (exit)
// Desc: Does nothing in this mode.
// More: PARAM: NONE
// Rtrn: Always successful
//-----------------------------------------------------------------------------

bool CConsoleDlg::CmdExit(char *cmdParam)
{
  return (true);
}

//-----------------------------------------------------------------------------
// Func: ConCmdList +++ CONSOLE ONLY +++ (cmdlist)
// Desc: Lists all of the console commands
// More: TODO: add PARAM: # to list at a time & pause when screen is full
// Rtrn: Success = 1
//-----------------------------------------------------------------------------

bool CConsoleDlg::CmdList(char *cmdParam)
{
  SCmdHandler *tempIndex = new SCmdHandler;
  
  for( tempIndex = cmdIndex->next; tempIndex != NULL; tempIndex = tempIndex->next )
  {
    print( TO_SERV_WND, tempIndex->cmdString );
  }

  return (true);
}

//-----------------------------------------------------------------------------
// Func: ConCmdQuit (quit)
// Desc: Quit/Leave/Terminate/Exit spark
//-----------------------------------------------------------------------------

bool CConsoleDlg::CmdQuit(char *cmdParam)
{
  SendMessage( hWnds->hWnd, WM_CLOSE, 0, 0 );
  return (true);
}

//-----------------------------------------------------------------------------
// Func: ConCmdTrack (track)
// Parm: track filename (with or without the extension)
// Rtrn: Success = 1
// Desc: loads a track
//-----------------------------------------------------------------------------

bool CConsoleDlg::CmdTrack(char *cmdParam)
{
  print( TO_SERV_WND, "Only applicable in client" );
  // Splash Screen ON

  // De-Allocate old Ships.

  // De-Allocate old Track

  // Allocate new Ships.

  // Allocate new Track.

  // Splash Screen OFF

  return(1);
}

//-----------------------------------------------------------------------------
// Func: ConCmdShip (ship)
// Parm: ship filename (with or without the extension)
// Rtrn: Success = 1
// Desc: loads a ship
//-----------------------------------------------------------------------------

bool CConsoleDlg::CmdShip(char *cmdParam)
{
  print( TO_SERV_WND, "Only applicable in client" );
  // Splash Screen ON

  // De-Allocate old Ships.

  // Allocate new Ships.

  // Splash Screen OFF

  return(1);
}

// E O F //////////////////////////////////////////////////////////////////////