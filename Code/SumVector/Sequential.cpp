//#include<iostream>
//#include<omp.h>
//#include<cstdlib>
//
//using namespace std;
//
//int main() {
//	int size = 99999;
//	int data1[99999];
//	int data2[99999];
//
//	for (int i = 0; i < size; i++) {
//		data1[i] = std::rand() % 100; // Sinh số ngẫu nhiên từ 0 đến 99
//	}
//
//	for (int i = 0; i < size; i++) {
//		data2[i] = std::rand() % 100; // Sinh số ngẫu nhiên từ 0 đến 99
//	}
//
//	double start = omp_get_wtime();
//	for (int i = 0; i < size; i++) {
//		data1[i] += data2[i];
//	}
//	double end = omp_get_wtime();
//	cout << "Thời gian thực thi: " << (end - start) * 1000 << "ms" << endl;
//	for (int i = 0; i < size; i++) {
//		cout << data1[i] << " ";
//	}
//}