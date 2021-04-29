#pragma once
#include "lacm.h"

void tlsm() {
    throw std::length_error("layer sizes mismatch\n");
}

vector::vector(uint32_t size_, double x) {
    size = size_;
    V = new double[size];
    fill(x);
}
vector::vector(uint32_t size_, double* V_) {
    size = size_;
    V = V_;
}
vector::vector(const vector& S) {
    size = S.size;
    V = new double[size];
    copyArr(S.V, V, size);
}
vector::~vector() {
    delete[] V;
}
vector& vector::operator=(const vector& B) {
    if (this == &B) {
        return *this;
    }
    if (size != B.size) {
        size = B.size;
        delete[] V;
        V = new double[size];
    }
    copyArr(B.V, V, size);
    return *this;
}

double vector::getTotal() {
    return totalByArr(V, size);
}
double vector::length2() const {
    return dot(*this, *this);
}
void vector::transferD(vector* B, uint32_t start, bool out_in, bool copy_add) {
    if (B->size < size + start) {
        tlsm();
    }
    else {
        double* V_ = B->V + start;
        if (copy_add) {
            copyArr(out_in ? V : V_, out_in ? V_ : V, size);
        }
        else {
            sumArr(V, V_, out_in ? V_ : V, size);
        }
    }
}
bool vector::transferDF(const std::string& path, bool out_in) {
    if (out_in) {
        saveArr(path, (char*)V, (uint64_t)sizeof(double) * size);
        return true;
    } else {
        return loadArr(path, (char*)V, (uint64_t)sizeof(double) * size);
    }
}
void vector::transferDI(sf::Image* img, uint32_t start, bool out_in, sf::Vector2u s) {
    if (!out_in) {
        s = img->getSize();
    }
    uint32_t size_ = s.x * s.y;
    if (size_ + start > size) {
        tlsm();
    } else {
        if (out_in) {
            img->create(s.x, s.y);
            auto C = (unsigned char*)img->getPixelsPtr();
            convertDoubleUchar4(V + start, C, size_);
        } else {
            auto C = (unsigned char*)img->getPixelsPtr();
            convertUchar4Double(C, V + start, size_);
        }
    }
}
void vector::transferDI3(sf::Image* img, uint32_t start, bool out_in, sf::Vector2u s) {
    if (!out_in) {
        s = img->getSize();
    }
    uint32_t size_ = s.x * s.y;
    if (size_ * 3 + start > size) {
        tlsm();
    } else {
        if (out_in) {
            img->create(s.x, s.y);
            auto C = (unsigned char*)img->getPixelsPtr();
            convertDouble3Uchar4(V + start, C, size_);
        } else {
            auto C = (unsigned char*)img->getPixelsPtr();
            convertUchar4Double3(C, V + start, size_);
        }
    }
}
void vector::transferDIF(const std::string& path, uint32_t start, bool out_in, sf::Vector2u s) {
    sf::Image img;
    if (out_in) {
        transferDI(&img, 0, true, s);
        img.saveToFile(path);
    }
    else {
        img.loadFromFile(path);
        transferDI(&img, 0, false);
    }
}
void vector::transferDIF3(const std::string& path, uint32_t start, bool out_in, sf::Vector2u s) {
    sf::Image img;
    if (out_in) {
        transferDI3(&img, 0, true, s);
        img.saveToFile(path);
    } else {
        img.loadFromFile(path);
        transferDI3(&img, 0, false);
    }
}

vector& vector::operator>>=(uint32_t size_) {
    uint32_t size$ = size < size_ ? size : size_;
    double* V_ = new double[size_];
    copyArr(V, V_, size$);
    if (size_ > size) {
        fillArr(V_ + size, size_ - size, 0.);
    }
    delete V; V = V_; size = size_;
    return *this;
}
vector vector::operator>>(uint32_t size_) {
    uint16_t size$ = size < size_ ? size : size_;
    double* V_ = new double[size_];
    copyArr(V, V_, size$);
    if (size_ > size) {
        fillArr(V_ + size, size_ - size, 0.);
    }
    return vector(size_, V_);
}
vector vector::operator|(const vector& B) {
    vector C(size + B.size);
    concat(this, &B, &C);
    return C;
}

