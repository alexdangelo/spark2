/////////////////////////////////////////////////////////////////////////////
// CConsoleOGL.h
// Created: April 11, 2000 - 6:08pm - Ryan McGrail
/////////////////////////////////////////////////////////////////////////////

#ifndef _GLCONSOLE_H
#define _GLCONSOLE_H

// D E F I N E ////////////////////////////////////////////////////////////////

//-----------------
// General limits
//-----------------

#define NUM_COMMANDS 32

#define CMDHIST_ROWS 32      // max number of saveable commands
#define CONSOLE_ROWS 512     // number of strings in the console
#define CONSOLE_COLS 127 + 1 // max string length in the console

//-----------------
// Font Info
// 256 16x16 chars
//-----------------

#define CONSOLE_FONTSIZE 16

//-----------------
// Console State
// 1 byte only (u8)
//-----------------

// Primary
#define CONSOLE_STATE_ACTIVE      0x01
#define CONSOLE_STATE_CREATING    0x02
#define CONSOLE_STATE_DESTROYING  0x04

// Secondary
#define CONSOLE_STATE_PAGEUP      0x10

// I N C L U D E //////////////////////////////////////////////////////////////

#include "Global.h"
#include "CConsole.h"

// T Y P E D E F //////////////////////////////////////////////////////////////

// used with future expantion in mind, i.e. color; as well as ease of access.
struct SConsoleString
{
  char text[CONSOLE_COLS];
};

class CConsoleOGL : public CConsole
{
  private:
    void construct( HWND WndHndl, int w, int h );
  public:

//-----------------
// Functions
//-----------------

    CConsoleOGL( HWND WndHndl, int w, int h, CClient *c, CServer *s ) : CConsole(c,s)
    { construct( WndHndl, w, h ); };
    ~CConsoleOGL() {};

    // I/O
    virtual void draw();
    virtual void advanceLine();
    virtual void print( int send, char *outStr, ... );
    virtual void getCommand( WPARAM wParam, LPARAM lParam );

    // Command Handling
    virtual bool executeCommand( char *cmdString );
    virtual bool addCommand( bool (CConsoleOGL::*function)(char *string), char *cmdString ){ return(true); }; // TODO: Write function.
    virtual bool remCommand( char *cmdString ){ return(true); }; // TODO: Write function.
    virtual void addToHistory( char *cmdString ){}; // TODO: Write function.
    
    // State
    virtual bool splash( int value = (-1) ); // Set and Get, it's a 2 for 1 deal!
    virtual bool active() { return( (State & (unsigned)0xFF) ? true : false ); };
    virtual void activate()   { State = ( (State & 0xF0) | CONSOLE_STATE_ACTIVE | CONSOLE_STATE_CREATING   ); };
    virtual void deactivate() { State = ( (State & 0xF0) | CONSOLE_STATE_ACTIVE | CONSOLE_STATE_DESTROYING ); };

//-----------------
// Cmnd Functions
//-----------------

    // CConsoleOGL_BaseCmds.cpp
    virtual bool CmdClear(char *szCmdParam);
    virtual bool CmdCrash(char *szCmdParam);
    virtual bool CmdBind (char *szCmdParam);
    virtual bool CmdEcho (char *szCmdParam);
    virtual bool CmdExec (char *szCmdParam);
    virtual bool CmdExit (char *szCmdParam);
    virtual bool CmdList (char *szCmdParam);
    virtual bool CmdQuit (char *szCmdParam);
    virtual bool CmdTrack(char *szCmdParam);
    virtual bool CmdShip (char *szCmdParam);

  private:

    void loadGfx();

    
    void drawString   (char *szString, _int32 row = 0, _int32 col = 0, float hOff = 0, float vOff = 0);
    void drawStringTWO(char *szString, _int32 row = 0, _int32 col = 0, float hOff = 0, float vOff = 0);

//-----------------
// Command handler
//-----------------

    struct SCommandFunction
    {
      bool (CConsoleOGL::*function)(char *szCommand);
      char szCommand[CONSOLE_COLS];
    } CommandList[NUM_COMMANDS];

    n32 dwCmdPoss;

//-----------------
// Gnereal Stuff
//-----------------

    HWND hWnd;

    u8   State;             // Console State, See above
    bool SplashState;       // Splash Screen State... not so fancy.

    n32 ScreenWidth;        // current screent width
    n32 ScreenHeight;       // current screent height

    n32 TickCurr;           // Time, Curr System Tick
    n32 TickLast;           // Time, Last System Tick
    n32 TickLastCursor;     // Cursor blink timer

    n8   CursorState;       // Cursor blink, on or off
    char CursorCharStr[2];  // replacement char on blink.

    float fVertPoss;        // current vertical possition
    float fHorzPoss;        // current vertical possition
    float fScrollStop;      // where the console stops
    float fScrollSpeed;     // scrolling animation speed

    GLuint uiBackIdent;     // console texture id
    GLuint uiFontIdent;     // TODO: remove, replace with CWinGLText, which still ahs to be written :)
    GLuint uiSplashIdent;   // Splash Screen id

//-------------------
// buffers / history
//-------------------

    n32 CurrentCol;

    n32 HistBufferViewLine;
    n32 HistBufferInputLine;

    n32 TextBufferInputLine;  // when pgup, remember the bottom
    n32 TextBufferRenderFrom; // render console text from here

    SConsoleString TextBuffer[CONSOLE_ROWS];
    SConsoleString HistBuffer[CMDHIST_ROWS];

};

//extern CConsoleOGL *tmpGCon; // Winmain.h - TEMP GL CONSOLE

///////////////////////////////////////////////////////////////////////////////

#endif // _GLCONSOLE_H

// E O F //////////////////////////////////////////////////////////////////////