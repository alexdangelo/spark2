// CShip.cpp
// Created on April 11, 2000
// Last modified Aug 14, 2000
//
// I N C L U D E //////////////////////////////////////////////////////////////
#include "CShip.h"

//---------------------------------------------------------------------------
// Func: CShip::CShip()
//---------------------------------------------------------------------------
CShip::CShip()
{ 
  scene       = NULL;
  trackPtr    = NULL;
  isPlayer    = false;
  initialized = 0;
 
  //init();
}

//---------------------------------------------------------------------------
// Func: CShip::~CShip()
//---------------------------------------------------------------------------
CShip::~CShip()
{ 
  unload();
}

//---------------------------------------------------------------------------
// Func: CShip::load(char *aseFilename)
// Rtrn: 0 if failed, 1 if succeeded
// Note: The ship MUST be attached to the scene. This is done by setting
//       the CShip::scene pointer to the current scene. For example:
//
//         CScene scene;
//         CPlayer player;
//         player.ship.attachToScene(&scene); // must be done first!!!!
//         if(!player.load("ships/ship01tc.ase")) { error } else { success }
//
//---------------------------------------------------------------------------
bool CShip::load(char *aseFilename)
{ 

//-------------------
// No Scene?
//   Don't Pass Go,
//   Don't collect $200
//-------------------

  if( scene == NULL )
  {
    error.msg("ERROR: Ship not associated with a scene! (CShip::load())");
    return false;
  }

//-------------------
// Perpare Raw Str
//-------------------

  // remove leading and trailing spaces
  strrlc(aseFilename); // !!!does nothing because aseFilename isn't directly operated on!!!
  strrtc(aseFilename); // !!!does nothing because aseFilename isn't directly operated on!!!

  char *allocFileName = new char[ strlen(aseFilename)+1 ];
  strcpy( allocFileName, aseFilename );

//-------------------
// Tokenize Path
//-------------------
   
  _strrev( allocFileName );                  // reverse the string

  char *token = strtok(allocFileName, " /\"\0\\"); // search for the first slash or quote

  if(token != NULL)
  {
    _strrev(token);                     // flip the filename around again
                                        //  to get the correct name      
    strcpy(allocFileName, token);         // copy back into the cmdParam, just to make sure
    allocFileName[strlen(allocFileName) + 1] = NULL; // terminate it! to be really sure
  }
  else
  {
    //console->print( 0, "Invalid Filename!" );
    return false;
  }

//-------------------
// Append new path
//  & extension
//-------------------

  char *fileAndPath = NULL;

  int pathSize = strlen("ships/") + strlen(allocFileName) + strlen(".ase") + 1;
  fileAndPath = new char[ pathSize ];
  memset(fileAndPath, NULL, pathSize);
  
  strcpy(fileAndPath, "ships/");
  strcat(fileAndPath, allocFileName);

  // case-insensitive scan for .ASE ending
  if( strncmp(_strrev(_strlwr(fileAndPath)), "esa.", 4) ) // if the extension isn't there
    strcat(_strrev(fileAndPath), ".ase");  // reverse it back and append extension
  else
    _strrev(fileAndPath); // reverse it back anyway
  
 // SAFE_DELARY(allocFileName); // this stopped working all of a sudden!
 
//-------------------
// Test to see if the
//  file exists
//-------------------

  if( this->openFile(fileAndPath) )
    this->closeFile();
  else
  {
    //console->print( 0, "Couldn't open %s", fileAndPath );
    error.msg( "Couldn't open %s", fileAndPath );
    SAFE_DELARY( fileAndPath );
    return false;
  }

//-------------------
// Do the loading
//-------------------

  if( !this->initialized )
    init(); // inits the scene

  if( !this->convert(fileAndPath) )
  {
    SAFE_DELARY( fileAndPath );
    return false;
  }
  else
  {
    SAFE_DELARY( fileAndPath );
  }

  return true;
}

