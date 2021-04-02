#pragma once

#include "mpi.h"
#include "omp.h"
#include "Matrix.h"
#include "MatrixHandler.h"


const int rowsM1 = 700;
const int columsM1 = 300;

const int rowsM2 = 300;
const int columsM2 = 500;

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


void multiplication_omp (int count_thread){
    clock_t t_start, t_finish;
    t_start = clock();
    for (int i = 0; i < m1.getCountColums(); i++)
        for (int j = 0; j < m1.getCountRows(); j++)
            m1.SetElemetMatrix(i, j, i + 1);

    for (int i = 0; i < m2.getCountColums(); i++)
        for (int j = 0; j < m2.getCountRows(); j++)
            m2.SetElemetMatrix(i, j, i + 1);


    omp_set_num_threads(count_thread);
    
   
    Matrix<int> matrix(m1.getCountColums(), m2.getCountRows());

#pragma omp parallel for
    for (auto h = 0; h < m1.getCountColums(); h++) {
        for (auto k = 0; k < m2.getCountRows(); k++) {
            int result = 0;
            for (auto p = 0; p < m2.getCountColums(); p++) {
                result += m1.GetElementMatrix(h, p) * m2.GetElementMatrix(p, k);
            }
            matrix.SetElemetMatrix(h, k, result);
            result = 0;
        }
    }
    

    t_finish = clock();

    printf("Time: %.4f\n", (double)(t_finish - t_start) / CLOCKS_PER_SEC);

}


void multiplication_mpi(int argc, char* argv[]) {
    int my_rank, rank_size;
    clock_t t_start, t_finish;

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
    MatrixHandler tmp;
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
    MPI_Scatter(reinterpret_cast<char*>(mh), sizeof(MatrixHandler), MPI_CHAR,
                reinterpret_cast<char*>(&tmp), sizeof(MatrixHandler), MPI_CHAR, 0, MPI_COMM_WORLD);


    Matrix<int>tm = Matrix<int>(m1.getCountColums(), m2.getCountRows());
    for (auto h = tmp.getStart(); h < tmp.getFinish(); h++) {
        for (auto k = 0; k < m2.getCountRows(); k++) {
            int result = 0;
            for (auto p = 0; p < m2.getCountColums(); p++) {
                result += m1.GetElementMatrix(h, p) * m2.GetElementMatrix(p, k);
            }
            tm.SetElemetMatrix(h, k, result);
        }
    }
    tmp.setMatrix(tm);


    Matrix<int>* rcv_mh = NULL;
    if (!my_rank)
        rcv_mh = new Matrix<int>[rank_size];
        
    MPI_Gather(reinterpret_cast<char*>(&tm), sizeof(Matrix<int>), MPI_CHAR,
               reinterpret_cast<char*>(rcv_mh), sizeof(Matrix<int>), MPI_CHAR,0,MPI_COMM_WORLD);

    //rcv_mh->PrintMatrix();

    if (!my_rank) {
        t_finish = clock();
        printf("Time: %.4f\n", (double)(t_finish - t_start) / CLOCKS_PER_SEC);
    }

    MPI_Finalize();
}



int main(int argc, char* argv[]) {

    //multiplication_thread(4);

    multiplication_omp(4);

    //mpiexec -n 4 D:\VsProject\MPI_lib_test\Debug\MPI_lib_test.exe
    //mpiexec -n 4 D:\work\project\MPI_lib_test\nemliyartym\MPI_lib_test\Debug\MPI_lib_test.exe
    //multiplication_mpi(argc, argv);

}






