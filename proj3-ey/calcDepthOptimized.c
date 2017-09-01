// CS 61C Fall 2015 Project 4

// include SSE intrinsics
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <x86intrin.h>
#endif

// include OpenMP
#if !defined(_MSC_VER)
#include <pthread.h>
#endif
#include <omp.h>

#include "calcDepthOptimized.h"
#include "calcDepthNaive.h"

/* DO NOT CHANGE ANYTHING ABOVE THIS LINE. */

void calcDepthOptimized(float *depth, float *left, float *right, int imageWidth, int imageHeight, int featureWidth, int featureHeight, int maximumDisplacement)
{
	#pragma omp parallel for collapse(2)
	/* The two outer for loops iterate through each pixel */
	for (int y = 0; y < imageHeight; y++)
	{
		for (int x = 0; x < imageWidth; x++)
		{	
			/* Set the depth to 0 if looking at edge of the image where a feature box cannot fit. */
			if ((y < featureHeight) || (y >= imageHeight - featureHeight) || (x < featureWidth) || (x >= imageWidth - featureWidth))
			{
				depth[y * imageWidth + x] = 0;
				continue;
			}
			float minimumSquaredDifference = -1;
			int minimumDy = 0;
			int minimumDx = 0;
			/* Iterate through all feature boxes that fit inside the maximum displacement box. 
			   centered around the current pixel. */
			for (int dy = -maximumDisplacement; dy <= maximumDisplacement; dy++)
			{
				for (int dx = -maximumDisplacement; dx <= maximumDisplacement; dx++)
				{
					/* Skip feature boxes that dont fit in the displacement box. */
					if (y + dy - featureHeight < 0 || y + dy + featureHeight >= imageHeight || x + dx - featureWidth < 0 || x + dx + featureWidth >= imageWidth)
					{
						continue;
					}

					float squaredDifference = 0;
					/* Initiate an array sum of 4 to store step sum.*/
					float array_sum[4];
					/* Initate a zero value of temp to sum up globally.*/
					__m128 temp = _mm_setzero_ps();
					/* Sum the squared difference within a box of +/- featureHeight and +/- featureWidth. */
					/* Reconstructing the Conditions.*/
					if (featureWidth <= 2) {
						/* Exactly the same with the navie method.*/
						for (int boxX = -featureWidth; boxX <= featureWidth; boxX++) {
							int leftX = x + boxX;
							int rightX = x + dx + boxX;
							for (int boxY = -featureHeight; boxY <= featureHeight; boxY++)
							{
								int leftY = y + boxY;
								int rightY = y + dy + boxY;
								float difference = left[leftY * imageWidth + leftX] - right[rightY * imageWidth + rightX];
								squaredDifference += difference * difference;
							}
						}
					} else if (featureWidth <= 4) {
						for (int boxX = 0; boxX < featureWidth/ 2 * 4; boxX += 4)
						{
							int leftX = x  + boxX - featureWidth;
							int rightX = x + dx + boxX - featureWidth;
							for (int boxY = -featureHeight; boxY <= featureHeight; boxY++)
							{
								int leftY = y + boxY;
								int rightY = y + dy + boxY;
								__m128 m = _mm_loadu_ps(left + leftY * imageWidth + leftX);
								__m128 n = _mm_loadu_ps(right + rightY * imageWidth + rightX);
								__m128 Difference = _mm_sub_ps(m, n);
								__m128 Square = _mm_mul_ps(Difference, Difference);
								temp = _mm_add_ps(Square, temp);
							}
						}
						for (int boxX = featureWidth/ 2 * 4; boxX <= 2 * featureWidth; boxX++) {
							int leftX = x + boxX - featureWidth;
							int rightX = x + dx + boxX - featureWidth;
							for (int boxY = -featureHeight; boxY <= featureHeight; boxY++)
							{
								int leftY = y + boxY;
								int rightY = y + dy + boxY;
								float difference = left[leftY * imageWidth + leftX] - right[rightY * imageWidth + rightX];
								squaredDifference += difference * difference;
							}
						}
				} else if (featureWidth <= 8) {
					for (int boxX = 0; boxX < featureWidth/4 * 8; boxX += 8)
						{
							int leftX = x + boxX - featureWidth ;
							int rightX = x + dx + boxX - featureWidth;
							for (int boxY = -featureHeight; boxY <= featureHeight; boxY++)
							{
								int leftY = y + boxY;
								int rightY = y + dy + boxY;
								__m128 m = _mm_loadu_ps(left +leftY * imageWidth + leftX);
								__m128 n = _mm_loadu_ps(right + rightY * imageWidth + rightX);
								__m128 m1 = _mm_loadu_ps(left + leftY * imageWidth + leftX + 4);
								__m128 n1 = _mm_loadu_ps(right + rightY * imageWidth + rightX + 4);
								__m128 Difference = _mm_sub_ps(m, n);
								__m128 Difference1 = _mm_sub_ps(m1, n1);
								__m128 Square = _mm_mul_ps(Difference, Difference);
								__m128 Square1 = _mm_mul_ps(Difference1, Difference1);
								temp = _mm_add_ps(Square, temp);
								temp = _mm_add_ps(Square1, temp);
							}
						}
						for (int boxX = featureWidth/4*8; boxX <= 2*featureWidth; boxX++) {
							int leftX = x + boxX - featureWidth;
							int rightX = x + dx + boxX - featureWidth;
							for (int boxY = -featureHeight; boxY <= featureHeight; boxY++)
							{
								int leftY = y + boxY;
								int rightY = y + dy + boxY;
								float difference = left[leftY * imageWidth + leftX] - right[rightY * imageWidth + rightX];
								squaredDifference += difference * difference;
							}
						}
					} else {
						for (int boxX = 0; boxX < featureWidth/8 * 16; boxX += 16)
						{
							int leftX = x + boxX - featureWidth;
							int rightX = x + dx + boxX - featureWidth;
							for (int boxY = -featureHeight; boxY <= featureHeight; boxY++)
							{
								int leftY = y + boxY;
								int rightY = y + dy + boxY;
								__m128 m = _mm_loadu_ps(left + leftY * imageWidth + leftX);
								__m128 n = _mm_loadu_ps(right + rightY * imageWidth + rightX);
								__m128 m1 = _mm_loadu_ps(left + leftY * imageWidth + leftX + 4);
								__m128 n1 = _mm_loadu_ps(right + rightY * imageWidth + rightX + 4);
								__m128 m2 = _mm_loadu_ps(left + leftY * imageWidth + leftX + 8);
								__m128 n2 = _mm_loadu_ps(right + rightY * imageWidth + rightX + 8);
								__m128 m3 = _mm_loadu_ps(left + leftY * imageWidth + leftX + 12);
								__m128 n3 = _mm_loadu_ps(right + rightY * imageWidth + rightX + 12);
								/* Compute the distance using the normal method.*/
								__m128 Difference = _mm_sub_ps(m, n);
								__m128 Difference1 = _mm_sub_ps(m1, n1);
								__m128 Difference2 = _mm_sub_ps(m2, n2);
								__m128 Difference3 = _mm_sub_ps(m3, n3);
								__m128 Square = _mm_mul_ps(Difference, Difference);
								__m128 Square1 = _mm_mul_ps(Difference1, Difference1);
								__m128 Square2 = _mm_mul_ps(Difference2, Difference2);
								__m128 Square3 = _mm_mul_ps(Difference3, Difference3);
								/* Accumulate the sum in temp.*/
								temp = _mm_add_ps(Square, temp);
								temp = _mm_add_ps(Square1, temp);
								temp = _mm_add_ps(Square2, temp);
								temp = _mm_add_ps(Square3, temp);
							}
						}
						for (int boxX = featureWidth/ 8 * 16; boxX <= 2 * featureWidth; boxX++) {
							int leftX = x + boxX - featureWidth;
							int rightX = x + dx + boxX - featureWidth;
							for (int boxY = -featureHeight; boxY <= featureHeight; boxY++)
							{
								int leftY = y + boxY;
								int rightY = y + dy + boxY;
								float difference = left[leftY * imageWidth + leftX] - right[rightY * imageWidth + rightX];
								squaredDifference += difference * difference;
							}
						}
					}
					/* Store the sum.*/
					_mm_storeu_ps(array_sum, temp);
					float arraysum = array_sum[0] + array_sum[1] + array_sum[2] + array_sum[3];
					/* Globally sum up.*/
					squaredDifference += arraysum;

					/* If the depth is zero, we should update the squaredDifference, since it is zero.*/
					if (squaredDifference == 0) {
						minimumDx = dx;
						minimumDy = dy;
						minimumSquaredDifference = squaredDifference;
					}
					if (minimumSquaredDifference == 0) {
						break;
					}

					/* 
					Check if you need to update minimum square difference. 
					This is when either it has not been set yet, the current
					squared displacement is equal to the min and but the new
					displacement is less, or the current squared difference
					is less than the min square difference.
					*/
					if ((minimumSquaredDifference == -1) || ((minimumSquaredDifference == squaredDifference)
						&& (displacementNaive(dx, dy) < displacementNaive(minimumDx, minimumDy)))
						|| (minimumSquaredDifference > squaredDifference))
					{
						minimumSquaredDifference = squaredDifference;
						minimumDx = dx;
						minimumDy = dy;
					}
				}
			}

			/* 
			Set the value in the depth map. 
			If max displacement is equal to 0, the depth value is just 0.
			*/
			if (minimumSquaredDifference != -1)
			{
				if (maximumDisplacement == 0)
				{
					depth[y * imageWidth + x] = 0;
				}
				else
				{
					depth[y * imageWidth + x] = displacementNaive(minimumDx, minimumDy);
				}
			}
		}
	}
}
