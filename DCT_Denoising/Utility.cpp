#include "Utility.h"

/// <summary>
/// Decomposes a color image into a single channel image of R, G, B and A
/// </summary>
/// <param name="Src">The data structure of the source image that needs to be processed.</param>
/// <param name="Blue">The data matrix of the image's blue channel.</param>
/// <param name="Green">The data matrix of the image's green channel.</param>
/// <param name="Red">The data matrix of the image's red channel.</param>
/// <param name="Alpha">The data matrix of the image's alpha channel.</param>
/// <remarks>With 8 bit parallel processing, the speed can be improved by about 20%.</remarks>
void SplitRGBA(TMatrix *Src, TMatrix *&Blue, TMatrix *&Green, TMatrix *&Red, TMatrix *&Alpha)
{

	Blue = CreateMatrix(Src->Width, Src->Height, Src->Depth, 1, true);
	Green = CreateMatrix(Src->Width, Src->Height, Src->Depth, 1, true);
	Red = CreateMatrix(Src->Width, Src->Height, Src->Depth, 1, true);
	if (Src->Channel == 4)	Alpha = CreateMatrix(Src->Width, Src->Height, Src->Depth, 1, true);

	int X, Y, Block, Width = Src->Width, Height = Src->Height;
	unsigned char *LinePS, *LinePB, *LinePG, *LinePR, *LinePA;
	const int BlockSize = 8;
	Block = Width / BlockSize;

	if (Src->Channel == 3)
	{
		for (Y = 0; Y < Height; Y++)
		{
			LinePS = Src->Data + Y * Src->WidthStep;
			LinePB = Blue->Data + Y * Blue->WidthStep;
			LinePG = Green->Data + Y * Green->WidthStep;
			LinePR = Red->Data + Y * Red->WidthStep;
			for (X = 0; X < Block * BlockSize; X += BlockSize)
			{
				LinePB[0] = LinePS[0];		LinePG[0] = LinePS[1];		LinePR[0] = LinePS[2];
				LinePB[1] = LinePS[3];		LinePG[1] = LinePS[4];		LinePR[1] = LinePS[5];
				LinePB[2] = LinePS[6];		LinePG[2] = LinePS[7];		LinePR[2] = LinePS[8];
				LinePB[3] = LinePS[9];		LinePG[3] = LinePS[10];		LinePR[3] = LinePS[11];
				LinePB[4] = LinePS[12];		LinePG[4] = LinePS[13];		LinePR[4] = LinePS[14];
				LinePB[5] = LinePS[15];		LinePG[5] = LinePS[16];		LinePR[5] = LinePS[17];
				LinePB[6] = LinePS[18];		LinePG[6] = LinePS[19];		LinePR[6] = LinePS[20];
				LinePB[7] = LinePS[21];		LinePG[7] = LinePS[22];		LinePR[7] = LinePS[23];
				LinePB += 8;				LinePG += 8;				LinePR += 8;				LinePS += 24;
			}
			for (; X < Width; X++)
			{
				LinePB[0] = LinePS[0];		LinePG[0] = LinePS[1];		LinePR[0] = LinePS[2];
				LinePB++;					LinePG++;					LinePR++;					LinePS += 3;
			}
		}
	}
	else if (Src->Channel == 4)
	{
		for (Y = 0; Y < Height; Y++)
		{
			LinePS = Src->Data + Y * Src->WidthStep;
			LinePB = Blue->Data + Y * Blue->WidthStep;
			LinePG = Green->Data + Y * Green->WidthStep;
			LinePR = Red->Data + Y * Red->WidthStep;
			LinePA = Alpha->Data + Y * Alpha->WidthStep;
			for (X = 0; X < Block * BlockSize; X += BlockSize)
			{
				LinePB[0] = LinePS[0];		LinePG[0] = LinePS[1];		LinePR[0] = LinePS[2];		LinePA[0] = LinePS[3];
				LinePB[1] = LinePS[4];		LinePG[1] = LinePS[5];		LinePR[1] = LinePS[6];		LinePA[1] = LinePS[7];
				LinePB[2] = LinePS[8];		LinePG[2] = LinePS[9];		LinePR[2] = LinePS[10];		LinePA[2] = LinePS[11];
				LinePB[3] = LinePS[12];		LinePG[3] = LinePS[13];		LinePR[3] = LinePS[14];		LinePA[3] = LinePS[15];
				LinePB[4] = LinePS[16];		LinePG[4] = LinePS[17];		LinePR[4] = LinePS[18];		LinePA[4] = LinePS[19];
				LinePB[5] = LinePS[20];		LinePG[5] = LinePS[21];		LinePR[5] = LinePS[22];		LinePA[5] = LinePS[23];
				LinePB[6] = LinePS[24];		LinePG[6] = LinePS[25];		LinePR[6] = LinePS[26];		LinePA[6] = LinePS[27];
				LinePB[7] = LinePS[28];		LinePG[7] = LinePS[29];		LinePR[7] = LinePS[30];		LinePA[7] = LinePS[31];
				LinePB += 8;				LinePG += 8;				LinePR += 8;				LinePA += 8;				LinePS += 32;
			}
			for (; X < Width; X++)
			{
				LinePB[0] = LinePS[0];		LinePG[0] = LinePS[1];		LinePR[0] = LinePS[2];		LinePA[0] = LinePS[3];
				LinePB++;					LinePG++;					LinePR++;					LinePA++;					LinePS += 4;
			}
		}
	}
}

