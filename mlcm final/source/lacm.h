#pragma once
#include <SFML/Graphics.hpp>
#include "bccm.h"

void tlsm();

class vector {
public:
    double* V;
    uint32_t size;
public:
    vector() : vector(0) { }
    vector(uint32_t size_, double x = 0);
    vector(uint32_t size_, double* V_);
    ~vector();
    vector(const vector& S);
    vector& operator=(const vector& B);

    double getTotal();
    double length2() const;
    void transferD(vector* B, uint32_t start, bool out_in, bool copy_add);
    bool transferDF(const std::string& path, bool out_in);
    void transferDI(sf::Image* img, uint32_t start, bool out_in, sf::Vector2u s = sf::Vector2u());
    void transferDI3(sf::Image* img, uint32_t start, bool out_in, sf::Vector2u s = sf::Vector2u());
    void transferDIF(const std::string& path, uint32_t start, bool out_in, sf::Vector2u s = sf::Vector2u());
    void transferDIF3(const std::string& path, uint32_t start, bool out_in, sf::Vector2u s = sf::Vector2u());

    vector& operator>>=(uint32_t size_);
    vector operator>>(uint32_t size_);
    vector operator|(const vector& B);

    void fill(double x = 0);
    void fillH();
    void print();
    void printASCIIART(int H, int W, int D = 1);

    vector& operator*=(double b);
    vector& operator/=(double b);
    vector operator*(double b) const;
    friend vector operator*(double a, const vector& B);
    vector operator/(double b) const;
    vector operator-() const;

    vector& operator+=(const vector& B);
    vector operator+(const vector& B) const;
    vector& operator-=(const vector& B);
    vector operator-(const vector& B) const;
    vector operator+(double b) const;
    vector operator-(double b) const;
    vector& operator+=(double b);
    vector& operator-=(double b);
    double& operator() (uint32_t i = 0);
};

vector hadam(const vector A, const vector B);
vector hadamDiv(const vector A, const vector B);
void multMatVec(const vector* M, const vector* I, vector* O);
void multVecTvec(const vector* M, const vector* I, vector* O);
void multTvecMat(const vector* M, const vector* I, vector* O);
double dot(const vector A, const vector B);

void add(const vector* A, const vector* B, vector* C);
void addTensVec(const vector* T, const vector* V, vector* O, int sx, int sy, int sz);

void relu(const vector* A, vector* B);
vector relu_(const vector* A);
void FConvolution(const vector* K, const vector* I, vector* O, int siy, int six, int siz, int sky, int skx, int sty, int stx, int by, int bx, int sox, int soy, int sow);
void FConvolution_(const vector* K, vector* dK, const vector* I, vector* dI, const vector* dO, int siy, int six, int siz, int sky, int skx, int sty, int stx, int by, int bx, int sox, int soy, int sow);
void pool(const vector* I, vector* O, int six, int siy, int sox, int soy, int scy, int scx, int sz);
void pool_(const vector* I, vector* dI, const vector* dO, int six, int siy, int sox, int soy, int scy, int scx, int sz);
void stretch(const vector* I, vector* O, int six, int siy, int sox, int soy, int scy, int scx, int sz);
void stretch_(vector* dI, const vector* dO, int six, int siy, int sox, int soy, int scy, int scx, int sz);
void sigma(const vector* I, vector* O);
void sigma_(const vector* I, vector* dI, const vector* dO);
void tanh(const vector* I, vector* O);
void tanh_(const vector* I, vector* dI, const vector* dO);

vector hot(int n, int k);
int hot_(vector v);

void concat(const vector* A, const vector* B, vector* C);