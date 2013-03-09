/////////////////////////////////////////////////////////////////////////////
// CPlayer.h
// Created: April 7, 2000 - 3:07am - Ryan McGrail
// The Player Class, keeps info for A player ( local or remote )
/////////////////////////////////////////////////////////////////////////////

#ifndef _CPLAYER_H
#define _CPLAYER_H

// I N C L U D E ////////////////////////////////////////////////////////////

#include "CShip.h"
#include "Global.h"

// T Y P E D E F / S T R U C T / C L A S S //////////////////////////////////

class CPlayer
{
protected:
  n8  shipId;
  n8  place;
  u8  checkpoints;
  u8  ip[4]; // IP Address
  
public:
  CPlayer();
  ~CPlayer();
  
  CShip ship;
  bool load(char *aseFilename);
  void unload();
  void init();
  void update();
  void draw();
  void setPlace( n8 p ) { place = p; };
  void allocCheckPoints();
  void resetCheckPoints();

};

/////////////////////////////////////////////////////////////////////////////

#endif // _CPLAYER_H

// E O F ////////////////////////////////////////////////////////////////////