//---------------------------------------------------------------------------
// Func: CShip::~unload()
//---------------------------------------------------------------------------
void CShip::unload()
{ 
  FSOUND_StopSound( channel );
  closeFile();
  model.unload();
  texture.unload();
  model.releaseTexPointer();
  detachFromScene(); // sets scene = NULL; trackPtr = NULL;
  initialized = 0;
}

//-----------------------------------------------------------------------------
// Func: bool CShip::attachToScene(CScene *existingScene)
// Desc: Gives this ship access to the "world" -- the models, track, powerups,
//       checkpoints, etc
// Rtns: 0 if the existingScene is invalid
//       1 on success
//-----------------------------------------------------------------------------
bool CShip::attachToScene(CScene *existingScene)
{
  if(existingScene != NULL)
  {
    scene = existingScene;
    connectToTrack(&scene->track);
    return 1;
  }
  else
  {
    return 0;
  }
}

//-----------------------------------------------------------------------------
// Func: bool CShip::connectToTrack(CTrack *currTrack)
// Desc: Gives the ship access to the track via a pointer. We need to do this
//       so we can detect gates, powerups, and other info
//-----------------------------------------------------------------------------
bool CShip::connectToTrack(CTrack *currTrack)
{
  if(currTrack != NULL)
  {
    trackPtr     = currTrack;
    finishlineId = trackPtr->getFinishlineId();

    return 1;
  }
  else
  {
    error.msg( "ERROR: Track doesn't exist" );
    return 0;
  }
}

//-----------------------------------------------------------------------------
// Func: void CShip::init()
// Desc: Inits all the vars for this ship
//-----------------------------------------------------------------------------
void CShip::init()
{
  if(!initialized)
  {
    // geometry
    model.setTexPointer(&texture); 

    if(!model.initialized)     model.init();
    if(!texture.initialized) texture.init();

    // game logic
    finishlineId      = -1;
    currLapNumber     = 0;
    nextGateToVisit   = 0;
    feelerRadius      = 64.0f;
    memset(oldpos, 0, sizeof(vect3));
    memset(pos,    0, sizeof(vect3));
    
    loadSounds();

    // done
    initialized = 1;
  }

  reset();
}

//-----------------------------------------------------------------------------
// Func: void CShip::draw()
// Desc: Draws the ship and bounding box at its current position
//-----------------------------------------------------------------------------
void CShip::draw()
{
  //---------------
  // Draw the Ship
  //---------------
  glPushMatrix();

    glTranslatef(this->pos[0], this->pos[1], this->pos[2]);
    glRotatef(this->heading, 0, 1, 0);
    glRotatef(this->roll, 0, 0, 1);

    //drawAxis();

    model.drawAll();


/*    
    // let's also draw the previous path
    glColor3f( 0.5f, 0.5f, 0.0f );
    glScalef(10,10,10);
    glBegin( GL_LINES );
      glVertex3f(oldpos[0], oldpos[1], oldpos[2]);
      glVertex3f(pos[0], pos[1], pos[2]);
    glEnd();
    glScalef(1/10,1/10,1/10);  
*/
  glPopMatrix();

  // this is the ship's "feeler" for detecting gates
  // it needs to be tied to the bounding box's z-length
  float dx = this->feelerRadius * (float)sin(heading*PIOVER180);
  float dy = 0;
  float dz = this->feelerRadius * (float)cos(heading*PIOVER180);

  glPushAttrib(GL_CURRENT_BIT);
  glColor3f(1,0,0);
  glBegin(GL_LINES); // ship collision line
    glVertex3f( this->pos[0] - dx,
                this->pos[1] - dy,
                this->pos[2] - dz );
    glVertex3f( this->pos[0] + dx,
                this->pos[1] + dy,
                this->pos[2] + dz );
  glEnd();
  glPopAttrib();
}

