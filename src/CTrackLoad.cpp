// CTrackLoad.cpp
// by Alex D'Angelo
// Created on Aug 09, 2000
// Last modified on Aug 09, 2000
//
//---------------------------------------------------------------------------

#include "CTrack.h"

//###########################################################################
// Note: All the game data extraction functions, call
//       param = strtok(NULL, seps);
//       The initial call of strtok(buffer, seps) was done when reading the
//       main type of data. For example, for data name "checkpoint 1", 
//       "checkpoint" would have been read first, then getCheckpoint() 
//       would be called. That's when the next parameter would be retrieved
//       via param = strtok(NULL, seps).
//###########################################################################
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Func: bool CTrack::initGates()
// Desc: Allocates enough mem for all the gates
// Rtns: 0 = not initialized, 1 = initialized
// Note: Must be called before loading in all the gates!
//---------------------------------------------------------------------------
bool CTrack::initGates()
{
  numGates = getNumGatesASE();
  gate     = new tGate[ numGates ];
  
  if(gate != NULL)
  {
    memset(gate, 0, numGates * sizeof(tGate));
    return 1;
  }
  else
  {
    gate = NULL;
    return 0;
  }
}

//---------------------------------------------------------------------------
// Func: int CTrack::getNumGatesASE()
// Desc: Counts the number of gates (checkpoints, plus the start and finish
//       line) in the level. The count is needed to determine how many
//       tGates to allocate and check again the ships.
// Rtns: The number of gates found.
// Note: Counts the number of gates an open ASE file stream
//---------------------------------------------------------------------------
int CTrack::getNumGatesASE()
{
  aseStream->clear();
  aseStream->seekg(0, ios::beg);

  char seps[] = " ,\"";
  char *param;
  
  while(!aseStream->eof())
  {
    if( findString( "*NODE_NAME" ) ) // Check to see if the object exists,
    {                                // Then:
      findString( "*NODE_NAME" );    // There are two identical *NODE_NAMEs
                                     // per object, so read the second name
      *aseStream >> buffer;           
      
      param = strtok(buffer, seps);

      if( !_stricmp("checkpoint", param) )
      {
        numCheckpoints++;
      }
      else if( !_stricmp("startline", param) )
      {
        numStartline++;
      }
      else if( !_stricmp("finishline", param) )
      {
        numFinishline++;
      }
    }
  }

  int gateCount = numCheckpoints + numStartline + numFinishline;

  if(gateCount <= 0) // should this be changed to 2 (1 each req) ?
  {
    error.msg( "ERROR:  Invalid number of gates (checkpoints, " \
               "startline, finish line)(%d) invalid", gateCount );
  }

  // return file to the beginning of the stream
  aseStream->clear();
  aseStream->seekg(0, ios::beg);

  return gateCount;
}


//---------------------------------------------------------------------------
// Func: void CTrack::getCheckpoint()
// Desc: Wraps getGate() by providing valid checkpoint id
// Note: Checkpoint ids must start at 1
//       The checkpoints *aren't* translated back to (0,0,0) like other 
//        models!
//---------------------------------------------------------------------------
void CTrack::getCheckpoint()
{
  char seps[] = " ,\"";
  char *param;
 
  param = strtok( NULL, seps ); // get checkpoint number
  
  if(param != NULL)
  {
    int id = atoi( param );     // convert into a number

    if( id < 1 )
    {
      error.msg( "ERROR: The checkpoint number must be 1 or larger " \
                 "(id was %d)", id );
    }
    else if (id > numCheckpoints)
    {
      error.msg( "ERROR: The checkpoint must not be larger than the "\
                 "total amount of checkpoints! (id was %d, numChkpts"\
                 " was %d)", id, numCheckpoints );
    }
    else
    {
      getGate( id );
      gate[id].isCheckpoint = 1;
    } 
  }
  else
  {
    error.msg( "ERROR:  checkpoint number not specified!" );
  }
}

//---------------------------------------------------------------------------
// Func: void CTrack::getStartline()
// Desc: Wraps getGate() by providing the startline id
// Note: The startline is assigned id 0 in the gate[] array
//---------------------------------------------------------------------------
void CTrack::getStartline()
{
  int startlineId = 0;
  
  getGate( startlineId );
  
  gate[startlineId].isStartline = 1;
}

