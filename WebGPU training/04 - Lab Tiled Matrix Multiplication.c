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

#define TILE_WIDTH 16

// Compute C = A * B
__global__ void matrixMultiplyShared(float *A, float *B, float *C,
                                     int numARows, int numAColumns,
                                     int numBRows, int numBColumns,
                                     int numCRows, int numCColumns) {
    //@@ Insert code to implement matrix multiplication here
    //@@ You have to use shared memory for this MP
    __shared__ float ds_A[TILE_WIDTH][TILE_WIDTH];
    __shared__ float ds_B[TILE_WIDTH][TILE_WIDTH];

    int largeur = numBColumns;
    int hauteur = numARows;


    //Create private value to accelerate their access
    int bx = blockIdx.x;
    int by = blockIdx.y;
    int tx = threadIdx.x;
    int ty = threadIdx.y;

    // Identify the row and column of the P element to work on
    int Row = by * blockDim.y + ty;
    int Col = bx * blockDim.x + tx;
    float Pvalue = 0.0;

    //Loop over every tile
    int nb_phase = (numAColumns - 1)/TILE_WIDTH + 1;
    for (int phase = 0; phase < nb_phase; phase ++){

        //Coolaborative Loading of A and B
        //On exclu le bord droite pour charger A:
        //Calcul de l'indice colonne maximum pour pas dépasser la matrix A = phase * TILE_WIDTH + tx
        if ((Row < hauteur)  && (phase * TILE_WIDTH + tx) < numAColumns){
            // Calcul de @ligne: Row * largeur
            // Calcul de @Col  : phase * TILE_WIDTH +  tx
            ds_A [ty][tx] = A [ Row * numAColumns + phase * TILE_WIDTH + tx] ;
        }
        else
            ds_A [ty][tx] = 0.0;

        //On exclu le bas de la matrice pour charger B:
        // phase * TILE_WIDTH + ty
        if ((Col < largeur && (phase * TILE_WIDTH + ty ) < numBRows )){
            // Calcul de @ligne: (phase * TILE_WIDTH + ty) * largeur
            // Calcul de @Col  : Col
            ds_B [ty][tx] = B [ (phase * TILE_WIDTH + ty) * largeur + Col] ;
        }
        else
            ds_B [ty][tx] = 0.0;

        __syncthreads();

        for (int i = 0; i < TILE_WIDTH; i++)
        {
            Pvalue += (ds_A[ty][i] * ds_B[i][tx]);
        }
        __syncthreads();
    }

    if (Row < hauteur && Col < largeur)
    {
        C[Row * largeur + Col] = Pvalue;
    }
}



int main(int argc, char **argv) {
    wbArg_t args;
    float *hostA; // The A matrix
    float *hostB; // The B matrix
    float *hostC; // The output C matrix
    float *deviceA;
    float *deviceB;
    float *deviceC;
    int numARows;    // number of rows in the matrix A
    int numAColumns; // number of columns in the matrix A
    int numBRows;    // number of rows in the matrix B
    int numBColumns; // number of columns in the matrix B
    int numCRows;    // number of rows in the matrix C (you have to set this)
    int numCColumns; // number of columns in the matrix C (you have to set
    // this)

    args = wbArg_read(argc, argv);

    wbTime_start(Generic, "Importing data and creating memory on host");
    hostA = (float *)wbImport(wbArg_getInputFile(args, 0), &numARows,
                              &numAColumns);
    hostB = (float *)wbImport(wbArg_getInputFile(args, 1), &numBRows,
                              &numBColumns);
    //@@ Set numCRows and numCColumns
    numCRows = numARows;
    numCColumns = numBColumns;
    //@@ Allocate the hostC matrix
    int sizeA = sizeof(float) * numARows * numAColumns;
    int sizeB = sizeof(float) * numBRows * numBColumns;
    int sizeC = sizeof(float) * numARows * numBColumns;
    hostC = (float *) malloc (sizeC);
    wbTime_stop(Generic, "Importing data and creating memory on host");


    wbLog(TRACE, "The dimensions of A are ", numARows, " x ", numAColumns);
    wbLog(TRACE, "The dimensions of B are ", numBRows, " x ", numBColumns);

    wbTime_start(GPU, "Allocating GPU memory.");
    //@@ Allocate GPU memory here
    wbCheck(cudaMalloc((void **) &deviceA, sizeA));
    wbCheck(cudaMalloc((void **) &deviceB, sizeB));
    wbCheck(cudaMalloc((void **) &deviceC, sizeC));

    wbTime_stop(GPU, "Allocating GPU memory.");

    wbTime_start(GPU, "Copying input memory to the GPU.");
    //@@ Copy memory to the GPU here
    wbCheck(cudaMemcpy(deviceA, hostA, sizeA, cudaMemcpyHostToDevice));
    wbCheck(cudaMemcpy(deviceB, hostB, sizeB, cudaMemcpyHostToDevice));
    wbTime_stop(GPU, "Copying input memory to the GPU.");

    //@@ Initialize the grid and block dimensions here
    dim3 dimGrid((numCColumns - 1)/TILE_WIDTH + 1, (numCRows - 1)/TILE_WIDTH + 1, 1);
    dim3 dimBlock(TILE_WIDTH, TILE_WIDTH, 1);

    wbTime_start(Compute, "Performing CUDA computation");
    //@@ Launch the GPU Kernel here
    matrixMultiplyShared <<<dimGrid,dimBlock>>> (deviceA, deviceB, deviceC,
            numARows, numAColumns,
            numBRows, numBColumns,
            numCRows, numCColumns);

    cudaDeviceSynchronize();
    wbTime_stop(Compute, "Performing CUDA computation");

    wbTime_start(Copy, "Copying output memory to the CPU");
    //@@ Copy the GPU memory back to the CPU here
    wbCheck(cudaMemcpy(hostC, deviceC, sizeC, cudaMemcpyDeviceToHost));

    wbTime_stop(Copy, "Copying output memory to the CPU");

    wbTime_start(GPU, "Freeing GPU Memory");
    //@@ Free the GPU memory here
    wbCheck(cudaFree(deviceA));
    wbCheck(cudaFree(deviceB));
    wbCheck(cudaFree(deviceC));

    wbTime_stop(GPU, "Freeing GPU Memory");

    wbSolution(args, hostC, numCRows, numCColumns);

    free(hostA);
    free(hostB);
    free(hostC);

    return 0;
}
