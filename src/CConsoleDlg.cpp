
// I N C L U D E //////////////////////////////////////////////////////////////

#include "CConsoleDlg.h"

// F U N C T I O N S //////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// CConsoleDlg - fake constructor
//-----------------------------------------------------------------------------

void CConsoleDlg::construct( SConsoleDlg *scw )
{
//-----------------
// Window Handles
//-----------------

  hWnds = new SConsoleDlg;
  hWnds->hWnd       = scw->hWnd;
  hWnds->hWndI      = scw->hWndI;
  hWnds->hWndO_game = scw->hWndO_game;
  hWnds->hWndO_serv = scw->hWndO_serv;

//-----------------
// Command History
//-----------------

  historyIndex = new SCmdHistory;
  historyIndex->next      = NULL;
  historyIndex->prev      = NULL;
  historyIndex->cmdString = NULL;

//-----------------
// Commands
//-----------------

  cmdIndex = new SCmdHandler;
  cmdIndex->next      = NULL;
  cmdIndex->prev      = NULL;
  cmdIndex->function  = NULL;
  cmdIndex->cmdString = NULL;

  addCommand( &CConsoleDlg::CmdBind,  "bind" );
  addCommand( &CConsoleDlg::CmdClear, "clear" );
  addCommand( &CConsoleDlg::CmdCrash, "crash" );
  addCommand( &CConsoleDlg::CmdEcho,  "echo" );
  addCommand( &CConsoleDlg::CmdExec,  "exec" );
  addCommand( &CConsoleDlg::CmdList,  "cmdlist" );
  addCommand( &CConsoleDlg::CmdQuit,  "quit" );
  addCommand( &CConsoleDlg::CmdTrack, "track" );
  addCommand( &CConsoleDlg::CmdShip,  "ship" );
}

//-----------------------------------------------------------------------------
// Func: Process and execute command
// Desc: Parse the command string and try to execute it's contents
// Rtrn: Command Found = 1, No Commands Found = 0
//-----------------------------------------------------------------------------

bool CConsoleDlg::executeCommand( char *str )
{
  char *pstr;
  char pre[256];
  char post[256];
  char temp[256];
  char skip[] = { " ,\n\r" };

// Condition input
  strrtc( str );  // remove trailing spaces
  strcpy( temp, str ); // strtok f*ks up the original string, make a back-up
  if ( (pstr = strtok( temp, skip )) == 0 ) // Get the string's pointer...
    return (false); // ... make sure it's legal, if so ... 
  strcpy( pre, pstr ); // ...make it local.
  strcpy( post, str + (strlen( pre )) ); // lump all the params together
  _strlwr( pre ); // convert command to lower case
  strrlc( post ); // command params still have leading space, remove it

// Attempt to execute input
  SCmdHandler *cmdList;
  for( cmdList = cmdIndex->next; cmdList != NULL; cmdList = cmdList->next )
  {
    if ( strcmp( cmdList->cmdString, pre ) == 0 )
    {
      (this->*(cmdList->function))(post); // Command found, execute.
      addToHistory( str );
      return (true);
    }
  }

  print( TO_SERV_WND, "Unknown Command \'%s\'", pre );
  
  return (false);
}

//-----------------------------------------------------------------------------
// AddCommand - add a command to the console
//-----------------------------------------------------------------------------

bool CConsoleDlg::addCommand( bool (CConsoleDlg::*function)(char *szCommand), char *cmdString )
{

  // TODO: check for duplicate commands

  if ( cmdIndex->prev == NULL )
  {
    if ( (cmdIndex->next = cmdIndex->prev = new SCmdHandler) == 0 )
      return (false); // out of memory

    cmdIndex->prev->prev = cmdIndex;
    cmdIndex->prev->next = NULL;
  }
  else
  {
    if ( (cmdIndex->prev->next = new SCmdHandler) == 0 )
      return (false); // out of memory

    cmdIndex->prev->next->prev = cmdIndex->prev;
    cmdIndex->prev->next->next = NULL;
    cmdIndex->prev = cmdIndex->prev->next;
  }

  cmdIndex->prev->function = function;
  cmdIndex->prev->cmdString = new char[ (strlen( cmdString )) + 1 ];
  strcpy( cmdIndex->prev->cmdString, cmdString );

  return (true);
}

//-----------------------------------------------------------------------------
// RemCommand - removes a command (not yet used but in place for aliasis
//-----------------------------------------------------------------------------

bool CConsoleDlg::remCommand( char * cmdString )
{
  SCmdHandler *CmdHndlrTemp = new SCmdHandler;

  for( CmdHndlrTemp = cmdIndex; CmdHndlrTemp->next != NULL; CmdHndlrTemp = CmdHndlrTemp->next)
    if ( strcmp( CmdHndlrTemp->cmdString, cmdString ) == 0 )
    {
      CmdHndlrTemp->function = NULL;
      CmdHndlrTemp->prev->next = CmdHndlrTemp->next;
      CmdHndlrTemp->next->prev = CmdHndlrTemp->prev;
      delete[] CmdHndlrTemp->cmdString;
      delete CmdHndlrTemp;
      return (true); // found and removed
    }
  return (false); // not found
}

//-----------------------------------------------------------------------------
// AddToHistory - adds user string to history. 
//-----------------------------------------------------------------------------

