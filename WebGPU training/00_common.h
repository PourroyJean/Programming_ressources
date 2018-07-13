//
// Created by Jean Pourroy on 04/07/2018.
//

#ifndef __00_common_H__
#define __00_common_H__


//An example usage is wbCheck(cudaMalloc(...)).


#define wbCheck(stmt) do {                                                    \
        cudaError_t err = stmt;                                               \
        if (err != cudaSuccess) {                                             \
            wbLog(ERROR, "Failed to run stmt ", #stmt);                       \
            wbLog(ERROR, "Got CUDA error ...  ", cudaGetErrorString(err));    \
            return -1;                                                        \
        }                                                                     \
    } while(0)

//@@ WbLog is a provided logging API (similar to Log4J).
//@@ The logging function wbLog takes a level which is either
//@@ OFF, FATAL, ERROR, WARN, INFO, DEBUG, or TRACE and a
//@@ message to be printed.
#define  wbLog (L,S) {}




#endif //__00_common_H__