//---------------------------------------------------------------------------
// Func: void CTrack::getFinishline()
// Desc: Wraps getGate() by providing the finishline id
// Note: The startline is assigned id numGates+1 in the gate[] array
//---------------------------------------------------------------------------
void CTrack::getFinishline()
{
  int finishlineId = numCheckpoints + 1;
  
  getGate( finishlineId );
  
  gate[finishlineId].isFinishline = 1;
}

//---------------------------------------------------------------------------
// Func: void CTrack::getGate(int id)
// Desc: Reads the geometry from the ASE file and stores it
//---------------------------------------------------------------------------
void CTrack::getGate(int id)
{
  gate[id].id = id; // how's that for redundancy

  int done = 0;
  
  while(!aseStream->eof() && !done)
  {
    *aseStream >> buffer;

    if(!strcmp(buffer, "*TM_POS"))
    {
      *aseStream >> gate[id].pos[0]        // swap y & z
                >> gate[id].pos[2]
                >> gate[id].pos[1];
    
      gate[id].pos[2] = -gate[id].pos[2]; // for OpenGL
    }
    else if(!strcmp(buffer, "*MESH_NUMVERTEX")) // just for error checking
    {
      int tempNumVert;
      *aseStream >> tempNumVert;
      if(tempNumVert != 4)
      {
        error.msg("ERROR: Number of verts for gate %d is %d. Should be 4",
                  id, tempNumVert);
      }
    }
    else if(!strcmp(buffer, "*MESH_NUMFACES")) // just for error checking
    {
      int tempNumFaces;
      *aseStream >> tempNumFaces;
      if(tempNumFaces != 2)
      {
        error.msg("ERROR: Number of faces for gate %d is %d. Should be 2",
                   id, tempNumFaces);
      }
    }
    else if(!strcmp(buffer, "*MESH_VERTEX"))
    {
      int index;
      *aseStream >> index;
      *aseStream >> gate[id].vert[index].x
                >> gate[id].vert[index].z
                >> gate[id].vert[index].y;
      
      // in 3ds negative z goes out of the screen; we want it to go in
      gate[id].vert[index].z = -gate[id].vert[index].z;
    }
    else if(!strcmp(buffer, "*MESH_FACE"))
    {
      int  index;
      char dummy[4];

      *aseStream >> index;                                       // 0
      *aseStream >> dummy                                        // :
                >> dummy >> gate[id].face[index].vertIndex[0]   // A: 5
                >> dummy >> gate[id].face[index].vertIndex[1]   // B: 0
                >> dummy >> gate[id].face[index].vertIndex[2];  // C: 6
    }
    else if(!strcmp( buffer, "*PROP_MOTIONBLUR" ) || 
            !strcmp( buffer, "*PROP_CASTSHADOW" )  )
    {
      done = 1;
    }
    else aseStream->getline(buffer, sizeof(buffer));
  }
} // end getGate()

