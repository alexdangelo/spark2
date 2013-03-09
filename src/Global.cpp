// Global.cpp /////////////////////////////////////////////////////////////////

// I N C L U D E //////////////////////////////////////////////////////////////

#include <string.h>
#include "Global.h"

// F U N C T I O N S //////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// Func:  Remove leading chars
// Desc:  Removes the leading chars, default removes spaces
//---------------------------------------------------------------------------

char *strrlc( char *string, char check /*=' '*/ )
{
  if( string[0] == check )
  {
    int length = strlen( string );
    char *newString = new char[length + 1];
    memmove( newString, string, length + 1 );

    for( int i = 1; i < length; i++ )
      if( newString[i] != check )
      {
        memmove( newString, newString + i, ++length - i );
        i = length;
      }
    return( newString );
  }
  return( string );
}

//---------------------------------------------------------------------------
// Func:  Remove trailing chars
// Desc:  Removes the trailing chars, default removes spaces
//---------------------------------------------------------------------------

char *strrtc( char *string, char check /*=' '*/ )
{
  int length = strlen( string );
  if( string[length-1] == check )
  {
    char *newString  = new char[length + 1];
    memmove( newString, string, length + 1 );

    for ( int i = --length; i > -1; i-- )
      if ( newString[i] != check )
      {
        newString[i + 1] = '\0';
        i = -1;
      }
    return( newString );
  }
  return( string );
}

// E O F //////////////////////////////////////////////////////////////////////