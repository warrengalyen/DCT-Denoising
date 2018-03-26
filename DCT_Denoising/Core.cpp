#include "stdafx.h"
#include "Utility.h"

/// <summary>
/// Obtain the number of bytes actually occupied by an element based on the type of the matrix element.
/// </summary>
/// <param name="Depth">According to the type of the matrix element.</param>
/// <returns>Only used internally.</returns>
int GetElementSize(int Depth)
{
	int Size;
	switch (Depth)
	{
	case DEPTH_8U:
		Size = sizeof(unsigned char);
		break;
	case DEPTH_8S:
		Size = sizeof(char);
		break;
	case DEPTH_16S:
		Size = sizeof(short);
		break;
	case DEPTH_32S:
		Size = sizeof(int);
		break;
	case DEPTH_32F:
		Size = sizeof(float);
		break;
	case DEPTH_64F:
		Size = sizeof(double);
		break;
	default:
		Size = 0;
		break;
	}
	return Size;
}

/// <summary>
/// Allocate memory function, 32-bit aligned
/// </summary>
/// <param name="Size">The memory size to use (in bytes).</param>
/// <param name="ZeroMemory">Whether the memory data is cleared.</param>
/// <returns>Returns a pointer to the allocated memory and returns NULL on failure.</returns>
/// <remarks>https://technet.microsoft.com/en-us/library/8z34s9c6</remarks>
void *AllocMemory(unsigned int Size, bool ZeroMemory)    
{
	// Considering the requirements of advanced functions such as SSE, AVX, the allocation start address
	// uses 32-byte alignment. This function is actually _mm_malloc.
	void *Ptr = _aligned_malloc(Size, 32);
	if (Ptr != NULL && ZeroMemory == true)
		memset(Ptr, 0, Size);
	return Ptr;
}

/// <summary>
/// Free memory
/// </summary>
/// <param name="Ptr">Memory data address.</param>
void FreeMemory(void *Ptr)
{
	if (Ptr != NULL) _aligned_free(Ptr);	//	This function is _mm_free
}

/// <summary>
/// Create new matrix data.
/// </summary>
/// <param name="Width">The width of the matrix.</param>
/// <param name="Height">The height of a matrix.</param>
/// <param name="Depth">The color depth of a matrix.</param>
/// <param name="Channel">The number of channels of a matrix.</param>
/// <param name="RowAligned">Whether each row of data needs four-byte alignment.</param>
/// <returns>Returns the created matrix data and returns NULL on failure.</returns>
TMatrix *CreateMatrix(int Width, int Height, int Depth, int Channel, int RowAligned)
{
	if (Width < 1 || Height < 1) return NULL;
	if (Depth < DEPTH_8U && Depth > DEPTH_64F) return NULL;
	if (Channel != 1 && Channel != 3 && Channel != 4) return NULL;

	TMatrix * Matrix = (TMatrix *)AllocMemory(sizeof(TMatrix), true);		//	Do not count on allocation to succeed
	Matrix->Width = Width;
	Matrix->Height = Height;
	Matrix->Depth = Depth;
	Matrix->Channel = Channel;
	Matrix->RowAligned = RowAligned;
	if (RowAligned)
		Matrix->WidthStep = WidthBytes(Width * Channel * GetElementSize(Depth));
	else
		Matrix->WidthStep = Width * Channel * GetElementSize(Depth);
	Matrix->Data = (unsigned char *)AllocMemory(Matrix->Height * Matrix->WidthStep, true);
	if (Matrix->Data == NULL)
	{
		FreeMemory(Matrix);
		return NULL;
	}
	return Matrix;
}

/// <summary>
/// Release a created matrix.
/// </summary>
/// <param name="Matrix">A matrix object that needs to be released.</param>
void FreeMatrix(TMatrix *Matrix)
{
	if (Matrix == NULL) return;
	if (Matrix->Data == NULL)
		FreeMemory(Matrix);
	else
	{
		FreeMemory(Matrix->Data);		// Release in proper order
		FreeMemory(Matrix);
	}
}

/// <summary>
/// Clone existing matrix.
/// </summary>
/// <param name="Src">Source matrix object</param>
/// <returns>The cloned matrix object.</returns>
TMatrix *CloneMatrix(TMatrix *Src)
{
	if (Src == NULL || Src->Data == NULL) return NULL;
	TMatrix *Dest = CreateMatrix(Src->Width, Src->Height, Src->Depth, Src->Channel, Src->RowAligned);
	if (Dest != NULL)
	{
		memcpy(Dest->Data, Src->Data, Dest->Height * Dest->WidthStep);
		return Dest;
	}
	return NULL;
}