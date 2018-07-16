// Histogram Equalization
// Get some ideas from: https://github.com/XimingCheng/webgpu

#include <wb.h>

#define DEBUG_MODE 1


#define wbCheck(stmt)                                                     \
  do {                                                                    \
    cudaError_t err = stmt;                                               \
    if (err != cudaSuccess) {                                             \
      wbLog(ERROR, "Failed to run stmt ", #stmt);                         \
      wbLog(ERROR, "Got CUDA error ...  ", cudaGetErrorString(err));      \
      return -1;                                                          \
    }                                                                     \
  } while (0)

#define HISTOGRAM_LENGTH 256
#define BLOCK_SIZE 256

//@@ insert code here
__global__ void castFloatToUsignedChar(int width, int height, int channels,
                                       unsigned char * ucharImage, float * inputImage){
    //Create private value to accelerate their access
    int bx = blockIdx.x;
    int by = blockIdx.y;
    int tx = threadIdx.x;
    int ty = threadIdx.y;

    // Identify the row and column of the P element to work on
    int Row = by * blockDim.y + ty;
    int Col = bx * blockDim.x + tx;

    if (Row < height && Col < width){
        ucharImage[Row*width+Col] = (unsigned char) (255 * inputImage[Row*width+Col]);
    }
}

__global__ void castFloatToUsignedChar2(int width, int height, int channels,
                                        unsigned char * ucharImage, float * inputImage) {
    int w = threadIdx.x + blockDim.x * blockIdx.x;
    if (w < width * height * channels)
        ucharImage[w] = (unsigned char) (255 * inputImage[w]);
}

__global__ void convertRGBtoGrayScale (int width, int height, unsigned char * ucharImage, unsigned char * grayImage){

    // Identify the row and column of the P element to work on
    int Row = blockIdx.y * blockDim.y + threadIdx.y;
    int Col = blockIdx.x * blockDim.x + threadIdx.x;
    int idx = Row*width+Col;
    if (Row < height && Col < width){
        unsigned char r = ucharImage[3*idx];
        unsigned char g = ucharImage[3*idx + 1];
        unsigned char b = ucharImage[3*idx + 2];
        grayImage[idx] = (unsigned char) (0.21*r + 0.71*g + 0.07*b);
    }
}

__global__ void computeHistogram (unsigned char * grayImage, int imageLentgh, unsigned int * histo){

    __shared__ unsigned int private_histo[256];
    int idx = threadIdx.x + blockIdx.x * blockDim.x;

    //Not with idx because we want to initialize histo_private for each block
    if (threadIdx.x < 256)
        private_histo[threadIdx.x] = 0;
    __syncthreads();

    int stride = blockDim.x * gridDim.x;
    while (idx < imageLentgh) {
        atomicAdd( &(private_histo[grayImage[idx]]), 1);
        idx += stride;
    }
    __syncthreads();


    if (threadIdx.x < 256) {
        atomicAdd(&(histo[threadIdx.x]), private_histo[threadIdx.x] );
    }

}

__global__ void calHistogram(unsigned char* grayImage, int size, unsigned int* histogram) {
    __shared__ unsigned int histo_private[HISTOGRAM_LENGTH];
    int t = threadIdx.x;
    if (t < HISTOGRAM_LENGTH)
        histo_private[t] = 0;
    __syncthreads();

    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    int stride = blockDim.x * gridDim.x;
    while (idx < size) {
        atomicAdd(&histo_private[grayImage[idx]], 1);
        idx += stride;
    }
    __syncthreads();

    if (t < HISTOGRAM_LENGTH)
        atomicAdd(&histogram[t], histo_private[t]);
}

__global__ void ComputetheCumulativeDistributionFunction (unsigned int * histogram, int size, float * distribution){

}


int main(int argc, char **argv) {
    wbArg_t args;
    int imageWidth;
    int imageHeight;
    int imageChannels;
    wbImage_t inputImage;
    wbImage_t outputImage;
    float *hostInputImageData;
    float *hostOutputImageData;
    const char *inputImageFile;

    //@@ Insert more code here
    float         * deviceInputImageData;
    unsigned char * deviceUcharImage;
    unsigned char * deviceGrayImage;
    unsigned int  * hostHistogram;
    unsigned int  * deviceHistogram;


    args = wbArg_read(argc, argv); /* parse the input arguments */

    inputImageFile = wbArg_getInputFile(args, 0);

    wbTime_start(Generic, "Importing data and creating memory on host");
    inputImage = wbImport(inputImageFile);
    imageWidth = wbImage_getWidth(inputImage);
    imageHeight = wbImage_getHeight(inputImage);
    imageChannels = wbImage_getChannels(inputImage);
    outputImage = wbImage_new(imageWidth, imageHeight, imageChannels);
    wbTime_stop(Generic, "Importing data and creating memory on host");

    //@@ insert code here
    hostInputImageData = wbImage_getData(inputImage);
    hostOutputImageData = wbImage_getData(outputImage);
    int imageLength = imageWidth * imageHeight;
    int imageRGBLength   = imageWidth * imageHeight * imageChannels;


#if DEBUG_MODE
    unsigned char* debug_uchar_x1  = (unsigned char*)malloc(imageLength * sizeof(unsigned char));
    unsigned char* debug_uchar_x3  = (unsigned char*)malloc(imageRGBLength * sizeof(unsigned char));
    unsigned int * debug_hist      = (unsigned int *)malloc(HISTOGRAM_LENGTH * sizeof( unsigned int));


    wbLog(TRACE, "The value of imageWidth = ", imageWidth);
    wbLog(TRACE, "The value of imageHeight = ", imageHeight);
#endif


//  wbTime_start(Compute, "Performing CUDA computation");
    //@@ Launch the GPU Kernel here

    //###################### PHASE 0: MEMORY INIT #####################################################################@

    wbCheck(cudaMalloc((void **) &deviceInputImageData,  imageRGBLength * sizeof(float)));
    wbCheck(cudaMalloc((void **) &deviceUcharImage,      imageRGBLength * sizeof(unsigned char)));
    wbCheck(cudaMalloc((void **) &deviceGrayImage,       imageLength    * sizeof(unsigned char)));

    wbCheck(cudaMemcpy(deviceInputImageData, hostInputImageData, imageRGBLength * sizeof(float), cudaMemcpyHostToDevice));

    //###################### PHASE 1: FLOAT TO UNSIGNED CHAR #################################################################@
    dim3 dimGrid(ceil(imageWidth*3/16.0), ceil(imageHeight*3/16.0), 1);
    dim3 dimBlock(16, 16, 1);
    //castFloatToUsignedChar <<<dimGrid,dimBlock>>> (imageWidth, imageHeight, imageChannels, deviceUcharImage, deviceInputImageData);

    dim3 dimBlockx(16, 1, 1);
    dim3 dimGridx((imageRGBLength - 1)/16 + 1, 1, 1);
    castFloatToUsignedChar2 <<<dimGridx,dimBlockx>>> (imageWidth, imageHeight, imageChannels, deviceUcharImage, deviceInputImageData);

    cudaDeviceSynchronize();

#if DEBUG_MODE
    wbCheck(cudaMemcpy(debug_uchar_x3, deviceUcharImage, imageRGBLength * sizeof(unsigned char), cudaMemcpyDeviceToHost));
    for (int i = 0;i < 256; i++) {
        //wbLog(TRACE, "The value of ucharImage = ", (int) debug_uchar_x3[i]);
    }
#endif


    //###################### PHASE 2: RGB TO GRAY #####################################################################################@
    dim3 dimGrid1  (ceil(imageWidth/16.0), ceil(imageHeight/16.0), 1);
    dim3 dimBlock1 (16, 16, 1);
    convertRGBtoGrayScale <<<dimGrid1,dimBlock1>>> (imageWidth, imageHeight, deviceUcharImage, deviceGrayImage);
    cudaDeviceSynchronize();
#if DEBUG_MODE
    wbCheck(cudaMemcpy(debug_uchar_x1, deviceGrayImage, imageLength * sizeof(unsigned char), cudaMemcpyDeviceToHost));
    for (int i = 1000;i < 1500; i++) {
        wbLog(TRACE, "The value of grayImage = [", i, "]", (int)debug_uchar_x1[i]);
    }
#endif

    //###################### PHASE 3: HISTOGRAM ########################################@
    hostHistogram = (unsigned int *) malloc (HISTOGRAM_LENGTH * sizeof(unsigned int));
    wbCheck(cudaMalloc((void **) &deviceHistogram,   HISTOGRAM_LENGTH * sizeof(unsigned int)));
    dim3 dimGrid2  (ceil(imageLength/BLOCK_SIZE), 1, 1);
    dim3 dimBlock2 (BLOCK_SIZE, 1, 1);
    computeHistogram <<<dimGrid2, dimBlock2>>> (deviceGrayImage, imageLength, deviceHistogram);

    //dim3 dimBlock2(BLOCK_SIZE, 1, 1);
    //dim3 dimGrid2((imageLength - 1)/BLOCK_SIZE + 1, 1, 1);
    //calHistogram <<< dimGrid2, dimBlock2 >>> (deviceGrayImage, imageLength, deviceHistogram);
    cudaDeviceSynchronize();
#if DEBUG_MODE

    wbCheck(cudaMemcpy(debug_hist, deviceHistogram, HISTOGRAM_LENGTH * sizeof(unsigned int), cudaMemcpyDeviceToHost));
    for (int i = 0;i < 256; i++) {
        wbLog(TRACE, "The value of histogram [", i, "]", (unsigned int)debug_hist[i]);
    }
#endif

    dim3 dimGrid3  (ceil(imageLength/BLOCK_SIZE), 1, 1);
    dim3 dimBlock3 (BLOCK_SIZE, 1, 1);
    ComputetheCumulativeDistributionFunction <<<dimGrid2, dimBlock2>>> (deviceHistogram, float * distribution);



    wbSolution(args, outputImage);

    //@@ insert code here

    return 0;
}
