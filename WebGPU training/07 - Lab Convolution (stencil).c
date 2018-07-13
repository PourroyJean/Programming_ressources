//
// Created by Jean Pourroy on 13/07/2018.
//

#include <wb.h>

#define wbCheck(stmt)                                                     \
  do {                                                                    \
    cudaError_t err = stmt;                                               \
    if (err != cudaSuccess) {                                             \
      wbLog(ERROR, "Failed to run stmt ", #stmt);                         \
      wbLog(ERROR, "Got CUDA error ...  ", cudaGetErrorString(err));      \
      return -1;                                                          \
    }                                                                     \
  } while (0)

#define NB_CHANNELS 3
#define MASK_WIDTH 5
#define Mask_radius MASK_WIDTH / 2 // 2

#define O_TILE_WIDTH 12
#define BLOCK_WIDTH (O_TILE_WIDTH + (MASK_WIDTH - 1)) // 12 + (5-1) = 16
#define I_TILE_WIDTH BLOCK_WIDTH

#define DEBUG_MODE 1





//@@ INSERT CODE HERE
//#define ACCESS_RGB_MATRIX (MAT, ROW,COL,CHANNEL) (MAT[ROW])

__global__ void stencil(float *deviceInputImageData,
                        float *deviceOutputImageData,
                        int imageWidth,
                        int imageHeight,
                        int channels,
                        int maskRows,
                        int maskColumns,
                        const float *__restrict__ deviceMaskData) {


    //Index
    int tx = threadIdx.x;
    int ty = threadIdx.y;

    //Who am I: of which cell of the output result am I responsible ?
    int num_row_o = blockIdx.y * O_TILE_WIDTH + ty;
    int num_col_o = blockIdx.x * O_TILE_WIDTH + tx;
    int base_add_pixel_o = (num_row_o * imageWidth + num_col_o) * NB_CHANNELS;


    //What should I load: which cell should I load into the share INPUT array
    int num_row_i = num_row_o - Mask_radius;
    int num_col_i = num_col_o - Mask_radius;
    int base_add_pixel_i = (num_row_i * imageWidth + num_col_i) * NB_CHANNELS;

    //We use a shared array as input for the block. 16x16 pixels  * 3 channels
    __shared__ float ds_INPUT[BLOCK_WIDTH * BLOCK_WIDTH * NB_CHANNELS];
    int my_ds_input_base_add = (ty * BLOCK_WIDTH + tx) * NB_CHANNELS;


    //Parallel load: need to check 2 things:
    // - As the image could have a size which can not be divided by O_TILE_WIDTH we check the thread is on the image.
    // - If the current block is on the boundaries, we also want to load 0.
    if ((num_row_i >= 0) && (num_row_i < imageHeight) &&
        (num_col_i >= 0) && (num_col_i < imageWidth)) {

        //Each thread of the block, load a pixel in INPUT.
        ds_INPUT[my_ds_input_base_add] = deviceInputImageData[base_add_pixel_i]; //R
        ds_INPUT[my_ds_input_base_add + 1] = deviceInputImageData[base_add_pixel_i + 1]; //G
        ds_INPUT[my_ds_input_base_add + 2] = deviceInputImageData[base_add_pixel_i + 2]; //B
    } else {
        ds_INPUT[my_ds_input_base_add] = 0; //R
        ds_INPUT[my_ds_input_base_add + 1] = 0; //G
        ds_INPUT[my_ds_input_base_add + 2] = 0; //B
    }
    __syncthreads();


    //Computation phase: some threads won't be used
    if (ty < O_TILE_WIDTH && tx < O_TILE_WIDTH &&  //Are we in the OUTPUT_TILE: the first O_TILE_WIDTH threads. ?
        num_row_o < imageHeight && num_col_o < imageWidth  //Am I on the image or outside ?
            ) {
        //
        float outputR = 0.0f;
        float outputG = 0.0f;
        float outputB = 0.0f;

        //Accumulating the values
        for (int i = 0; i < maskColumns; i++) {
            for (int j = 0; j < maskRows; j++) {
                const float current_M = deviceMaskData[i * MASK_WIDTH + j];
                outputR += current_M * ds_INPUT[((i + ty) * BLOCK_WIDTH + j + tx) * NB_CHANNELS];
                outputG += current_M * ds_INPUT[((i + ty) * BLOCK_WIDTH + j + tx) * NB_CHANNELS + 1];
                outputB += current_M * ds_INPUT[((i + ty) * BLOCK_WIDTH + j + tx) * NB_CHANNELS + 2];
            }
        }

        deviceOutputImageData[base_add_pixel_o] = outputR;
        deviceOutputImageData[base_add_pixel_o + 1] = outputG;
        deviceOutputImageData[base_add_pixel_o + 2] = outputB;

    }
    __syncthreads();


}

int main(int argc, char *argv[]) {
    wbArg_t args;
    int maskRows;
    int maskColumns;
    int imageChannels;
    int imageWidth;
    int imageHeight;
    char *inputImageFile;
    char *inputMaskFile;
    wbImage_t inputImage;
    wbImage_t outputImage;
    float *hostInputImageData;
    float *hostOutputImageData;
    float *hostMaskData;
    float *deviceInputImageData;
    float *deviceOutputImageData;
    float *deviceMaskData;

    args = wbArg_read(argc, argv); /* parse the input arguments */

    inputImageFile = wbArg_getInputFile(args, 0);
    inputMaskFile = wbArg_getInputFile(args, 1);

    inputImage = wbImport(inputImageFile);
    hostMaskData = (float *) wbImport(inputMaskFile, &maskRows, &maskColumns);

    assert(maskRows == 5);    /* mask height is fixed to 5 in this mp */
    assert(maskColumns == 5); /* mask width is fixed to 5 in this mp */

    imageWidth = wbImage_getWidth(inputImage);
    imageHeight = wbImage_getHeight(inputImage);
    imageChannels = wbImage_getChannels(inputImage);

    outputImage = wbImage_new(imageWidth, imageHeight, imageChannels);

    hostInputImageData = wbImage_getData(inputImage);
    hostOutputImageData = wbImage_getData(outputImage);

    wbTime_start(GPU, "Doing GPU Computation (memory + compute)");

    wbTime_start(GPU, "Doing GPU memory allocation");
    cudaMalloc((void **) &deviceInputImageData,
               imageWidth * imageHeight * imageChannels * sizeof(float));
    cudaMalloc((void **) &deviceOutputImageData,
               imageWidth * imageHeight * imageChannels * sizeof(float));
    cudaMalloc((void **) &deviceMaskData,
               maskRows * maskColumns * sizeof(float));
    wbTime_stop(GPU, "Doing GPU memory allocation");

    wbTime_start(Copy, "Copying data to the GPU");
    cudaMemcpy(deviceInputImageData, hostInputImageData,
               imageWidth * imageHeight * imageChannels * sizeof(float),
               cudaMemcpyHostToDevice);
    cudaMemcpy(deviceMaskData, hostMaskData,
               maskRows * maskColumns * sizeof(float),
               cudaMemcpyHostToDevice);
    wbTime_stop(Copy, "Copying data to the GPU");

    wbTime_start(Compute, "Doing the computation on the GPU");
    //@@ INSERT CODE HERE
    dim3 dimBlock(BLOCK_WIDTH, BLOCK_WIDTH, 1);
    dim3 dimGrid((imageWidth - 1) / O_TILE_WIDTH + 1, (imageHeight - 1) / O_TILE_WIDTH + 1, 1);


    stencil << < dimGrid, dimBlock >> > (deviceInputImageData, deviceOutputImageData, imageWidth, imageHeight,
            imageChannels, maskRows, maskColumns, deviceMaskData);


    wbTime_stop(Compute, "Doing the computation on the GPU");

    wbTime_start(Copy, "Copying data from the GPU");
    cudaMemcpy(hostOutputImageData, deviceOutputImageData,
               imageWidth * imageHeight * imageChannels * sizeof(float),
               cudaMemcpyDeviceToHost);
    wbTime_stop(Copy, "Copying data from the GPU");

    #if DEBUG_MODE

    for (int i = 0; i < 10; i += 3) {
        wbLog(TRACE, "The value of pixel ", i, " chanel 1 ", hostOutputImageData[i]);
        wbLog(TRACE, "The value of pixel ", i, " chanel 2 ", hostOutputImageData[i + 1]);
        wbLog(TRACE, "The value of pixel ", i, " chanel 3 ", hostOutputImageData[i + 2]);
    }
    #endif

    wbTime_stop(GPU, "Doing GPU Computation (memory + compute)");

    wbSolution(args, outputImage);

    cudaFree(deviceInputImageData);
    cudaFree(deviceOutputImageData);
    cudaFree(deviceMaskData);

    free(hostMaskData);
    wbImage_delete(outputImage);
    wbImage_delete(inputImage);

    return 0;
}
