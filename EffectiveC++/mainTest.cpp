//
// Created by Jean Pourroy on 2019-12-04.
//


#include <iostream>

#include <sys/time.h> //taking time
#include <time.h> //taking time

struct timeval start_time, end_time;


#define TIC gettimeofday(&start_time, NULL);
#define TOC gettimeofday(&end_time, NULL); TIME_ELAPSED = (end_time.tv_sec - start_time.tv_sec) * 1000000 + (end_time.tv_usec - start_time.tv_usec);

float TIME_ELAPSED;

using namespace std;

void print_times(char *s) {
    int total_usecs;
    printf("%s%.2f\n", s, ((float) TIME_ELAPSED) / 1000.0);
}


void div(int MAX) {
    double X = 999999999.9;
    TIC
    for (int i = 1; i < MAX; ++i) {
        X = X / 1.0000123456;
        X = X / 1.0000123456;
        X = X / 1.0000123456;
        X = X / 1.0000123456;
        X = X / 1.0000123456;
        X = X / 1.0000123456;
        X = X / 1.0000123456;
        X = X / 1.0000123456;
        X = X / 1.0000123456;
        X = X / 1.0000123456;
    }
    TOC
    print_times("Division time :");
    cout << "Division X  : " << X << endl;
}

void mult(int MAX) {
    double X = 999999999.9;
    TIC
    for (int i = 1; i < MAX; ++i) {
        X = X * 0.99998765455;
        X = X * 0.99998765455;
        X = X * 0.99998765455;
        X = X * 0.99998765455;
        X = X * 0.99998765455;
        X = X * 0.99998765455;
        X = X * 0.99998765455;
        X = X * 0.99998765455;
        X = X * 0.99998765455;
        X = X * 0.99998765455;
    }
    TOC
    print_times("Mult time :");
    cout << "Mult X  : " << X << endl;


}

int main() {
    int MAX = 10000000;
    div(MAX);
    mult(MAX);
    return 0;
}