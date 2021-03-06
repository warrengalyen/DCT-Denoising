#include "Utility.h"

#define PATCHSIZE 8

static __declspec(align(16)) float Temp1[PATCHSIZE * PATCHSIZE];		//	Defined as a global variable to reduce dregs caused by multiple memory allocations and releases
static __declspec(align(16)) float Temp2[PATCHSIZE * PATCHSIZE];

// 8*8 1D DCT Coefficient, 16 Byte Alignment mainly to take fully advantage of SSE
static const __declspec(align(16)) float DCTbasis[PATCHSIZE * PATCHSIZE] =
{
	0.3535534, 0.3535534, 0.3535534, 0.3535534, 0.3535534, 0.3535534, 0.3535534, 0.3535534,
	0.4903926, 0.4157348, 0.2777851, 0.0975452,-0.0975452,-0.2777851,-0.4157348,-0.4903926,
	0.4619398, 0.1913417,-0.1913417,-0.4619398,-0.4619398,-0.1913417, 0.1913417, 0.4619398,
	0.4157348,-0.0975452,-0.4903926,-0.2777851, 0.2777851, 0.4903926, 0.0975452,-0.4157348,
	0.3535534,-0.3535534,-0.3535534, 0.3535534, 0.3535534,-0.3535534,-0.3535534, 0.3535534,
	0.2777851,-0.4903926, 0.0975452, 0.4157348,-0.4157348,-0.0975452, 0.4903926,-0.2777851,
	0.1913417,-0.4619398, 0.4619398,-0.1913417,-0.1913417, 0.4619398,-0.4619398, 0.1913417,
	0.0975452,-0.2777851, 0.4157348,-0.4903926, 0.4903926,-0.4157348, 0.2777851,-0.0975452
};

// 8*8 1-D IDCT transform coefficients
static const __declspec(align(16)) float IDCTbasis[PATCHSIZE * PATCHSIZE] =
{
	0.3535534, 0.4903926, 0.4619398, 0.4157348, 0.3535534, 0.2777851, 0.1913417, 0.0975452,
	0.3535534, 0.4157348, 0.1913417,-0.0975452,-0.3535534,-0.4903926,-0.4619398,-0.2777851,
	0.3535534, 0.2777851,-0.1913417,-0.4903926,-0.3535534, 0.0975452, 0.4619398, 0.4157348,
	0.3535534, 0.0975452,-0.4619398,-0.2777851, 0.3535534, 0.4157348,-0.1913417,-0.4903926,
	0.3535534,-0.0975452,-0.4619398, 0.2777851, 0.3535534,-0.4157348,-0.1913417, 0.4903926,
	0.3535534,-0.2777851,-0.1913417, 0.4903926,-0.3535534,-0.0975452, 0.4619398,-0.4157348,
	0.3535534,-0.4157348, 0.1913417, 0.0975452,-0.3535534, 0.4903926,-0.4619398, 0.2777851,
	0.3535534,-0.4903926, 0.4619398,-0.4157348, 0.3535534,-0.2777851, 0.1913417,-0.0975452
};

