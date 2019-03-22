/* Test and timing harness program for developing a multichannel
   multikernel convolution (as used in deep learning networks)

   Note there are some simplifications around this implementation,
   in particular with respect to computing the convolution at edge
   pixels of the image.

   Author: David Gregg
   Date:   February 2019

   Version 1.5 : Modified the code so that the input and kernel
                 are tensors of 16-bit integer values

   Version 1.4 : Modified the random generator to reduce the range
                 of generated values;

   Version 1.3 : Fixed which loop variables were being incremented
                 in write_out();
                 Fixed dimensions of output and control_output
                 matrices in main function

   Version 1.2 : Changed distribution of test data to (hopefully)
                 eliminate random walk of floating point error;
                 Also introduced checks to restrict kernel-order to
                 a small set of values

   Version 1.1 : Fixed bug in code to create 4d matrix
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <assert.h>
#include <omp.h>
#include <math.h>
#include <stdint.h>
#include <x86intrin.h>
#include <xmmintrin.h>
#include <string.h>
// #include "hashmap.h"
// # define NHASH 9997

// struct symbol
// {
//   char* name;
//   int value;
// };

// struct symbol symtab[NHASH];
// struct symbol * lookup(char *sym);
// int set(char *sym, int value);
// // map_t myMap;
// struct hashmap* myMap;

/* the following two definitions of DEBUGGING control whether or not
   debugging information is written out. To put the program into
   debugging mode, uncomment the following line: */
/*#define DEBUGGING(_x) _x */
/* to stop the printing of debugging information, use the following line: */
#define DEBUGGING(_x)


/* write 3d matrix to stdout */
void write_out(int16_t *** a, int dim0, int dim1, int dim2)
{
  int i, j, k;

  for ( i = 0; i < dim0; i++ ) {
    printf("Outer dimension number %d\n", i);
    for ( j = 0; j < dim1; j++ ) {
      for ( k = 0; k < dim2 - 1; k++ ) {
        printf("%d, ", a[i][j][k]);
      }
      // print end of line
      printf("%f\n", a[i][j][dim2-1]);
    }
  }
}


/* create new empty 4d float matrix */
float **** new_empty_4d_matrix_float(int dim0, int dim1, int dim2, int dim3)
{
  float **** result = malloc(dim0 * sizeof(float***));
  float *** mat1 = malloc(dim0 * dim1 * sizeof(float**));
  float ** mat2 = malloc(dim0 * dim1 * dim2 * sizeof(float*));
  float * mat3 = malloc(dim0 * dim1 * dim2 *dim3 * sizeof(float));
  int i, j, k;


  for ( i = 0; i < dim0; i++ ) {
    result[i] = &(mat1[i*dim1]);
    for ( j = 0; j < dim1; j++ ) {
      result[i][j] = &(mat2[i*dim1*dim2 + j*dim2]);
      for ( k = 0; k < dim2; k++ ) {
        result[i][j][k] = &(mat3[i*dim1*dim2*dim3+j*dim2*dim3+k*dim3]);
      }
    }
  }

  return result;
}

double *** new_empty_3d_matrix_double(int dim0, int dim1, int dim2)
{
  double *** result = malloc(dim0 * sizeof(double**));
  double ** mat1 = malloc(dim0 * dim1 * sizeof(double*));
  double * mat2 = malloc(dim0 * dim1 * dim2 * sizeof(double));
  int i, j;


  for ( i = 0; i < dim0; i++ ) {
    result[i] = &(mat1[i*dim1]);
    for ( j = 0; j < dim1; j++ ) {
      result[i][j] = &(mat2[i*dim1*dim2 + j*dim2]);
    }
  }

  return result;
}

/* create new empty 3d matrix */
float *** new_empty_3d_matrix_float(int dim0, int dim1, int dim2)
{
  float **** mat4d;
  float *** mat3d;

  // create a 4d matrix with single first dimension
  mat4d = new_empty_4d_matrix_float(1, dim0, dim1, dim2);
  // now throw away out first dimension
  mat3d = mat4d[0];
  free(mat4d);
  return mat3d;
}

/* create new empty 4d int16_t matrix */
int16_t **** new_empty_4d_matrix_int16(int dim0, int dim1, int dim2, int dim3)
{
  int16_t **** result = malloc(dim0 * sizeof(int16_t***));
  int16_t *** mat1 = malloc(dim0 * dim1 * sizeof(int16_t**));
  int16_t ** mat2 = malloc(dim0 * dim1 * dim2 * sizeof(int16_t*));
  int16_t * mat3 = malloc(dim0 * dim1 * dim2 *dim3 * sizeof(int16_t));
  int i, j, k;


  for ( i = 0; i < dim0; i++ ) {
    result[i] = &(mat1[i*dim1]);
    for ( j = 0; j < dim1; j++ ) {
      result[i][j] = &(mat2[i*dim1*dim2 + j*dim2]);
      for ( k = 0; k < dim2; k++ ) {
        result[i][j][k] = &(mat3[i*dim1*dim2*dim3+j*dim2*dim3+k*dim3]);
      }
    }
  }

  return result;
}

