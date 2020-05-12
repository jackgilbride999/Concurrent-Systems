//
// CSU33014 Summer 2020 Additional Assignment
// Part A of a two-part assignment
//

// Please examine version each of the following routines with names
// starting partA. Where the routine can be vectorized, please
// complete the corresponding vectorized routine using SSE vector
// intrinsics. Where is cannot be vectorized...

// Note the restrict qualifier in C indicates that "only the pointer
// itself or a value directly derived from it (such as pointer + 1)
// will be used to access the object to which it points".

#include <immintrin.h>
#include <stdio.h>

#include "csu33014-annual-partA-code.h"

/****************  routine 0 *******************/

// Here is an example routine that should be vectorized
void partA_routine0(float *restrict a, float *restrict b,
                    float *restrict c)
{
  for (int i = 0; i < 1024; i++)
  {
    a[i] = b[i] * c[i];
  }
}

// here is a vectorized solution for the example above
void partA_vectorized0(float *restrict a, float *restrict b,
                       float *restrict c)
{
  __m128 a4, b4, c4;

  for (int i = 0; i < 1024; i = i + 4)
  {
    b4 = _mm_loadu_ps(&b[i]);
    c4 = _mm_loadu_ps(&c[i]);
    a4 = _mm_mul_ps(b4, c4);
    _mm_storeu_ps(&a[i], a4);
  }
}

/***************** routine 1 *********************/

// in the following, size can have any positive value
float partA_routine1(float *restrict a, float *restrict b,
                     int size)
{
  float sum = 0.0;

  for (int i = 0; i < size; i++)
  {
    sum = sum + a[i] * b[i];
  }
  return sum;
}

// insert vectorized code for routine1 here
float partA_vectorized1(float *restrict a, float *restrict b,
                        int size)
{
  __m128 a4, b4, sum4, a4b4;
  float temp[4];
  int size_remainder = size % 4; // the remaining of number of elements after our vectorized runs
  float sum = 0.0;
  int i;
  for (i = 0; i < size - size_remainder; i += 4)
  {
    // load sum into vector
    sum4 = _mm_load1_ps(&sum);
    // load four a values and four b values into vector
    a4 = _mm_loadu_ps(&a[i]);
    b4 = _mm_loadu_ps(&b[i]);
    // multiply a[i] by b[i]
    a4b4 = _mm_mul_ps(a4, b4);
    // extract the products
    _mm_storeu_ps(&temp[0], a4b4);
    // add products to sum in the same order that they were added in sequential loop, ensuring no difference in results
    sum = ((((sum + temp[3]) + temp[2]) + temp[1]) + temp[0]);
  }
  // add the reamining 0, 1, 2 or 3 products between size+size_remainder and size
  for (i = size - size_remainder; i < size; i++)
  {
    sum = sum + a[i] * b[i];
  }
  return sum;
}

/******************* routine 2 ***********************/

// in the following, size can have any positive value
void partA_routine2(float *restrict a, float *restrict b, int size)
{
  for (int i = 0; i < size; i++)
  {
    a[i] = 1 - (1.0 / (b[i] + 1.0));
  }
}

// in the following, size can have any positive value
void partA_vectorized2(float *restrict a, float *restrict b, int size)
{
  __m128 a4, b4, one4;
  float one = 1;
  one4 = _mm_load1_ps(&one);
  int size_remainder = size % 4; // the remaining of number of elements after our vectorized runs
  int i;
  for (i = 0; i < size - size_remainder; i += 4)
  {
    // load four elements from b, add 1, divide result by one
    b4 = _mm_loadu_ps(&b[i]);
    b4 = _mm_add_ps(b4, one4);
    b4 = _mm_div_ps(one4, b4);
    // subtract result from one
    a4 = _mm_sub_ps(one4, b4);
    // store back to a
    _mm_storeu_ps(&a[i], a4);
  }
  // for the reamining 0, 1, 2 or 3 products between size+size_remainder and size
  for (i = size - size_remainder; i < size; i++)
  {
    a[i] = 1 - (1.0 / (b[i] + 1.0));
  }
}

/******************** routine 3 ************************/

// in the following, size can have any positive value
void partA_routine3(float *restrict a, float *restrict b, int size)
{
  for (int i = 0; i < size; i++)
  {
    if (a[i] < 0.0)
    {
      a[i] = b[i];
    }
  }
}