//	http://stackoverflow.com/questions/5200338/a-cache-efficient-matrix-transpose-program
//	http://stackoverflow.com/questions/16737298/what-is-the-fastest-way-to-transpose-a-matrix-In-c
//	https://msdn.microsoft.com/en-us/library/4d3eabky(v=vs.90).aspx
//	Efficient matrix transpose algorithm, speed is approximately 4 times that of direct writing
inline void TransposeSSE4x4(float *Src, float *Dest)
{
	__m128 Row0 = _mm_load_ps(Src);
	__m128 Row1 = _mm_load_ps(Src + 8);
	__m128 Row2 = _mm_load_ps(Src + 16);
	__m128 Row3 = _mm_load_ps(Src + 24);

	// _MM_TRANSPOSE4_PS(Row0, Row1, Row2, Row3);							 //	Or use this SSE macro

	__m128 Temp0 = _mm_shuffle_ps(Row0, Row1, _MM_SHUFFLE(1, 0, 1, 0));      //	Row0[0] Row0[1] Row1[0] Row1[1]    
	__m128 Temp2 = _mm_shuffle_ps(Row0, Row1, _MM_SHUFFLE(3, 2, 3, 2));      //	Row0[2] Row0[3] Row1[2] Row1[3]        
	__m128 Temp1 = _mm_shuffle_ps(Row2, Row3, _MM_SHUFFLE(1, 0, 1, 0));      //	Row2[0] Row2[1] Row3[0] Row3[1]        
	__m128 Temp3 = _mm_shuffle_ps(Row2, Row3, _MM_SHUFFLE(3, 2, 3, 2));		 //	Row2[2] Row2[3] Row3[2] Row3[3]          

	Row0 = _mm_shuffle_ps(Temp0, Temp1, _MM_SHUFFLE(2, 0, 2, 0));				//	Row0[0] Row1[0] Row2[0] Row3[0]             
	Row1 = _mm_shuffle_ps(Temp0, Temp1, _MM_SHUFFLE(3, 1, 3, 1));				//	Row0[1] Row1[1] Row2[1] Row3[1]                     
	Row2 = _mm_shuffle_ps(Temp2, Temp3, _MM_SHUFFLE(2, 0, 2, 0));				//	Row0[2] Row1[2] Row2[2] Row3[2]                
	Row3 = _mm_shuffle_ps(Temp2, Temp3, _MM_SHUFFLE(3, 1, 3, 1));				//	Row0[3] Row1[3] Row2[3] Row3[3]             

	_mm_store_ps(Dest, Row0);
	_mm_store_ps(Dest + 8, Row1);
	_mm_store_ps(Dest + 16, Row2);
	_mm_store_ps(Dest + 24, Row3);
}

unsigned char ClampToByte(int Value)
{
	return ((Value | ((signed int)(255 - Value) >> 31)) & ~((signed int)Value >> 31));
}

