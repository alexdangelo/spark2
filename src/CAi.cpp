// CAi.cpp
// By Alex D'Angelo
// Created on July 19, 2000
// Last modified on July 19, 2000
//
//---------------------------------------------------------------------------

#include "CAi.h"

const int numShipFilenames = 4;
char shipFilename[4][64] = {{ "speeder01.ase" },
                            { "speeder02.ase" },
                            { "michael.ase"   },
                            { "ship01tc.ase"  }};

//---------------------------------------------------------------------------
// Should I used an array or linked list for all the AI ships? Since the
// number of players won't change during the course of a race, it would be
// easiest to use an array (CShip *pships = new CShip[numAiPlayers]).
//
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Func: CAi::CAi()
// Desc: Constructor. CAi::init(number) must be called before this object
//       can be used.
// Parm: None
//---------------------------------------------------------------------------
CAi::CAi()
{
  aiShip      = NULL;
  scene       = NULL;
  trackPtr    = NULL;
  numAiShips  = 0;
  initialized = 0;
}

//---------------------------------------------------------------------------
// Func: CAi::~CAi()
// Desc: Destructor. Just encapsulates the CAi::unload() func.
// Parm: None.
//---------------------------------------------------------------------------
CAi::~CAi()
{
  unload();
  scene       = NULL;
  trackPtr    = NULL;
}

//---------------------------------------------------------------------------
// Func: CAi::unload()
// Desc: Releases everything from memory and resets the object
// Parm: None.
// Note: We don't set the scene ptr to NULL because once we have access to
//       that object we might be loading other tracks and the object's
//       location won't change for the duration of the game
//---------------------------------------------------------------------------
void CAi::unload()
{
  if( this->initialized )
  {
    SAFE_DELARY( aiShip );
    numAiShips  = 0;
    initialized = 0;
  }
}

//---------------------------------------------------------------------------
// Func: bool CAi::attachToScene(CScene *existingScene);
// Desc: Connect the ship to the scene (and track). This function MUST be
//       called first, otherwise the ship won't be able to detect and
//       interact with the world and track.
//       Also connects the track pointer.
// Parm: CScene *existingScene: the currently loaded scene
// Rtns: 1 if successful. 0 if failed.
//---------------------------------------------------------------------------
bool CAi::attachToScene(CScene *existingScene)
{
  scene = existingScene;
  
  if( scene != NULL )
  {
    trackPtr = &scene->track;
    if( trackPtr != NULL )
    {
      return 1;
    }
    else
    {
      return 0;
    }
  }
  else
  {
    return 0;
  }
}

//---------------------------------------------------------------------------
// Func: bool CAi::init(int numberAiShips)
// Desc: Initializes all the AI ships and their locations and properties
// Parm: int numberAiShips: the number of AI ships to be created
//---------------------------------------------------------------------------
bool CAi::init(int numberAiShips)
{
  if( scene == NULL )
  {
    error.msg( "CLIENT: CAi::init(): AI scene pointer hasn't been " \
               "assigned! Do ai.attachToScene(&scene) first!" );
    return 0;
  }

  if( initialized == 1 )
  {
    error.msg( "CLIENT: CAi::init(): AI already initialized." );
    return 0;
  }

  if( numberAiShips < 1 )
  {
    error.msg( "ERROR:  numAiShips <= 0 (CAi::init())" );
    return 0;
  }

  numAiShips = numberAiShips;
  aiShip     = new CShip[ numAiShips ];

  int i;
  for( i = 0; i < numAiShips; i++ )
  {
    // Load game data
    aiShip[i].attachToScene( scene );
    aiShip[i].init();                       // contains vel = {0,0,0}, dir = {x,y,z}, ...
    aiShip[i].load( "ships/nullshp.ase" );  // load the null ship as a placeholder
  }

  initialized = 1;
  return initialized;
}

//---------------------------------------------------------------------------
// Func: void CAi::loadShips()
// Note: ai.attachToScene(&scene); must be called first
void CAi::loadShips()
{
  if( scene == NULL ) 
  {
    error.msg( "CLIENT: Scene not initialized! (CAi::loadShips)" );
  }

  if( this->initialized )
  {
    error.msg( "ERROR:  AI ships already initialized (CAi::loadShips)" );
    return;
  }

  init( trackPtr->getNumStartPos() ); // add as many ai ships as there are start positions
  
  srand( (unsigned)time( NULL ) );

  for( int i = 0; i < trackPtr->getNumStartPos(); i++ ) // i functions both at the currShip idx and the startPos idx
  {
    int randShipId = rand() % numShipFilenames;

    loadShip  ( i, shipFilename[randShipId]   );
    setShipPos( i, trackPtr->getStartPos( i ) ); // getStartPos should return a (vect3 *)
    alignShipWithTrack( i );
  }
}

