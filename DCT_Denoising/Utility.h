#include "stdafx.h"
#include <memory.h>
#include <stdlib.h>
#include <pmmintrin.h>
#include "Core.h"
#include <math.h>
#include "omp.h"

#define WidthBytes(bytes) (((bytes * 8) + 31) / 32 * 4)

void SplitRGBA(TMatrix *Src, TMatrix *&Blue, TMatrix *&Green, TMatrix *&Red, TMatrix *&Alpha);
void CombineRGBA(TMatrix *Dest, TMatrix *Blue, TMatrix *Green, TMatrix *Red, TMatrix *Alpha);

