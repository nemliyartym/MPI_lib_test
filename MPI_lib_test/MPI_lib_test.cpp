#pragma once

#include <iostream>
#include "stdio.h"
#include "Matrix.h"
#include "MatrixHandler.h"
#include "mpi.h"
#include "time.h"
#include "windows.h"

const int rowsM1 = 700;
const int columsM1 = 300;

const int rowsM2 = 300;
const int columsM2 = 500;

const int MAX_THREADS = 4;

int mass1[rowsM1][columsM1] = {
    {1 ,2 ,3 },
    {4 ,5 ,6 },
    {7 ,8 ,9 },
    {10,11,12},
    {13,14,15},
    {16,17,18},
    {19,20,21}

};

int mass2[rowsM2][columsM2] = {
    {1 ,2 ,3 ,4 ,5 },
    {6 ,7 ,8 ,9 ,10},
    {11,12,13,14,15}
};

int main(int argc, char* argv[]) {

    clock_t start, finish;
    
    Matrix<int> m1(rowsM1,columsM1);
    Matrix<int> m2(rowsM2, columsM2);
    
    for (auto i = 0; i < rowsM1; i++)
        for (auto j = 0; j < columsM1; j++)
            m1.SetElemetMatrix(i, j, mass1[i][j]);

    for (auto i = 0; i < rowsM2; i++)
        for (auto j = 0; j < columsM2; j++)
            m2.SetElemetMatrix(i, j, mass2[i][j]);

    start = clock();  

    HANDLE threads[MAX_THREADS];
    MatrixHandler mh[MAX_THREADS];

    for (auto i = 0; i < MAX_THREADS; i++) {
        int start = i*(m1.getCountColums()/ MAX_THREADS);
        int finish;
        if (i == MAX_THREADS - 1)
            finish = (i + 1) * m1.getCountColums() / MAX_THREADS;
        else finish = (i + 1) * (m1.getCountColums() / MAX_THREADS);
        mh[i].setMatrices(m1,m2);
        mh[i].setRange(start, finish);
        threads[i] = CreateThread(0, 0, &MatrixHandler::static_proxy, &mh[i], 0, 0);
        WaitForMultipleObjects(MAX_THREADS, threads, TRUE, INFINITE);
    }
            

    finish = clock();
    printf("Time: %.4f\n", (double)(finish - start) / CLOCKS_PER_SEC);

    return 0;

}