void CConsoleDlg::addToHistory( char *cmdString )
{
  int historyUsed = 0;
  SCmdHistory *historyCurr = historyIndex;

  for ( historyUsed = 0; historyCurr->next != NULL; historyUsed++, historyCurr = historyCurr->next );

  if ( historyUsed >= HISTORY_LENGTH )
  {
    SCmdHistory *historyTemp = historyIndex->next;

    // make the third link the second.
    historyIndex->next->next->prev = historyIndex->next->prev;
    historyIndex->next = historyIndex->next->next;

    // clean-up and delete the old second link.
    delete[] historyTemp->cmdString;
    historyTemp->next = NULL;
    historyTemp->prev = NULL;
    delete historyTemp;
  }

  if ( (historyCurr->cmdString == NULL) || (strcmp( historyCurr->cmdString, cmdString ) != 0) )
  { // ^above^ OR statements MUST be in this order, or the program WILL crash.
    historyCurr->next = new SCmdHistory;
    historyCurr->next->prev = historyCurr;
    historyCurr = historyCurr->next;
    historyCurr->next = 0;
    historyCurr->cmdString = new char[ (strlen( cmdString )) + 1 ];
    strcpy( historyCurr->cmdString, cmdString );
  }
}

//-----------------------------------------------------------------------------
// print - outputs to the server xor game window.
//-----------------------------------------------------------------------------

void CConsoleDlg::print( int send, char *outStr, ... )
{
  u16 strLenFix = 1023; // + 1
  char *newOutStr;
  char *tempStr = new char[ strLenFix + 1 ];

//-----------------
// Parse string
//-----------------

  va_list marker;
  va_start( marker, outStr ); // Initialize variable arguments.

  int i = 0;
  for( i = 0; (*outStr != NULL) && (i < strLenFix); outStr++, i++ )
  {
    if ( *outStr == '%' )
    {
      switch( *(++outStr) )
      {
      case '%':
        *(tempStr + i) = '%';
        break;

      case 'c': // int, single-byte character
      case 'C': // int, wide character
        *(tempStr + i) = va_arg( marker, char );
        break;

      case 'd': // int, Signed decimal integer
      case 'i': // int, Signed decimal integer
        i += strlen( ltoa( va_arg( marker, int ), (tempStr + i--), 10 ) );
        break;

      case 'o': // int, Unsigned octal integer
        i += strlen( ltoa( va_arg( marker, unsigned int ), (tempStr + i--), 8 ) );
        break;
      case 'u': // int, Unsigned decimal integer
        i += strlen( ultoa( va_arg( marker, unsigned int ), (tempStr + i--), 10 ) );
        break;
      case 'x': // int, Unsigned hexadecimal integer, using "abcdef"
        i += strlen( ltoa( va_arg( marker, unsigned int ), (tempStr + i--), 16 ) );
        break;
      case 'X': // int, Unsigned hexadecimal integer, using "ABCDEF" 
        ltoa( va_arg( marker, unsigned int ), (tempStr + i), 16 );
        i += strlen( _strupr(tempStr + i--) );
        break;

      case 'e': // double, see below.
        i += sprintf( tempStr + i--, "%e", va_arg( marker, double ) ); 
        break;
      case 'E': // double, see below.
        i += sprintf( tempStr + i--, "%E", va_arg( marker, double ) ); 
        break;
      case 'f': // double, see below.
        i += sprintf( tempStr + i--, "%f", va_arg( marker, double ) ); 
        break;
      case 'g': // double, see below.
        i += sprintf( tempStr + i--, "%g", va_arg( marker, double ) ); 
        break;
      case 'G': // double, see below.
        i += sprintf( tempStr + i--, "%G", va_arg( marker, double ) ); 
        break;

      case 'n': // Pointer to int, see below.
      case 'p': // Pointer to void, see below.
        break;

      case 's': // string, 
      case 'S': // string,
        i += strlen( strcpy( (tempStr + i--), va_arg( marker, char * ) ) );
        break;

      default:
        i--;
      } // switch
    } // if
    else
    {
      *(tempStr + i) = (char)(*outStr);
    }

    if ( *outStr == 0 ) break; // stupid bug hack.
  } // for

  va_end( marker ); // Reset variable arguments.

  *(tempStr + i) = 0; // NULL termination
  newOutStr = new char[ (strlen(tempStr) + 1) ]; // custom fit size
  strcpy( newOutStr, tempStr ); // make local

  if( send == TO_GAME_WND )
  {
    int id = SendMessage( hWnds->hWndO_game, LB_ADDSTRING, (WPARAM) 0, (LPARAM) newOutStr); 
    SendMessage( hWnds->hWndO_game, LB_SETCURSEL, (WPARAM) id, (LPARAM) 0 );
  }
  else // send == TO_SERV_WND
  {
    int id = SendMessage( hWnds->hWndO_serv, LB_ADDSTRING, (WPARAM) 0, (LPARAM) newOutStr);
    SendMessage( hWnds->hWndO_serv, LB_SETCURSEL, (WPARAM) id, (LPARAM) 0 );
  }

  delete[] tempStr;
  delete[] newOutStr;
}

// E O F //////////////////////////////////////////////////////////////////////