/// <summary>
/// One-dimensional DCT transform of 8*8 and its inverse transform.
/// </summary>
/// <param name="In">The input data.</param>
/// <param name="Out">The output data.</param>
/// <param name="Invert">Whether to perform inverse transformation.</param>
/// <remarks> 1. Input and output cannot be the same, i.e. in-place operation is not supported.</remarks>
/// <remarks> 2. The algorithm only translates directly on the IPOL, using SSE acceleration.</remarks>
/// <remarks> 3. In the 8*8DCT transform in JPEG compression and other programs, the algorithm multiplication is less optimized, but it is not easy to use SSE optimization. I use the code test there to be slower than the following.</remarks>
void DCT8X81D(float *In, float *Out, bool Invert)
{
	__m128 Sum, A, B;
	const float *Data = (const float *)&Sum;
	A = _mm_load_ps(In);
	B = _mm_load_ps(In + 4);

	if (Invert == FALSE)
	{
		/*for (int Y = 0; Y < PATCHSIZE; Y ++)
		{
			Out[Y] = 0;
			for (int X = 0; X < PATCHSIZE; X ++)
			{
				Out[Y] += In[X] * DCTbasis[Y * PATCHSIZE + X];
			}
		}*/

		Sum = _mm_mul_ps(A, _mm_load_ps(DCTbasis));
		Sum = _mm_add_ps(Sum, _mm_mul_ps(B, _mm_load_ps(DCTbasis + 4)));
		Out[0] = Data[0] + Data[1] + Data[2] + Data[3];							//	Is there a better way to achieve this??

		Sum = _mm_mul_ps(A, _mm_load_ps(DCTbasis + 8));
		Sum = _mm_add_ps(Sum, _mm_mul_ps(B, _mm_load_ps(DCTbasis + 12)));		//	Do not use a Sum variable and use multiple instructions to increase the level of parallelism
		Out[1] = Data[0] + Data[1] + Data[2] + Data[3];

		Sum = _mm_mul_ps(A, _mm_load_ps(DCTbasis + 16));
		Sum = _mm_add_ps(Sum, _mm_mul_ps(B, _mm_load_ps(DCTbasis + 20)));
		Out[2] = Data[0] + Data[1] + Data[2] + Data[3];

		Sum = _mm_mul_ps(A, _mm_load_ps(DCTbasis + 24));
		Sum = _mm_add_ps(Sum, _mm_mul_ps(B, _mm_load_ps(DCTbasis + 28)));
		Out[3] = Data[0] + Data[1] + Data[2] + Data[3];

		Sum = _mm_mul_ps(A, _mm_load_ps(DCTbasis + 32));
		Sum = _mm_add_ps(Sum, _mm_mul_ps(B, _mm_load_ps(DCTbasis + 36)));
		Out[4] = Data[0] + Data[1] + Data[2] + Data[3];

		Sum = _mm_mul_ps(A, _mm_load_ps(DCTbasis + 40));
		Sum = _mm_add_ps(Sum, _mm_mul_ps(B, _mm_load_ps(DCTbasis + 44)));
		Out[5] = Data[0] + Data[1] + Data[2] + Data[3];

		Sum = _mm_mul_ps(A, _mm_load_ps(DCTbasis + 48));
		Sum = _mm_add_ps(Sum, _mm_mul_ps(B, _mm_load_ps(DCTbasis + 52)));
		Out[6] = Data[0] + Data[1] + Data[2] + Data[3];

		Sum = _mm_mul_ps(A, _mm_load_ps(DCTbasis + 56));
		Sum = _mm_add_ps(Sum, _mm_mul_ps(B, _mm_load_ps(DCTbasis + 60)));
		Out[7] = Data[0] + Data[1] + Data[2] + Data[3];
	}
	else
	{
		/*for (int Y = 0; Y < PATCHSIZE; Y ++)
		{
			Out[Y] = 0;
			for (int X = 0; X < PATCHSIZE; X ++)
			{
				Out[Y] += In[X] * IDCTbasis[Y * PATCHSIZE + X];
			}
		}*/

		Sum = _mm_mul_ps(A, _mm_load_ps(IDCTbasis));
		Sum = _mm_add_ps(Sum, _mm_mul_ps(B, _mm_load_ps(IDCTbasis + 4)));
		Out[0] = Data[0] + Data[1] + Data[2] + Data[3];

		Sum = _mm_mul_ps(A, _mm_load_ps(IDCTbasis + 8));
		Sum = _mm_add_ps(Sum, _mm_mul_ps(B, _mm_load_ps(IDCTbasis + 12)));
		Out[1] = Data[0] + Data[1] + Data[2] + Data[3];

		Sum = _mm_mul_ps(A, _mm_load_ps(IDCTbasis + 16));
		Sum = _mm_add_ps(Sum, _mm_mul_ps(B, _mm_load_ps(IDCTbasis + 20)));
		Out[2] = Data[0] + Data[1] + Data[2] + Data[3];

		Sum = _mm_mul_ps(A, _mm_load_ps(IDCTbasis + 24));
		Sum = _mm_add_ps(Sum, _mm_mul_ps(B, _mm_load_ps(IDCTbasis + 28)));
		Out[3] = Data[0] + Data[1] + Data[2] + Data[3];

		Sum = _mm_mul_ps(A, _mm_load_ps(IDCTbasis + 32));
		Sum = _mm_add_ps(Sum, _mm_mul_ps(B, _mm_load_ps(IDCTbasis + 36)));
		Out[4] = Data[0] + Data[1] + Data[2] + Data[3];

		Sum = _mm_mul_ps(A, _mm_load_ps(IDCTbasis + 40));
		Sum = _mm_add_ps(Sum, _mm_mul_ps(B, _mm_load_ps(IDCTbasis + 44)));
		Out[5] = Data[0] + Data[1] + Data[2] + Data[3];

		Sum = _mm_mul_ps(A, _mm_load_ps(IDCTbasis + 48));
		Sum = _mm_add_ps(Sum, _mm_mul_ps(B, _mm_load_ps(IDCTbasis + 52)));
		Out[6] = Data[0] + Data[1] + Data[2] + Data[3];

		Sum = _mm_mul_ps(A, _mm_load_ps(IDCTbasis + 56));
		Sum = _mm_add_ps(Sum, _mm_mul_ps(B, _mm_load_ps(IDCTbasis + 60)));
		Out[7] = Data[0] + Data[1] + Data[2] + Data[3];
	}
}

