// CSceneLoad.cpp (Spark2) 
// by Alex D'Angelo
// Created on April 26, 2000
// Last modified on May 25, 2000
// 
// The level ASE file contains both model data and game data. The game data
// is encoded in the models through the model name and position. Depending
// on the game data model name, the actual model may or may not be drawn. 
// The names are case-insensitive.
//
// GAME DATA
// ---------
// To enter game data into the 3dStudio scene, create any object (a cube or
// plane is fine) and name it according to the following instructions. The
// actual may or may not be drawn, depending on the model's name. 
//
// The naming convention for game data is:
//   gameDataName param
// 
// Here's a list of the game data names, parameters, encoding type, if it is
// drawn, and if so, what is drawn:
//
// startpos      number           (model;  not drawn: the smaller the number, closer to start line)
// checkpoint    number           (model;  not drawn: the number starts at 1
// laps          numOfLaps        (model;  not drawn)
// powerup       nameOfPowerup    (model;  drawn: at pos model is created at)
// track         <no parameters>  (model;  drawn: the actual track)
// trackspline   <no parameters>  (spline; not drawn)
// mapspline     <no parameters>  (spline; drawn: 2d overhead view)
// worldlight    <no parameters>  (light;  drawn: the light)
// hidden        <doesn't matter> (model or spline, not loaded and not drawn by defn)
// startline     <no parameters>  (model;  not drawn: must be a vertical quad) 
// finishline    <no parameters>  (model;  not drawn: must be a vertical quad)
//                                (note: if the finishline isn't specified, the startline will be used as the finish line)
//                                (note: a separate model should be created to display the start/finish line "gate")
//
// <removed: now determined by number of start positions> maxNumPlayers numPlayers       (model;  not drawn)
//
// An example of creating game data is to create a cube, name it 
// "powerup shield" (without the quotes), and position it in the world where
// the powerup should appear. 
//
// Another example would be to name the cube "laps 03"; the position of the 
// cube wouldn't matter since it is not drawn. 
//
// MODELS
// ----------
// These models are what you want drawn in the scene. Models can be called
// anything, however, they take advantage of a modeldata + instance system
// where there can be one model and 10 instances of that model.
//
// The naming convention is "modelnameXX", where XX is a number (no quotes)
// The first model with the prefix "modelname" will have its data stored
// and an instance will be created of where the model should be drawn. Any
// other models with the prefix of "modelname" will cause the program to
// create an instance referring to the original model.
//
// (end of info)
//
// To do:
// *Do something with checkpoints, powerups, mapspline, ... (May 25, 2000)
//
// *does ase2lev handle names without numbers (ie will it read in "Tree"
//  and make a model and/or inst correctly?)
// *what about duplicate model names?
//
//---------------------------------------------------------------------------

#include "CScene.h"

//#define breakLawsOfPhysics() shutdownPhysics();
#define rainFireAndBrimstone()
#define turnSeasToBlood()
#define pulverizeWorld()
#define obliterateUniverse()

//---------------------------------------------------------------------------
// Func: void CScene::load(char *aseFilename)
// Desc: Loads a level
//
// Notes: (June 13, 2000)
// *Deal with the filename if there are " " (quotes) around the filename
// *Deal with filenames with spaces! 
// *Deal with loading the ship or temporarily detaching and re-attaching to the scene
// 
//---------------------------------------------------------------------------

bool CScene::load(char *aseFilename)
{
  // remove leading and trailing spaces
//  strrlc(aseFilename); // !!!does nothing because aseFilename isn't directly operated on!!!
//  strrtc(aseFilename); // !!!does nothing because aseFilename isn't directly operated on!!!
  
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

  int pathSize = strlen("tracks/") + strlen(allocFileName) + strlen(".ase") + 1;
  fileAndPath = new char[ pathSize ];
  memset(fileAndPath, NULL, pathSize);
  

  strcpy(fileAndPath, "tracks/");
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

  if( !this->initialized ) init(); // inits the scene

  if( this->convert(fileAndPath) )
  {
    // Load and play the music
    
    mod = NULL; //FMUSIC_LoadSong( "audio/nova.s3m" );
    /*
    if(!mod)
    {
      //error.msg("CLIENT: CScene::Load(): %s", FMOD_ErrorString(FSOUND_GetError()));
	    error.msg( "CLIENT: CScene::Load(): couldn't load song" );
    }
    FMUSIC_PlaySong(mod);
    */
    
    SAFE_DELARY( fileAndPath );
    return true;
  }
  else
  {
    SAFE_DELARY( fileAndPath );
    return false;
  }
}

