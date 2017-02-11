/******************************************************************************
** Copyright (c) 2017, Intel Corporation                                     **
** All rights reserved.                                                      **
**                                                                           **
** Redistribution and use in source and binary forms, with or without        **
** modification, are permitted provided that the following conditions        **
** are met:                                                                  **
** 1. Redistributions of source code must retain the above copyright         **
**    notice, this list of conditions and the following disclaimer.          **
** 2. Redistributions in binary form must reproduce the above copyright      **
**    notice, this list of conditions and the following disclaimer in the    **
**    documentation and/or other materials provided with the distribution.   **
** 3. Neither the name of the copyright holder nor the names of its          **
**    contributors may be used to endorse or promote products derived        **
**    from this software without specific prior written permission.          **
**                                                                           **
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS       **
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT         **
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR     **
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT      **
** HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,    **
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED  **
** TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR    **
** PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF    **
** LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING      **
** NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS        **
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.              **
******************************************************************************/
/* Alexander Heinecke (Intel Corp.)
******************************************************************************/
#include <libxsmm.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#if defined(_OPENMP)
# include <omp.h>
#endif

#if defined(_WIN32) || defined(__CYGWIN__)
/* note: later on, this leads to (correct but) different than expected norm-values */
# define drand48() ((double)rand() / RAND_MAX)
# define srand48 srand
#endif

int main(int argc, char* argv[])
{
  void* fpointer;
  libxsmm_smatcopyfunction skernel;
  libxsmm_matcopy_descriptor desc;
  float *a, *b;
  int lda, ldb;
  int i, j;
  int error = 0;

  printf("This is a tester for JIT matcopy kernels!\n");
  desc.m = atoi(argv[1]);
  desc.n = atoi(argv[2]);
  desc.lda = atoi(argv[3]);
  desc.ldb = atoi(argv[4]);
  desc.datatype = LIBXSMM_DNN_DATATYPE_F32;
  desc.prefetch = 0;
  
  a = (float *) malloc(desc.m * desc.lda * sizeof(float));
  b = (float *) malloc(desc.m * desc.ldb * sizeof(float));
  
  
  for (i=0; i < desc.m; i++ ) {
    for (j=0; j < desc.n; j++) {
      a[j+desc.lda*i] = 1.0 * rand();
    }
  }
  
  /* test dispatch call */
  fpointer = libxsmm_xmatcopydispatch( &desc );
  printf("address of F32 kernel: %lld\n", (size_t)fpointer);
  skernel = (libxsmm_smatcopyfunction)fpointer;
  
  /* let's call */
  skernel(a, &lda, b, &ldb);
  
  for (i=0; i < desc.m; i++ ) {
    for (j=0; j < desc.n; j++) {
      if ( (a[j+desc.lda*i] - b[j+desc.ldb*i]) > 0.000000001 ) {
        printf("ERROR!!!\n");
        error = 1;
      }
    }
  }
  
  if (error == 0) {
    printf("CORRECT copy!!!!\n");
  }
  
  return 0;
}

