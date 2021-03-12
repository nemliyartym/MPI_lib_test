#pragma once

#include "Matrix.h"
#include "windows.h"
#include "time.h"
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

	MatrixHandler operator=(const MatrixHandler& mh) {
		MatrixHandler(mh.m1, mh.m2);
		setRange(mh.start, mh.finish);
		setMatrix(mh.matrix);
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
	
		matrix = Matrix<int>(m1.getCountColums(), m2.getCountRows());
		
		//cout << start << " " << finish << endl;
		for (auto h = start; h < finish; h++) {
			cout <<"start " << start <<  endl;
			for (auto k = 0; k < m2.getCountRows(); k++) {
				int result = 0;
				cout << m2.getCountColums()<< endl;
				for (auto p = 0; p < m2.getCountColums(); p++) {
					result += m1.GetElementMatrix(h, p) * m2.GetElementMatrix(p, k);
				}
				//cout << h << k << " " << endl;
				//cout << matrix.getCountColums() << endl;
				//cout << matrix.getCountRows() << endl;
				matrix.SetElemetMatrix(h,k,result);
				
				result = 0;
			}
			cout << "zbs" << endl;
		}

		return matrix;
	}

	void setMatrix(Matrix<int> m) {
		matrix = m;
	}

	Matrix<int> getMatrix() { 
		return matrix;
	}

	Matrix<int> getM1() {
		return m1;
	}

	Matrix<int> getM2() {
		return m2;
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

	const int getStart() {
		return start;
	}

	const int getFinish() {
		return finish;
	}

	void setMatrices(Matrix<int> matrix1, Matrix<int> matrix2) {
		m1 = matrix1;
		m2 = matrix2;
	}

	

private:
	Matrix<int>m1;
	Matrix<int>m2;
	Matrix<int>matrix;

	HANDLE mut;
	int start = 0, finish = 0;

	DWORD ThreadFunc() {

		DWORD waitResult;
		if (waitResult = WaitForSingleObject(mut, INFINITE) == WAIT_OBJECT_0) {
			MatrixMultiplication();
			ReleaseMutex(mut);
		}
		return 0;
	}

};

