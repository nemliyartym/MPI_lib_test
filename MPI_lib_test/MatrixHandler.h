#pragma once

#include "Matrix.h"
#include "windows.h"
#include <iostream>

using namespace std;

class MatrixHandler {

public:

	MatrixHandler(Matrix<int> matrix1, Matrix<int> matrix2) : m1(matrix1), m2(matrix2) {
		InitializeCriticalSection(&cs);
	}
	~MatrixHandler() {
		DeleteCriticalSection(&cs);
	}

	static DWORD __stdcall static_proxy(LPVOID lpParam) {
		return static_cast<MatrixHandler*>(lpParam)->ThreadFunc();
	}

	Matrix<int> MatrixMultiplication_thread(const int count_p) {

		if (m1.getCountRows() != m2.getCountColums()) 
			return Matrix<int>(0,0);
	
		Matrix<int> matrix(m1.getCountColums(), m2.getCountRows());

		for (auto h = 0; h < m1.getCountColums(); h++) {
			for (auto k = 0; k < m2.getCountRows(); k++) {
				int result = 0;
				for (auto p = 0; p < m2.getCountColums(); p++) {
					result += m1.GetElementMatrix(h, p) * m2.GetElementMatrix(p, k);
				}
				matrix.SetElemetMatrix(h,k,result);
				result = 0;
			}
		}
		return matrix;
	}


private:
	Matrix<int>m1;
	Matrix<int>m2;
	CRITICAL_SECTION cs;

	DWORD ThreadFunc() {
		//EnterCriticalSection(&cs);
		//LeaveCriticalSection(&cs);
		std::cout << "it's work" << std::endl;
		return 0;
	}

};

