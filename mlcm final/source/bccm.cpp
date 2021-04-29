#pragma once
#include <stdio.h>
#include <string.h>
#include <fstream>

unsigned int hash(unsigned int a) {
    a = (a + 0x7ed55d16) + (a << 12);
    a = (a ^ 0xc761c23c) ^ (a >> 19);
    a = (a + 0x165667b1) + (a << 5);
    a = (a + 0xd3a2646c) ^ (a << 9);
    a = (a + 0xfd7046c5) + (a << 3);
    a = (a ^ 0xb55a4f09) ^ (a >> 16);
    return a;
}
unsigned int hash(unsigned int a, unsigned int b) {
    return hash(hash(a) ^ b);
}
unsigned int hash(unsigned int a, unsigned int b, unsigned int c) {
    return hash(hash(a, b) ^ c);
}
double hashd(unsigned int a) {
    return (double)hash(a) / (unsigned int)0xffffffff;
}
double hashd(unsigned int a, unsigned int b) {
    return hashd(hash(a) + b);
}

void multMat(double* Il, int sm, double* Ir, int si, double* O, int so) {
    for (int o1 = 0; o1 < so; o1++) {
        for (int o2 = 0; o2 < si; o2++) {
            double sum = 0;
            for (int s = 0; s < sm; s++) {
                sum += Il[o1 * sm + s] * Ir[s * si + o2];
            }
            O[o1 * si + o2] = sum;
        }
    }
}
void transpose(double* I, int si, int so, double* O) {
    for (int o1 = 0; o1 < so; o1++) {
        for (int o2 = 0; o2 < si; o2++) {
            O[o2 * so + o1] = I[o1 * si + o2];
        }
    }
}

