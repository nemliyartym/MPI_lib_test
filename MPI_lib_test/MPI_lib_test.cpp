#pragma once

#include "mpi.h"
#include "Matrix.h"
#include "MatrixHandler.h"

const int rowsM1 = 7;
const int columsM1 = 3;
const int rowsM2 = 3;
const int columsM2 = 5;

Matrix<int> m1(rowsM1, columsM1);
Matrix<int> m2(rowsM2, columsM2);

void multiplication_thread(int const  MAX_THREADS = 1) {
    clock_t t_start, t_finish;

    for (int i = 0; i < m1.getCountColums(); i++)
        for (int j = 0; j < m1.getCountRows(); j++)
            m1.SetElemetMatrix(i, j, i + 1);
 
    for (int i = 0; i < m2.getCountColums(); i++)
        for (int j = 0; j < m2.getCountRows(); j++)
            m2.SetElemetMatrix(i, j, i + 1);

    t_start = clock();

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
    
    t_finish = clock();
    printf("Time: %.4f\n", (double)(t_finish - t_start) / CLOCKS_PER_SEC);
    
}

void multiplication_mpi(int argc, char* argv[]) {
    int my_rank, rank_size;
    clock_t t_start, t_finish;
    MPI_Status status;

    for (int i = 0; i < m1.getCountColums(); i++)
        for (int j = 0; j < m1.getCountRows(); j++)
            m1.SetElemetMatrix(i, j, i + 1);

    for (int i = 0; i < m2.getCountColums(); i++)
        for (int j = 0; j < m2.getCountRows(); j++)
            m2.SetElemetMatrix(i, j, i + 1);

    t_start = clock();

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &rank_size);

    MatrixHandler* mh = NULL;
    if (!my_rank) {
        mh = new MatrixHandler[rank_size];
        for (auto i = 0; i < rank_size; i++) {
            int start = i * (m1.getCountColums() / rank_size);
            int finish;
            if (i == rank_size - 1)
                finish = (i + 1) * m1.getCountColums() / rank_size;
            else finish = (i + 1) * (m1.getCountColums() / rank_size);
            mh[i].setMatrices(m1, m2);
            mh[i].setRange(start, finish);
        }
    }

    MatrixHandler tmp;
    MPI_Scatter(reinterpret_cast<char*>(mh),sizeof(MatrixHandler),MPI_CHAR,
                reinterpret_cast<char*>(&tmp),sizeof(MatrixHandler),MPI_CHAR,0, MPI_COMM_WORLD);

  
    Matrix<int> tm = tmp.MatrixMultiplication();
    cout << "zbs\n";

    Matrix<int> *mm = NULL;
    if (!my_rank)
        mm = new Matrix<int>[rank_size];
    
    MPI_Gather(reinterpret_cast<char*>(&tm), sizeof(MatrixHandler), MPI_CHAR,
               reinterpret_cast<char*>(mm), sizeof(MatrixHandler), MPI_CHAR,0,MPI_COMM_WORLD);

    if (!my_rank) {
        for (auto i = 0; i < rank_size; i++) {
            if(mm)
                mm[i].PrintMatrix();
        }
    }

    //Склеиваем матрицы
    if (!my_rank) {
        t_finish = clock();
        printf("Time: %.4f\n", (double)(t_finish - t_start) / CLOCKS_PER_SEC);
    }

    MPI_Finalize();
}



int main(int argc, char* argv[]) {

    //multiplication_thread(4);

    //mpiexec -n 4 D:\VsProject\MPI_lib_test\Debug\MPI_lib_test.exe
    multiplication_mpi(argc, argv);

}






