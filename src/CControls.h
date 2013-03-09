/////////////////////////////////////////////////////////////////////////////
// CControls.h
// Created: April 7, 2000 - 2:30am - Ryan McGrail
// This might be a useless class, but it's late and I thought it would be
// good for keeping track of key binds. Since I love making things into
// classes it seemed only natural to make this one as well =Þ
/////////////////////////////////////////////////////////////////////////////

#ifndef _CCONTROLS_H
#define _CCONTROLS_H

// D E F I N E //////////////////////////////////////////////////////////////

#define MAX_ACTIONS 32 // number of things a client can do and have rebound
                       // i.e. accelerate, break, screenshot, etc.

// I N C L U D E ////////////////////////////////////////////////////////////

#include "Global.h"

// T Y P E D E F / S T R U C T / C L A S S //////////////////////////////////

struct SBindings
{
  n16 key;
  n16 command;
};

class CControls
{
public:
  n8  bind( u8 vkey, char *command );
  n8  unbind( u8 vkey );
  n8  unbindall();
  n8  doAction( u8 vkey ); // On key press call corrisponding function(s).

private:
  SBindings bindings[MAX_ACTIONS];

};
/////////////////////////////////////////////////////////////////////////////

#endif // _CCONTROLS_H

// E O F ////////////////////////////////////////////////////////////////////