//---------------------------------------------------------------------------
// Func: bool CAi::loadShip(int shipId, char *aseFilename)
// Desc: Loads a ship model file (.ASE) for a given ship in the aiShip array
// Parm: int shipId:  number of the AI ship
//       char *aseFn: the filename of the ASE file of the ship
// Rtrn: 1 if successfully loaded ship. 0 if failed.
//---------------------------------------------------------------------------
// usage: ai.loadShip(ship_id, "ships/speeder01.ase");
bool CAi::loadShip(int shipId, char *aseFilename)
{
  if( (shipId > (numAiShips - 1)) || (shipId < 0))
  {
    error.msg( "CLIENT: CAi::loadShip(): AI ship id (%d) out of range", shipId );
    return 0;
  }
  else
  {
    if(aiShip[shipId].initialized)
    {
      aiShip[shipId].unload();
    }

    aiShip[shipId].attachToScene( scene );
    bool   success = aiShip[shipId].load( aseFilename );
    return success;
  }
}

//---------------------------------------------------------------------------
// Func: void CAi::setShipPos( int shipId, vect3 *startPos )
// Desc: Places a ship at the given startPos position
// Parm: int shipId: an index for a loaded ship, with bounds [0, numAiShips)
//---------------------------------------------------------------------------
void CAi::setShipPos( int shipId, vect3 *startPos )
{
  if( (shipId > (numAiShips - 1)) || (shipId < 0))
  {
    error.msg( "CLIENT: CAi::loadShip(): AI ship id (%d) out of range", shipId );
  }

  aiShip[ shipId ].setPos( *startPos );
}

//---------------------------------------------------------------------------
// Func: void CAi::alignShipwithTrack( int shipId )
// Desc: Rotates the ship so it's z-axis is parallel with the spline at
//       the location of the ship
// Parm: int shipId: an index for a loaded ship, with bounds [0, numAiShips)
//---------------------------------------------------------------------------
// this requires access to the trackspline and 
// we also have to figure out the best way to describe the ship's orientation
//  and rotation
void CAi::alignShipWithTrack( int shipId )
{
  ; // this needs to be written
}

//---------------------------------------------------------------------------
// temp way of doing this. we just position them at vertex 0 of the track spline
//
/*
void CAi::setShipsAtStartLine()
{
  int i;

  if(scene->track.spline.splineHead->next == NULL)
  {
    error.msg( "CLIENT: setShipsAtStart(): Error: no track spline" );
    
    for( i = 0; i < numAiShips; i++) // place ships at default location
    {
      aiShip[i].pos[0] = 0;
      aiShip[i].pos[1] = 0;
      aiShip[i].pos[2] = 0;
    }
    
    return;
  }

  if(scene->track.spline.splineHead->next->numVerts < numAiShips) // in case the spline has only a few verts
  {
    error.msg( "CLIENT: setShipsAtStart(): Error: more ships than trackspline verts" );
    return;
  }

  // everything's ok, so place one ship per spline node
  for( i = 0; i < numAiShips; i++)
  {
    aiShip[i].pos[0] = scene->track.spline.splineHead->next->vert[i][0];
    aiShip[i].pos[1] = scene->track.spline.splineHead->next->vert[i][1];
    aiShip[i].pos[2] = scene->track.spline.splineHead->next->vert[i][2];
  }
}
*/

//---------------------------------------------------------------------------
// Func: void CAi::update()
// Desc: Updates all the AI logic and positions of the ships
// Parm: None.
//---------------------------------------------------------------------------
// Works, although the ship spins like a top! July 23, 2000
// NOTE: turn and thrust have been disabled as they are obsolete as of 2000-12-17
void CAi::update()
{
  // move all the ships and stuff
/*
  given a ship at (x,y)
  find the closest node
  then go to the next highest node % num_nodes
*/
  int i;
  int targetNode;
  int numNodes = scene->track.spline.splineHead->next->numVerts;
  vect3 *node = scene->track.spline.splineHead->next->vert;

  for( i = 0; i < numAiShips; i++)
  {
    targetNode = (scene->track.getClosestNode(aiShip[i].pos) + 1) % numNodes;
    //targetNode = rand() % numNodes;

    // now make a vector from the ship to the target node and move that way
    vect3 new_direction;
    new_direction[0] = node[targetNode][0] - aiShip[i].pos[0];
    new_direction[1] = node[targetNode][1] - aiShip[i].pos[1];
    new_direction[2] = node[targetNode][2] - aiShip[i].pos[2];

    // find out where the ship is currently heading
    vect3 ship_forward;
    
    ship_forward[0] = sinf(aiShip[i].heading * PIOVER180);
    ship_forward[1] = 0.0f;
    ship_forward[2] = cosf(aiShip[i].heading * PIOVER180);

    // get angle between the ship heading and the target node
    normalize(new_direction);
    normalize(ship_forward);
    
    dot(new_direction, ship_forward);
    float theta = acos( dot(new_direction, ship_forward) );

    float degree = theta * ONE_EIGHTY_OVER_PI; 

    if( degree > 0 ) { aiShip[i].turnLeft(); }
    if( degree < 0 ) { aiShip[i].turnRight(); }

    aiShip[i].accelerate();
    //aiShip[i].turn(degree);
    //aiShip[i].thrust(3);
  }
}

//---------------------------------------------------------------------------
// Func: void CAi::draw()
// Desc: Draws all the AI ships
// Parm: None.
//---------------------------------------------------------------------------
void CAi::draw()
{
  int i;

  for( i = 0; i < numAiShips; i++ )
  {
    aiShip[i].draw();
  }
}

//-- E O F ------------------------------------------------------------------