/* create new empty 3d matrix */
int16_t *** new_empty_3d_matrix_int16(int dim0, int dim1, int dim2)
{
  int16_t **** mat4d;
  int16_t *** mat3d;

  // create a 4d matrix with single first dimension
  mat4d = new_empty_4d_matrix_int16(1, dim0, dim1, dim2);
  // now throw away out first dimension
  mat3d = mat4d[0];
  free(mat4d);
  return mat3d;
}

/* take a copy of the matrix and return in a newly allocated matrix */
int16_t **** copy_4d_matrix(int16_t **** source_matrix, int dim0,
                            int dim1, int dim2, int dim3)
{
  int i, j, k, l;
  int16_t **** result = new_empty_4d_matrix_int16(dim0, dim1, dim2, dim3);

  for ( i = 0; i < dim0; i++ ) {
    for ( j = 0; j < dim1; j++ ) {
      for ( k = 0; k < dim2; k++ ) {
        for ( l = 0; l < dim3; l++ ) {
          result[i][j][k][l] = source_matrix[i][j][k][l];
        }
      }
    }
  }
  return result;
}

/* create a matrix and fill it with random numbers */
int16_t **** gen_random_4d_matrix_int16(int dim0, int dim1, int dim2, int dim3)
{
int16_t **** result;
int i, j, k, l;
struct timeval seedtime;
  int seed;

  result = new_empty_4d_matrix_int16(dim0, dim1, dim2, dim3);

  /* use the microsecond part of the current time as a pseudorandom seed */
  gettimeofday(&seedtime, NULL);
  seed = seedtime.tv_usec;
  srandom(seed);

  /* fill the matrix with random numbers */
  const int range = 1 << 10; // 2^10
  //const int bias = 1 << 16; // 2^16
  int16_t offset = 0.0;
  for ( i = 0; i < dim0; i++ ) {
    for ( j = 0; j < dim1; j++ ) {
      for ( k = 0; k < dim2; k++ ) {
        for ( l = 0; l < dim3; l++ ) {
          // generate uniform random integer with mean of zero
          long long rand = random();
          // now cut down the range and bias the mean to reduce
          // the likelihood of large floating point round-off errors
          int reduced_range = (rand % range);
          result[i][j][k][l] = reduced_range;
        }
      }
    }
  }

  return result;
}

/* create a matrix and fill it with random numbers */
int16_t *** gen_random_3d_matrix_int16(int dim0, int dim1, int dim2)
{
  int16_t **** mat4d;
  int16_t *** mat3d;

  // create a 4d matrix with single first dimension
  mat4d = gen_random_4d_matrix_int16(1, dim0, dim1, dim2);
  // now throw away out first dimension
  mat3d = mat4d[0];
  free(mat4d);
  return mat3d;
}

/* check the sum of absolute differences is within reasonable epsilon */
void check_result(float *** result, float *** control,
                  int dim0, int dim1, int dim2)
{
  int i, j, k;
  double sum_abs_diff = 0.0;
  const double EPSILON = 0.0625;

  //printf("SAD\n");

  for ( i = 0; i < dim0; i++ ) {
    for ( j = 0; j < dim1; j++ ) {
      for ( k = 0; k < dim2; k++ ) {
        double diff = fabs(control[i][j][k] - result[i][j][k]);
        assert( diff >= 0.0 );
        sum_abs_diff = sum_abs_diff + diff;
      }
    }
  }

  if ( sum_abs_diff > EPSILON ) {
    fprintf(stderr, "WARNING: sum of absolute differences (%f) > EPSILON (%f)\n",
            sum_abs_diff, EPSILON);
  }
  else {
    printf("COMMENT: sum of absolute differences (%f)  within acceptable range (%f)\n", sum_abs_diff, EPSILON);
  }
}

/* the slow but correct version of matmul written by David */
void multichannel_conv(int16_t *** image, int16_t **** kernels,
		       float *** output, int width, int height,
		       int nchannels, int nkernels, int kernel_order)
{
  int h, w, x, y, c, m;

  for ( m = 0; m < nkernels; m++ ) {
    for ( w = 0; w < width; w++ ) {
      for ( h = 0; h < height; h++ ) {
        double sum = 0.0;
        for ( c = 0; c < nchannels; c++ ) {
          for ( x = 0; x < kernel_order; x++) {
            for ( y = 0; y < kernel_order; y++ ) {
              sum += (double) image[w+x][h+y][c] * (double) kernels[m][c][x][y];
            }
          }
          output[m][w][h] = (float) sum;
        }
      }
    }
  }
}