void fillArr(double* O, int so, double x) {
    for (int o = 0; o < so; o++) {
        O[o] = x;
    }
}
void fillArrHash(double* O, int so) {
    static unsigned int q = 0;
    int o_ = (int)(long long int)O;
    for (int o = 0; o < so; o++) {
        O[o] = hashd(o_ + o, q);
    }
    q++;
}
void copyArr(double* I, double* O, int so) {
    memcpy(O, I, so * sizeof(double));
}
void sumArr(double* I1, double* I2, double* O, int so) {
    for (int o = 0; o < so; o++) {
        O[o] = I1[o] + I2[o];
    }
}
void sumArrScalar(double* I1, double I2, double* O, int so) {
    for (int o = 0; o < so; o++) {
        O[o] = I1[o] + I2;
    }
}
void sumMatVec(double* Im, double* Iv, double* O, int soy, int sox) {
    for (int y = 0; y < soy; y++) {
        for (int x = 0; x < sox; x++) {
            O[y * sox + x] = Im[y * sox + x] + Iv[y];
        }
    }
}
double totalByArr(double* I, int si) {
    double sum = 0;
    for (int i = 0; i < si; i++) {
        sum += I[i];
    }
    return sum;
}
void totalByRows(double* Im, double* O, int six, int siy) {
    for (int y = 0; y < siy; y++) {
        O[y] = 0.;
        for (int x = 0; x < six; x++) {
            O[y] += Im[six * y + x];
        }
    }
}
void hadam(double* I1, double* I2, double* O, int so) {
    for (int o = 0; o < so; o++) {
        O[o] = I1[o] * I2[o];
    }
}
void hadamDiv(double* I1, double* I2, double* O, int so) {
    for (int o = 0; o < so; o++) {
        O[o] = I1[o] / I2[o];
    }
}
void multArrScalar(double* I, double* O, int so, double x) {
    for (int o = 0; o < so; o++) {
        O[o] = I[o] * x;
    }
}
const double reluA = 0.2;
//#define RELUTANH
void reluArr(double* I, double* O, int so) {
    for (int o = 0; o < so; o++) {
        double i = I[o];
#ifdef RELUTANH
        O[o] = i < -1 ? reluA * (i + 1) - 1 : i < 1 ? i : reluA * (i - 1) + 1;
#else
        O[o] = i < 0 ? reluA * i : 1. < i ? (i - 1) * reluA + 1 : i;
#endif
    }
}
void relu_Arr(double* I, double* O, int so) {
    for (int o = 0; o < so; o++) {
        double i = I[o];
#ifdef RELUTANH
        O[o] = i < -1 ? reluA : i < 1 ? 1 : reluA;
#else
        O[o] = O[o] = i < 0 ? reluA : 1. < i ? reluA : 1.;
#endif
    }
}
#define POOLSUM
void reluPool(double* I, double* O, int siy, int six, int soy, int sox, int scy, int scx, int sz) {
    for (int z$ = 0; z$ < sz; z$++) {
        for (int y$ = 0; y$ < soy; y$++) {
            for (int x$ = 0; x$ < sox; x$++) {
                double m = -1e20;
                int my_ = -1, mx_ = -1;
#ifndef POOLSUM
                for (int dy = 0; dy < scy; dy++) {
                    int y_ = y$ * scy + dy;
                    if (y_ >= siy)break;
                    for (int dx = 0; dx < scx; dx++) {
                        int x_ = x$ * scx + dx;
                        if (x_ >= six)break;
                        double m_ = I[z$ * six * siy + y_ * six + x_];
                        if (m_ > m) {
                            m = m_;
                            my_ = y_;
                            mx_ = x_;
                        }
                    }
                }
                O[z$ * sox * soy + y$ * sox + x$] = m;
#else
                O[z$ * sox * soy + y$ * sox + x$] = 0.;
                for (int dy = 0; dy < scy; dy++) {
                    int y_ = y$ * scy + dy;
                    if (y_ >= siy)break;
                    for (int dx = 0; dx < scx; dx++) {
                        int x_ = x$ * scx + dx;
                        if (x_ >= six)break;
                        O[z$ * sox * soy + y$ * sox + x$] += I[z$ * six * siy + y_ * six + x_];
                    }
                }
#endif
            }
        }
    }
}
void relu_Pool(double* I, double* dI, double* dO, int siy, int six, int soy, int sox, int scy, int scx, int sz) {
    for (int z$ = 0; z$ < sz; z$++) {
        for (int y$ = 0; y$ < soy; y$++) {
            for (int x$ = 0; x$ < sox; x$++) {
#ifndef POOLSUM
                double m = -1e20;
                int my_ = -1, mx_ = -1;
                for (int dy = 0; dy < scy; dy++) {
                    int y_ = y$ * scy + dy;
                    if (y_ >= siy)break;
                    for (int dx = 0; dx < scx; dx++) {
                        int x_ = x$ * scx + dx;
                        if (x_ >= six)break;
                        double m_ = I[z$ * six * siy + y_ * six + x_];
                        if (m_ > m) {
                            m = m_;
                            my_ = y_;
                            mx_ = x_;
                        }
                    }
                }
                for (int dy = 0; dy < scy; dy++) {
                    int y_ = y$ * scy + dy;
                    if (y_ >= siy)break;
                    for (int dx = 0; dx < scx; dx++) {
                        int x_ = x$ * scx + dx;
                        if (x_ >= six)break;
                        dI[z$ * six * siy + y_ * six + x_] = (mx_ == x_ && my_ == y_) ? dO[z$ * sox * soy + y$ * sox + x$] : 0.;
                    }
                }
#else
                for (int dy = 0; dy < scy; dy++) {
                    int y_ = y$ * scy + dy;
                    if (y_ >= siy)break;
                    for (int dx = 0; dx < scx; dx++) {
                        int x_ = x$ * scx + dx;
                        if (x_ >= six)break;
                        dI[z$ * six * siy + y_ * six + x_] = dO[z$ * sox * soy + y$ * sox + x$];
                    }
                }
#endif
            }
        }
    }
}
void stretch(double* I, double* O, int siy, int six, int soy, int sox, int scy, int scx, int sz) {
    int ii, io, ik;
    for (int z = 0; z < sz; z++) {
        for (int y = 0; y < soy; y++) {
            for (int x = 0; x < sox; x++) {
                ii = (x / scx) + (y / scx) * six + z * siy * six;
                io = x + y * sox + z * soy * sox;
                O[io] = I[ii];
            }
        }
    }
}
void stretch_(double* dI, double* dO, int siy, int six, int soy, int sox, int scy, int scx, int sz) {
    int ii, io, ik;
    for (int z = 0; z < sz; z++) {
        for (int y = 0; y < soy; y++) {
            for (int x = 0; x < sox; x++) {
                ii = (x / scx) + (y / scx) * six + z * siy * six;
                io = x + y * sox + z * soy * sox;
                dI[ii] += dO[io];
            }
        }
    }
}
void sigma(double* I, double* O, int s) {
    for (int i = 0; i < s; i++) {
        O[i] = 1. / (1. + exp(-I[i]));
    }
}
void sigma_(double* I, double* dI, double* dO, int s) {
    double ei;
    for (int i = 0; i < s; i++) {
        ei = exp(-I[i]);
        dI[i] = ei / ((1 + ei) * (1 + ei)) * dO[i];
    }
}
void tanh(double* I, double* O, int s) {
    for (int i = 0; i < s; i++) {
        O[i] = 2. / (1. + exp(-I[i])) - 1.;
    }
}
void tanh_(double* I, double* dI, double* dO, int s) {
    double ei;
    for (int i = 0; i < s; i++) {
        ei = exp(-I[i]);
        dI[i] = 2. * ei / ((1 + ei) * (1 + ei)) * dO[i];
    }
}

