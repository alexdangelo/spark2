
// I N C L U D E //////////////////////////////////////////////////////////////

#include "Global.h"
#include "CDisplay.h"
#include "CConsoleOGL.h"

// F U N C T I O N S //////////////////////////////////////////////////////////

void CConsoleOGL::construct( HWND WndHndl, int w, int h )
{
  hWnd = WndHndl;
  ScreenWidth = w;
  ScreenHeight = h;

//-----------------
// Initial States
//-----------------

  State           = 0x00;
  TickCurr        = GetTickCount();
  TickLast        = TickCurr;
  TickLastCursor  = TickCurr;
  CursorState     = 1;
  
  fVertPoss = 0.0;
  fHorzPoss = ScreenHeight; // Start at the top of the screen (scrolled up).

  fScrollStop  = (ScreenHeight >> 1); // Bottom = 0, Top = Height
  fScrollSpeed = 10;

//-----------------
// Command List
//-----------------

  dwCmdPoss = 0; // Max = NUM_COMMANDS - 1 (last spot reserved for NULL termination)

  strcpy(CommandList[dwCmdPoss].szCommand, "clear");    CommandList[dwCmdPoss++].function = &CConsoleOGL::CmdClear;
  strcpy(CommandList[dwCmdPoss].szCommand, "cmdlist");  CommandList[dwCmdPoss++].function = &CConsoleOGL::CmdList;
  strcpy(CommandList[dwCmdPoss].szCommand, "crash");    CommandList[dwCmdPoss++].function = &CConsoleOGL::CmdCrash;
  strcpy(CommandList[dwCmdPoss].szCommand, "bind");     CommandList[dwCmdPoss++].function = &CConsoleOGL::CmdBind;
  strcpy(CommandList[dwCmdPoss].szCommand, "echo");     CommandList[dwCmdPoss++].function = &CConsoleOGL::CmdEcho;
  strcpy(CommandList[dwCmdPoss].szCommand, "exec");     CommandList[dwCmdPoss++].function = &CConsoleOGL::CmdExec;
  strcpy(CommandList[dwCmdPoss].szCommand, "exit");     CommandList[dwCmdPoss++].function = &CConsoleOGL::CmdExit;
  strcpy(CommandList[dwCmdPoss].szCommand, "quit");     CommandList[dwCmdPoss++].function = &CConsoleOGL::CmdQuit;
  strcpy(CommandList[dwCmdPoss].szCommand, "track");    CommandList[dwCmdPoss++].function = &CConsoleOGL::CmdTrack;
  strcpy(CommandList[dwCmdPoss].szCommand, "ship");     CommandList[dwCmdPoss++].function = &CConsoleOGL::CmdShip;
  //strcpy(CommandList[dwCmdPoss].szCommand, "s_info");   CommandList[dwCmdPoss++].function = &CConsoleOGL::ConCmdSndInfo;

  strcpy(CommandList[dwCmdPoss].szCommand, "");         CommandList[dwCmdPoss  ].function = NULL;

//-----------------
// Clear buffers
//-----------------

  CurrentCol = 1;

  HistBufferViewLine   = 0;
  HistBufferInputLine  = 0;
  TextBufferInputLine  = (CONSOLE_ROWS - 1);
  TextBufferRenderFrom = 1;

  for(int i = 0; i < CONSOLE_ROWS; i++)
    for(int j = 0; j < CONSOLE_COLS; j++)
      TextBuffer[i].text[j] = NULL;

  TextBuffer[0].text[0] = ']';
  TextBuffer[0].text[1] = '\0';

  loadGfx();
}

//-----------------------------------------------------------------------------
// Func: Init
// Desc: Do all the stuff we couldn't do till after the window is up, and stuff
//       that have return values.
// More: had to make this seperate from the constructor because it was being
//       done before all of the memory was allocated and causing the program
//       to crash. That and some stuff had to return things... a short comming
//       of C++ =Þ
//-----------------------------------------------------------------------------

