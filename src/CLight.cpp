// CLight.cpp (Spark2)
// by Alex D'Angelo
// Created on May 10, 2000
// Last modified on May 11, 2000
//
// Overview: reads in lights
//
// Note:  This code is almost identical to the spline reading code.
//        The only differences are in the init and get functions; the 
//        link and unlink functions just have different variable names.
//
//---------------------------------------------------------------------------

#include "ConvCommon.h"

//---------------------------------------------------------------------------
CLight::CLight()
{
  initialized = 0;
  init();
}

//---------------------------------------------------------------------------
CLight::~CLight()
{
  unload();
}

//---------------------------------------------------------------------------
void CLight::init()
{
  if(!initialized)
  {
    numLights = 0;

    lightHead = NULL;
  
    lightHead = new tLight;
    memset(lightHead, 0, sizeof(tLight));
  
    lightHead->prev = lightHead;
    lightHead->next = lightHead;
  
    lightHead->typeName[0] = NULL; // should I memset it all to NULL?

    initialized = 1;
  }
}

//---------------------------------------------------------------------------
void CLight::unload()
{
  releaseLights();
  initialized = 0;
}

//---------------------------------------------------------------------------
void CLight::add()
{
  // cout << "Adding light" << endl << endl;

  tLight *new_light = new tLight;
  
  initLight (new_light);
  getLight  (new_light);
  linkLight (new_light); // adds light to linked-list of lights

  numLights++;
}


//-----------------------------------------------------------------------------
void CLight::initLight(tLight *currLight)
{
  /* Get this info:
    *LIGHTOBJECT 
	    *NODE_NAME "Omni01"
	    *LIGHT_TYPE Omni
	    *NODE_TM {
		    *NODE_NAME "Omni01"
        ...
		    *TM_ROW0 1.0000	0.0000	0.0000
		    *TM_ROW1 0.0000	1.0000	0.0000
		    *TM_ROW2 0.0000	0.0000	1.0000
		    *TM_ROW3 1.9814	-1.0223	127.6348
		    *TM_POS 1.9814	-1.0223	127.6348
  */

  bool done = 0;
  
  while(!aseStream->eof() && !done)
  {
    *aseStream >> buffer;

    if(!strcmp(buffer, "*LIGHT_TYPE"))
            *aseStream >> currLight->typeName; // handle the different types in the main loop
    
    else if(!strcmp(buffer, "*TM_POS"))
    {
            *aseStream >> currLight->worldpos[0]  // remember, 3ds likes to switch y and z
                      >> currLight->worldpos[2]
                      >> currLight->worldpos[1];

            // 3ds' z is our negative z; hopefully this will work
            currLight->worldpos[2] *= -1;

            done = 1;
    }

    else aseStream->getline(buffer, sizeof(buffer)); // finish reading the line and move on to the next
  }
}

//---------------------------------------------------------------------------
void CLight::getLight(tLight *currLight)
{
  if(!strcmp("Omni", currLight->typeName)) // figure out what kind of light we have
  {
    ; // null statement. do nothing because there's no more info for this
      // kind of light
  }
  else if(!strcmp("Target", currLight->typeName))
  {
    findString("*TM_POS");
    *aseStream >> currLight->targetpos[0]  // remember, 3ds likes to switch y and z
              >> currLight->targetpos[2]
              >> currLight->targetpos[1];

    // 3ds' z is our negative z; hopefully this will work
    currLight->targetpos[2] *= -1;
  }
  else
  {
    error.msg( "Found a type of light that I don't know about (%s)", 
                currLight->typeName);
    /*
    // cout << "Found a type of light that I don't know about "
         << "(" 
         << currLight->typeName 
         << ")" << endl;
    */
  }

  getLightParams(currLight);
}

//-----------------------------------------------------------------------------
void CLight::getLightParams(tLight *currLight)
{
  /* Get this info:
	  *LIGHT_SHADOWS Off
	  *LIGHT_USELIGHT 1
	  *LIGHT_SPOTSHAPE Circle
	  *LIGHT_USEGLOBAL 0
	  *LIGHT_ABSMAPBIAS 0
	  *LIGHT_OVERSHOOT 0
	  *LIGHT_SETTINGS 
		  *TIMEVALUE 0
		  *LIGHT_COLOR 1.0000	1.0000	1.0000
		  *LIGHT_INTENS 1.0000
		  *LIGHT_ASPECT -1.0000
		  *LIGHT_TDIST -1.0000
		  *LIGHT_MAPBIAS 1.0000
		  *LIGHT_MAPRANGE 4.0000
		  *LIGHT_MAPSIZE 512
		  *LIGHT_RAYBIAS 0.0000
  */

  bool done = 0;
  
  while(!aseStream->eof() && !done)
  {
    *aseStream >> buffer;

    if(!strcmp(buffer, "*LIGHT_SHADOWS"))
    {
      char castshadows[10];
      
      *aseStream >> castshadows;
      if(!strcmp(castshadows, "Off"))
      {
        currLight->hasShadow = 0;
      }
      else if(!strcmp(castshadows, "On"))
      {
        currLight->hasShadow = 1;
      }
      else 
      {
        error.msg( "Couldn't figure out if the light is supposed to cast shadows!" );
        error.msg( "Setting light to not cast shadows" );
        currLight->hasShadow = 0;
      }
    }

    else if(!strcmp(buffer, "*LIGHT_USELIGHT"))
    {
            int active;
            
            *aseStream >> active;
            
            if(active)
              currLight->isActive = 1;
            else
              currLight->isActive = 0;
    }

    else if(!strcmp(buffer, "*LIGHT_COLOR"))
            *aseStream >> currLight->color[0]
                      >> currLight->color[1]
                      >> currLight->color[2];

    else if(!strcmp(buffer, "*LIGHT_INTENS"))
    {
            *aseStream >> currLight->intens;
            done = 1;
    }
            
    else aseStream->getline(buffer, sizeof(buffer)); // finish reading the line and move on to the next
  }
}

//---------------------------------------------------------------------------
// Releases all the lights
//
void CLight::releaseLights()
{
  if(lightHead != NULL)       // if it hasn't been released yet
  {
    tLight *currLight = lightHead->next;
  
    // cout << "Releasing splines..." << endl;
    while(currLight != lightHead)
    {
      unlinkLight(&currLight);// automatically does currLight=currLight->next
    }
  
    // cout << "Releasing light head" << endl;
    SAFE_DELETE(lightHead);
  }
}

//---------------------------------------------------------------------------
// Links the current light to the light list
//
void CLight::linkLight(tLight  *currLight)
{
  currLight->next        = lightHead->next;
  lightHead->next        = currLight;
  currLight->prev        = lightHead;
  currLight->prev        = currLight->next->prev;
  currLight->next->prev  = currLight;
}

//---------------------------------------------------------------------------
// Unlinks the current light from the light list
//
void CLight::unlinkLight(tLight **currLight)
{
  tLight *temp = *currLight;

  (*currLight)->next->prev = (*currLight)->prev;
  (*currLight)->prev->next = (*currLight)->next;

  if((*currLight)->next == lightHead)
  {
    *currLight = (*currLight)->prev;
  }
  else
  {
    *currLight = (*currLight)->next;
  }

  SAFE_DELETE(temp);
}

//---------------------------------------------------------------------------
// Case-insensitive search
//
char *CLight::findString(char *string)
{
  memset(buffer, '\0', sizeof(char) * CONV_BUFFER_LEN);  // clear the buffer
                                                    // because it's static

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