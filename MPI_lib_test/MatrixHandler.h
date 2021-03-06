#pragma once

#include "Matrix.h"
#include "windows.h"
#include <iostream>

using namespace std;

class MatrixHandler {

public:
	MatrixHandler() {
		mut = CreateMutex(NULL, FALSE, NULL);
		if (!mut)
			cout << "Mutex was no't create!" << endl;
	}

	MatrixHandler(Matrix<int> matrix1, Matrix<int> matrix2) : m1(matrix1), m2(matrix2) {
		MatrixHandler();
	}
	~MatrixHandler() {
		CloseHandle(mut);
	}

	static DWORD __stdcall static_proxy(LPVOID lpParam) {
		return static_cast<MatrixHandler*>(lpParam)->ThreadFunc();
	}

	Matrix<int> MatrixMultiplication() {

		if (m1.getCountRows() != m2.getCountColums()) 
			return Matrix<int>(0,0);
	
		Matrix<int> matrix(m1.getCountColums(), m2.getCountRows());

		for (auto h = start; h < finish; h++) {
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

	void setRange(const int value1,const int value2) {
		if (value1 < value2) {
			start = value1;
			finish = value2;
		}
		else {
			start = value2;
			finish = value1;
		}
	}

	void setMatrices(Matrix<int> matrix1, Matrix<int> matrix2) {
		m1 = matrix1;
		m2 = matrix2;
	}

private:
	Matrix<int>m1;
	Matrix<int>m2;
	HANDLE mut;
	int start = 0, finish = 0;

	DWORD ThreadFunc() {

		DWORD waitResult;
		if (waitResult = WaitForSingleObject(mut, INFINITE) == WAIT_OBJECT_0) {
			//std::cout << "it's work  start=" << start<< " finish=" << finish << std::endl;
			MatrixMultiplication();
			ReleaseMutex(mut);
		}
		return 0;
	}

};