void CConsoleOGL::loadGfx()
{
  pngInfo PngInfo;

  if( !(uiBackIdent = pngBind("images\\ui\\conBack.png", PNG_NOMIPMAP, PNG_SOLID, NULL,     GL_CLAMP, GL_NEAREST, GL_NEAREST)) )
    error.msg("CONSYS ERROR: Unable to load console background.");

  if( !(uiFontIdent = pngBind("images\\ui\\conFont.png", PNG_NOMIPMAP, PNG_ALPHA, &PngInfo, GL_CLAMP, GL_NEAREST, GL_NEAREST)) )
    error.msg("CONSYS ERROR: Unable to load console font.");

  if( !(uiSplashIdent = pngBind("images\\ui\\splash.png", PNG_NOMIPMAP, PNG_ALPHA, NULL, GL_CLAMP, GL_NEAREST, GL_NEAREST)) )
    error.msg("CONSYS ERROR: Unable to load splash screen.");

  if ( (PngInfo.Width != 256) || (PngInfo.Height != 256) ) // font bitmap is 256x256 w/ 16x16 chars
    error.msg("WARNING: Invalid font format.");
}

//-----------------------------------------------------------------------------
// Func: Splash
// Desc: Draws the console
//-----------------------------------------------------------------------------

#pragma warning( disable : 4800 ) // conversion from int to bool

bool CConsoleOGL::splash( int value /*=(-1)*/ )
{
  if( value == (-1) )
    return this->SplashState;
  else 
    return this->SplashState = value;
}

#pragma warning( default : 4800 ) // conversion from int to bool

//-----------------------------------------------------------------------------
// Func: Draw
// Desc: Draws the console
//-----------------------------------------------------------------------------

void CConsoleOGL::draw()
{
  float w1 = ScreenWidth  >> 1;
  float h1 = ScreenHeight >> 1;

//-----------------
// going down
//-----------------

  if ( State & CONSOLE_STATE_CREATING )
  {
    if ( fHorzPoss > fScrollStop )
    {
      TickCurr = GetTickCount();

      if ( (TickCurr - TickLast) > 1 )
      {
        TickLast = TickCurr;
        fHorzPoss -= fScrollSpeed;
      }
    }
    else
    {
      // change state, no longer creating
      State &= (~CONSOLE_STATE_CREATING);
      if ( fHorzPoss < fScrollStop )
          fHorzPoss = fScrollStop;
    }
  }

//-----------------
// going up
//-----------------

  else if ( State & CONSOLE_STATE_DESTROYING )
  {
    if ( fHorzPoss < ScreenHeight )
    {
      TickCurr = GetTickCount();

      if ( (TickCurr - TickLast) > 1 )
      {
        TickLast = TickCurr;
        fHorzPoss += fScrollSpeed;
      }
    }
    else
    {
      // change console state: not active and not longer destroying.
      State &= 0xF0;
      if ( fHorzPoss > ScreenHeight )
        fHorzPoss = ScreenHeight;
    }
  }

//-----------------
// main draw
//-----------------
  glBlendFunc( GL_SRC_ALPHA, GL_DST_ALPHA );

  glEnable( GL_TEXTURE_2D );

  glPushMatrix();
  {
    // Splash Screen  - temp
    if( this->SplashState )
    {
      glBindTexture(GL_TEXTURE_2D, uiSplashIdent);
      {
        glBegin(GL_QUADS);
        {
          glTexCoord2f(1, 1); glVertex3f( w1, -h1, 0);
          glTexCoord2f(1, 0); glVertex3f( w1,  h1, 0);
          glTexCoord2f(0, 0); glVertex3f(-w1,  h1, 0);
          glTexCoord2f(0, 1); glVertex3f(-w1, -h1, 0);
        }
        glEnd();
      }
    }
    
    // Move Everything else    
    glTranslatef(0.0f, fHorzPoss, 0.0f);

    // Console Background
    glBindTexture(GL_TEXTURE_2D, uiBackIdent);
    {
      glBegin(GL_QUADS);
      {
        glTexCoord2f(1, 1); glVertex3f( w1, -h1, 0);
        glTexCoord2f(1, 0); glVertex3f( w1,  h1, 0);
        glTexCoord2f(0, 0); glVertex3f(-w1,  h1, 0);
        glTexCoord2f(0, 1); glVertex3f(-w1, -h1, 0);
      }
      glEnd();
    }

    // Font Texture
    glBindTexture(GL_TEXTURE_2D, uiFontIdent);
    {
      glEnable(GL_BLEND);

      // Draw Input Line
      drawStringTWO(TextBuffer[0].text, 0, 0, w1, h1); // drawString(TextBuffer[0].text, 0, 0, w1, h1);

      // Draw Buffered Text
      int line = (TextBufferRenderFrom + 1);
      unsigned int NumberOfRows = ( (int)ScreenHeight >> 4 ); // divide by CONSOLE_FONTSIZE (= 16)
      for(unsigned int row = 1; row < NumberOfRows; row++, line++)
      {
        if( line > (CONSOLE_ROWS - 1) )
          line = 1;

        drawStringTWO(TextBuffer[line].text, row, 0, w1, h1); // drawString(TextBuffer[line].text, row, 0, w1, h1);
      }

      // Blinking Cursor //////////////////////////////

      if ( ( (TickCurr = GetTickCount()) - TickLastCursor) > 500)
      {
        TickLastCursor = TickCurr;
        if(CursorState) { CursorState = 0; strcpy(CursorCharStr, " "); }
        else            { CursorState = 1; strcpy(CursorCharStr, "_"); }
      }

      drawStringTWO(CursorCharStr, 0, CurrentCol, w1, h1); // drawString(CursorCharStr, 0, CurrentCol, w1, h1);

      glDisable(GL_BLEND);
    } // GL_TEXTURE_2D fontid
  }
  glPopMatrix();
}

