/////////////////////////////////////////////////////////////////////////////
// Types.h
// ummm... typedefs. because it's not enough to be normal.
// but really these needed to be seperated from global.h because not 
// everything can include global.h b/c of evil syntax errors.
/////////////////////////////////////////////////////////////////////////////

#ifndef _TYPES_H
#define _TYPES_H

// T Y P E D E F / S T R U C T / C L A S S //////////////////////////////////

typedef float tVect;
typedef tVect vect2[2];
typedef tVect vect3[3];
typedef tVect vect4[4];

struct tVert2
{
  float x,y;
};

struct tVert3
{
  float x,y,z;
};

struct tTriangle
{
  int vertIndex[3];   // triangle vert index for this triangle
  int tcoordIndex[3]; // texcoord index for the face of this triangle
  vect3 norm;         // face normal of triangle
};

#ifdef _MSC_VER

typedef _int8  n8;  // byte, char   (-128 to 127)
typedef _int16 n16; // short, int   (-32,768 to 32,767)
typedef _int32 n32; // dword, long  (-2,147,483,648 to 2,147,483,647)
typedef _int64 n64; // REALLY LARGE (-9,223,372,036,854,775,808 to 9,223,372,036,854,775,807)

typedef unsigned _int8  u8;  // (0 to 255) + 1 = total
typedef unsigned _int16 u16; // (0 to 65,535) + 1 = total
typedef unsigned _int32 u32; // (0 to 4,294,967,295) + 1 = total
typedef unsigned _int64 u64; // (0 to 18,446,744,073,709,551,615) + 1 = total

typedef unsigned int   UINT;
typedef unsigned int   uint;
typedef unsigned char  BYTE;
typedef unsigned char  byte;
typedef unsigned short WORD;

#endif // _MSC_VER

/////////////////////////////////////////////////////////////////////////////

#endif // _TYPES_H

// E O F ////////////////////////////////////////////////////////////////////