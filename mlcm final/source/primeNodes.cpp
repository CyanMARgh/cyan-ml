#pragma once
#include "nodes.h"

//default nodes
reluNode::reluNode(nodeT1* inNode, int size) : nodeT2(inNode, size, size) {
    if (sizeVal != inNode->sizeVal) {
        tlsm();
    }
}
void reluNode::forward() {
    relu(arg, val);
}
void reluNode::backward() {
    *dArg += hadam(*dVal, relu_(arg));
}
sigmaNode::sigmaNode(nodeT1* inNode, int size) : nodeT2(inNode, size, size) { }
void sigmaNode::forward() {
    sigma(arg, val);
}
void sigmaNode::backward() {
    vector dArg_(dArg->size);
    sigma_(arg, &dArg_, dVal);
    *dArg += dArg_;
}
tanhNode::tanhNode(nodeT1* inNode, int size) : nodeT2(inNode, size, size) { }
void tanhNode::forward() {
    tanh(arg, val);
}
void tanhNode::backward() {
    vector dArg_(dArg->size);
    tanh_(arg, &dArg_, dVal);
    *dArg += dArg_;
}
addNode::addNode(nodeT1* inNode1, nodeT1* inNode2, int size) : nodeT3(inNode1, inNode2, size, size, size) { }
addNode::addNode(nodeT1* inNode, int size) : nodeT3(inNode, size, size, size) { }
void addNode::forward() {
    add(arg, par, val);
}
void addNode::backward() {
    *dArg += *dVal;
    *dPar += *dVal;
}
matMultNode::matMultNode(nodeT1* inNode, int sizeI, int sizeO) : nodeT3(inNode, sizeI, sizeI* sizeO, sizeO) {
    scale = 1. / sizeI;
}
void matMultNode::forward() {
    multMatVec(par, arg, val);
}
void matMultNode::backward() {
    vector ddZ(sizeArg);
    vector ddW(sizePar);
    multTvecMat(dVal, par, &ddZ);
    multVecTvec(dVal, arg, &ddW);
    *dArg += ddZ;
    *dPar += ddW;
}

//loss nodes
lossNode::lossNode(nodeT1* inNode1, nodeT1* inNode2, int size) : nodeT3(inNode1, inNode2, size, size, 1) { }
double lossNode::getErr() {
    return (*val)();
}
void lossNode::setGrad(double grad) {
    (*dVal)() = grad;
}
lossL2::lossL2(nodeT1* inNode1, nodeT1* inNode2, int size) : lossNode(inNode1, inNode2, size) { }
void lossL2::forward() {
    static vector d;
    d = *par - *arg;
    (*val)() = d.length2();
}
void lossL2::backward() {
    vector d = 2 * (*par - *arg) * (*dVal)();
    *dPar += d;
    *dArg -= d;
}
logAbsLoss::logAbsLoss(nodeT1* inNode1, nodeT1* inNode2) : lossNode(inNode1, inNode2, 1) { }
void logAbsLoss::forward() {
    double i = (*arg)();
    double r = (*par)();
    (*val)() = -log(abs(r - i));
}
void logAbsLoss::backward() {
    double i = (*arg)();
    double r = (*par)();
    double dO = (*dVal)();
    double dr = dO / (i - r);
    (*dPar)() += dr;
    (*dArg)() -= dr;
}
idLoss::idLoss(nodeT1* inNode1, nodeT1* inNode2, int size) : lossNode(inNode1, inNode2, size) { }
void idLoss::forward() {
    *val = *arg;
}
void idLoss::backward() {
    *dArg = *dVal;
}

//tensor nodes
tensBiasSquare::tensBiasSquare(nodeT1* inNode, int S_, int z_) : nodeT3(inNode, S_* S_* z_, z_, S_* S_* z_) {
    S = S_; z = z_;
}
void tensBiasSquare::forward() {
    addTensVec(arg, par, val, S, S, z);
}
void tensBiasSquare::backward() {
    vector dPar_(dPar->size);
    totalByRows(dVal->V, dPar_.V, S * S, z);
    *dPar += dPar_;
    *dArg += *dVal;
}
kernelSquare::kernelSquare(nodeT1* inNode, int s_, int sk_, int so_, int z_, int w_, int b_, int st_) : nodeT3(inNode, s_* s_* z_, sk_* sk_* z_* w_, so_* so_* w_) {
    s = s_; sk = sk_; so = so_; z = z_; w = w_; b = b_; st = st_;
    scale = 1. / (1. * sk * sk * z);
}
void kernelSquare::forward() {
    FConvolution(par, arg, val, s, s, z, sk, sk, st, st, b, b, so, so, w);
}
void kernelSquare::backward() {
    vector dArg_(dArg->size), dPar_(dPar->size);
    FConvolution_(par, &dPar_, arg, &dArg_, dVal, s, s, z, sk, sk, st, st, b, b, so, so, w);
    *dPar += dPar_; *dArg += dArg_;
}
deconvSquare::deconvSquare(nodeT1* inNode, int s, int z, int so, int w) : kernelSquare(inNode, s, so - s + 1, so, z, w, so - s) { }
poolSquare::poolSquare(nodeT1* inNode, int si_, int so_, int sc_, int sz_) : nodeT2(inNode, si_* si_* sz_, so_* so_* sz_) {
    si = si_; so = so_;
    sc = sc_; sz = sz_;
}
void poolSquare::forward() {
    pool(arg, val, si, si, so, so, sc, sc, sz);
}
void poolSquare::backward() {
    vector dArg_(dArg->size);
    pool_(arg, &dArg_, dVal, si, si, so, so, sc, sc, sz);
    *dArg += dArg_;
}
stretchNodeSquare::stretchNodeSquare(nodeT1* inNode, int s_, int z_, int S_) : nodeT2(inNode, s_* s_* z_, S_* S_* z_) {
    s = s_; S = S_; z = z_;
}
void stretchNodeSquare::forward() {
    stretch(arg, val, s, s, S, S, S / s, S / s, z);
}
void stretchNodeSquare::backward() {
    vector dArg_(dArg->size);
    stretch_(&dArg_, dVal, s, s, S, S, S / s, S / s, z);
    *dArg += dArg_;
}

