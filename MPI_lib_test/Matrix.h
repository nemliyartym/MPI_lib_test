#pragma once

#include <iostream>

template<typename T> 
class Matrix {

public:
    Matrix() {
        n = m = 0;
        M = nullptr;
    }

    Matrix(int rows, int colums) : 
        m(rows), n(colums) {
        M = new T * [m];
        for (auto i = 0; i < m; i++)
            M[i] = static_cast<T*>(new T[n]);

        for (auto i = 0; i < m; i++)
            for (auto j = 0; j < n; j++)
                M[i][j] = 0;
    }

    Matrix(const Matrix& cM) {
        m = cM.m;
        n = cM.n;
        M = new T * [m];

        for (auto i = 0; i < m; i++)
            M[i] = static_cast<T*>(new T[n]);

        for (auto i = 0; i < m; i++)
            for (auto j = 0; j < n; j++)
                M[i][j] = cM.M[i][j];
    }

    T GetElementMatrix(int i, int j) {
        if ((m > 0 && n > 0) && (i < m && j < n))
            return M[i][j];
        else return 0;
    }

    void SetElemetMatrix(int i, int j, T value) {
        if ((m > 0 && n > 0) && (i < m && j < n))
            M[i][j] = value;
    }

    void PrintMatrxi() {
        std::cout << "Matrix:" << std::endl;
        for (auto i = 0; i < m; i++) {
            for (auto j = 0; j < n; j++)
                std::cout << M[i][j] << "\t";
            std::cout << std::endl;
        }
        std::cout << "End" << std::endl;
    }

    Matrix operator=(const Matrix& cM) {

        if (n > 0)
            for (auto i = 0; i < m; i++)
                delete[] M[i];
        if (m > 0)
            delete[] M;

        m = cM.m;
        n = cM.n;
        M = new T * [m];

        for (auto i = 0; i < m; i++)
            M[i] = static_cast<T*>(new T[n]);
          
        for (auto i = 0; i < m; i++)
            for (auto j = 0; j < n; j++)
                M[i][j] = cM.M[i][j];

        return *this;
    }

    ~Matrix() {
        if (n > 0)
            for (auto i = 0; i < m; i++)
                delete[] M[i];
        if (m > 0)
            delete[] M;
    }

    int getCountColums() {
        return m;
    }
    int getCountRows() {
        return n;
    }

private:
    T** M;  //матрица
    int m; //количество строк
    int n; //количество стоблцов


};