//-----------------------------------------------------------------------------
// Func: AdvanceLine
// Desc: Advances the console one line ('\n')
//-----------------------------------------------------------------------------

void CConsoleOGL::advanceLine()
{
  if( --TextBufferInputLine < 1 )
    TextBufferInputLine = (CONSOLE_ROWS - 1);

  if( ~(State & CONSOLE_STATE_PAGEUP) )
    TextBufferRenderFrom = TextBufferInputLine;
}

//-----------------------------------------------------------------------------
// Func: Input
// Desc: WM_CHAR handler
//-----------------------------------------------------------------------------

void CConsoleOGL::getCommand( WPARAM wParam, LPARAM lParam )
{
  int valid, x;

  switch(wParam)
  {
  case '`' : break; // ~

  case '\n':  case '\r':

    valid = 0;
    for( x = 1; x < CONSOLE_COLS && !valid && TextBuffer[0].text[x] != '\0'; x++ )
      if( TextBuffer[0].text[x] != ' ' )
        valid = 1;

    if( valid )
    {
      strcpy(TextBuffer[TextBufferInputLine].text, TextBuffer[0].text);

      if( --TextBufferInputLine < 1 )
        TextBufferInputLine = (CONSOLE_ROWS - 1);
      if( State | (~CONSOLE_STATE_PAGEUP))
        TextBufferRenderFrom = TextBufferInputLine;

      executeCommand( NULL );
    }

    CurrentCol = 1;
    TextBuffer[0].text[1] = '\0';
    break;

  case '\b':
    if(CurrentCol > 1)
      TextBuffer[0].text[--CurrentCol] = '\0';
    break;

  default:
    if(CurrentCol < CONSOLE_COLS)
    {
      TextBuffer[0].text[CurrentCol++] = (char)wParam;
      TextBuffer[0].text[CurrentCol]   = '\0';
    }
  }
}

//-----------------------------------------------------------------------------
// Func: Print
// Desc: Prints a string to the console.
// Note: Send param is ignored.
//-----------------------------------------------------------------------------

void CConsoleOGL::print( int send, char *outStr, ... )
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

//-----------------
// Do output
//-----------------

  strcpy( TextBuffer[TextBufferInputLine].text, newOutStr );
  advanceLine();

//-----------------
// Clean up
//-----------------

  delete[] tempStr;
  delete[] newOutStr;
}

//-----------------------------------------------------------------------------
// Func: ExecuteCommand
// Desc: Executes a command that has been identified by ProcessCommand
// Rtrn: Execute Successful = 1, Else = 0
//-----------------------------------------------------------------------------

