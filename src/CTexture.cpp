// CTexture.cpp (Spark2)
// by Alex D'Angelo
// Created on May 08, 2000
// Last modified on Aug 15, 2000
//
// Overview: All the functions for reading, processing, and linking a
//           texture to the texture list.
//
// Note: Now reads ambient, diffuse, specular, and transparency values for
//       each texture. The shine and shinestrength might be useful, but for
//       now I'm not reading them.
//
// Implement the materials using glMaterial()...
// 
//---------------------------------------------------------------------------

#include "ConvCommon.h"

//---------------------------------------------------------------------------
CTexture::CTexture()
{
  initialized = 0;
  init();
}

//---------------------------------------------------------------------------
void CTexture::init()
{
  if(!initialized)
  {
    numTextures = 0;

    texHead = new tTexture;

    memset(texHead, 0, sizeof(tTexture));

    texHead->prev        = texHead;
    texHead->next        = texHead;
    texHead->texFilename = NULL;

    initialized = 1;
  }
}

//---------------------------------------------------------------------------
CTexture::~CTexture()
{
  unload();
}

//---------------------------------------------------------------------------
void CTexture::unload()
{
  if(texHead != NULL) // if it hasn't been released yet
  {
    tTexture *currTex = texHead->next;

    while(currTex != texHead)
    {
      unlinkTex(&currTex);
      //currTex = currTex->next; // wait... doesn't this happen automatically? (AD May 25, 2000)
    }
  }

  initialized = 0;
}

//---------------------------------------------------------------------------
// creates a new tex, fills it out, adds to a linked list
//
void CTexture::add()
{
  tTexture *newTex = new tTexture;
  
  memset(newTex, 0, sizeof(tTexture));  // init the texture
  newTex->next          = NULL;
  newTex->prev          = NULL;
  newTex->texFilename   = NULL;
  newTex->uOffset       = 0;
  newTex->vOffset       = 0;
  newTex->face          = GL_FRONT;
  newTex->uTile         = 1;
  newTex->vTile         = 1;
  newTex->isTiled       = 0;
  newTex->shine         = 0.5f;
  newTex->shineStrength = 0.5f;
  newTex->isDecal       = 1; // for now
  
  readTex(newTex);
  linkTex(newTex);
  
  numTextures++;
}

//---------------------------------------------------------------------------
void CTexture::readTex(tTexture *newTex)
{
  bool done = 0; 
  
  // The texture reading was triggered by finding "*MATERIAL*, so
  // the next value in the stream is the material id number. Therefore:
  *aseStream >> newTex->texId;  

  while(!aseStream->eof() && !done)
  {
    *aseStream >> buffer;
    
    if(!strcmp(buffer, "*MATERIAL_AMBIENT"))
            *aseStream >> newTex->ambient[0]
                      >> newTex->ambient[1]
                      >> newTex->ambient[2];

    else if(!strcmp(buffer, "*MATERIAL_DIFFUSE"))
            *aseStream >> newTex->diffuse[0]
                      >> newTex->diffuse[1]
                      >> newTex->diffuse[2];

    else if(!strcmp(buffer, "*MATERIAL_SPECULAR"))
            *aseStream >> newTex->specular[0]
                      >> newTex->specular[1]
                      >> newTex->specular[2];

    else if(!strcmp(buffer, "*MATERIAL_SHINE"))
            *aseStream >> newTex->shine;

    else if(!strcmp(buffer, "*MATERIAL_SHINESTRENGTH"))
            *aseStream >> newTex->shineStrength;

    else if(!strcmp(buffer, "*MATERIAL_TRANSPARENCY"))
            *aseStream >> newTex->transparency;

    else if(!strcmp(buffer, "*BITMAP"))                 // Texture filename
            getTexFilename(newTex);

    else if(!strcmp(buffer, "*UVW_U_TILING"))
            *aseStream >> newTex->uTile;
    
    else if(!strcmp(buffer,"*UVW_V_TILING"))
            *aseStream >> newTex->vTile;
    
    else if(!strcmp(buffer, "*UVW_U_OFFSET"))
            *aseStream >> newTex->uOffset;
    
    else if(!strcmp(buffer,"*UVW_V_OFFSET"))
            *aseStream >> newTex->vOffset;

    else if(!strcmp(buffer,"*BITMAP_FILTER"))
            done = 1;

    else aseStream->getline(buffer, sizeof(buffer));     // finish reading line
  }
  
  // this is questionable for determining tiling...
  if( ( fabs(newTex->uTile - 1.0f) < 0.01 )  && ( fabs(newTex->vTile - 1.0f) < 0.01 ) )
  {
    newTex->isTiled = 0;
  }
  else
  {
    newTex->isTiled = 1;
  }

  // cout << endl; 
  /*
  // cout << "Texture name  = " << newTex->texFilename << endl
       << "Texture id    = " << newTex->texId   << endl
       << "u tiling      = " << newTex->uTile   << endl
       << "v tiling      = " << newTex->vTile   << endl
       << "u offset      = " << newTex->uOffset << endl
       << "v offset      = " << newTex->vOffset << endl
       << "is tiled      = " << newTex->isTiled << endl
       << endl;
  */
}

