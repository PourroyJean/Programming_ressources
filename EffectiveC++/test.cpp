//
// Created by Jean Pourroy on 2020-03-25.
//

#define TMP_FILE_monitoring "/nfs/pourroy/code/THESE/performance_modelisation/build/tmp_kg_output"
int NB_lOOP = 100;
int NB_lOOP_IN = 10000000;
int NB_INST = 4;
int P_COUNT = 0;
int P_UNROLLING = 16;
int CPU_BIND = 0;
int FLOP_SP_PER_LOOP = 0;
int FLOP_DP_PER_LOOP = 64;
bool is_check_freq = true;
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <stdint.h>
#include <iomanip>
#include <utility>
#include <fstream>
#include <inttypes.h>
#include <sched.h>

float check_frequency ();

uint64_t rdtsc() {
    uint32_t lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return (uint64_t) hi << 32 | lo;
}

#include <sys/time.h>
#include <string>

using namespace std;

double mygettime() {
    struct timeval tp;
    struct timezone tzp;
    int i;
    i = gettimeofday(&tp, &tzp);
    return ((double) tp.tv_sec + (double) tp.tv_usec * 1.e-6);
}

int main(int argc, char **argv) {
    unsigned long long int nbCycleIn;
    unsigned long long int cycleInStart, cycleInEnd;
    uint64_t cycle_total, instructions_total, instructions_executed, instructions_executed_total, loop_nb_instruction, nb_total_loop_iteration = 0;
    double IPC, inst_second = 0.0;
    int i;
    double timeStart, timeEnd, time_total;
    std::pair<int, double> *pairArr = new pair<int, double>[NB_lOOP];
    float Base_vs_Current_freq = 1;
    double flop_cycle_sp ,flop_cycle_dp ,flops_sp ,flops_dp;

    std::cout.precision(3);



    //    Let's bind the process to a particular core
    int cpu_bind = CPU_BIND;
    cpu_set_t mycpumask;
    CPU_ZERO(&mycpumask);
    if (argc > 1 && argv[1] >= 0 && argv[512]){
        CPU_BIND = atoi(argv[1]);
    }
    CPU_SET(CPU_BIND, &mycpumask);
    sched_setaffinity(0, sizeof(cpu_set_t), &mycpumask);
    /* double-check */
    sched_getaffinity(0, sizeof(cpu_set_t), &mycpumask);
    for (i = 0; i < sysconf(_SC_NPROCESSORS_CONF); i++) {
        if (CPU_ISSET(i, &mycpumask))
            printf("+ Running on CPU #%d\n", i);
    };

    for (i = 0; i < NB_lOOP; i++) {
        timeStart = mygettime();
        cycleInStart = rdtsc();
        __asm__ (""
                 //Initialisation opérandes à 1
                 "myBench: "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm2; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm3; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm4; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm5; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm2; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm3; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm4; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm5; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm2; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm3; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm4; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm5; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm2; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm3; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm4; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm5; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm2; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm3; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm4; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm5; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm2; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm3; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm4; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm5; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm2; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm3; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm4; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm5; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm2; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm3; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm4; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm5; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm2; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm3; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm4; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm5; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm2; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm3; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm4; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm5; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm2; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm3; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm4; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm5; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm2; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm3; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm4; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm5; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm2; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm3; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm4; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm5; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm2; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm3; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm4; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm5; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm2; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm3; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm4; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm5; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm2; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm3; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm4; "
                 "movapd %%xmm1, %%xmm7;" "vaddsd %%xmm0, %%xmm1, %%xmm5; "
                 "sub  $0x1, %%eax;"
                 "jnz  myBench;"		: "=r" (instructions_executed) : "a" (NB_lOOP_IN));
        cycleInEnd = rdtsc();
        timeEnd = mygettime();
        pairArr[i] = make_pair(cycleInEnd - cycleInStart, timeEnd - timeStart);
        cycle_total += (cycleInEnd - cycleInStart);
        time_total += timeEnd - timeStart;
        instructions_executed_total += instructions_executed;
    }
    ofstream mFile_template_start(TMP_FILE_monitoring, std::ios_base::binary);

    loop_nb_instruction = (uint64_t) NB_INST * (uint64_t) P_UNROLLING ;
    nb_total_loop_iteration = (uint64_t)(NB_lOOP_IN * (uint64_t) NB_lOOP);
    instructions_total = nb_total_loop_iteration * loop_nb_instruction;

    IPC = double(instructions_total) / double(cycle_total);
    double freq = (cycle_total / time_total )/1000000000;


    flop_cycle_sp  = float((FLOP_SP_PER_LOOP * nb_total_loop_iteration)) / cycle_total  ;
    flop_cycle_dp  = float((FLOP_DP_PER_LOOP * nb_total_loop_iteration)) / cycle_total ;
    flops_sp = (freq * 1000000000 * float((FLOP_SP_PER_LOOP * nb_total_loop_iteration)) / (cycle_total)) ;
    flops_dp = freq * 1000000000 * float((FLOP_DP_PER_LOOP * nb_total_loop_iteration)) / cycle_total ;

    //Applying some correction depending on the cpu clock: if the CPU is capped or has turbo: the rdtsc instruction will not work as expected
    float coef_freq = check_frequency ();
    if (coef_freq < 0.98){
        IPC /=  coef_freq;
        freq *= coef_freq;
        flop_cycle_sp  /=coef_freq;
        flop_cycle_dp  /=coef_freq;
    }
    else if (coef_freq > 1.02){
        IPC /= coef_freq;
        freq *= coef_freq;
    }

    inst_second = IPC * freq;


    cout << "------------------  INSTRUCTIONS SUMMARY -----------------------" << endl;
    if (P_COUNT) {
        instructions_executed_total -= (NB_INST * NB_lOOP);
        cout << setw(20) << "instructions_total" << setw(25) << "instructions_executed"     << setw(20) << "cycle_total" << setw(13) << "FREQUENCY" << setw(10) << "IPC" << std::endl;
        cout << setw(25) << instructions_total   << setw(25) << instructions_executed_total << setw(20) << cycle_total   << setw(13) << freq        << setw(10) << IPC * Base_vs_Current_freq << std::endl;

    } else {
        cout << "_label_|" <<  setw(18) <<"NB INSTRUCTIONS"    << setw(10) << "Time"     << setw(13) << "FREQUENCY" << setw(20) << "Giga_inst/sec" << setw(10) << "IPC" << std::endl;
        cout << "_value_|" <<  setw(18) <<instructions_total   << setw(10) << time_total << setw(13) << freq        << setw(20) <<   inst_second   << setw(10) << IPC   << std::endl;
    }

    cout << endl;

    cout << "----------------------  FLOP SUMMARY  --------------------------" << endl;
    cout << setw(10) << "PRECISION"  << setw(15) << "FLOP/cycle"   << setw(20) << "FLOP/second" << endl;
    cout << setw(10) << "Single"     << setw(15) << flop_cycle_sp  << setw(20) << flops_sp << endl ;
    cout << setw(10) << "Double"     << setw(15) << flop_cycle_dp  << setw(20) << flops_dp << endl ;
    cout << "----------------------------------------------------------------" << endl;





    for (int i = 0; i < NB_lOOP; ++i) {
        mFile_template_start << pairArr[i].first << " " << to_string(pairArr[i].second) << endl;
    }
    mFile_template_start.close();

    return 0;
}