// void fillImageHashSet(int16_t *** image, int width, int height, int kernel_order, int nchannels)
// {
//   int wx, hy, c;
//   // #pragma omp parallel for collapse(2)
//   for ( wx = 0; wx < width + kernel_order; wx++ ) {
//       for ( hy = 0; hy < height + kernel_order; hy++ ) {
//         for ( c = 0; c < nchannels; c++ ) {
//           char *wxbuff = (char*) malloc(20);
//           char *hybuff = (char*) malloc(20);
//           char *cbuff = (char*) malloc(20);
//           char *indexbuff = (char*) malloc(61);
//           sprintf(wxbuff, "%d", wx);
//           sprintf(hybuff, "%d", hy);
//           sprintf(cbuff, "%d", c);
//           strcat(indexbuff, wxbuff);
//           strcat(indexbuff, hybuff);
//           strcat(indexbuff, cbuff);
//           // hashmapsett(myMap, indexbuff, image[wx][hy][c]);
//           free(wxbuff);
//           free(hybuff);
//           free(cbuff);
//           free(indexbuff);
//           // set(indexbuff, image[wx][hy][c]);
//         }
//       }
//   }
// }

/* the fast version of matmul written by the team */
void team_conv(int16_t *** restrict image, int16_t **** restrict kernels, float *** restrict output,
               int width, int height, int nchannels, int nkernels,
               int kernel_order)
{
  // myMap = hashmap_create();
  // fillImageHashSet(image, width, height, kernel_order, nchannels);
  int h, w, x, y, c, m;
  // double*** sumArr = new_empty_3d_matrix_double(nkernels, width, height);

  // #pragma omp parallel for collapse(3)
  // for (int i = 0; i < nkernels; i++)
  // {
  //   for (int j = 0; j < width; j++)
  //   {
  //     for (int k = 0; k < height; k++)
  //     {
  //         sumArr[i][j][k] = 0;
  //     }
  //   }
  // }


  #pragma omp parallel for collapse(3)
  for ( m = 0; m < nkernels; m++ ) {
    for ( w = 0; w < width; w++ ) {
      for ( h = 0; h < height; h++ ) {
        double sum[] = [4];
        double ans =0.0;
        __m128 a4 = _mm_loadu_ps(&sum[0]);
        for ( c = 0; c < nchannels; c++ ) {
          for ( x = 0; x < kernel_order; x++) {
            for ( y = 0; y < kernel_order; y++ ) {
              // char *wxbuff = (char*) malloc(20);
              // char *hybuff = (char*) malloc(20);
              // char *cbuff = (char*) malloc(20);
              // char *indexbuff = (char*) malloc(61);
              // sprintf(wxbuff, "%d", w+x);
              // sprintf(hybuff, "%d", h+y);
              // sprintf(cbuff, "%d", c);
              // strcat(indexbuff, wxbuff);
              // strcat(indexbuff, hybuff);
              // strcat(indexbuff, cbuff);
              //Access optimally in 2 threads, and wait til we get back a result
              //sum += (double) image[w+x][h+y][c] * (double) kernels[m][c][x][y];
              
              __m128 b4 = _mm_loadu_ps(&image[w+x][h+y][c]);
              __m128 c4 = _mm_loadu_ps(&kernels[m][c][x][y]);

              __m128 d4 = _m128 _mm_mul_ps(__m128 a, __m128 b);

              __m128 a4 = _mm_add_ps(a4, d4);

              // output[m][w][h] = (float)sum;
              // float dubb = output[m][w][h];
              // if (dubb != (float)sum)
              // {
              //   fprintf(stderr, "WHAT %f, %f\n", dubb, sum);
              // }
              // if (m%20 == 0)
              // {
              //   fprintf(stderr,"%d", m);
              // }
              // sumArr[m][w][h] = sumArr[m][w][h] + (double) image[w+x][h+y][c] * (double) kernels[m][c][x][y];
              // sum += lookup(indexbuff) -> value * (double) kernels[m][c][x][y];
              // free(wxbuff);
              // free(hybuff);
              // free(cbuff);
              // free(indexbuff);
            }
          }
          //Hashmap for this and insert at the end using vectorisiation
          //output[m][w][h] = (float) sum;
          (float)_mm_store_ps(sum, __m128 a4);
          output[m][w][h] = sum[0] + sum [1] + sum[2] + sum[3];
        }
      }
    }
  }

  // #pragma omp parallel for collapse(3)
  // for (int i = 0; i < nkernels; i++)
  // {
  //   for (int j = 0; j < width; j++)
  //   {
  //     for (int k = 0; k < height; k++)
  //     {
  //         output[i][j][k] = (float) sumArr[i][j][k];
  //     }
  //   }
  // }
}