/// <summary>
/// 8*8 two-dimensional DCT transform and its inverse transform.
/// </summary>
/// <param name="In">The input data.</param>
/// <param name="Out">The output data.</param>
/// <param name="Invert">Whether to perform inverse transformation.</param>
/// <remarks> 1. Input and output cannot be the same, i.e. in-place operation is not supported.</remarks>
/// <remarks> 2. The algorithm only translates directly on the IPOL, using SSE acceleration.</remarks>
void DCT8X82D(float *In, float *Out, bool Invert)
{
	DCT8X81D(In, Temp1, Invert);
	DCT8X81D(In + 8, Temp1 + 8, Invert);
	DCT8X81D(In + 16, Temp1 + 16, Invert);			//	DCT transform by row
	DCT8X81D(In + 24, Temp1 + 24, Invert);
	DCT8X81D(In + 32, Temp1 + 32, Invert);
	DCT8X81D(In + 40, Temp1 + 40, Invert);
	DCT8X81D(In + 48, Temp1 + 48, Invert);
	DCT8X81D(In + 56, Temp1 + 56, Invert);

	TransposeSSE4x4(Temp1, Temp2);
	TransposeSSE4x4(Temp1 + 4, Temp2 + 32);			//	Transpose results
	TransposeSSE4x4(Temp1 + 32, Temp2 + 4);
	TransposeSSE4x4(Temp1 + 36, Temp2 + 36);

	DCT8X81D(Temp2, Temp1, Invert);
	DCT8X81D(Temp2 + 8, Temp1 + 8, Invert);
	DCT8X81D(Temp2 + 16, Temp1 + 16, Invert);		//	Rows are again DCT transformed
	DCT8X81D(Temp2 + 24, Temp1 + 24, Invert);
	DCT8X81D(Temp2 + 32, Temp1 + 32, Invert);
	DCT8X81D(Temp2 + 40, Temp1 + 40, Invert);
	DCT8X81D(Temp2 + 48, Temp1 + 48, Invert);
	DCT8X81D(Temp2 + 56, Temp1 + 56, Invert);

	TransposeSSE4x4(Temp1, Out);
	TransposeSSE4x4(Temp1 + 4, Out + 32);			//	Transpose results
	TransposeSSE4x4(Temp1 + 32, Out + 4);
	TransposeSSE4x4(Temp1 + 36, Out + 36);
}

/// <summary>
/// There has been a two-dimensional DCT transformation of 8*8 for DCT results.
/// </summary>
/// <param name="In">The input data.</param>
/// <param name="Out">The output data.</param>
/// <remarks> 1. Input and output cannot be the same, i.e. in-place operation is not supported.</remarks>
/// <remarks> 2. The algorithm only translates directly on the IPOL, using SSE acceleration.</remarks>
void DCT8X82DR(float *In, float *Out)
{
	TransposeSSE4x4(In, Temp2);
	TransposeSSE4x4(In + 4, Temp2 + 32);			//	Transpose results
	TransposeSSE4x4(In + 32, Temp2 + 4);
	TransposeSSE4x4(In + 36, Temp2 + 36);

	DCT8X81D(Temp2, Temp1, false);
	DCT8X81D(Temp2 + 8, Temp1 + 8, false);
	DCT8X81D(Temp2 + 16, Temp1 + 16, false);		//	Rows are again DCT transformed
	DCT8X81D(Temp2 + 24, Temp1 + 24, false);
	DCT8X81D(Temp2 + 32, Temp1 + 32, false);
	DCT8X81D(Temp2 + 40, Temp1 + 40, false);
	DCT8X81D(Temp2 + 48, Temp1 + 48, false);
	DCT8X81D(Temp2 + 56, Temp1 + 56, false);

	TransposeSSE4x4(Temp1, Out);
	TransposeSSE4x4(Temp1 + 4, Out + 32);			//	Transpose results
	TransposeSSE4x4(Temp1 + 32, Out + 4);
	TransposeSSE4x4(Temp1 + 36, Out + 36);
}

/// <summary>
/// 8*8 two-dimensional DCT transform and its inverse transform.
/// </summary>