bool CConsoleOGL::executeCommand( char *cmdString )
{
  char *cmdParams;

  if( cmdString == NULL )
  {
    cmdString = new char[ strlen(&TextBuffer[0].text[1]) + 1 ];
    strcpy( cmdString, &TextBuffer[0].text[1] );
  }

  // extract the command and it's param(s)
  strtok(cmdString, " \n\r");

  // Get the cammand's parameters
  cmdParams = &TextBuffer[0].text[1] + strlen( cmdString );

  strlwr( cmdString );

  // try to excute command
  for(int i = 0; CommandList[i].function != NULL; i++)
  {
    // check if it's in the command list
    if( strcmp( CommandList[i].szCommand, cmdString ) == 0 )
    {
      // Command found, execute it.
      (this->*(CommandList[i].function))(cmdParams);
      
      strcpy( HistBuffer[HistBufferInputLine].text, &TextBuffer[0].text[1] );

      if( ++HistBufferInputLine > (CMDHIST_ROWS - 1) )
        HistBufferInputLine = 0;

      HistBufferViewLine = HistBufferInputLine;      
      
      return(true);
    }
  }

  print( 0, "Unknown Command '%s'", cmdString );
  
  return(false);
}

//-----------------------------------------------------------------------------
// Func: DrawString
// Desc: Draws a string to the screen.
// More: glBindTexture() *** MUST *** be called to set the texture to a font
//       before excution of this function. Though it could've been possible
//       to have called from this function (assuming the font id is stored in 
//       the console struct), it increases the execution dramatically for some-
//       thing that only needs to be done once. This makes a HUGE difference 
//       when this outputting large amounts of text.
// 
//       The difference between the Row/Col and the Offsets; Offset is a pixel
//       based offset, while Row/Col is based on the font hieght/width.
//
// Rtrn: NONE (duh, it's VOID!)
//-----------------------------------------------------------------------------
/*
void CConsoleOGL::drawString(char *szString, _int32 dwRow, _int32 dwCol,
                               float fHorzOffset, float fVertOffset)
{
  char cChar;

  GLfloat fVertex[4];
  GLfloat fTexCoord[4];

  glColor3f(1.0f, 1.0f, 1.0f);

  for(int i = 0; (szString[i] != '\0') && (i < CONSOLE_COLS); i++)
  {
    if((szString[i] >= 0) && (szString[i] <= 255)) //(szString[i] <= 127))
    {
      cChar = szString[i];

      if(cChar == '^')
      {
        switch( szString[i+1] )
        {
        case '^': i++; break;
        case '0': 
          if( isdigit(szString[i+2]) )
          {
            switch( szString[i+2] )
            {
            case '0': glColor3f(0.0f, 0.0f, 0.0f); i += 2; break; // Black
            case '1': glColor3f(1.0f, 1.0f, 1.0f); i += 2; break; // White
            case '2': glColor3f(1.0f, 0.0f, 0.0f); i += 2; break; // Red
            case '3': glColor3f(1.0f, 1.0f, 0.0f); i += 2; break; // Yellow
            case '4': glColor3f(0.0f, 1.0f, 0.0f); i += 2; break; // Green
            case '5': glColor3f(0.0f, 1.0f, 1.0f); i += 2; break; // Cyan
            case '6': glColor3f(0.0f, 0.0f, 1.0f); i += 2; break; // Blue
            case '7': glColor3f(1.0f, 0.0f, 1.0f); i += 2; break; // Magenta
            case '8': glColor3f(0.6f, 0.0f, 0.0f); i += 2; break; // -Red
            case '9': glColor3f(0.6f, 0.6f, 0.0f); i += 2; break; // -Yellow (Orange)
            default:  glColor3f(1.0f, 1.0f, 1.0f); i += 2; break; // error: default to white
            }
          }
          else { i++; glColor3f(1.0f, 1.0f, 1.0f); } // error: default to white
          break;
        
        case '1':
          if( isdigit(szString[i+2]) )
          {
            switch( szString[i+2] )
            {
            case '0': glColor3f(0.0f, 0.6f, 0.0f); i += 2; break; // -Green
            case '1': glColor3f(0.0f, 0.6f, 0.6f); i += 2; break; // -Cyan
            case '2': glColor3f(0.0f, 0.0f, 0.6f); i += 2; break; // -Blue
            case '3': glColor3f(0.6f, 0.0f, 0.6f); i += 2; break; // -Magenta
            case '4': glColor3f(0.3f, 0.0f, 0.0f); i += 2; break; // --Red
            case '5': glColor3f(0.3f, 0.3f, 0.0f); i += 2; break; // --Yellow (-Orange)
            case '6': glColor3f(0.0f, 0.3f, 0.0f); i += 2; break; // --Green
            case '7': glColor3f(0.0f, 0.3f, 0.3f); i += 2; break; // --Cyan
            case '8': glColor3f(0.0f, 0.0f, 0.3f); i += 2; break; // --Blue
            case '9': glColor3f(0.3f, 0.0f, 0.3f); i += 2; break; // --Magenta
            default:  glColor3f(1.0f, 1.0f, 1.0f); i += 2; break; // error: default to white
            }
          }
          else { i++; glColor3f(1.0f, 1.0f, 1.0f); } // error: default to white
          break;

        case '2':
          if( isdigit(szString[i+2]) )
          {
            switch( szString[i+2] )
            {
            case '0': glColor3f(0.0f, 0.0f, 0.0f); i += 2; break; // Black
            case '1': glColor3f(0.1f, 0.1f, 0.1f); i += 2; break; // 10% Gray
            case '2': glColor3f(0.2f, 0.2f, 0.2f); i += 2; break; // 20% Gray
            case '3': glColor3f(0.3f, 0.3f, 0.3f); i += 2; break; // 30% Gray
            case '4': glColor3f(0.4f, 0.4f, 0.4f); i += 2; break; // 40% Gray
            case '5': glColor3f(0.5f, 0.5f, 0.5f); i += 2; break; // 50% Gray
            case '6': glColor3f(0.6f, 0.6f, 0.6f); i += 2; break; // 60% Gray
            case '7': glColor3f(0.7f, 0.7f, 0.7f); i += 2; break; // 70% Gray
            case '8': glColor3f(0.8f, 0.8f, 0.8f); i += 2; break; // 80% Gray
            case '9': glColor3f(0.9f, 0.9f, 0.9f); i += 2; break; // 90% Gray
            default:  glColor3f(1.0f, 1.0f, 1.0f); i += 2; break; // error: default to white
            }
          }
          else { i++; glColor3f(1.0f, 1.0f, 1.0f); } // error: default to white
          break;

        } // end switch
      }

      else if(cChar == '\n')
      {
        advanceLine();
      }

      else if(cChar == '\t')
      {
        // TODO: possible, list commands?
      }

      else
      {
        fVertex[0] = -fHorzOffset + ((float)CONSOLE_FONTSIZE * (float)dwCol++);
        fVertex[1] = -fVertOffset + ((float)CONSOLE_FONTSIZE * (float)dwRow);
        fVertex[2] =  fVertex[0]  +  (float)CONSOLE_FONTSIZE;
        fVertex[3] =  fVertex[1]  +  (float)CONSOLE_FONTSIZE;
/*
        // check if we're running off the screen
        if( (fVertex[2] * 2) > g_dwScreenWidth )
        {
          TextBufferRenderFrom = (TextBufferInputLine + 1);

          fVertex[1] -= dwFontHeight;
          fVertex[3] -= dwFontHeight;
          fVertex[0] -= g_dwScreenWidth;
          fVertex[2] -= g_dwScreenWidth;
        }
*//*
        fTexCoord[0] = (GLfloat) ((((cChar) % 16) * 16) / 256.0f);
        fTexCoord[1] = (GLfloat) ((((cChar) >> 4) * 16) + (float)CONSOLE_FONTSIZE) / 256.0f;
        fTexCoord[2] = (fTexCoord[0] * 256.0f + 16) / 256.0f;
        fTexCoord[3] = (fTexCoord[1] * 256.0f - 16) / 256.0f;
        
        glColor3f(0.0f, 0.0f, 0.0f); // New May 30, 2000 AD - sets the text bg color to black
        
        glBegin(GL_QUADS);
	        glTexCoord2f(fTexCoord[0], fTexCoord[3]); glVertex3f(fVertex[0], fVertex[3], 8);
	        glTexCoord2f(fTexCoord[0], fTexCoord[1]); glVertex3f(fVertex[0], fVertex[1], 8);
	        glTexCoord2f(fTexCoord[2], fTexCoord[1]); glVertex3f(fVertex[2], fVertex[1], 8);
          glTexCoord2f(fTexCoord[2], fTexCoord[3]); glVertex3f(fVertex[2], fVertex[3], 8);
	      glEnd();
      } // end else
    }
  }
  // Rest color to white.
  glColor3f(1.0f, 1.0f, 1.0f);
}
*/