//---------------------------------------------------------------------------
// Func: void CScene::unload()
// Desc: Armageddon. 
//---------------------------------------------------------------------------
//
void CScene::unload()
{
  //shutdownPhysics(); //breakLawsOfPhysics();
  //physicsInitialized = 0;

    rainFireAndBrimstone();
  
    turnSeasToBlood();

    pulverizeWorld()

  model.unload();
  spline.unload();

  track.unload();

  light.unload();
  texture.unload();
  
  FMUSIC_FreeSong(mod);
  
  initialized = 0;

    obliterateUniverse();
}

//---------------------------------------------------------------------------
// Func: bool CScene::openFile(char *aseFilename)
// Desc: Opens an ASE file so the rest of the class can do what it does
//       best: read in ASE files and convert them.
//---------------------------------------------------------------------------
//
bool CScene::openFile(char *aseFilename)
{
  aseStream.open(aseFilename, ios::in | ios::_Nocreate);

  if(!aseStream.is_open()) 
  {
    error.msg( "ERROR:  Cannot open %s for conversion.", aseFilename );
    
    aseStream.close();
    
    return 0;
  }
  else
  {
    //error.msg( "Opened %s successfully!", aseFilename );
    return 1;
  }
}

//---------------------------------------------------------------------------
// Func: void CScene::closeFile()
// Desc: Closes the current file stream
//---------------------------------------------------------------------------
//
void CScene::closeFile()
{
  if(aseStream.is_open())
  {
    // cout << "Closing file" << endl;
    aseStream.close();
  }
}

