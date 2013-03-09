// CShipLoad.cpp (Spark2)
// by Alex D'Angelo
// Created on May 23, 2000
// Last modified on May 24, 2000
//
// Notes: All the ship loading functions 
//
//        Implement game vars for the ship! (ShieldStrength, weight, ...)
//        We can feed these figures to MathEngine too!
//
//---------------------------------------------------------------------------

#include "CShip.h"

//---------------------------------------------------------------------------
// Func: CShip::openFile(char *aseFilename)
//
// Returns 1 if filestream successfully opened
//         0 if unsuccessful
//---------------------------------------------------------------------------
bool CShip::openFile(char *aseFilename)
{
  aseStream.open(aseFilename, ios::in | ios::_Nocreate);

  if(!aseStream.is_open()) 
  {
    error.msg( "ERROR:  Cannot open %s for conversion", aseFilename );
    
    aseStream.close();
    
    return 0;
  }
  else
  {
    // cout << "Opened " << aseFilename << " successfully." << endl;
    return 1;
  }
}

//---------------------------------------------------------------------------
// Func: CShip::closeFile()
//---------------------------------------------------------------------------
void CShip::closeFile()
{
  if(aseStream.is_open())
  {
    // cout << "Closing file" << endl;
    aseStream.close();
  }
}

//---------------------------------------------------------------------------
// Func: CShip::convert(char *aseFilename)
//
// Note: Also known as CShip::load(char *aseFilename)
//    
// The main conversion loop.
// All the string comparisons are case insensitive
//
// Returns 1 if conversion successful
//         0 if unsuccessful
//---------------------------------------------------------------------------
//
bool CShip::convert(char *aseFilename)
{
  bool goodToGo = 0;

  goodToGo = openFile(aseFilename);

  if(goodToGo)
  {
    while(!aseStream.eof())
    {
      aseStream >> buffer;

      if( !_stricmp("*MATERIAL", buffer) )                  // found a texture
      {
        // cout << "Found a texture" << endl;
        
        texture.setFileStream(&aseStream);
        texture.add();
      }
      else if( !strcmp("*GEOMOBJECT", buffer) )             // we found a model
      {
        findString("*NODE_NAME");                           // get the name
        
        //-----------------
        // Parse model name
        //-----------------
        memset(buffer, '\0', sizeof(char) * CONV_BUFFER_LEN);
        aseStream.getline(buffer, sizeof(char) * CONV_BUFFER_LEN);
       
        char seps[] = " ,\"";
        char *token;
        token = strtok( _strlwr(buffer), seps );
        
        // each model should have physical properties associated with it, unless it just sits
        //  there and no one can interact with it. 
// ###################################
// ## Look and fix/doc/add to this! ##
// ###################################
        if( !_stricmp(token, "physics") )
        {
          // get the params
          token = strtok( NULL, seps );

          if(token != NULL)
          {
            if( _stricmp( token, "bbox" )        ||
                _stricmp( token, "boundingbox" )  ) // we should enable multiple bounding boxes (mcd) for the ships
            {                                       //  and then lash them together with rigid joints (?)
              
              // getBoundingBox(); // which just gets the half length, width, and height
              // the bounding box must be centered at (0,0,0), as must the ship
              // the bounding box's size is determined by max(box's points)
            }
            else if( _stricmp( token, "boundingsphere" ) ||
                     _stricmp( token, "bsphere" )         )
            {
              // getBoundingSphere(); // which really just gets the radius of the bSph
            }
            else
            {
              // don't do anything for now
              // but  also consider weight, accel, etc.
            }
          }
        }
        else // this is a normal ship model
        {
          char *modelName = token; // alias

          // cout << "Found model: " << modelName << endl;
          model.setFileStream(&aseStream);
          model.add(modelName);
        }
      }

      else aseStream.getline(buffer, sizeof(buffer));
    } // end while

    // cout << "Done reading ASE" << endl;
    closeFile();

    //----------------------------------------
    // Init OpenGL-related stuff and Textures
    //----------------------------------------
    model.genVertArrayAll();
    
    texture.loadAll();
    model.loadAllTextures();

    //-----------------
    // Done loading ASE
    //------------------
    return 1;
  }
  else
  {
    error.msg( "Couldn't load ASE file" );
    closeFile();
    return 0;
  }
}

//---------------------------------------------------------------------------
// Func: char *findString()  --- Looks for a string in an open text file.
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
//---------------------------------------------------------------------------
//
char *CShip::findString(char *string)
{
  memset(buffer, '\0', sizeof(char) * CONV_BUFFER_LEN);  // clear the buffer
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

//-- E O F ------------------------------------------------------------------