void kConvolution(double* I, double* K, double* O, int six, int skx, int stx, int bx, int sox) {
    for (int x$ = 0, x0 = -bx; x$ < sox; x$++, x0 += stx) {
        for (int dx = 0, x_ = x0; dx < skx; dx++, x_++) {
            if (x_ >= 0 && x_ < six) {
                O[x$] += I[x_] * K[dx];
            }
        }
    }
}
void kConvolution_(double* I, double* dI, double* K, double* dK, double* dO, int six, int skx, int stx, int bx, int sox) {
    for (int x$ = 0, x0 = -bx; x$ < sox; x$++, x0 += stx) {
        double dO$ = dO[x$];
        for (int dx = 0, x_ = x0; dx < skx; dx++, x_++) {
            if (x_ >= 0 && x_ < six) {
                dI[x_] += dO$ * K[dx];
                dK[dx] += dO$ * I[x_];
            }
        }
    }
}
void fConvolution(double* I, double* K, double* O, int siy, int six, int sky, int skx, int sty, int stx, int by, int bx, int sox, int soy) {
    for (int y$ = 0; y$ < soy; y$++) {
        for (int x$ = 0; x$ < sox; x$++) {
            for (int dy = 0; dy < sky; dy++) {
                for (int dx = 0; dx < skx; dx++) {
                    int y_ = y$ * sty - by + dy; y_ = y_ < 0 ? 0 : y_ >= siy ? siy - 1 : y_;
                    int x_ = x$ * stx - bx + dx; x_ = x_ < 0 ? 0 : x_ >= six ? six - 1 : x_;
                    O[x$ + sox * y$] += I[x_ + six * y_] * K[dx + skx * dy];
                }
            }
        }
    }
}
void fConvolution_(double* I, double* dI, double* K, double* dK, double* dO, int siy, int six, int sky, int skx, int sty, int stx, int by, int bx, int soy, int sox) {
    for (int y$ = 0; y$ < soy; y$++) {
        for (int x$ = 0; x$ < sox; x$++) {
            double dO$ = dO[x$ + sox * y$];
            for (int dy = 0; dy < sky; dy++) {
                for (int dx = 0; dx < skx; dx++) {
                    int y_ = y$ * sty - by + dy; y_ = y_ < 0 ? 0 : y_ >= siy ? siy - 1 : y_;
                    int x_ = x$ * stx - bx + dx; x_ = x_ < 0 ? 0 : x_ >= six ? six - 1 : x_;
                    dI[x_ + six * y_] += dO$ * K[dx + skx * dy];
                    dK[dx + skx * dy] += dO$ * I[x_ + six * y_];
                }
            }
        }
    }
}
void FConvolution(double* I, double* K, double* O, int siy, int six, int siz, int sky, int skx, int sty, int stx, int by, int bx, int sox, int soy, int sow) {
    double* I_, * K_, * O_;
    for (int w$ = 0; w$ < sow; w$++) {
        for (int z_ = 0; z_ < siz; z_++) {
            I_ = I + siy * six * z_;
            K_ = K + siz * sky * skx * w$ + sky * skx * z_;
            O_ = O + soy * sox * w$;
            fConvolution(I_, K_, O_, siy, six, sky, skx, sty, stx, by, bx, sox, soy);
        }
    }
}
void FConvolution_(double* I, double* dI, double* K, double* dK, double* dO, int siy, int six, int siz, int sky, int skx, int sty, int stx, int by, int bx, int sox, int soy, int sow) {
    double* I_, * dI_, * K_, * dK_, * dO_;
    for (int w$ = 0; w$ < sow; w$++) {
        for (int z_ = 0; z_ < siz; z_++) {
            I_ = I + siy * six * z_;
            dI_ = dI + siy * six * z_;
            K_ = K + siz * sky * skx * w$ + sky * skx * z_;
            dK_ = dK + siz * sky * skx * w$ + sky * skx * z_;
            dO_ = dO + soy * sox * w$;
            fConvolution_(I_, dI_, K_, dK_, dO_, siy, six, sky, skx, sty, stx, by, bx, sox, soy);
        }
    }
}

