
// I N C L U D E //////////////////////////////////////////////////////////////

#include "Global.h"
#include "CScene.h"   // model / ship / track loading & unloading
#include "CConsoleOGL.h"

// F U N C T I O N S //////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// Func: ConCmdClear +++ CONSOLE ONLY +++ (clear)
// Desc: Clear the console buffer
// More: PARAM: NONE
// Rtrn: Success = 1
//-----------------------------------------------------------------------------

bool CConsoleOGL::CmdClear(char *szCmdParam)
{
  for(int i = 0; i < CONSOLE_ROWS; i++)
    for(int j = 0; j < CONSOLE_COLS; j++)
      TextBuffer[i].text[j] = NULL;

  TextBuffer[0].text[0] = ']';
  TextBuffer[0].text[1] = '\0';

  CurrentCol = 1;

  return(true);
}

//-----------------------------------------------------------------------------
// Func: ConCmdBind (bind)
// Desc: Bind a key to a command (or string of commands)
// More: PARAM: string to echo
// Rtrn: Success = 1
//-----------------------------------------------------------------------------

bool CConsoleOGL::CmdBind(char *szCmdParam)
{
  print( 0, "TODO: bind" );
  return(true);
}

//-----------------------------------------------------------------------------
// Func: ConCmdCrash
// Desc: Crash or really reboot (if I can get it to wrok...)
//-----------------------------------------------------------------------------

bool CConsoleOGL::CmdCrash(char *szCmdParam)
{
 // error.crash();
  return(true); // even though there will be no returning, heheh
}

//-----------------------------------------------------------------------------
// Func: ConCmdEcho (echo)
// Desc: Echo a string to the console
// More: PARAM: string to echo
// Rtrn: Success = 1
//-----------------------------------------------------------------------------

bool CConsoleOGL::CmdEcho(char *szCmdParam)
{
  // sure there was input
  if( TextBuffer[0].text[5] != '\0' )
  {
    int valid = 0; // check that the input isn't just space
    for( int x = 0; x < CONSOLE_COLS && !valid && szCmdParam[x] != '\0'; x++ )
      if( szCmdParam[x] != ' ' )
        valid = 1;

    if( valid )
      print( 0, szCmdParam );
  }
  return(true);
}

//-----------------------------------------------------------------------------
// Func: ConCmdExec (exec)
// Desc: Executes a file
// More: PARAM: file to execute
// Rtrn: Success = 1, Failure = 0
//-----------------------------------------------------------------------------

bool CConsoleOGL::CmdExec(char *szCmdParam)
{
  print( 0, "TODO: Exec File" );
  return(true);
}

//-----------------------------------------------------------------------------
// Func: ConCmdExit +++ CONSOLE ONLY +++ (exit)
// Desc: Closes (exits) the console
// More: PARAM: NONE
// Rtrn: Success = 1
//-----------------------------------------------------------------------------

bool CConsoleOGL::CmdExit(char *szCmdParam)
{
  deactivate();
  return(true);
}

//-----------------------------------------------------------------------------
// Func: ConCmdList +++ CONSOLE ONLY +++ (cmdlist)
// Desc: Lists all of the console commands
// More: TODO: add PARAM: # to list at a time & pause when screen is full
// Rtrn: Success = 1
//-----------------------------------------------------------------------------

bool CConsoleOGL::CmdList(char *szCmdParam)
{
  for(int i = 0; (i < NUM_COMMANDS) && (CommandList[i].function != NULL); i++)
    print( 0, CommandList[i].szCommand );
  return(true);
}

//-----------------------------------------------------------------------------
// Func: ConCmdClear (quit)
// Desc: Terminate the program
// Rtrn: Success = 1
//-----------------------------------------------------------------------------

bool CConsoleOGL::CmdQuit(char *szCmdParam)
{
  SendMessage(hWnd, WM_CLOSE, 0, 0);
  return(true);
}

//-----------------------------------------------------------------------------
// Func: ConCmdTrack (track)
// Parm: track filename (with or without the extension)
// Rtrn: Success = 1
// Desc: loads a track
//-----------------------------------------------------------------------------

