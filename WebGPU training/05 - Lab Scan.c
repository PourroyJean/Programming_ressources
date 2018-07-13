// MP Scan
// Given a list (lst) of length n
// Output its prefix sum = {lst[0], lst[0] + lst[1], lst[0] + lst[1] + ...
// +
// lst[n-1]}

#include <wb.h>

#define DEBUG_MODE 1
#define BLOCK_SIZE 512 //@@ You can change this
#define SECTION_SIZE (BLOCK_SIZE << 1)
int NB_BLOCK = 0;

#define wbCheck(stmt)                                                     \
  do {                                                                    \
    cudaError_t err = stmt;                                               \
    if (err != cudaSuccess) {                                             \
      wbLog(ERROR, "Failed to run stmt ", #stmt);                         \
      wbLog(ERROR, "Got CUDA error ...  ", cudaGetErrorString(err));      \
      return -1;                                                          \
    }                                                                     \
  } while (0)


__global__ void phase1_scan(float *input, float *output, float *res, int len) {
    //int i = blockIdx.x * blockDim.x + threadIdx.x;
    //int t = threadIdx.x;
    int index_start = blockIdx.x * SECTION_SIZE;
    int global_index_t1 = index_start + threadIdx.x;
    int global_index_t2 = index_start + threadIdx.x + BLOCK_SIZE;
    int local_index_t1 = threadIdx.x;
    int local_index_t2 = threadIdx.x + BLOCK_SIZE;


    //Shared load: we load 2 elements by thread
    __shared__ float XY[SECTION_SIZE];
    //First
    if (global_index_t1 < len) {
        XY[local_index_t1] = input[global_index_t1];
    } else {
        XY[local_index_t1] = 0;
    }
    //Second
    if (global_index_t2 < len) {
        XY[local_index_t2] = input[global_index_t2];
    } else {
        XY[local_index_t2] = 0;
    }
    __syncthreads();

    //First part of the scan
    for (unsigned int stride = 1; stride <= BLOCK_SIZE; stride *= 2) {
        //Select the correct index:
        //Iteration 1: 1, 3, 5, 7
        //Iteration 2: 3, 7, 11
        //Iteration 3: 7
        int index = (threadIdx.x + 1) * 2 * stride - 1;

        if (index < SECTION_SIZE) {
            //We add the element on the left: with is at my_index - stride
            XY[index] += XY[index - stride];
        }
        __syncthreads();
    }


    //Second part of the scan: distribution
    for (int stride = BLOCK_SIZE / 2; stride > 0; stride /= 2) {
        __syncthreads();
        //Select the correct index, we share the biggest results to other cell to be used
        //Iteration 1: 3
        //Iteration 2: 2, 4, 6
        int index = (threadIdx.x + 1) * stride * 2 - 1;
        if (index + stride < SECTION_SIZE) {
            XY[index + stride] += XY[index];
        }
    }
    __syncthreads();

    if (global_index_t1 < len) {
        output[global_index_t1] = XY[local_index_t1];
    }
    if (global_index_t2 < len) {
        output[global_index_t2] = XY[local_index_t2];
    }

    int SS = SECTION_SIZE;
    //Do we use an array to store a temporal result
    //Only one do it
    if (res && local_index_t1 == 0) {
        res[blockIdx.x] = XY[SECTION_SIZE - 1];
    }


}

__global__ void phase2_add(float *output, float *deviceScanResults_cumulated, int len) {

    int index_start = (blockIdx.x + 1) * SECTION_SIZE;
    int global_index_t1 = index_start + threadIdx.x;
    int global_index_t2 = global_index_t1 + BLOCK_SIZE;

    if (global_index_t1 < len) {
        output[global_index_t1] += deviceScanResults_cumulated[blockIdx.x];
    }

    //Second
    if (global_index_t2 < len) {
        output[global_index_t2] += deviceScanResults_cumulated[blockIdx.x];
    }

}


int main(int argc, char **argv) {
    wbArg_t args;
    float *hostInput;  // The input 1D list
    float *hostOutput; // The output list
    float *deviceInput;
    float *deviceOutput;
    int numElements; // number of elements in the list
    float *deviceScanResults = NULL;
    float *deviceScanResults_cumulated;

    args = wbArg_read(argc, argv);

    wbTime_start(Generic, "Importing data and creating memory on host");
    hostInput = (float *) wbImport(wbArg_getInputFile(args, 0), &numElements);
    hostOutput = (float *) malloc(numElements * sizeof(float));
    wbTime_stop(Generic, "Importing data and creating memory on host");

    // wbLog(TRACE, "The number of input elements in the input is ", numElements);

    wbTime_start(GPU, "Allocating GPU memory.");
    wbCheck(cudaMalloc((void **) &deviceInput, numElements * sizeof(float)));
    wbCheck(cudaMalloc((void **) &deviceOutput, numElements * sizeof(float)));
    wbTime_stop(GPU, "Allocating GPU memory.");

    wbTime_start(GPU, "Clearing output memory.");
    wbCheck(cudaMemset(deviceOutput, 0, numElements * sizeof(float)));
    wbTime_stop(GPU, "Clearing output memory.");

    wbTime_start(GPU, "Copying input memory to the GPU.");
    wbCheck(cudaMemcpy(deviceInput, hostInput, numElements * sizeof(float),
                       cudaMemcpyHostToDevice));
    wbTime_stop(GPU, "Copying input memory to the GPU.");

    //@@ Initialize the grid and block dimensions here
    //dim3 dimGrid(ceil(numElements/BLOCK_SIZE), 1, 1);
    //dim3 dimBlock(BLOCK_SIZE, 1);

    wbTime_start(Compute, "Performing CUDA computation");
    //@@ Modify this to complete the functionality of the scan
    //@@ on the deivce
    NB_BLOCK = ceil((numElements - 1) / SECTION_SIZE) + 1;

    wbLog(TRACE, "numElements  ", numElements);
    wbLog(TRACE, "BLOCK_SIZE   ", BLOCK_SIZE);
    wbLog(TRACE, "SECTION_SIZE ", SECTION_SIZE);
    wbLog(TRACE, "NB_BLOCK     ", NB_BLOCK);


    //We allocate the scan result array only if needed
    if (NB_BLOCK > 1) {
        wbCheck(cudaMalloc((void **) &deviceScanResults, NB_BLOCK * sizeof(float)));
        wbCheck(cudaMalloc((void **) &deviceScanResults_cumulated, NB_BLOCK * sizeof(float)));

    }


    //Maximum block dimensions: 1024 x 1024 x 64
    dim3 dimGrid(NB_BLOCK, 1, 1);
    dim3 dimBlock(BLOCK_SIZE, 1, 1);

    phase1_scan << < dimGrid, dimBlock >> > (deviceInput, deviceOutput, deviceScanResults, numElements);

    if (NB_BLOCK > 1) {
        dim3 dimGrid1(1, 1, 1);
        dim3 dimBlock1(BLOCK_SIZE, 1, 1);
        phase1_scan << < dimGrid1, dimBlock1 >> > (deviceScanResults, deviceScanResults_cumulated, NULL, NB_BLOCK);

        dim3 dimGrid2(NB_BLOCK - 1, 1, 1);
        dim3 dimBlock2(BLOCK_SIZE, 1, 1);
        phase2_add << < dimGrid2, dimBlock2 >> > (deviceOutput, deviceScanResults_cumulated, numElements);


    }

    cudaDeviceSynchronize();
    wbTime_stop(Compute, "Performing CUDA computation");

    wbTime_start(Copy, "Copying output memory to the CPU");
    wbCheck(cudaMemcpy(hostOutput, deviceOutput, numElements * sizeof(float),
                       cudaMemcpyDeviceToHost));
    wbTime_stop(Copy, "Copying output memory to the CPU");

    wbTime_start(GPU, "Freeing GPU Memory");
    cudaFree(deviceInput);
    cudaFree(deviceOutput);
    wbTime_stop(GPU, "Freeing GPU Memory");

    #if DEBUG_MODE

    // for (int i = 0;i < 10; i++) {
//    for (int i = numElements- 10;i < numElements ; i++) {
//    for (int i = 512 - 5; i < 512 + 5; i++) {
    for (int i = 1024 - 3; i < 1024 + 3; i++) {
        wbLog(TRACE, "The value of hostInput [", i, "]", hostInput[i]);
    }
//      for (int i = 0;i < 10; i++) {
//  for (int i = numElements- 10;i < numElements ; i++) {
//    for (int i = 512 - 5; i < 512 + 5; i++) {
    for (int i = 1024 - 3; i < 1024 + 3; i++) {
        wbLog(TRACE, "The value of hostOutput [", i, "]", hostOutput[i]);
    }
    #endif

    wbSolution(args, hostOutput, numElements);

    free(hostInput);
    free(hostOutput);

    return 0;
}