//-----------------------------------------------------------------------------
// Func: DrawString
// Desc: Draws a string to the screen.
// More: glBindTexture() *** MUST *** be called to set the texture to a font
//       before excution of this function. Though it could've been possible
//       to have called from this function (assuming the font id is stored in 
//       the console struct), it increases the execution dramatically for some-
//       thing that only needs to be done once. This makes a HUGE difference 
//       when this outputting large amounts of text.
// 
//       The difference between the Row/Col and the Offsets; Offset is a pixel
//       based offset, while Row/Col is based on the font hieght/width.
//
// Rtrn: NONE (duh, it's VOID!)
//-----------------------------------------------------------------------------

void CConsoleOGL::drawStringTWO(char *szString, _int32 dwRow, _int32 dwCol,
                               float fHorzOffset, float fVertOffset)
{
  long stringLength = strlen( szString );
  GLfloat fVertex[4];
  GLfloat fTexCoord[4];

  glColor3f(1.0f, 1.0f, 1.0f);

  for(int i = 0; szString[i] && (i < stringLength); i++)
  {
    if( ((szString[i] > 0) && (szString[i] < 128)) && (szString[i] == '\\') )
    {/*
      switch( szString[i+1] )
      {
        case '\\': { i++; i++; } break;
        case 'c' :
          {
            if( szString[i+2] == '(' )
            {
              i += 3;
              float numbers[3] = {0};
              for( int j = 0; j < 3 && isdigit( szString[i] ); j++, i++ )
              {
                char c[2] = {szString[i], 0};
                numbers[j] = atof( c );
              }
              glColor3f(numbers[0], numbers[1], numbers[2]);
              i++;
            }
            else
            { i += 3; glColor3f(1.0f, 1.0f, 1.0f); }
          }
          break;
        
        case 'n':
          szString[i+1] = 'x';
          advanceLine();
          CurrentCol--;
          i++; i++;
          break;

        case 't':
          CurrentCol--;
          i++; i++;
          // TODO: possible, list commands?
          break;

        case 'x':
          i++; i++;
          break;

        default: break;

      }*/ // end switch
    }
    else
    {
      fVertex[0] = -fHorzOffset + ((float)CONSOLE_FONTSIZE * (float)dwCol++);
      fVertex[1] = -fVertOffset + ((float)CONSOLE_FONTSIZE * (float)dwRow);
      fVertex[2] =  fVertex[0]  +  (float)CONSOLE_FONTSIZE;
      fVertex[3] =  fVertex[1]  +  (float)CONSOLE_FONTSIZE;

      fTexCoord[0] = (GLfloat) ((((szString[i]) % 16) * 16) / 256.0f);
      fTexCoord[1] = (GLfloat) ((((szString[i]) >> 4) * 16) + (float)CONSOLE_FONTSIZE) / 256.0f;
      fTexCoord[2] = (fTexCoord[0] * 256.0f + 16) / 256.0f;
      fTexCoord[3] = (fTexCoord[1] * 256.0f - 16) / 256.0f;
      
      glColor3f(0.0f, 0.0f, 0.0f); // set text bg color to black
      
      glBegin(GL_QUADS);
	      glTexCoord2f(fTexCoord[0], fTexCoord[3]); glVertex3f(fVertex[0], fVertex[3], 8);
	      glTexCoord2f(fTexCoord[0], fTexCoord[1]); glVertex3f(fVertex[0], fVertex[1], 8);
	      glTexCoord2f(fTexCoord[2], fTexCoord[1]); glVertex3f(fVertex[2], fVertex[1], 8);
        glTexCoord2f(fTexCoord[2], fTexCoord[3]); glVertex3f(fVertex[2], fVertex[3], 8);
	    glEnd();
    } // end else
  }
  glColor3f(1.0f, 1.0f, 1.0f); // Rest color to white.
}

// E O F //////////////////////////////////////////////////////////////////////