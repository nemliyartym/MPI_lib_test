#pragma once

#include "mpi.h"
#include "Matrix.h"
#include "MatrixHandler.h"


void multiplication_thread(int const  MAX_THREADS = 1) {
    clock_t t_start, t_finish;

    Matrix<int> m1(7, 3);
    Matrix<int> m2(3, 5);

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
    Matrix<int> matrix(m1.getCountColums(), m2.getCountRows());
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
    MPI_Status status;
    MPI_Request req;
    clock_t t_start, t_finish;

    const int rows = 9;
    const int colums = 9;


    t_start = clock();

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &rank_size);

    int size = rows * colums;
    
    int rcv_size = (rows / rank_size) * colums;
    if (my_rank + 1 == rank_size)
        rcv_size += (rows % rank_size) * colums;

    int* m1 = new int[rcv_size];
    int* m2 = new int[rcv_size];


    if (!my_rank) {
        m1 = new int[rcv_size];
        m2 = new int[rcv_size];
        for (auto i = 0; i < rcv_size; i++) {
            m1[i] = i + 1;
            m2[i] = rcv_size - i;
        }
        for (auto i = 1; i < rank_size; i++) {
            MPI_Send(m1, rcv_size, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    } else {
        MPI_Recv(m1, rcv_size, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

    }
    
    for (auto i = 0; i < rcv_size; i++) {
        cout << m1[i] <<" ";
    }

    //int *rcv_m1 = new int[rcv_size];
    //int* rcv_m2 = new int[rcv_size];
    
    //MPI_Scatter(m1, rcv_size, MPI_INT, rcv_m1, rcv_size, MPI_INT, 0, MPI_COMM_WORLD);
    //MPI_Scatter(m2, rcv_size, MPI_INT, rcv_m2, rcv_size, MPI_INT, 0, MPI_COMM_WORLD);

    //for (auto i = 0; i < rcv_size; i++)
      //  cout << rcv_m1[i] << " ";
    //cout << endl;
    
    //for (auto i = 0; i < rcv_size; i++)
      //  cout << rcv_m2[i] << " ";

//    MPI_Gather(reinterpret_cast<char*>(&tm), sizeof(Matrix<int>), MPI_CHAR,
//               reinterpret_cast<char*>(rcv_mh), sizeof(Matrix<int>), MPI_CHAR,0,MPI_COMM_WORLD);

    

    if (!my_rank) {
        t_finish = clock();
        printf("Time: %.4f\n", (double)(t_finish - t_start) / CLOCKS_PER_SEC);
    }

    MPI_Finalize();
}



int main(int argc, char* argv[]) {

    //multiplication_thread(4);

    //mpiexec -n 4 D:\VsProject\MPI_lib_test\Debug\MPI_lib_test.exe
    //mpiexec -n 4 D:\work\project\MPI_lib_test\nemliyartym\MPI_lib_test\Debug\MPI_lib_test.exe
    multiplication_mpi(argc, argv);

}






