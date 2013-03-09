// CPlayer.cpp
// Created on April 11, 2000
// Last modified April 13, 2000
//
// I N C L U D E //////////////////////////////////////////////////////////////

#include "Global.h"
#include "CPlayer.h"
#include "CShip.h"

// F U N C T I O N S //////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// Func: CPlayer::CPlayer()
// Desc: 
//-----------------------------------------------------------------------------
CPlayer::CPlayer()
{
  // nothing for now
}

//-----------------------------------------------------------------------------
// Func: CPlayer::~CPlayer()
// Desc: 
//-----------------------------------------------------------------------------
CPlayer::~CPlayer()
{
  // nothing for now
}

//-----------------------------------------------------------------------------
// Func: bool CPlayer::load(char *aseFilename)
// Desc: 
//-----------------------------------------------------------------------------
bool CPlayer::load(char *aseFilename)
{
  return ship.load(aseFilename);
}

//-----------------------------------------------------------------------------
// Func: CPlayer::unload()
// Desc: 
//-----------------------------------------------------------------------------
void CPlayer::unload()
{
  ship.unload();
}

//-----------------------------------------------------------------------------
// Func: CPlayer::init()
// Desc: get the player ready to race -- init the ship, stats, and place at
//       the start line
//-----------------------------------------------------------------------------
void CPlayer::init()
{
//  char modelname[SIZE];
  
//  strcpy(modelname, ship_table[shipId]);

//  ship.load(model); // it will check to see if there's another instance of the model loaded
  
  ship.reset();
}

//-----------------------------------------------------------------------------
// Func: CPlayer::update()
// Desc: updates the ship's position and stats per frame
//-----------------------------------------------------------------------------
void CPlayer::update()
{
  // move the ship, mathengine is done in here. ME actually moves the ship
  // fire guns
  // calc new stats for the ship (here?)
}

//-----------------------------------------------------------------------------
// Func: CPlayer::draw()
// Desc: draws the ship
//-----------------------------------------------------------------------------
void CPlayer::draw()
{
//  ship->draw();
}

//-----------------------------------------------------------------------------
// Func: CPlayer::drawHUD()
// Desc: draws the players's HUD
//-----------------------------------------------------------------------------
/*
void CPlayer::drawHUD()
{
  // draw map, radar, curr weapon, weapon count, place, shield strength, etc
}
*/
// E O F ////////////////////////////////////////////////////////////////////
