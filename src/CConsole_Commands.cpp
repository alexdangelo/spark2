
// I N C L U D E //////////////////////////////////////////////////////////////

#include "Global.h"
#include "CConsole.h"

#include "CScene.h"   // model / ship / track loading & unloading

// F U N C T I O N S //////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// Func: ConCmdClear +++ CONSOLE ONLY +++ (clear)
// Desc: Clear the console buffer
// More: PARAM: NONE
// Rtrn: Success = 1
//-----------------------------------------------------------------------------

bool CConsole::ConCmdClear(char *szCmdParam)
{
  for(int i = 0; i < CONSOLE_ROWS; i++)
    for(int j = 0; j < CONSOLE_COLS; j++)
      TextBuffer[i].text[j] = NULL;

  TextBuffer[0].text[0] = ']';
  TextBuffer[0].text[1] = '\0';

  CurrentCol = 1;

  return(1);
}

//-----------------------------------------------------------------------------
// Func: ConCmdBind (bind)
// Desc: Bind a key to a command (or string of commands)
// More: PARAM: string to echo
// Rtrn: Success = 1
//-----------------------------------------------------------------------------

bool CConsole::ConCmdBind(char *szCmdParam)
{
  write("TODO: bind");

  return(1);
}

//-----------------------------------------------------------------------------
// Func: ConCmdCrash
// Desc: Crash... or really reboot.
//-----------------------------------------------------------------------------

bool CConsole::ConCmdCrash(char *szCmdParam)
{
 // error.crash();

  return(1); // even though there will be no returning, heheh
}

//-----------------------------------------------------------------------------
// Func: ConCmdEcho (echo)
// Desc: Echo a string to the console
// More: PARAM: string to echo
// Rtrn: Success = 1
//-----------------------------------------------------------------------------

bool CConsole::ConCmdEcho(char *szCmdParam)
{
  // sure there was input
  if( TextBuffer[0].text[5] != '\0' )
  {
    int valid = 0; // check that the input isn't just space
    for( int x = 0; x < CONSOLE_COLS && !valid && szCmdParam[x] != '\0'; x++ )
      if( szCmdParam[x] != ' ' )
        valid = 1;

    if( valid )
      write(szCmdParam);
  }
  return(1);
}

//-----------------------------------------------------------------------------
// Func: ConCmdExec (exec)
// Desc: Executes a file
// More: PARAM: file to execute
// Rtrn: Success = 1, Failure = 0
//-----------------------------------------------------------------------------

bool CConsole::ConCmdExec(char *szCmdParam)
{
  write("TODO: Exec File");
  return(1);
}

//-----------------------------------------------------------------------------
// Func: ConCmdExit +++ CONSOLE ONLY +++ (exit)
// Desc: Closes (exits) the console
// More: PARAM: NONE
// Rtrn: Success = 1
//-----------------------------------------------------------------------------

bool CConsole::ConCmdExit(char *szCmdParam)
{
  deactivate();
  return(1);
}

//-----------------------------------------------------------------------------
// Func: ConCmdList +++ CONSOLE ONLY +++ (cmdlist)
// Desc: Lists all of the console commands
// More: TODO: add PARAM: # to list at a time & pause when screen is full
// Rtrn: Success = 1
//-----------------------------------------------------------------------------

bool CConsole::ConCmdList(char *szCmdParam)
{
  for(int i = 0; (i < NUM_COMMANDS) && (CommandList[i].function != NULL); i++)
    write(CommandList[i].szCommand);
  return(1);
}

//-----------------------------------------------------------------------------
// Func: ConCmdClear (quit)
// Desc: Terminate the program
// Rtrn: Success = 1
//-----------------------------------------------------------------------------

bool CConsole::ConCmdQuit(char *szCmdParam)
{
  SendMessage(hWnd, WM_CLOSE, 0, 0);
  return(1);
}

//-----------------------------------------------------------------------------
// Func: ConCmdTrack (track)
// Parm: track name (with or without the extension)
// Rtrn: Success = 1
// Desc: load a track
//-----------------------------------------------------------------------------

bool CConsole::ConCmdTrack(char *szCmdParam)
{
  // Splash Screen ON

  // De-Allocate old Ships.

  // De-Allocate old Track
  // client->scene.unload(); // I need to get access to the client!

  // Allocate new Ships.

  // Allocate new Track.
  //client->scene.load(szCmdParam); // I need to get access to the client!

  // Splash Screen OFF

  return(1);
}

// E O F //////////////////////////////////////////////////////////////////////