#include "stdafx.h"

enum RETVAL
{
	RET_OK,									//	Normal
	RET_ERR_OUTOFMEMORY,					//	Out of memory
	RET_ERR_NULLREFERENCE,					//	Empty reference
	RET_ERR_ARGUMENTOUTOFRANGE,				//	The parameters are not within the normal range
	RET_ERR_PARAMISMATCH,					//	Parameter mismatch
	RET_ERR_DIVIDEBYZERO,
	RET_ERR_NOTSUPPORTED,
	RET_ERR_UNKNOWN
};

enum DEPTH
{
	DEPTH_8U,			//	unsigned char
	DEPTH_8S,			//	char
	DEPTH_16S,			//	short
	DEPTH_32S,			//  int
	DEPTH_32F,			//	float
	DEPTH_64F			//	double
};

enum EdgeMode				// Boundary processing methods for some domain algorithms
{
	Tile = 0,				// Repeating edge pixels
	Smear = 1				// Mirrored edge pixels
};

struct TMatrix
{
	int Width;					// The width of a matrix
	int Height;					// The height of a matrix
	int Channel;				// Number of matrix channels
	int Depth;					// The type of matrix element
	int WidthStep;				// The number of bytes occupied by a line element of a matrix
	int RowAligned;				// Whether to use four-byte alignment
	unsigned char *Data;		// Data of a matrix
};

struct Complex
{
	float Real;
	float Imag;
};
union Cv32suf
{
	int i;
	unsigned u;
	float f;
};

TMatrix *CreateMatrix(int Width, int Height, int Depth, int Channel, int RowAligned);	// Creating a data matrix
TMatrix *CloneMatrix(TMatrix *Src);														// Cloned data matrix
void FreeMatrix(TMatrix *Matrix);														// Release data matrix
void *AllocMemory(unsigned int size, bool ZeroMemory = false);							// Allocating memory, 32 byte alignment
void FreeMemory(void *Ptr);																// Release memory
int GetElementSize(int Depth);															//	Get element size