double fixC_D(unsigned char c) {
    return (int)c / 255.;
}
unsigned char fixD_C(double d) {
    int p = (int)(d * 255);
    p = p > 255 ? 255 : p < 0 ? 0 : p;
    return (unsigned char)p;
}
void convertUchar4Double(unsigned char* C, double* D, int size) {
    for (int i = 0; i < size; i++) {
        D[i] = fixC_D(C[i * 4]);
    }
}
void convertDoubleUchar4(double* D, unsigned char* C, int size) {
    for (int i = 0; i < size; i++) {
        unsigned char p = fixD_C(D[i]);
        C[i * 4] = p;
        C[i * 4 + 1] = p;
        C[i * 4 + 2] = p;
        C[i * 4 + 3] = 255;
    }
}
void convertUchar4Double3(unsigned char* C, double* D, int size) {
    for (int i = 0; i < size; i++) {
        D[i] = fixC_D(C[i * 4]);
        D[size + i] = fixC_D(C[i * 4 + 1]);
        D[size * 2 + i] = fixC_D(C[i * 4 + 2]);
    }
}
void convertDouble3Uchar4(double* D, unsigned char* C, int size) {
    for (int i = 0; i < size; i++) {
        C[i * 4] = fixD_C(D[i]);
        C[i * 4 + 1] = fixD_C(D[size + i]);
        C[i * 4 + 2] = fixD_C(D[size * 2 + i]);
        C[i * 4 + 3] = 255;
    }
}

void printArr(double* V, int s) {
    for (int i = 0; i < s; i++) {
        printf("%f", V[i]);
        printf(i != s - 1 ? ", " : "");
    }
}
void printMat(double* M, int sy, int sx) {
    for (int y = 0; y < sy; y++) {
        for (int x = 0; x < sx; x++) {
            printf("%f", M[x + y * sx]);
            printf((x != sx - 1) ? ", " : (y != sy - 1) ? "\n" : "");
        }
    }
}
bool saveArr(std::string path, char* arr, uint64_t size) {
    std::ofstream out(path, std::ios::out | std::ios::binary);
    out.write(arr, size);
    out.close();
    return true;
}
bool loadArr(std::string path, char* arr, uint64_t size) {
    std::ifstream in(path, std::ios::in | std::ios::binary);
    bool l = !!in;
    if (l) {
        in.read(arr, size);
    }
    in.close();
    return l;
}