//---------------------------------------------------------------------------
// Func: void CTrack::calcGateCollisionZones()
// Desc: Converts triangle into Triangle3 format for collision detection
// Notes: We put ASE tri 0 into Triangle3 tri because that one seems to be
//        on the ground and seems more likely that we'll hit that first
//
// "Triangle has vertices (1,0,0), (0,1,0), and (0,0,1), so triangle is
//  (1,0,0) + u*(-1,1,0) + v*(-1,0,1) for 0 <= u, 0 <= v, and u+v <= 1."
//
//  The ASE data is structured as:
//   pt0-3:xyz
//   faceidx0: 2 0 3
//   faceidx1: 1 3 0
//
//   the points, when plotted according to the index, look like:
//   
//   2-------3
//   |    /  |
//   |  /    | 
//   0-------1
//
//   knowing this, for tri 0 we need to use vert 2 as the base vertex
//                 for tri 1 we need to use vert 1 as the base vertex
//   
//
//   dave eberly's format is b + u*e0 + v*e1, 
//   where b = vertex 0
//         u = vertex 1 - vertex 0
//         v = vertex 2 - vertex 0
//
//
//   triangle 0              tri 0 in terms of faces
//   b = pt 2                b = face0[0]
//   e0= pt 0 - pt 2         u = face0[1] - face0[0] == face0[1] - b
//   e1= pt 3 - pt 2         u = face0[2] - face0[0] == face0[2] - b
//
//   and tri 1               tri 1 in terms of faces                
//   b = pt 1                b = face1[0]                           
//   e0= pt 0 - pt 1         u = face1[1] - face1[0] == face1[1] - b
//   e1= pt 3 - pt 1         u = face1[2] - face1[0] == face1[2] - b
//
//---------------------------------------------------------------------------
void CTrack::calcGateCollisionZones()
{
  int vertId;
  int i;

  for( i = 0; i < numGates; i++ )
  {
    // Triangle 0 is actually triangle 1 in the ASE file
    vertId = gate[i].face[1].vertIndex[0];
    gate[i].tri[0].b.x = gate[i].vert[vertId].x;
    gate[i].tri[0].b.y = gate[i].vert[vertId].y;
    gate[i].tri[0].b.z = gate[i].vert[vertId].z;

    vertId = gate[i].face[1].vertIndex[1];
    gate[i].tri[0].e0.x = gate[i].vert[vertId].x - gate[i].tri[0].b.x;
    gate[i].tri[0].e0.y = gate[i].vert[vertId].y - gate[i].tri[0].b.y;
    gate[i].tri[0].e0.z = gate[i].vert[vertId].z - gate[i].tri[0].b.z;

    vertId = gate[i].face[1].vertIndex[2];
    gate[i].tri[0].e1.x = gate[i].vert[vertId].x - gate[i].tri[0].b.x;
    gate[i].tri[0].e1.y = gate[i].vert[vertId].y - gate[i].tri[0].b.y;
    gate[i].tri[0].e1.z = gate[i].vert[vertId].z - gate[i].tri[0].b.z;

    // Triangle 1 is actually triangle 0 in the ASE file
    vertId = gate[i].face[0].vertIndex[0];
    gate[i].tri[1].b.x = gate[i].vert[vertId].x;
    gate[i].tri[1].b.y = gate[i].vert[vertId].y;
    gate[i].tri[1].b.z = gate[i].vert[vertId].z;

    vertId = gate[i].face[0].vertIndex[1];
    gate[i].tri[1].e0.x = gate[i].vert[vertId].x - gate[i].tri[1].b.x;
    gate[i].tri[1].e0.y = gate[i].vert[vertId].y - gate[i].tri[1].b.y;
    gate[i].tri[1].e0.z = gate[i].vert[vertId].z - gate[i].tri[1].b.z;

    vertId = gate[i].face[0].vertIndex[2];
    gate[i].tri[1].e1.x = gate[i].vert[vertId].x - gate[i].tri[1].b.x;
    gate[i].tri[1].e1.y = gate[i].vert[vertId].y - gate[i].tri[1].b.y;
    gate[i].tri[1].e1.z = gate[i].vert[vertId].z - gate[i].tri[1].b.z;
  }
}

//---------------------------------------------------------------------------
// Func: bool CTrack::initStartPos()
// Desc: Allocates mem for all the start positions
// Rtns: 0 = not initialized, 1 = initialized
// Note: Must be called BEFORE loading in the start positions
//---------------------------------------------------------------------------
bool CTrack::initStartPos()
{
  shipStartPos = NULL;

  numStartPos = getNumStartPosASE();
  numPlayersMax = numStartPos;

  shipStartPos = new vect3[ numStartPos ];
  
  if(shipStartPos != NULL)
  {
    memset(shipStartPos, 0, numStartPos * sizeof(vect3));
    return 1;
  }
  else
  {
    return 0;
  }
}