//---------------------------------------------------------------------------
// Func: bool CScene::convert(char *aseFilename)
//
// Desc:
// The main conversion loop.
// All the string comparisons are case insensitive
//
// Returns 1 if conversion successful
//         0 if unsuccessful
//
// Note: Searches the file twice. The first is for "critical" init data,
//       such as the number of checkpoints or number of players, which in 
//       turn is used to allocate an array big enough for all the potential
//       data.
//---------------------------------------------------------------------------
//
bool CScene::convert(char *aseFilename)
{
  bool goodToGo = 0;

  goodToGo = openFile(aseFilename);

  if(goodToGo)
  {
    track.setFileStream      (&aseStream);
    model.setTexPointer      (&texture);
    track.model.setTexPointer(&texture);
    
    texture.setFileStream     (&aseStream);
    light.setFileStream       (&aseStream);
    track.spline.setFileStream(&aseStream);
    spline.setFileStream      (&aseStream);
    track.setFileStream       (&aseStream);
    track.model.setFileStream (&aseStream);
    model.setFileStream       (&aseStream);

    //-------------------------------
    // # scan for init / critical values first #
    //-------------------------------
    //track.setFileStream(aseStream);
    track.initGates();
    
    //track.setFileStream(aseStream);
    track.initStartPos();

    //track.setFileStream(aseStream);
    track.initPowerups();

    // Go back to the beginning of the file
    aseStream.clear();
    aseStream.seekg(0, ios::beg);

    //-------------------------------
    // Now scan for everything
    //-------------------------------
    while(!aseStream.eof())
    {
      aseStream >> buffer;
      //  aseStream.getline(buffer, sizeof(buffer));

      if( !_stricmp("*MATERIAL", buffer) )                  // found a texture
      {
        // cout << "Found a texture" << endl;
        
        //texture.setFileStream(aseStream);
        texture.add();
      }
      else if( !_stricmp("*LIGHTOBJECT", buffer) )          // we found a light
      {
        //light.setFileStream(aseStream);
        light.add();
      }
      else if( !_stricmp("*SHAPEOBJECT", buffer) )          // we found a spline
      {
        findString("*NODE_NAME");                           // get the name

        memset(buffer, '\0',      sizeof(char) * CONV_BUFFER_LEN);
        aseStream.getline(buffer, sizeof(char) * CONV_BUFFER_LEN);
       
        char seps[] = " -_,\"";
        char *splineName;
        splineName = strtok( _strlwr(buffer), seps);

        // cout << "Found a spline with name: " << splineName << endl;

        if( !_stricmp(splineName, "hidden") )
        {
          ; // do nothing; we don't want to draw (therefore, not load) this object
        }
        else if( !_stricmp(splineName, "trackspline") )
        {
          // cout << "Adding track spline..." << endl;
          
          //track.spline.setFileStream(aseStream);
          track.spline.add(splineName);
        }
        else if( !_stricmp(splineName, "mapspline") )
        {
          error.msg( "Found a map spline. Skipping. (Not implemented yet)." );
        }
        else
        {
          // cout << "Adding spline..." << endl;
          //spline.setFileStream(aseStream);
          spline.add(splineName);
        }
      }
      else if( !strcmp("*GEOMOBJECT", buffer) )             // we found a model
      {
        findString("*NODE_NAME");                           // get the name
        
        //------------------
        // Parse model name
        //------------------
        memset(buffer, '\0', sizeof(char) * CONV_BUFFER_LEN);
        aseStream.getline(buffer, sizeof(char) * CONV_BUFFER_LEN);
       
        char seps[] = " -_,\"";
        char *token;
        token = strtok( _strlwr(buffer), seps);
        
        //-------------------------
        // Get game and model data
        //-------------------------
        if( !_stricmp(token, "hidden") )
        {
          ; // do nothing; we don't want to draw (therefore, not load) this object
        }
        else if( !_stricmp(token, "laps") )
        {
          //track.setFileStream(aseStream);
          track.getLaps();
        }
        else if( !_stricmp(token, "powerup") )
        {
          //track.setFileStream(aseStream);
          track.getPowerup();
        }
        else if( !_stricmp(token, "startpos") )
        {
          //track.setFileStream(aseStream);
          track.getStartPos();
        }
        else if( !_stricmp(token, "playerstartpos") )
        {
          //track.setFileStream(aseStream);
          findString( "*TM_POS" );
          if(!aseStream.eof())
          {
            aseStream >> track.playerStartPos[0]               // swap y & z
                      >> track.playerStartPos[2]
                      >> track.playerStartPos[1];
            track.playerStartPos[2] *= -1; // flip for 3ds
          }
        }
        else if( !_stricmp(token, "checkpoint") )
        {
          //track.setFileStream(aseStream);
          track.getCheckpoint();
        }
        else if( !_stricmp(token, "startline") )
        {
          //track.setFileStream(aseStream);
          track.getStartline();
        }
        else if( !_stricmp(token, "finishline") )
        {
          //track.setFileStream(aseStream);
          track.getFinishline();
        }
        else if( !_stricmp(token, "track") || !_stricmp(token, "trackmodel"))
        {
          char *trackName = token; // alias

          //track.model.setFileStream(aseStream);
          track.model.add(trackName);
        }
        else // it's just a generic model
        {
          char *modelName = token; // alias

          // cout << "Found model: " << modelName << endl;
          //model.setFileStream(aseStream);
          model.add(modelName);
        }
      }
      else
      {
          aseStream.getline(buffer, sizeof(buffer));//strlen(buffer));
      }
      
      //error.msg("Reading line %d", aseStream.tellg());
    } // end while
    // cout << "Done reading ASE" << endl;

    //----------------------------------------
    // Done loading ASE
    //----------------------------------------
    closeFile();    
    loadedOk = 1; // sets var inside CScene so I know it loaded ok when debugging

    //----------------------------------------
    // Init OpenGL-related stuff and Textures
    //----------------------------------------
    model.genVertArrayAll();
    track.model.genVertArrayAll();

    texture.loadAll();
    model.loadAllTextures();
    track.model.loadAllTextures();
    //track.genCdModel();

    spline.genVertArrayAll();
    track.spline.genVertArrayAll();

    //----------------------------------------
    // Init checkpoints
    //----------------------------------------
    track.calcGateCollisionZones();
    
    return 1;
  }
  else
  {
    error.msg("ERROR: Couldn't load ASE file (CScene::convert)");
    closeFile();
    loadedOk = 0; // sets var inside CScene so I know it loaded ok when debugging
    return 0;
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
char *CScene::findString(char *string)
{
  memset(buffer, '\0', sizeof(char) * CONV_BUFFER_LEN);// clear the buffer
                                                       // because it's static

  while(_stricmp(buffer, string) && !aseStream.eof()) // case insensitive
  {
    aseStream >> buffer; 
  }

  if(!aseStream.eof())      // if we're not at eof we found it
    return &buffer[0];
  else                      // else we found nothing
    return NULL;
}

//---------------------------------------------------------------------------
// Note: For all the game data extraction functions, they call
//       param = strtok(NULL, seps);
//       This will get the rest of the parameters since when reading the
//       parameters, a call to strtok(buffer, seps) is called first.
//---------------------------------------------------------------------------

//-- E O F ------------------------------------------------------------------