int mainCall(int argc, char ** argv)
{
  //float image[W][H][C];
  //float kernels[M][C][K][K];
  //float output[M][W][H];

  int16_t *** image, **** kernels;
  float *** control_output, *** output;
  long long mul_time;
  long long mul_time_dav;
  int width, height, kernel_order, nchannels, nkernels;
  struct timeval start_time;
  struct timeval stop_time;
  struct timeval start_time_dav;
  struct timeval stop_time_dav;

  if ( argc != 6 ) {
    fprintf(stderr, "Usage: conv-harness <image_width> <image_height> <kernel_order> <number of channels> <number of kernels>\n");
    exit(1);
  }
  else {
    width = atoi(argv[1]);
    height = atoi(argv[2]);
    kernel_order = atoi(argv[3]);
    nchannels = atoi(argv[4]);
    nkernels = atoi(argv[5]);
  }
  switch ( kernel_order ) {
  case 1:
  case 3:
  case 5:
  case 7: break;
  default:
    fprintf(stderr, "FATAL: kernel_order must be 1, 3, 5 or 7, not %d\n",
            kernel_order);
    exit(1);
  }

  /* allocate the matrices */
  image = gen_random_3d_matrix_int16(width+kernel_order, height + kernel_order,
                               nchannels);
  kernels = gen_random_4d_matrix_int16(nkernels, nchannels, kernel_order, kernel_order);
  output = new_empty_3d_matrix_float(nkernels, width, height);
  control_output = new_empty_3d_matrix_float(nkernels, width, height);

  //DEBUGGING(write_out(A, a_dim1, a_dim2));

  gettimeofday(&start_time_dav, NULL);
  /* use a simple multichannel convolution routine to produce control result */
  multichannel_conv(image, kernels, control_output, width,
                    height, nchannels, nkernels, kernel_order);
  gettimeofday(&stop_time_dav, NULL);
  mul_time_dav = (stop_time_dav.tv_sec - start_time_dav.tv_sec) * 1000000L +
    (stop_time_dav.tv_usec - start_time_dav.tv_usec);

  /* record starting time of team's code*/
  gettimeofday(&start_time, NULL);

  /* perform student team's multichannel convolution */
  team_conv(image, kernels, output, width,
                    height, nchannels, nkernels, kernel_order);

  /* record finishing time */
  gettimeofday(&stop_time, NULL);
  mul_time = (stop_time.tv_sec - start_time.tv_sec) * 1000000L +
    (stop_time.tv_usec - start_time.tv_usec);
  printf("Team conv time: %lld microseconds, a %lld speedup! %lld\n", mul_time, mul_time_dav / mul_time, mul_time_dav);

  DEBUGGING(write_out(output, nkernels, width, height));

  /* now check that the team's multichannel convolution routine
     gives the same answer as the known working version */
  check_result(output, control_output, nkernels, width, height);

  return 0;
}

// static unsigned symhash(char *sym)
// {
//   unsigned int hash = 0;
//   unsigned c;

//   while(c = *sym++) hash = hash*9 ^ c;

//   return hash;
// }

// struct symbol* lookup(char *sym)
// {
//   struct symbol *sp = &symtab[symhash(sym)%NHASH];
//   int scount = NHASH;

//   while(--scount >= 0)
//   {
//     if (sp -> name && strcmp(sp -> name, sym) == 0)
//     {
//       return sp;
//     }

//     if(!sp->name)
//     {
//       return 0;
//     }

//     if(++sp >= symtab+NHASH)
//     {
//       sp = symtab;
//     }
//   }
//   return 0;
// }

// int set(char *sym, int value)
// {
//   struct symbol *sp = &symtab[symhash(sym)%NHASH];
//   int scount = NHASH;

//   while(--scount >= 0)
//   {
//     if (!sp -> name)
//     {
//       sp -> name = malloc(sizeof(sym));
//       strcpy(sp -> name, sym);
//       sp -> value = value;
//       return 1;
//     }
//     else if (strcmp(sp -> name, sym) == 0)
//     {
//       sp -> value = value;
//       return 1;
//     }

//     if(++sp >= symtab+NHASH)
//     {
//       sp = symtab;
//     }
//   }
  // yyerror("table overflow");
  // abort();
// }