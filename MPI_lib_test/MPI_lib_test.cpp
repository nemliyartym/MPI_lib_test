#pragma once

#include <iostream>
#include "stdio.h"
#include "Matrix.h"
#include "MatrixHandler.h"
#include "time.h"
#include "windows.h"


void multiplication_thread(int const  MAX_THREADS = 1) {

    const int rowsM1 = 7;
    const int columsM1 = 3;
    const int rowsM2 = 3;
    const int columsM2 = 5;
    clock_t start, finish;

    Matrix<int> m1(rowsM1, columsM1);
    Matrix<int> m2(rowsM2, columsM2);

    for (int i = 0; i < m1.getCountColums(); i++)
        for (int j = 0; j < m1.getCountRows(); j++)
            m1.SetElemetMatrix(i,j,i+1);

    for (int i = 0; i < m2.getCountColums(); i++)
        for (int j = 0; j < m2.getCountRows(); j++)
            m2.SetElemetMatrix(i, j, i + 1);

    start = clock();

    //запараллеливаем умножение матриц
    HANDLE *threads = new HANDLE[MAX_THREADS];
    MatrixHandler* mh = new MatrixHandler[MAX_THREADS];
    for (auto i = 0; i < MAX_THREADS; i++) {
        int start = i * (m1.getCountColums() / MAX_THREADS);
        int finish;
        if (i == MAX_THREADS - 1)
            finish = (i + 1) * m1.getCountColums() / MAX_THREADS;
        else finish = (i + 1) * (m1.getCountColums() / MAX_THREADS);
        mh[i].setMatrices(m1, m2);
        mh[i].setRange(start, finish);

        threads[i] = CreateThread(0, 0, &MatrixHandler::static_proxy, &mh[i], 0, 0);
        WaitForMultipleObjects(MAX_THREADS, threads, TRUE, INFINITE);
    }

    //Склеиваем матрицы
    Matrix<int> matrix(rowsM1, columsM2);
    for (auto i = 0; i < MAX_THREADS; i++) {
        Matrix<int> tmp_matrix = mh[i].getMatrix();
        for (auto k = mh[i].getStart(); k < matrix.getCountColums(); k++) {
            for (auto l = 0; l < matrix.getCountRows(); l++) {
                matrix.SetElemetMatrix(k, l, tmp_matrix.GetElementMatrix(k, l));
            }
        }
    }
    
    finish = clock();
    printf("Time: %.4f\n", (double)(finish - start) / CLOCKS_PER_SEC);
    
}




int main(int argc, char* argv[]) {

   multiplication_thread(4);

}






