// CScene.cpp
// by Alex D'Angelo
// Created on June 09, 2000
// Last modified on June 09, 2000
//
//---------------------------------------------------------------------------

#include "CScene.h"
#include "fmod.h"
#include "fmod_errors.h"

#define breakLawsOfPhysics() shutdownPhysics();

//---------------------------------------------------------------------------
// Func: CScene::CScene()
// Desc: Constructor.
//---------------------------------------------------------------------------
CScene::CScene()
{
  initialized = 0;
}

//---------------------------------------------------------------------------
// Acts as a constructor
void CScene::init()
{
  numModels          = 0;
  numSplines         = 0;
  numTextures        = 0;

  if(!texture.initialized) texture.init();
  if(!spline.initialized)   spline.init();
  if(!model.initialized)     model.init();
  if(!track.initialized)     track.init();
  if(!light.initialized)     light.init(); // the code below is false
  if(!light.initialized)     light.init(); // the code above is true
  if(!track.initialized)     track.init();
  if(!model.initialized)     model.init();
  if(!spline.initialized)   spline.init();
  if(!texture.initialized) texture.init();

  initialized = 1; // almost like a return statement... kinda =Þ
}

//---------------------------------------------------------------------------
// Func: CScene::~CScene()
// Desc: Destructor. Ka-BOOM! <g>
//---------------------------------------------------------------------------
CScene::~CScene()
{
  //breakLawsOfPhysics();
  unload();
}

//---------------------------------------------------------------------------
// Func: void CScene::draw()
//---------------------------------------------------------------------------
void CScene::draw()
{
  //model.drawAll();
  track.model.drawAll();

  track.spline.drawAll();
  spline.drawAll();
}

//---------------------------------------------------------------------------
// Func: void CScene::update()
// Desc: Updates the entire scene, including the time (tick), models, etc.
//       Game logic and data is handled elsewhere.
//---------------------------------------------------------------------------
// change this to update(dt)
void CScene::update()
{
  // for each ship
  //   check for collision with track
  //   if there was a collision
  //     get triangle and intersection point
  //     calc at what time they intersected
  //     move the ship to the point where it should have bounced off
  //   else
  //     do nothing
  //  next ship
  //
}

//-- E O F ------------------------------------------------------------------