void vector::fill(double x) {
    fillArr(V, size, x);
}
void vector::fillH() {
    fillArrHash(V, size);
}
void vector::print() {
    printf("{");
    for (uint32_t i = 0; i < size; i++) {
        printf("%.3f", V[i]);
        printf(i + 1 == size ? "}\n" : ", ");
    }
}
void vector::printASCIIART(int H, int W, int D) {
    if (H * W * D != size) {
        throw std::length_error("layer sizes mismatch\n");
    }
    const int mlp = 13;
    char lp[mlp+1] = " .,-~:;=!*#$@";
    for (int z = 0; z < D; z++) {
        for (int y = 0; y < H; y++) {
            for (int x = 0; x < W; x++) {
                int p = (int)(mlp * V[H * W * z + W * y + x]);
                p = p >= mlp ? mlp - 1 : p < 0 ? 0 : p;
                char c = lp[p];
                printf("%c", c);
                if (x == W - 1)printf("\n");
            }
            if (y == H - 1 && z < D - 1) {
                for (int x = 0; x < W; x++) {
                    printf("=");
                }
                printf("\n");
            }
        }
    }
}

vector& vector::operator*=(double b) {
    multArrScalar(V, V, size, b);
    return *this;
}
vector& vector::operator/=(double b) {
    return *this *= (1. / b);
}
vector vector::operator*(double b) const {
    vector C(size);
    multArrScalar(V, C.V, size, b);
    return C;
}
vector operator*(double a, const vector& B) {
    return B * a;
}
vector vector::operator/(double b) const {
    return *this * (1. / b);
}
vector vector::operator-() const {
    return *this * -1.;
}

vector& vector::operator+=(const vector& B) {
    if (size != B.size) {
        tlsm();
    }
    sumArr(V, B.V, V, size);
    return *this;
}
vector vector::operator+(const vector& B) const {
    if (size != B.size) {
        tlsm();
    }
    vector C(size);
    sumArr(V, B.V, C.V, size);
    return C;
}
vector& vector::operator-=(const vector& B) {
    return *this += -B;
}
vector vector::operator-(const vector& B) const {
    return *this + -B;
}
vector vector::operator+(double b) const {
    int sa = size;
    vector C(size);
    sumArrScalar(V, b, C.V, size);
    return C;
}
vector vector::operator-(double b) const {
    return *this + -b;
}
vector& vector::operator+=(double b) {
    sumArrScalar(V, b, V, size);
    return *this;
}
vector& vector::operator-=(double b) {
    return *this += -b;
}
double& vector::operator() (uint32_t i) {
    if (i < 0 || i >= size) {
        tlsm();
    }
    return V[i];
}

vector hadam(const vector A, const vector B) {
    uint32_t sa = A.size, sb = B.size;
    if (sa != sb) {
        tlsm();
    }
    vector C(sa);
    hadam(A.V, B.V, C.V, sa);
    return C;
}
vector hadamDiv(const vector A, const vector B) {
    uint32_t sa = A.size, sb = B.size;
    if (sa != sb) {
        tlsm();
    }
    vector C(sa);
    hadamDiv(A.V, B.V, C.V, sa);
    return C;
}

void multMatVec(const vector* M, const vector* I, vector* O) {
    if (I->size * O->size != M->size) {
        tlsm();
    }
    multMat(M->V, I->size, I->V, 1, O->V, O->size);
}
void multVecTvec(const vector* M, const vector* I, vector* O) {
    if (M->size * I->size != O->size) {
        tlsm();
    }
    multMat(M->V, 1, I->V, I->size, O->V, M->size);
}
void multTvecMat(const vector* M, const vector* I, vector* O) {
    if (M->size * O->size != I->size) {
        tlsm();
    }
    multMat(M->V, M->size, I->V, O->size, O->V, 1);
}
double dot(const vector A, const vector B) {
    uint32_t sa = A.size, sb = B.size;
    if (sa != sb) {
        tlsm();
    }
    double c;

    multMat(A.V, sa, B.V, 1, &c, 1);

    return c;
}

void add(const vector* A, const vector* B, vector* C) {
    uint32_t sa = A->size, sb = B->size, sc = C->size;
    if (sa != sb || sb != sc) {
        tlsm();
    }
    sumArr(A->V, B->V, C->V, sa);
}
void addTensVec(const vector* T, const vector* V, vector* O, int sx, int sy, int sz) {
    if (T->size != sx * sy * sz || T->size != O->size || V->size != sz) {
        tlsm();
    }
    sumMatVec(T->V, V->V, O->V, sz, sx * sy);
}