//---------------------------------------------------------------------------
// Reads a texture filename. Extracts the actual filename from the path
//
void CTexture::getTexFilename(tTexture *currTex)
{
  char buffer[CONV_BUFFER_LEN];

  memset(buffer, '\0', sizeof(char) * CONV_BUFFER_LEN);
  //*aseStream >> buffer;
  aseStream->getline(buffer, sizeof(buffer)); 
  
  strrlc(buffer,'"'); // remove leading quote;
  strrlc(buffer,' '); // remove any spaces before the x:\
  
  buffer[ strlen(buffer) - 1 ] = NULL;// get rid of trailing quote

  _strrev(buffer);                    // reverse the string
  
  char *pdest = strchr(buffer, '\\'); // search for the first slash

  if(pdest != NULL)
  {
    int pos = pdest - buffer;         // pointer math; get offset
    buffer[pos] = NULL;               // terminate it!
  }  
  
  _strrev(buffer);                    // flip the filename around again
                                      //  to get the correct name
  
  currTex->texFilename = new char [ strlen(buffer) + 1 ];
  strcpy(currTex->texFilename, buffer);
}

//---------------------------------------------------------------------------
// This links the current texture to the texture linked-list
//
void CTexture::linkTex(tTexture *newTex)
{
  newTex->next       = texHead->next;
  texHead->next      = newTex;
  newTex->prev       = texHead;
  newTex->prev       = newTex->next->prev;
  newTex->next->prev = newTex;
}

//---------------------------------------------------------------------------
// This unlinks the current texture to the texture linked-list
//
void CTexture::unlinkTex(tTexture **currTex)
{
  tTexture *temp = *currTex;

  SAFE_DELARY( (*currTex)->texFilename );         // free filename

  (*currTex)->next->prev = (*currTex)->prev;  // free link
  (*currTex)->prev->next = (*currTex)->next;

  if((*currTex)->next == texHead)
  {
    *currTex = (*currTex)->prev;
  }
  else
  {
    *currTex = (*currTex)->next;
  }

  delete temp;
}

//---------------------------------------------------------------------------
// Func: CTexture::tTexture *CTexture::find(unsigned int texId)
//
// Desc: Searches the texture list in the class for a certain texture id
//
// Return: A tTexture pointer to the current texture
//
//         Example:
//          CTexture::tTexture *found_texture = NULL;
//          unsigned int textureId = 4;
//
//          found_texture = (CTexture *)CTexture::find(textureId);
//
//         If the texture id cannot be found, NULL is returned
//
//---------------------------------------------------------------------------
//
CTexture::tTexture *CTexture::find(unsigned int texId)
{
  bool found = 0;
  tTexture *foundTex = NULL;
  tTexture *search   = texHead;

  while(search->next != texHead && (found == 0))
  {
    if( search->next->texId == texId )
    {
      foundTex = search->next;
      found = 1;
    }
    search = search->next;
  }

  return foundTex;
}

//---------------------------------------------------------------------------
// Func: void CTexture::loadAll()
//
// Desc: Loads all the textures
//---------------------------------------------------------------------------
//
void CTexture::loadAll()
{
  tTexture *currTex = texHead->next;

  while(currTex != texHead)
  {
    loadTex(currTex);
    currTex = currTex->next; 
  }
}

//---------------------------------------------------------------------------
// Func: GLuint CTexture::loadTex(unsigned int texId)
//
// Desc: Loads a .PNG image file depending on a given texId
//
// Params: unsigned int texId: 
//           texture id extracted from the ASE file
// 
// Returns 0 if the texture was not found or couldn't be loaded
//         non-zero OpenGL texture id if the texture was loaded sucessfully
//                  or if the texture was already loaded before
//
// Notes: Calls CTexture::loadTex(tTexture *currTex) internally
//
//---------------------------------------------------------------------------
//
GLuint CTexture::loadTexWithId(unsigned int texId)
{
  tTexture *currTex = (tTexture *)find(texId); // does the texture exist?

  if(currTex != NULL)         // if the texture exists
  {
    if(currTex->isLoaded)
    {
      return currTex->glTexId;// it's already been loaded
    }
    else                      // else we need to load the texture
    {
      currTex->glTexId = loadTex(currTex);
      
      return currTex->glTexId;
    }
  }
  else
  {
    // print an error message saying the file couldn't be found
    // load the default texture
    return 0;
  }
}