//---------------------------------------------------------------------------
// Func: int CTrack::getNumStartPos()
// Desc: Counts the number of "startpos" tags (specified in *NODE_NAME)
// Rtrn: The number of start positions available (therefore the 
//       max num of players)
// Note: The maximum number of players should be determined by the number of
//       start positions.
//---------------------------------------------------------------------------
int CTrack::getNumStartPosASE()
{
  aseStream->clear();
  aseStream->seekg(0, ios::beg);

  char seps[] = " ,\"";
  char *param = NULL;
  int playerCount = 0;

  while(!aseStream->eof())
  {
    if( findString( "*NODE_NAME" ) )
    {
      findString( "*NODE_NAME" ); // we're guaranteed to have a
                                  //  second *NODE_NAME
      *aseStream >> buffer;
      
      param = strtok(buffer, seps);
      
      if( !_stricmp( "startpos", param ) )
      {
        playerCount++;
      } 
    }
  }

  // return file to the beginning of the stream
  aseStream->clear();
  aseStream->seekg(0, ios::beg);

  return playerCount;
}

//---------------------------------------------------------------------------
// Func: void CTrack::getStartpos()
// Desc: Extracts the (xyz) start position for a ship from an open ASE
//       file stream
// Note: Start positions start at 1, however, they are stored in pos (id-1)
//       We label the start positions starting at 1 because imo, most people
//       start at 1, instead of 0 like us C programmers.
//---------------------------------------------------------------------------
void CTrack::getStartPos()
{
  char seps[] = " ,\"";
  char *param;

  param = strtok(NULL, seps); // get startposition number

  if(param != NULL)
  {
    int num = atoi(param); // convert into a number
    
    if(num <= numPlayersMax && num > 0)
    {
      // We need to get the position, so search for
      // *TM_POS 0.0000	16.0000	0.0000
      findString( "*TM_POS" );
      //memset(buffer, '\0',      sizeof(char) * CONV_BUFFER_LEN);
      //aseStream->getline(buffer, sizeof(char) * CONV_BUFFER_LEN);

      *aseStream >> shipStartPos[num-1][0]               // swap y & z
                >> shipStartPos[num-1][2]
                >> shipStartPos[num-1][1];
      
      shipStartPos[num-1][2] = -shipStartPos[num-1][2]; // for opengl
    }
    else
    {
      error.msg("ERROR:  Start pos (%d) is greater or less than" \
                " the max number (%d)", num, numPlayersMax);
    }
  }
  else
  {
    error.msg( "ERROR:  ship starting position number not specified!" );
  }
}

//---------------------------------------------------------------------------
// Func: vect3 *CTrack::getStartPos( int i )
// Rtns: Returns the i'th start position. On invalid index, returns stPos[0]
//---------------------------------------------------------------------------
vect3 *CTrack::getStartPos( int i )
{
  if( i > (numStartPos - 1) ||
      i < 0                  )
  {
    error.msg( "CLIENT: Invalid start position id (%d). Using id 0", i );
  }
  
  return &shipStartPos[ i ];
}

//---------------------------------------------------------------------------
// Func: bool CTrack::initPowerups()
// Desc: Allocates enough mem for the powerups before reading them in from 
//       an open ASE file stream
// Rtns: 0 = not initialized, 1 = initialized
// Note: Must be called BEFORE reading in any powerups from the ASE file
//---------------------------------------------------------------------------
bool CTrack::initPowerups()
{
  numPowerups = getNumPowerupsASE();
  powerup = new tPowerup[ numPowerups ] ;
  
  if(powerup != NULL)
  {
    memset(powerup, 0, numPowerups * sizeof(tPowerup));
    return 1;
  }
  else
  {
    powerup = NULL;
    return 0;
  }
}

//---------------------------------------------------------------------------
// Func: int CTrack::getNumPowerupsASE()
// Desc: Counts the number of powerups in the level. The count is needed to
//       determine how many tPowerups to allocate and check again the ships.
// Rtns: The number of gates found.
//---------------------------------------------------------------------------
int CTrack::getNumPowerupsASE()
{
  aseStream->clear();
  aseStream->seekg(0, ios::beg);

  char seps[] = " ,\"";
  char *param = NULL;
  int powerupCount = 0;

  while(!aseStream->eof())
  {
    if( findString( "*NODE_NAME" ) )
    {
      findString( "*NODE_NAME" ); // we're guaranteed to have a
                                  //  second *NODE_NAME
      *aseStream >> buffer;
      
      param = strtok(buffer, seps);
      
      if( !_stricmp( "powerup", param ) )
      {
        powerupCount++;
      } 
    }
  }

  // return file to the beginning of the stream
  aseStream->clear();
  aseStream->seekg(0, ios::beg);
  
  return powerupCount;
}