// in the following, size can have any positive value
void partA_vectorized3(float *restrict a, float *restrict b, int size)
{

  __m128 a4, b4, zero4, less4, greater_equal4;
  float zero = 0;
  zero4 = _mm_load1_ps(&zero);
  int size_remainder = size % 4; // the remaining of number of elements after our vectorized runs
  int i;
  for (i = 0; i < size - size_remainder; i += 4)
  {
    a4 = _mm_loadu_ps(&a[i]);
    b4 = _mm_loadu_ps(&b[i]);
    // if a[i] < 0, we need to store b[i]
    less4 = _mm_cmplt_ps(a4, zero4);
    less4 =_mm_and_ps(b4, less4);
    // if a >= 0, we need to store a[i]
    greater_equal4 = _mm_cmpge_ps(a4, zero4);
    greater_equal4 = _mm_and_ps(a4, greater_equal4);
    // store the respective a[i]s and b[i]s into one vector
    b4 = _mm_add_ps(less4, greater_equal4);
    // write vector contents to memory
    _mm_storeu_ps(&a[i], b4);
  }
  // replace the following code with vectorized code
  for (i = size - size_remainder; i < size; i++)
  {
    if (a[i] < 0.0)
    {
      a[i] = b[i];
    }
  }
}

/********************* routine 4 ***********************/

// hint: one way to vectorize the following code might use
// vector shuffle operations
void partA_routine4(float *restrict a, float *restrict b,
                    float *restrict c)
{
  for (int i = 0; i < 2048; i = i + 2)
  {
    a[i] = b[i] * c[i] - b[i + 1] * c[i + 1];
    a[i + 1] = b[i] * c[i + 1] + b[i + 1] * c[i];
  }
}

void partA_vectorized4(float *restrict a, float *restrict b,
                       float *restrict c)
{
   __m128 b4, c4, bshuffle, cshuffle, shuffleproduct;
   float temp[4] = {0,0,0,0};
  // replace the following code with vectorized code
  for (int i = 0; i < 2048; i = i + 2)
  {
    //a[i] = b[i] * c[i] - b[i + 1] * c[i + 1];
    //a[i + 1] = b[i] * c[i + 1] + b[i + 1] * c[i];

    // b4 = {b[i+3], b[i+2], b[i+1], b[i]}
    // c4 = {c[i+3], c[i+2], c[i+1], c[i]}
    b4 = _mm_loadu_ps(&b[i]);
    c4 = _mm_loadu_ps(&c[i]);

    // bshuffle = {b[i+1], b[i], b[i+1], b[i]}
    // cshuffle = {c[i], c[i+1], c[i+1], c[i]}
    bshuffle = _mm_shuffle_ps(b4, b4, _MM_SHUFFLE(1, 0, 1, 0));
    cshuffle = _mm_shuffle_ps(c4, c4, _MM_SHUFFLE(0, 1, 1, 0));

    // shuffle product = {b[i+1]*c[i], b[i]*c[i+1], b[i+1]*c[i+1], b[i]*c[i]}
    shuffleproduct = _mm_mul_ps(bshuffle, cshuffle);

    // temp = {b[i]*c[i], b[i+1]*c[i+1], b[i]*c[i+1], b[i+1]*c[i]}
    _mm_storeu_ps(&temp[0], shuffleproduct);

    a[i] = temp[0] - temp[1];
    a[i+1] = temp[2] + temp[3];
  }
}

/********************* routine 5 ***********************/

// in the following, size can have any positive value
void partA_routine5(unsigned char *restrict a,
                    unsigned char *restrict b, int size)
{
  __m128 a4, b4;
  for (int i = 0; i < size; i++)
  {
    a[i] = b[i];
  }
}

void partA_vectorized5(unsigned char *restrict a,
                       unsigned char *restrict b, int size)
{
  // replace the following code with vectorized code
  for (int i = 0; i < size; i++)
  {
    a[i] = b[i];
  }
}

/********************* routine 6 ***********************/

void partA_routine6(float *restrict a, float *restrict b,
                    float *restrict c)
{
  a[0] = 0.0;
  for (int i = 1; i < 1023; i++)
  {
    float sum = 0.0;
    for (int j = 0; j < 3; j++)
    {
      sum = sum + b[i + j - 1] * c[j];
    }
    a[i] = sum;
  }
  a[1023] = 0.0;
}

void partA_vectorized6(float *restrict a, float *restrict b,
                       float *restrict c)
{
  // replace the following code with vectorized code
  a[0] = 0.0;
  for (int i = 1; i < 1023; i++)
  {
    float sum = 0.0;
    for (int j = 0; j < 3; j++)
    {
      sum = sum + b[i + j - 1] * c[j];
    }
    a[i] = sum;
  }
  a[1023] = 0.0;
}