void relu(const vector* A, vector* B) {
    uint32_t sa = A->size;
    uint32_t sb = B->size;
    if (sa != sb) {
        tlsm();
    }
    reluArr(A->V, B->V, sa);
}
vector relu_(const vector* A) {
    uint32_t sa = A->size;
    vector B(sa);
    relu_Arr(A->V, B.V, sa);
    return B;
}
void FConvolution(const vector* K, const vector* I, vector* O, int siy, int six, int siz, int sky, int skx, int sty, int stx, int by, int bx, int sox, int soy, int sow) {
    if (I->size != six * siy * siz || O->size != sox * soy * sow || K->size != skx * skx * siz * sow || soy * sty + sky - 1 != siy + 2 * by || sox * stx + skx - 1 != six + 2 * bx) {
        tlsm();
    }
    O->fill();
    FConvolution(I->V, K->V, O->V, siy, six, siz, sky, skx, sty, stx, by, bx, sox, soy, sow);
}
void FConvolution_(const vector* K, vector* dK, const vector* I, vector* dI, const vector* dO, int siy, int six, int siz, int sky, int skx, int sty, int stx, int by, int bx, int sox, int soy, int sow) {
    if (I->size != dI->size || K->size != dK->size || I->size != six * siy * siz || dO->size != sox * soy * sow || K->size != skx * skx * siz * sow || (soy * sty + sky - 1 != siy + 2 * by) || (sox * stx + skx - 1 != six + 2 * bx)) {
        tlsm();
    }
    dI->fill();
    dK->fill();
    FConvolution_(I->V, dI->V, K->V, dK->V, dO->V, siy, six, siz, sky, skx, sty, stx, by, bx, sox, soy, sow);
}
void pool(const vector* I, vector* O, int six, int siy, int sox, int soy, int scy, int scx, int sz) {
    if (I->size != six * siy * sz || O->size != sox * soy * sz || (six - 1) / scx != sox - 1 || (siy - 1) / scy != soy - 1) {
        tlsm();
    }
    reluPool(I->V, O->V, siy, six, soy, sox, scy, scx, sz);
}
void pool_(const vector* I, vector* dI, const vector* dO, int six, int siy, int sox, int soy, int scy, int scx, int sz) {
    if (I->size != six * siy * sz || dO->size != sox * soy * sz || I->size != dI->size || (six - 1) / scx != sox - 1 || (siy - 1) / scy != soy - 1) {
        tlsm();
    }
    relu_Pool(I->V, dI->V, dO->V, siy, six, soy, sox, scy, scx, sz);
}
void stretch(const vector* I, vector* O, int six, int siy, int sox, int soy, int scy, int scx, int sz) {
    if (I->size != six * siy * sz || O->size != sox * soy * sz || sox != six * scx || siy * scy != soy) {
        tlsm();
    }
    stretch(I->V, O->V, siy, six, soy, sox, scy, scx, sz);
}
void stretch_(vector* dI, const vector* dO, int six, int siy, int sox, int soy, int scy, int scx, int sz) {
    if (dI->size != six * siy * sz || dO->size != sox * soy * sz || sox != six * scx || siy * scy != soy) {
        tlsm();
    }
    dI->fill();
    stretch_(dI->V, dO->V, siy, six, soy, sox, scy, scx, sz);
}
void sigma(const vector* I, vector* O) {
    int s = I->size;
    if (s != O->size) tlsm();
    sigma(I->V, O->V, s);
}
void sigma_(const vector* I, vector* dI, const vector* dO) {
    int s = dI->size;
    if (s != dO->size || s!= I->size) tlsm();
    sigma_(I->V, dI->V, dO->V, s);
}
void tanh(const vector* I, vector* O) {
    int s = I->size;
    if (s != O->size) tlsm();
    tanh(I->V, O->V, s);
}
void tanh_(const vector* I, vector* dI, const vector* dO) {
    int s = dI->size;
    if (s != dO->size || s != I->size) tlsm();
    tanh_(I->V, dI->V, dO->V, s);
}


vector hot(int n, int k) {
    vector v(n);
    if (k >= 0 && k < n) v(k) = 1.;
    return v;
}
int hot_(vector v) {
    double vm = 0., v_;
    int k = -1;
    for (uint16_t i = 0; i < v.size; i++) {
        v_ = v(i);
        if (v_ > vm) {
            vm = v_;
            k = i;
        }
    }
    return k;
}

void concat(const vector* A, const vector* B, vector* C) {
    if (A->size + B->size != C->size) {
        tlsm();
    }
    copyArr(A->V, C->V, A->size);
    copyArr(B->V, C->V + A->size, B->size);
}