//---------------------------------------------------------------------------
// Func: void CTrack::getPowerup()
// work on defining what a powerup type is and how to implement that
// we should also get rid of always allocating 255 bytes for the string
// This sets the values for the powerup
//---------------------------------------------------------------------------
void CTrack::getPowerup()
{
  char seps[] = " ,\"";
  char *param;

  param = strtok(NULL, seps);

  if(param != NULL)         // make sure there is a parameter
  {
    param = _strlwr(param); // get the type of powerup
    
    strcpy( powerup[ currPowerupId ].szName, param ); // we should do some error checking to make sure strlen(param) < 255
    
    if( !_stricmp( powerup[ currPowerupId ].szName, "speed" ) )
    {
      powerup[ currPowerupId ].typeId     = PU_SPEED;
      powerup[ currPowerupId ].spawnDelay = 808;
      powerup[ currPowerupId ].spawnReset = 808;
      powerup[ currPowerupId ].active     = 1;
    }
    else if( !_stricmp( powerup[ currPowerupId ].szName, "time" ) )
    {
      powerup[ currPowerupId ].typeId     = PU_TIME;
      powerup[ currPowerupId ].spawnDelay = 777;
      powerup[ currPowerupId ].spawnReset = 777;
      powerup[ currPowerupId ].active     = 1;
    }
    else
    {
      powerup[ currPowerupId ].typeId     = PU_UNKNOWN; // default for now
      powerup[ currPowerupId ].spawnDelay = 666;
      powerup[ currPowerupId ].spawnReset = 666;
      powerup[ currPowerupId ].active     = 1;
    }
   
    //error.msg( "WARNING: Alex needs to finish reading in the powerup type!" );

    // We just need to get the position, so search for
    // *TM_POS 0.0000	16.0000	0.0000
    findString( "*TM_POS" );

    *aseStream >> powerup[ currPowerupId ].pos[0]    // swap y and z
              >> powerup[ currPowerupId ].pos[2]
              >> powerup[ currPowerupId ].pos[1];

    powerup[ currPowerupId ].pos[2] *= -1;          // for opengl

    currPowerupId++;
  }
  else
  {
    error.msg( "ERROR:  powerup type not specified!" );
  }
}

//---------------------------------------------------------------------------
// Func: void CTrack::getLaps()
// Desc: Extracts the number of laps a ship need to make to complete the race
//       from an open ASE file stream
// Note: For tracks that have different starting and ending points (like a
//       drag race), the number of laps should be 0. 
//---------------------------------------------------------------------------
void CTrack::getLaps()
{
  char seps[] = " ,\"";
  char *param;

  // cout << "Found number of laps" << endl;

  param = strtok(NULL, seps); // get checkpoint number

  if(param != NULL)
  {
    numLaps = atoi(param); // convert into a number
    if(numLaps < 0)
    {
      error.msg( "ERROR:  Invalid number of laps (%d)", numLaps );
    }
  }
  else
  {
    error.msg( "ERROR:  Number of laps not specified! Defaulting to 0" );
    numLaps = 0;
  }
}

//---------------------------------------------------------------------------
// Func: char *findString()  --- Searches for a string in an open text file.
// 
// Parameters:
// char *string  - string to search for
//
// Returns:
// A pointer to the string if it was found
//
// Notes: 
// *The search is case-insensitive
// *This function will not do a sub-string search. It will 
//  only look for strings delimited by white space
// *This function searches from the current file position. 
//
//---------------------------------------------------------------------------
//
char *CTrack::findString(char *string)
{
  memset(buffer, '\0', sizeof(char) * CONV_BUFFER_LEN);// clear the buffer
  
  while(_stricmp(buffer, string) && !aseStream->eof()) // case insensitive
  {
    *aseStream >> buffer; 
  }

  if(!aseStream->eof())      // if we're not at eof we found it
    return &buffer[0];
  else                      // else we found nothing
    return NULL;
}

//-- E O F ------------------------------------------------------------------