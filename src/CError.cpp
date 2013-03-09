
// I N C L U D E //////////////////////////////////////////////////////////////

#include "CError.h"

// G L O B A L ////////////////////////////////////////////////////////////////

CError error;

///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// Func: kill
// Desc: kills the program, mem left.
//-----------------------------------------------------------------------------

CError::CError()
{
  firstRun = 1;
  tickCurr = tickLast = GetTickCount();
  strcpy( path, "error.log" );
};

//-----------------------------------------------------------------------------
// Func: kill
// Desc: kills the program, mem left.
//-----------------------------------------------------------------------------
/*
__forceinline void CError::kill() { };

//-----------------------------------------------------------------------------
// Func: crash
// Desc: crashes program, and computer if possible.
//-----------------------------------------------------------------------------

__forceinline void CError::crash()
{
  __asm
  {
    mov eax, 00h
    mov ebx, 00h
    mov ecx, 00h
    mov edx, 00h
  }
};
*/

//-----------------------------------------------------------------------------
// Func: msg
// Desc: Outputs an error message.
//-----------------------------------------------------------------------------

void CError::msg( char *string ... )
{
  char newstr[MAX_LENGTH] = {NULL};
  va_list marker;
  va_start( marker, string ); // Initialize variable arguments.


//-----------------
// Parse string
//-----------------


  for( int i = 0; (*string != NULL) || (i < MAX_LENGTH); string++, i++ )
  {
    if ( *string == '%' )
    {
      switch( *(++string) )
      {
      case '%':
        newstr[i] = '%';
        break;

      case 'c': // int, single-byte character
      case 'C': // int, wide character
        newstr[i] = va_arg( marker, char );
        break;

      case 'd': // int, Signed decimal integer
      case 'i': // int, Signed decimal integer
        i += strlen( ltoa( va_arg( marker, int ), (newstr + i--), 10 ) );
        break;

      case 'o': // int, Unsigned octal integer
        i += strlen( ltoa( va_arg( marker, unsigned int ), (newstr + i--), 8 ) );
        break;
      case 'u': // int, Unsigned decimal integer
        i += strlen( ultoa( va_arg( marker, unsigned int ), (newstr + i--), 10 ) );
        break;
      case 'x': // int, Unsigned hexadecimal integer, using "abcdef"
        i += strlen( ltoa( va_arg( marker, unsigned int ), (newstr + i--), 16 ) );
        break;
      case 'X': // int, Unsigned hexadecimal integer, using "ABCDEF" 
        i += strlen( ltoa( va_arg( marker, unsigned int ), (newstr + i--), 16 ) );
        break;

      case 'e': // double, see below.
        i += sprintf( newstr + i--, "%e", va_arg( marker, double ) ); 
        break;
      case 'E': // double, see below.
        i += sprintf( newstr + i--, "%E", va_arg( marker, double ) ); 
        break;
      case 'f': // double, see below.
        i += sprintf( newstr + i--, "%f", va_arg( marker, double ) ); 
        break;
      case 'g': // double, see below.
        i += sprintf( newstr + i--, "%g", va_arg( marker, double ) ); 
        break;
      case 'G': // double, see below.
        i += sprintf( newstr + i--, "%G", va_arg( marker, double ) ); 
        break;

      case 'n': // Pointer to int, see below.
      case 'p': // Pointer to void, see below.
        break;

      case 's': // string, 
      case 'S': // string, 
        i += strlen( strcat( newstr + i--, va_arg( marker, char * ) ) );
        break;

      default:
        i--;
      } // switch
    } // if
    else
    {
      newstr[i] = (char)(*string);
    }

    if ( *string == 0 ) break; // stupid bug hack.
  } // for

  va_end( marker ); // Reset variable arguments.

  
//-----------------  
// Output to file
//-----------------

  fstream file;
  char time[9], date[9];
  char strBuffer[MAX_LENGTH];

//-- OPEN File --

  file.open( path, ios::app | ios::out); //, filebuf::sh_read );
  file.clear();

//-- First Run Message -- 

  if ( firstRun )
  {
    _strdate( date ); _strtime( time );

    sprintf( strBuffer, "\n[[ ---- BEGIN ERROR LOG ---- %s - %s ---- BEGIN ERROR LOG ---- ]]\n", date, time );

    file.clear();
    file.write( strBuffer, strlen( strBuffer ) );
    file.clear();

    firstRun = 0;
  }

//-- ERROR message --

  _strtime( time );
  sprintf( strBuffer, "[ %s ] %s\n", time, newstr );

  file.clear();    
  file.write( strBuffer, strlen( strBuffer ) );
  file.clear();
  file.close();
}