/// <summary>
/// Extend existing matrix data by specified data.
/// </summary>
/// <param name="Src">Original image data.</param>
/// <param name="Dest">Processed image data.</param>
/// <param name="sigma">Standard deviation of noise.</param>
/// <remarks> 1. Reference: http://www.ipol.im/pub/art/2011/ys-dct/。 </remarks>
/// <remarks> 2. The algorithm translates only directly on IPOL, using SSE acceleration</remarks>
/// <remarks> 3. Supports in_place operations.</remarks>
void __stdcall DCT_Denoising(TMatrix *Src, TMatrix *Dest, float Sigma)
{
	if (Src->Channel == 3)
	{
		TMatrix *Blue = NULL, *Green = NULL, *Red = NULL, *Alpha = NULL;

		SplitRGBA(Src, Blue, Green, Red, Alpha);

		DCT_Denoising(Blue, Blue, Sigma);
		DCT_Denoising(Green, Green, Sigma);
		DCT_Denoising(Red, Red, Sigma);

		CombineRGBA(Dest, Blue, Green, Red, Alpha);

		FreeMatrix(Blue);
		FreeMatrix(Green);
		FreeMatrix(Red);
		FreeMatrix(Alpha);
		return;
	}
	else
	{
		float Threshold = 3 * Sigma;
		int X, Y, XX, YY, Index;
		int Width = Src->Width, Height = Src->Height;

		float *DctOut = (float *)AllocMemory(64 * sizeof(float), false);
		float *Sum = (float *)AllocMemory(Width * Height * sizeof(float), true);
		float *Weight = (float *)AllocMemory(Width * Height * sizeof(float), true);
		float *Kernel = (float *)AllocMemory(8 * Height * sizeof(float), false);
		float *DctRow = (float *)AllocMemory(8 * Height * sizeof(float), false);

		Index = 0;
		for (Y = 0; Y < Height; Y++)
		{
			unsigned char *LinePS = Src->Data + Y * Src->WidthStep;
			for (X = 0; X < 8; X++)												// Did not consider the processing of an image with a width less than 8
			{
				Kernel[Index] = LinePS[X];
				Index++;
			}
		}

		for (X = 0; X < Width - 8; X++)
		{
			if (X != 0)
			{
				memcpy(Kernel, Kernel + 1, (8 * Height - 1) * sizeof(float));	// Move data forward
				unsigned char *LinePS = Src->Data + (X + 7);
				float *CurKer = Kernel + 7;
				for (Y = 0; Y < Height; Y++)
				{
					CurKer[0] = LinePS[0];										// Just refresh the next element
					CurKer += 8;
					LinePS += Src->WidthStep;
				}
			}

			for (Y = 0; Y < Height; Y++)
				DCT8X81D(Kernel + 8 * Y, DctRow + 8 * Y, false);				// One-dimensional DCT transform on the line

			for (Y = 0; Y < Height - 8; Y++)
			{
				DCT8X82DR(DctRow + 8 * Y, DctOut);								// Take advantage of duplicated information

				for (YY = 0; YY < 64; YY++)
					if (fabs(DctOut[YY]) < Threshold) DctOut[YY] = 0;			// Threshold processing

				DCT8X82D(DctOut, DctOut, true);									// Change back

																				//Index = 0;
																				//for (YY = 0; YY < 8; YY++)		
																				//{
																				//	for (XX = 0; XX< 8; XX++)
																				//	{
																				//		Sum[(Y + YY) * Width + (X + XX)] += Patch[Index++];
																				//		Weight[(Y + YY) * Width + (X + XX)]++;
																				//	}
																				//}

				for (YY = 0; YY < 8; YY++)										// Update weights and cumulative values
				{
					float *LinePD = Sum + (Y + YY) * Width + X;
					float *LinePW = Weight + (Y + YY) * Width + X;
					float *LinePS = DctOut + YY * 8;
					__m128 A, B, Ones = _mm_set1_ps(1.0);

					A = _mm_add_ps(_mm_loadu_ps(LinePD), _mm_load_ps(LinePS));
					B = _mm_add_ps(_mm_loadu_ps(LinePD + 4), _mm_load_ps(LinePS + 4));		// Update accumulated value
					_mm_storeu_ps(LinePD, A);
					_mm_storeu_ps(LinePD + 4, B);

					A = _mm_add_ps(Ones, _mm_loadu_ps(LinePW));
					B = _mm_add_ps(Ones, _mm_loadu_ps(LinePW + 4));							// Update weight
					_mm_storeu_ps(LinePW, A);
					_mm_storeu_ps(LinePW + 4, B);
				}
			}
		}

		Index = 0;
		for (Y = 0; Y < Height; Y++)
		{
			unsigned char *LinePD = Dest->Data + Y * Dest->WidthStep;
			for (X = 0; X < Width; X++)
			{
				LinePD[X] = ClampToByte(Sum[Index] / Weight[Index]);
				Index++;
			}
		}
		FreeMemory(Sum);
		FreeMemory(DctOut);
		FreeMemory(DctRow);
		FreeMemory(Weight);
		FreeMemory(Kernel);
	}
}