/// <summary>
/// A single channel image of R, G, B and A is merged into a color image.
/// </summary>
/// <param name="Dest">The data matrix of the merged image.</param>
/// <param name="Blue">The data matrix of the image's blue channel.</param>
/// <param name="Green">The data matrix of the image's green channel.</param>
/// <param name="Red">The data matrix of the image's red channel.</param>
/// <param name="Alpha">The data matrix of the image's alpha channel.</param>
/// <remarks>With 8 bit parallel processing, the speed can be improved by about 20%.</remarks>
void CombineRGBA(TMatrix *Dest, TMatrix *Blue, TMatrix *Green, TMatrix *Red, TMatrix *Alpha)
{
	int X, Y, Block, Width = Dest->Width, Height = Dest->Height;
	unsigned char *LinePD, *LinePB, *LinePG, *LinePR, *LinePA;
	const int BlockSize = 8;
	Block = Width / BlockSize;

	if (Dest->Channel == 3)
	{
		for (Y = 0; Y < Height; Y++)
		{
			LinePD = Dest->Data + Y * Dest->WidthStep;
			LinePB = Blue->Data + Y * Blue->WidthStep;
			LinePG = Green->Data + Y * Green->WidthStep;
			LinePR = Red->Data + Y * Red->WidthStep;
			for (X = 0; X < Block * BlockSize; X += BlockSize)
			{
				LinePD[0] = LinePB[0];		LinePD[1] = LinePG[0];		LinePD[2] = LinePR[0];
				LinePD[3] = LinePB[1];		LinePD[4] = LinePG[1];		LinePD[5] = LinePR[1];
				LinePD[6] = LinePB[2];		LinePD[7] = LinePG[2];		LinePD[8] = LinePR[2];
				LinePD[9] = LinePB[3];		LinePD[10] = LinePG[3];		LinePD[11] = LinePR[3];
				LinePD[12] = LinePB[4];		LinePD[13] = LinePG[4];		LinePD[14] = LinePR[4];
				LinePD[15] = LinePB[5];		LinePD[16] = LinePG[5];		LinePD[17] = LinePR[5];
				LinePD[18] = LinePB[6];		LinePD[19] = LinePG[6];		LinePD[20] = LinePR[6];
				LinePD[21] = LinePB[7];		LinePD[22] = LinePG[7];		LinePD[23] = LinePR[7];
				LinePB += 8;				LinePG += 8;				LinePR += 8;				LinePD += 24;
			}
			for (; X < Width; X++)
			{
				LinePD[0] = LinePB[0];		LinePD[1] = LinePG[0];		LinePD[2] = LinePR[0];
				LinePB++;					LinePG++;					LinePR++;					LinePD += 3;
			}
		}
	}
	else if (Dest->Channel == 4)
	{
		for (Y = 0; Y < Height; Y++)
		{
			LinePD = Dest->Data + Y * Dest->WidthStep;
			LinePB = Blue->Data + Y * Blue->WidthStep;
			LinePG = Green->Data + Y * Green->WidthStep;
			LinePR = Red->Data + Y * Red->WidthStep;
			LinePA = Alpha->Data + Y * Alpha->WidthStep;
			for (X = 0; X < Block * BlockSize; X += BlockSize)
			{
				LinePD[0] = LinePB[0];		LinePD[1] = LinePG[0];		LinePD[2] = LinePR[0];		LinePD[3] = LinePA[0];
				LinePD[4] = LinePB[1];		LinePD[5] = LinePG[1];		LinePD[6] = LinePR[1];		LinePD[7] = LinePA[1];
				LinePD[8] = LinePB[2];		LinePD[9] = LinePG[2];		LinePD[10] = LinePR[2];		LinePD[11] = LinePA[2];
				LinePD[12] = LinePB[3];		LinePD[13] = LinePG[3];		LinePD[14] = LinePR[3];		LinePD[15] = LinePA[3];
				LinePD[16] = LinePB[4];		LinePD[17] = LinePG[4];		LinePD[18] = LinePR[4];		LinePD[19] = LinePA[4];
				LinePD[20] = LinePB[5];		LinePD[21] = LinePG[5];		LinePD[22] = LinePR[5];		LinePD[23] = LinePA[5];
				LinePD[24] = LinePB[6];		LinePD[25] = LinePG[6];		LinePD[26] = LinePR[6];		LinePD[27] = LinePA[6];
				LinePD[28] = LinePB[7];		LinePD[29] = LinePG[7];		LinePD[30] = LinePR[7];		LinePD[31] = LinePA[7];
				LinePB += 8;				LinePG += 8;				LinePR += 8;				LinePA += 8;				LinePD += 32;
			}
			for (; X < Width; X++)
			{
				LinePD[0] = LinePB[0];		LinePD[1] = LinePG[0];		LinePD[2] = LinePR[0];		LinePD[3] = LinePA[0];
				LinePB++;					LinePG++;					LinePD++;					LinePA++;					LinePD += 4;
				X++;
			}
		}
	}
}