//-----------------------------------------------------------------------------
// Func: void CShip::drawWireBox(float halfx, float halfy, float halfz)
// Desc: glut doesn't have a glutWireBox(x,y,z)!, hence this func
//       halfx represents half the length of the box
//       halfy represents half the height of the box
//       halfz represents half the width of the box
// Note: The box is centered at (0,0,0)
//-----------------------------------------------------------------------------
void CShip::drawWireBox(float halfx, float halfy, float halfz)
{
  glPushAttrib(GL_ENABLE_BIT);
    glDisable( GL_TEXTURE_2D );

    glBegin( GL_LINES );
    {
      float size = 30.0;
      glVertex3f( -halfx,  halfy,  halfz ); glVertex3f(  halfx,  halfy,  halfz );
      glVertex3f(  halfx,  halfy,  halfz ); glVertex3f(  halfx,  halfy, -halfz );
      glVertex3f(  halfx,  halfy, -halfz ); glVertex3f( -halfx,  halfy, -halfz );
      glVertex3f( -halfx,  halfy, -halfz ); glVertex3f( -halfx,  halfy,  halfz );
                                                                              
      glVertex3f( -halfx, -halfy,  halfz ); glVertex3f( -halfx,  halfy,  halfz );
      glVertex3f(  halfx, -halfy,  halfz ); glVertex3f(  halfx,  halfy,  halfz );
      glVertex3f(  halfx, -halfy, -halfz ); glVertex3f(  halfx,  halfy, -halfz );
      glVertex3f( -halfx, -halfy, -halfz ); glVertex3f( -halfx,  halfy, -halfz );
                                                                              
      glVertex3f( -halfx, -halfy,  halfz ); glVertex3f(  halfx, -halfy,  halfz );
      glVertex3f(  halfx, -halfy,  halfz ); glVertex3f(  halfx, -halfy, -halfz );
      glVertex3f(  halfx, -halfy, -halfz ); glVertex3f( -halfx, -halfy, -halfz );
      glVertex3f( -halfx, -halfy, -halfz ); glVertex3f( -halfx, -halfy,  halfz );
    }
    glEnd();

  glPopAttrib();
}

//-----------------------------------------------------------------------------
// Func: void CShip::drawAxis()
// Desc: Draws an axis relative to the current transform.
// Note: (r, g, b) == (x,y,z)
//-----------------------------------------------------------------------------
void CShip::drawAxis()
{
  glPushMatrix();
  glPushAttrib( GL_CURRENT_BIT | GL_ENABLE_BIT );
  glDisable( GL_TEXTURE_2D );

  glBegin( GL_LINES );
    glColor3f (   1.0f, 0.0f, 0.0f ); //x red
    glVertex3f(   0.0f, 0.0f, 0.0f );
    glVertex3f( 100.0f, 0.0f, 0.0f );

    glColor3f ( 0.0f,   1.0f, 0.0f ); //y green
    glVertex3f( 0.0f,   0.0f, 0.0f );
    glVertex3f( 0.0f, 100.0f, 0.0f );

    glColor3f ( 0.0f, 0.0f,   1.0f ); //z blue
    glVertex3f( 0.0f, 0.0f,   0.0f );
    glVertex3f( 0.0f, 0.0f, 100.0f );
  glEnd();

  glPopAttrib();
  glPopMatrix();
}

//-----------------------------------------------------------------------------
// Func: void CShip::loadSounds()
// Desc: Sounds are loaded (hence the name). 
//       The names of the sound files to load are hard-coded
// Note: Only loads sounds for the player
//-----------------------------------------------------------------------------
void CShip::loadSounds()
{
  if( initialized || !isPlayer )
  {
    return;
  }

  char *sndFile = "audio/ambmines.wav"; //"audio/b.wav"; //

  sndEngine = FSOUND_Sample_Load( FSOUND_FREE, 
                                  sndFile,
                                  FSOUND_8BITS | FSOUND_MONO | FSOUND_LOOP_NORMAL, 
                                  0 );
  if( sndEngine == NULL )
  {
    error.msg( "CLIENT: CShip::loadSounds(): couldn't load %s", sndFile );
  }
  
  channel = FSOUND_PlaySound( FSOUND_FREE, sndEngine );
  FSOUND_SetVolume( channel, 0 );

  error.msg( "CLIENT: Loaded sound %s", sndFile);
}

//-- E O F ------------------------------------------------------------------