//-----------------------------------------------------------------------------
// Func: glgeterrors ()
// Desc: Outputs any and all OpenGL error messages to whatever the best means 
//       is available. If any error is found, a 1 is sent back to let other
//       functions know and act upon it if desired.
//-----------------------------------------------------------------------------

bool CError::getGLErrors()
{
  GLenum eErrorMsg;
  char errMsg[256];
  bool bErrorFound = 0;

  while( (eErrorMsg = glGetError()) != GL_NO_ERROR)
  {
    switch(eErrorMsg)
    {
      case GL_INVALID_OPERATION:
        strcpy( errMsg, "GL_INVALID_OPERATION" );
      break;

      case GL_INVALID_VALUE:
        strcpy( errMsg, "GL_INVALID_VALUE" );
      break;

      case GL_INVALID_ENUM:
        strcpy( errMsg, "GL_INVALID_ENUM" );
      break;

      case GL_STACK_OVERFLOW:
        strcpy( errMsg, "GL_STACK_OVERFLOW" );
      break;

      case GL_STACK_UNDERFLOW:
        strcpy( errMsg, "GL_STACK_OVERFLOW" );
      break;

      case GL_OUT_OF_MEMORY:
        strcpy( errMsg, "GL_OUT_OF_MEMORY" );
      break;

      default:
        strcpy( errMsg, "Undefined" );
      break;
    }
    msg( "ERROR: GL: %s", errMsg );
    bErrorFound = 1;
  }
  
  return bErrorFound;
}

///////////////////////////////////////////////////////////////////////////////

/* TYPES
c int or wint_t When used with printf functions, specifies a single-byte character; when used with wprintf functions, specifies a wide character. 
C int or wint_t When used with printf functions, specifies a wide character; when used with wprintf functions, specifies a single-byte character. 
d int Signed decimal integer. 
i int  Signed decimal integer. 
o int  Unsigned octal integer. 
u int  Unsigned decimal integer. 
x int Unsigned hexadecimal integer, using “abcdef.” 
X int Unsigned hexadecimal integer, using “ABCDEF.” 
e  double Signed value having the form [ – ]d.dddd e [sign]ddd where d is a single decimal digit, dddd is one or more decimal digits, ddd is exactly three decimal digits, and sign is + or –. 
E double Identical to the e format except that E rather than e introduces the exponent. 
f double Signed value having the form [ – ]dddd.dddd, where dddd is one or more decimal digits. The number of digits before the decimal point depends on the magnitude of the number, and the number of digits after the decimal point depends on the requested precision. 
g double Signed value printed in f or e format, whichever is more compact for the given value and precision. The e format is used only when the exponent of the value is less than –4 or greater than or equal to the precision argument. Trailing zeros are truncated, and the decimal point appears only if one or more digits follow it. 
G double Identical to the g format, except that E, rather than e, introduces the exponent (where appropriate). 
n  Pointer to integer  Number of characters successfully written so far to the stream or buffer; this value is stored in the integer whose address is given as the argument. 
p Pointer to void Prints the address pointed to by the argument in the form xxxx:yyyy where xxxx is the segment and yyyy is the offset, and the digits x and y are uppercase hexadecimal digits. 
s String  When used with printf functions, specifies a single-byte–character string; when used with wprintf functions, specifies a wide-character string. Characters are printed up to the first null character or until the precision value is reached. 
S String When used with printf functions, specifies a wide-character string; when used with wprintf functions, specifies a single-byte–character string. Characters are printed up to the first null character or until the precision value is reached. 
*/

// E O F //////////////////////////////////////////////////////////////////////