float check_frequency() {
    if (!is_check_freq) {
        return 1;
    }

    cout << "--------------------  CHECK FREQUENCY  ------------------------" << endl;

    int i;
    double timeStart, timeEnd, time_total, freq_Base;
    unsigned long long int cycleInStart, cycleInEnd;
    int N_LOOP = 5;

    //********************
    //** BASE FREQUENCY  *
    //********************
    timeStart = mygettime();
    cycleInStart = rdtsc();
    usleep(10000);
    cycleInEnd = rdtsc();
    timeEnd = mygettime();
    unsigned long long int cycleSpent = (cycleInEnd - cycleInStart);

    freq_Base = cycleSpent / (1000000000 * (timeEnd - timeStart));
    cout << "+ Base      frequency is " << freq_Base << "Ghz" << endl;


    //********************
    //** REAL FREQUENCY  *
    //********************
    cycleInStart = rdtsc();
    for (i = 0; i < N_LOOP; i++) {
        __asm__ ("aloop: "
                 "       sub    $0x1,%%eax;"
                 "       sub    $0x1,%%eax;"
                 "       sub    $0x1,%%eax;"
                 "       sub    $0x1,%%eax;"
                 "       sub    $0x1,%%eax;"
                 "       sub    $0x1,%%eax;"
                 "       sub    $0x1,%%eax;"
                 "       sub    $0x1,%%eax;"
                 "       jnz    aloop" : : "a" (40000000UL)
        );
    }
    cycleInEnd = rdtsc();
    cycleSpent = (cycleInEnd - cycleInStart);

    unsigned long long int NbInstruction = (double) N_LOOP * (double) 40000000UL;
    float ipc = NbInstruction / (double) cycleSpent; //Should be equal to 1: 1 inst. per cycle
    float freq_Real = freq_Base * ipc;
    cout << "+ Current   frequency is " << freq_Real << "Ghz" << endl;



    //***************************************************
    //** OUTPUT: warning if the frequency is different  *
    //***************************************************
    if (freq_Base - freq_Real > 0.05) {
        cout << "+ /!\\ The frequency seems to be capped: -" << (1-ipc)*100 <<'%' <<  endl;
    } else if (freq_Real - freq_Base > 0.05) {
        cout << "+ /!\\ Turbo seems to be ON: +" << ipc  <<'%'  << " (be carful with the following values)"<< endl;
    } else{
        cout << "+ OK: the core is running at his frequency based value" << endl;
    }
    cout << endl << flush;



    return  ipc;
}