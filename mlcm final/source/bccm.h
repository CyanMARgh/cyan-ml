#pragma once
#include <fstream>

unsigned int hash(unsigned int a);
unsigned int hash(unsigned int a, unsigned int b);
unsigned int hash(unsigned int a, unsigned int b, unsigned int c);
double hashd(unsigned int a);

void multMat(double* Il, int sm, double* Ir, int si, double* O, int so);
void transpose(double* I, int si, int so, double* O);

void fillArr(double* O, int so, double x);
void fillArrHash(double* O, int so);
void copyArr(double* I, double* O, int so);
void sumArr(double* I1, double* I2, double* O, int so);
void sumArrScalar(double* I1, double I2, double* O, int so);
void sumMatVec(double* Im, double* Iv, double* O, int soy, int sox);
double totalByArr(double* I, int si);
void totalByRows(double* Im, double* O, int six, int siy);
void hadam(double* I1, double* I2, double* O, int so);
void hadamDiv(double* I1, double* I2, double* O, int so);
void multArrScalar(double* I, double* O, int so, double x);

void reluArr(double* I, double* O, int so);
void relu_Arr(double* I, double* O, int so);
void reluPool(double* I, double* O, int siy, int six, int soy, int sox, int scy, int scx, int sz = 1);
void relu_Pool(double* I, double* dI, double* dO, int siy, int six, int soy, int sox, int scy, int scx, int sz = 1);
void stretch(double* I, double* O, int siy, int six, int soy, int sox, int scy, int scx, int sz);
void stretch_(double* dI, double* dO, int siy, int six, int soy, int sox, int scy, int scx, int sz);
void sigma(double* I, double* O, int s);
void sigma_(double* I, double* dI, double* dO, int s);
void tanh(double* I, double* O, int s);
void tanh_(double* I, double* dI, double* dO, int s);


void kConvolution(double* I, double* K, double* O, int six, int skx, int stx, int bx, int sox);
void kConvolution_(double* I, double* dI, double* K, double* dK, double* dO, int six, int skx, int stx, int bx, int sox);
void fConvolution(double* I, double* K, double* O, int siy, int six, int sky, int skx, int sty, int stx, int by, int bx, int sox, int soy);
void fConvolution_(double* I, double* dI, double* K, double* dK, double* dO, int siy, int six, int sky, int skx, int sty, int stx, int by, int bx, int soy, int sox);
void FConvolution(double* I, double* K, double* O, int siy, int six, int siz, int sky, int skx, int sty, int stx, int by, int bx, int sox, int soy, int sow);
void FConvolution_(double* I, double* dI, double* K, double* dK, double* dO, int siy, int six, int siz, int sky, int skx, int sty, int stx, int by, int bx, int sox, int soy, int sow);

void printArr(double* I, int s);
void printMat(double* I, int sy, int sx);

double fixC_D(unsigned char c);
unsigned char fixD_C(double d);
void convertUchar4Double(unsigned char* C, double* D, int size);
void convertDoubleUchar4(double* D, unsigned char* C, int size);
void convertUchar4Double3(unsigned char* C, double* D, int size);
void convertDouble3Uchar4(double* D, unsigned char* C, int size);

bool saveArr(std::string path, char* arr, uint64_t size);
bool loadArr(std::string path, char* arr, uint64_t size);