//---------------------------------------------------------------------------
// Func: GLuint CTexture::loadTex(tTexture *currTex)
//
// Desc: Loads a .PNG image file based on info in the tTexture struct
//
// Params: tTexture *currTex
//           currTex contains the filename (plus other info(?)) needed to
//           load a texture
// 
// Returns 0 if the texture was not found or couldn't be loaded
//         non-zero OpenGL texture id if the texture was loaded sucessfully
//                  or if the texture was already loaded before
//
//---------------------------------------------------------------------------
//
GLuint CTexture::loadTex(tTexture *currTex)
{
  if(currTex->texFilename == NULL)
  {
    error.msg("CLIENT: Texture name doesn't exist");
    return 0;
  }
      
  //-----------------------------
  // create image path + filename
  //-----------------------------
  int  pathLength = strlen(currTex->texFilename) + strlen("images\\") + 1;
  char *imagePath = new char [ pathLength ];
  memset(imagePath, NULL,      pathLength);
  strcpy(imagePath, "images\\");
  strcat(imagePath, currTex->texFilename);

  //-----------------------------
  // Make sure filename is valid
  //-----------------------------
  bool success = validateTexFilename(imagePath);
  if(success == 0)
  {
    error.msg("CLIENT: Texture filename (%s) not valid or does not exist", imagePath); 
    SAFE_DELARY(imagePath);
    return 0;
  }
  
  //-----------------------------
  // load png
  //-----------------------------
  
  //currTex->glTexId = pngBind(imagePath, PNG_NOMIPMAP, PNG_SOLID, NULL, GL_CLAMP, GL_NEAREST, GL_NEAREST); // are these the best params to use?
/* This isn't a good way to turn on and off tiling, since a tex may be tiled in one spot and not in another
  if( currTex->isTiled )
  {
    currTex->glTexId = pngBind(imagePath, PNG_NOMIPMAP, PNG_SOLID, NULL,
                               GL_REPEAT, GL_NEAREST, GL_NEAREST); // are these the best params to use?
  }
  else
  {
    currTex->glTexId = pngBind(imagePath, PNG_NOMIPMAP, PNG_SOLID, NULL, 
                               GL_CLAMP, GL_NEAREST, GL_NEAREST); // are these the best params to use?
  }
*/

  // Tile by default
  currTex->glTexId = pngBind(imagePath, PNG_NOMIPMAP, PNG_SOLID, NULL,
                             GL_REPEAT, GL_NEAREST, GL_NEAREST); // are these the best params to use?
  
  currTex->isLoaded = 1;

  if(currTex->isLoaded)
    error.msg("CLIENT: Successfully loaded texture: %s", imagePath);  
  else
    error.msg("CLIENT: Couldn't load texture: %s", imagePath);

  // cleanup
  SAFE_DELARY(imagePath);

  return currTex->glTexId;
}

//---------------------------------------------------------------------------
// Func: bool CTexture::validateTexFilename(char *texFilename)
// Desc: Tests a filename to make sure it exists
// Rtns: 0 if file does not exist or could not be opened
//       1 if file does exist
//---------------------------------------------------------------------------
bool CTexture::validateTexFilename(char *texFilename)
{
  ifstream texStream;
  texStream.open(texFilename, ios::in | ios::_Nocreate);

  if(!texStream.is_open()) 
  {
    // the filename and path is invalid
    texStream.close();

    // write error message to error log
    error.msg( "WARNING: Couldn't open %s", texFilename );
    return 0; // unsuccessful
  }
  else
  {
    return 1; // successful
  }
}

//---------------------------------------------------------------------------
// Func: void CTexture::useMaterialProperties()
// Desc: loads up the current texture's properties for lighting
//---------------------------------------------------------------------------
// Think about how to better implement this! (June 15, 2000)
void CTexture::useMaterialProperties(tTexture *currTex)
{
  glMaterialfv(currTex->face, GL_AMBIENT,   (GLfloat *)currTex->ambient);
  glMaterialfv(currTex->face, GL_DIFFUSE,   (GLfloat *)currTex->diffuse);
  glMaterialfv(currTex->face, GL_SPECULAR,  (GLfloat *)currTex->specular);
  glMaterialf (currTex->face, GL_SHININESS, (GLfloat  )currTex->shine);
  
  //glMaterialfv(currTex->face, GL_EMISSION, ... ); // not used
}

//-- E O F ------------------------------------------------------------------
