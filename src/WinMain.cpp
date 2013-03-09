///////////////////////////////////////////////////////////////////////////////
// WinMain.cpp
// Created: April 11, 2000 - 9:46pm - Ryan McGrail
///////////////////////////////////////////////////////////////////////////////

#include "WinMain.h"

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "fmod.h"
#include "fmod_errors.h"	// optional

// G L O B A L ////////////////////////////////////////////////////////////////

bool RunOK = true;
bool DServer = false;

CClient  *client  = NULL;  // extern in CClient.h
CServer  *server  = NULL;  // extern in CServer.h
CConsole *console = NULL;  // extern in CConsole.h

// W I N M A I N //////////////////////////////////////////////////////////////

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow)
{
  MSG msg; 
  
  // deleted before the Message Loop.
  char *title = new char[128];
  sprintf(title, "Spark - Built: %s", __DATE__ );

//-----------------
// Command Line
//-----------------

  for ( int argc = (__argc - 1); argc > 0 ; argc-- )
  {
    if ( strcmp( _strlwr( __argv[argc] ), "dedicated" ) )
      DServer = true;
  }

//-----------------
// Game Init
//-----------------
// Dedicated Server Init
  if ( DServer )
  {
    // Create Server
    server = new CServer( DServer );
    
    // Create Server's window
    strcat( title, " - Dedicated Server" );
    if ( !(RunOK = server->OpenWindow( (WNDPROC)ServerWndProc, title )) )
      error.msg( "SERVER ERROR: Initialization FAILED." );
    else
    {
      // Create Console
      SConsoleDlg scw;
      scw.hWnd = server->display.getHandle();
      scw.hWndI = server->wndIn.hWnd;
      scw.hWndO_game = server->wndOutGame.hWnd;
      scw.hWndO_serv = server->wndOutServ.hWnd;
      SendMessage( server->display.getHandle(), WM_SETFOCUS, 0, 0 );
      console = new CConsoleDlg( &scw, NULL, server );

      error.msg( "SERVER: Initialization Successful" );
    }
  }
// Client Init
  else 
  {
    //-----------------
    // Sound Init
    //-----------------
    if (FSOUND_GetVersion() < FMOD_VERSION)
    {
      error.msg("Error : You are using the wrong DLL version!  You should be using FMOD %.02f\n", FMOD_VERSION);
	    exit(1);
    }

    if(!FSOUND_Init(44100, 32, 0))
	  {
      error.msg("CLIENT: %s\n", FMOD_ErrorString(FSOUND_GetError()));
      return false;
	  }

    //-----------------
    // Create Client
    //-----------------
    client = new CClient( server );

    // Create Client's window
    strcat( title, " - Client" );
    if ( !(RunOK = client->OpenWindow( (WNDPROC)ClientWndProc, title )) )
    {
      error.msg( "CLIENT ERROR: Initialization FAILED.", RunOK );
    }
    else
    {
      // Create Console
      console = new CConsoleOGL( client->display.getHandle(), client->display.getWidth(), client->display.getHeight(), client, NULL );
      //console->activate();
      console->deactivate();
      
      // Finalize client init
      if( client->init() )
      {
        SendMessage( client->display.getHandle(), WM_SETFOCUS, 0, 0 );
        error.msg( "CLIENT: Initialization Successful" );
      }
      else
      {
        error.msg( "CLIENT: Couldn't initialize" );
        MessageBox(NULL, "CLIENT: Couldn't Initialize", "Error", MB_OK);
        RunOK = false;
      }
    }
  }


//-----------------
// init clean up
//-----------------


  SAFE_DELETE( title );


//-----------------
// Game Runtime
//-----------------


  while ( RunOK )
  {
    if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ))
    {
      if ( msg.message == WM_QUIT )
        break;

      TranslateMessage( &msg );
      DispatchMessage( &msg );
    }

    if ( DServer )
    {
      RunOK = server->update();
    }
    else
    {
      if ( client->isServer() )
      {
        RunOK = server->update();
      }

      RunOK = (client->update() ? true : false);

      InvalidateRect( client->display.getHandle(), NULL, FALSE );

    }
  }


//-----------------
// Error Handling
//-----------------


  if ( !RunOK )
  {
    error.msg( "SYSTEM ERROR: A fatal error has occured; shutting down." );
  }


//-----------------
// cleanup & exit
//-----------------
  FSOUND_Close(); // shutdown the sound system
  SAFE_DELETE (client);
  SAFE_DELETE (server);

  return msg.wParam; // Shutdown Game is in WndProc -- WM_DESTROY

}

// C L I E N T / S E R V E R   P R O C S //////////////////////////////////////

//-----------------------------------------------------------------------------
// Client Proc
//-----------------------------------------------------------------------------

LRESULT CALLBACK ClientWndProc(HWND hWnd, UINT message, WPARAM  wParam, LPARAM lParam)
{
  switch( message )
  {
    case WM_PAINT:
      SwapBuffers( client->display.getHDC() );
      ValidateRect( hWnd, NULL );
    
      break;

    case WM_KEYDOWN:
      if ( client != NULL && wParam == VK_TILDE )
        if ( console->active() )
          console->deactivate();
        else
          console->activate();

      break;

    case WM_CHAR:
      if ( console->active() )
        console->getCommand( wParam, lParam );
      
      break;
    
    case WM_KILLFOCUS:
      client->display.setFocus( false );
      break;

    case WM_SETFOCUS:
      client->display.setFocus( true );

#ifndef _DEBUG // disable "always on-top" so we can see the debugger ;)
    case WM_WINDOWPOSCHANGED:
    case WM_WINDOWPOSCHANGING:
      SetWindowPos( hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE ); // HWND_TOP
#endif

    break;

    case WM_DESTROY:
      //client->unload();
      //client->shutdown();
      PostQuitMessage(0);
      break;

    default:
      return(DefWindowProc(hWnd, message, wParam, lParam));

  } // end switch
  
  return (0);
}

//-----------------------------------------------------------------------------
// Server Proc
//-----------------------------------------------------------------------------

LRESULT CALLBACK ServerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch( message )
  {
    case WM_CREATE:
    case WM_ACTIVATE:
    case WM_SETFOCUS:
      SetFocus( server->wndIn.hWnd );
      break;    

    case WM_COMMAND:
      server->getCommand( wParam, lParam );
      break;

    case WM_DESTROY:
      //server->shutdown();
      PostQuitMessage(0);
      break;

    default:
      return(DefWindowProc(hWnd, message, wParam, lParam));
  }
  return (0);
}

// E O F //////////////////////////////////////////////////////////////////////