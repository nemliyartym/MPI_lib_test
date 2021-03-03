#pragma once

#include <iostream>
#include "stdio.h"
#include "Matrix.h"
#include "MatrixHandler.h"
#include "mpi.h"
#include "time.h"
#include "windows.h"

const int rowsM1 = 70;
const int columsM1 = 30;

const int rowsM2 = 30;
const int columsM2 = 50;


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

    MatrixHandler mh(m1, m2);
    Matrix<int> result = mh.MatrixMultiplication_thread(4);

    for (auto i = 0; i < 10; i++) {
        HANDLE h = CreateThread(0, 0, &MatrixHandler::static_proxy, &mh, 0, 0);
    }

    finish = clock();
    printf("Time: %.4f\n", (double)(finish - start) / CLOCKS_PER_SEC);

    return 0;

}






