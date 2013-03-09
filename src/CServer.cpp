// I N C L U D E //////////////////////////////////////////////////////////////

#include "CServer.h"

// S T A T I C   I N I T //////////////////////////////////////////////////////

int g_count = 0;
SChildWnd::SChildWnd() { id = ++g_count; };

// F U N C T I O N S //////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// CServer - constructor
//-----------------------------------------------------------------------------

CServer::CServer( bool dServ )
{
  dedicated = dServ;

  ds.fov = 0.0;
  ds.width = 600;
  ds.height = 440;
  ds.aspect = 0.0;
  ds.fullscreen = 0;
  ds.colorDepth = 0;

  ds.xPos = ((GetSystemMetrics( SM_CXFULLSCREEN ) - ds.width)  >> 1);
  ds.yPos = ((GetSystemMetrics( SM_CYFULLSCREEN ) - ds.height) >> 1);
}

//-----------------------------------------------------------------------------
// CServer::OpenWindow - Creates a window for the console
//-----------------------------------------------------------------------------

bool CServer::OpenWindow( WNDPROC proc, char *title )
{
//-----------------
// Parent Window
//-----------------


  ds.wndProc = proc;
  strcpy( ds.wndTitle, title );
  if( display.init( ds, WS_EX_CLIENTEDGE, 0 ) != 1 )
  {
    error.msg( "SERVER ERROR: Unable to create parent window." );
    return (false);
  }


//-----------------
// Child Windows
//-----------------


// Dynamic window sizing
  int margin = 6;
  int width  = display.getWidth() - ((margin << 1) + 10);
  int height = display.getHeight() - GetSystemMetrics( SM_CYSIZE );
  static HINSTANCE hInstance = GetModuleHandle( NULL );


// Input Window
  if( NULL == 
    (wndIn.hWnd = CreateWindowEx( WS_EX_CLIENTEDGE,
    "EDIT", NULL,
    WS_CHILD | WS_VISIBLE | WS_BORDER | 
    ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_WANTRETURN, 
    margin, height - margin - 32, width, 24,
    display.getHandle(), (HMENU) wndIn.id, hInstance, NULL )) )
  {
   error.msg( "SERVER ERROR: Unable to create child (input) window." );
   return (false);
  }
  else
  {
    SendMessage( wndIn.hWnd, EM_LIMITTEXT, (CONSOLE_COLS - 2), 0 );
  }


// Output1 (game)
  if( NULL == 
    (wndOutGame.hWnd = CreateWindowEx( WS_EX_CLIENTEDGE,
    "LISTBOX", NULL,
    WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER |
    LBS_NOSEL | LBS_DISABLENOSCROLL, 
    margin, margin + 8, width, (height >> 1) - (margin + 32),
    display.getHandle(), (HMENU) wndOutGame.id, hInstance, NULL )) )
  {
   error.msg( "SERVER ERROR: Unable to create child (client output) window." );
   return (false);
  }


// Output2 (server)
  if( NULL == 
    (wndOutServ.hWnd = CreateWindowEx( WS_EX_CLIENTEDGE,
    "LISTBOX", NULL,
    WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER |
    LBS_NOSEL | LBS_DISABLENOSCROLL,
    margin, (height >> 1) - 8,
    width, (height >> 1) - (margin + 32),
    display.getHandle(), (HMENU) wndOutServ.id,
    hInstance, NULL )) )
  {
   error.msg( "SERVER ERROR: Unable to create child (server output) window." );
   return (false);
  }

//-----------------

  return (true);
}

//-----------------------------------------------------------------------------
// Func: Update
//-----------------------------------------------------------------------------

bool CServer::update()
{
  return (true);
}

//-----------------------------------------------------------------------------
// Func: getCommand - deals with WM_COMMAND
//-----------------------------------------------------------------------------

void CServer::getCommand( WPARAM wParam, LPARAM lParam )
{
  char cmdString[256] = {NULL};

  if ( LOWORD (wParam) == wndIn.id )
  {
    switch ( HIWORD (wParam) )
    {
      case EN_CHANGE:
      {
        if ( SendMessage( wndIn.hWnd, EM_GETLINECOUNT, 0, 0 ) > 1 )
        {
          *((LPWORD)cmdString) = SendMessage( wndIn.hWnd, EM_LINELENGTH, 0, 0 );
          SendMessage( wndIn.hWnd, EM_GETLINE, 0, (LPARAM) cmdString );
          SendMessage( wndIn.hWnd, EM_SETSEL, 0, -1 );
          SendMessage( wndIn.hWnd, EM_REPLACESEL, 0, (LPARAM) "" );

          console->executeCommand( cmdString );
        }
      } // EN_CNANGE
    } // end switch HIWORD
  } // end if LOWORD
}

// E O F //////////////////////////////////////////////////////////////////////