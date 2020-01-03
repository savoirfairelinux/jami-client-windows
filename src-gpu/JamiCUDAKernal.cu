// System includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <cstdint>

// CUDA runtime
#include <cuda_runtime.h>

// helper functions and utilities to work with CUDA
#include <helper_cuda.h>
#include <helper_functions.h>
#include "device_launch_parameters.h"


__global__ void
seperateUVChannelNV12_Kernal(uint8_t* UVbuf, uint8_t* Ubuf, uint8_t* Vbuf, int linesize, int widthOfOutPut, int height)
{
    // the thread index should be the same as the output array's index ,which is twice of the input's index
    const unsigned int tidX = blockDim.x * blockIdx.x + threadIdx.x;
    const unsigned int tidY = blockDim.y * blockIdx.y + threadIdx.y;

    if(tidX < widthOfOutPut && tidY < height) {
        unsigned int indexUInUVArray = linesize * tidY + (2 * tidX - 1);
        unsigned int indexVInUVArray = linesize * tidY + (2 * tidX);

        unsigned int indexInOutputArray = widthOfOutPut * tidY + tidX;
        Ubuf[indexInOutputArray] = UVbuf[indexUInUVArray];
        Vbuf[indexInOutputArray] = UVbuf[indexVInUVArray];
    }
}

extern "C" bool
seperateUVChannelNV12()
{
    bool result = false;
}