bool CConsoleOGL::CmdTrack(char *szCmdParam)
{
  bool goodToGo = true;
  char *allocFileName = new char[ strlen(szCmdParam)+1 ];
  strcpy( allocFileName, szCmdParam );

//-------------------
// Tokenize Path
//-------------------
   
  _strrev( allocFileName );                  // reverse the string

  char *token = strtok(allocFileName, " /\"\0\\"); // search for the first slash or quote

  if(token != NULL)
  {
    _strrev(token);                     // flip the filename around again
                                        //  to get the correct name      
    strcpy(allocFileName, token);         // copy back into the cmdParam, just to make sure
    allocFileName[strlen(allocFileName) + 1] = NULL; // terminate it! to be really sure
  }
  else
  {
    print( 0, "Invalid Filename!" );
    SAFE_DELARY( allocFileName );
    goodToGo = false;
    return false;
  }

//-------------------
// Append new path
//  & extension
//-------------------

  char *fileAndPath = NULL;

  int pathSize = strlen("tracks/") + strlen(allocFileName) + strlen(".ase") + 1;
  fileAndPath = new char[ pathSize ];
  memset(fileAndPath, NULL, pathSize);

  strcpy(fileAndPath, "tracks/");
  strcat(fileAndPath, allocFileName);
  
  // case-insensitive scan for .ASE ending
  if( strncmp(_strrev(_strlwr(fileAndPath)), "esa.", 4) ) // if the extension isn't there
    strcat(_strrev(fileAndPath), ".ase");  // reverse it back and append extension
  else
    _strrev(fileAndPath); // reverse it back anyway
  
 // SAFE_DELARY(allocFileName); // this stopped working all of a sudden!

//-------------------
// Test to see if the
//  file exists
//-------------------

  if( this->client->scene.openFile(fileAndPath) )
      this->client->scene.closeFile();
  else
  {
    print( 0,  "Couldn't open %s", fileAndPath );
    error.msg( "Couldn't open %s", fileAndPath );
    SAFE_DELARY( fileAndPath );
    goodToGo = false;
    return false;
  }
  
  if( goodToGo == true )
  {
    error.msg( "--------------------------------" );
    error.msg( "CLIENT: Attempting to load track %s", szCmdParam, "..." );
    print( 0,  "--------------------------------" );
    print( 0,  "Loading track...");

    // We make sure the new track exists before unloading the old track
    // I think the string parsing and verification should be in here, not in
    // scene.load(filename). That makes everything easier, since the way everything
    // is set up right now we have to essentially run through scene.load twice.
    // Same goes for the ship

    console->splash( 1 ); // turn on the splash screen

    this->client->scene.unload();
    //this->client->player.ship.unload();

    // Load the new
    if( this->client->scene.load(szCmdParam) )
    {
      this->client->player.ship.attachToScene(&this->client->scene); // I think we only have to do this once, in client.init()
      this->client->ai.attachToScene(&this->client->scene);          // same for this
    
      this->client->ai.unload();
      this->client->ai.loadShips();
      this->client->player.ship.reset();

      //this->client->player.ship.load("ships/ship01tc.ase"); // do error checking if loaded ok
      error.msg( "Track loaded successfully" );
      print( 0,  "Track loaded successfully");
    }
    else
    {
      error.msg( "CLIENT: Couldn't load track" );
      print( 0,  "CLIENT: Couldn't load track" );
      return false;
    }
    
    console->splash( 0 ); // turn off the splash screen
  }

  return true;
}

//-----------------------------------------------------------------------------
// Func: ConCmdShip (ship)
// Parm: ship filename (with or without the extension)
// Rtrn: Success = 1
// Desc: loads a ship
//-----------------------------------------------------------------------------

bool CConsoleOGL::CmdShip(char *szCmdParam)
{
  bool goodToGo = true;
  char *allocFileName = new char[ strlen(szCmdParam)+1 ];
  strcpy( allocFileName, szCmdParam );

//-------------------
// Tokenize Path
//-------------------
   
  _strrev( allocFileName );                  // reverse the string

  char *token = strtok(allocFileName, " /\"\0\\"); // search for the first slash or quote

  if(token != NULL)
  {
    _strrev(token);                     // flip the filename around again
                                        //  to get the correct name      
    strcpy(allocFileName, token);         // copy back into the cmdParam, just to make sure
    allocFileName[strlen(allocFileName) + 1] = NULL; // terminate it! to be really sure
  }
  else
  {
    print( 0, "Invalid Filename!" );
    SAFE_DELARY( allocFileName );
    goodToGo = false;
    return false;
  }

//-------------------
// Append new path
//  & extension
//-------------------

  char *fileAndPath = NULL;

  int pathSize = strlen("ships/") + strlen(allocFileName) + strlen(".ase") + 1;
  fileAndPath = new char[ pathSize ];
  memset(fileAndPath, NULL, pathSize);

  strcpy(fileAndPath, "ships/");
  strcat(fileAndPath, allocFileName);
  
  // case-insensitive scan for .ASE ending
  if( strncmp(_strrev(_strlwr(fileAndPath)), "esa.", 4) ) // if the extension isn't there
    strcat(_strrev(fileAndPath), ".ase");  // reverse it back and append extension
  else
    _strrev(fileAndPath); // reverse it back anyway
  
 // SAFE_DELARY(allocFileName); // this stopped working all of a sudden!

//-------------------
// Test to see if the
//  file exists
//-------------------

  if( this->client->player.ship.openFile(fileAndPath) )
      this->client->player.ship.closeFile();
  else
  {
    print( 0, "Couldn't open %s", fileAndPath );
    error.msg( "Couldn't open %s", fileAndPath );
    SAFE_DELARY( fileAndPath );
    goodToGo = false;
    return false;
  }

  // the filename was ok, so we'll load the ship

  if( goodToGo == true )
  {
    print( 0, "Loading ship..." );
    
    // Unload everything

    // look at the comment for CmdTrack above!
    // We make sure the new ship exists before unloading the old ship
    // then
    this->client->player.ship.unload();

    // Load everything
    bool success = 0;
    if( this->client->scene.initialized )
    {
      this->client->player.ship.attachToScene( &this->client->scene );
      success = this->client->player.ship.load( szCmdParam );
    }
    else
    {
      error.msg( "ERROR: Scene not loaded!" );
      print( 0,  "ERROR: Scene not loaded!" ); // was printf(0, "...") whoops!
      success = 0;
    }
  
    if(success)
    {
      print( 0, "Ship loaded successfully!");
    }
    else
    {
      this->client->player.ship.load( NULL );
      error.msg( "ERROR: Couldn't load ship" );
      print( 0,  "ERROR: Couldn't load ship" );
    }
  }
  return(true);
}

// E O F //////////////////////////////////////////////////////////////////////