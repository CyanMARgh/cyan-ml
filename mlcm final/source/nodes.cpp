#pragma once
#include "nodes.h"

nodeT1::nodeT1(int size) {
    sizeVal = size;
    val = new vector(sizeVal);
    dVal = new vector(sizeVal);
}
nodeT1::~nodeT1() {
    delete val;
    delete dVal;
}

void nodeT1::print() {
    val->print();
}
void nodeT1::transferD(vector* B, int start, bool out_in, bool delta_base, bool copy_add, bool A_W) {
    if (A_W) {
        (delta_base ? dVal : val)->transferD(B, start, out_in, copy_add);
    }
}
void nodeT1::transferDF(const std::string& path, bool out_in, bool A_W) {
    if (A_W) {
        vector B(sizeVal);
        if (out_in) {
            transferD(&B, 0, true, false, true, A_W);
            B.transferDF(path, true);
        }
        else {
            transferD(&B, 0, false, false, true, A_W);
            B.transferDF(path, false);
        }
    }
}
void nodeT1::clearGrad() {
    dVal->fill();
}
void nodeT1::fillH(bool A_W) {
    if (A_W) {
        val->fillH();
        *val = (*val - .5) * 2.;
    }
}

nodeT2::nodeT2(nodeT1* inNode, int inSize, int outSize) : nodeT1(outSize) {
    sizeArg = inSize;
    if (inNode) {
        connectIn1(inNode);
    }
}
void nodeT2::connectIn1(nodeT1* inNode) {
    if (sizeArg != inNode->sizeVal) tlsm();
    arg = inNode->val;
    dArg = inNode->dVal;
}
void nodeT2::transferDF(const std::string& path, bool out_in, bool A_W) {
    vector B(A_W ? sizeVal : sizePar);
    if (out_in) {
        transferD(&B, 0, true, false, true, A_W);
        B.transferDF(path, true);
    }
    else {
        if(B.transferDF(path, false)) transferD(&B, 0, false, false, true, A_W);
    }
}
void nodeT2::getParGrad(vector* B) {
    if (B->size != sizePar) tlsm();
    transferD(B, 0, true, true, false, false);
}
void nodeT2::applyParGrad(vector* B) {
    if (B->size != sizePar) tlsm();
    transferD(B, 0, false, false, false, false);
}

nodeT3::nodeT3(nodeT1* inNode, int inSize, int parSize, int outSize) : nodeT2(inNode, inSize, outSize) {
    sizePar = parSize;
    par = new vector(sizePar);
    dPar = new vector(sizePar);
    fillH(false);
    par_own = true;
}
nodeT3::nodeT3(nodeT1* inNode1, nodeT1* inNode2, int inSize, int parSize, int outSize) : nodeT2(inNode1, inSize, outSize) {
    sizePar = parSize;
    if (inNode2) {
        connectIn2(inNode2);
    } else {
        dPar = par = NULL;
    }
    par_own = false;
}
void nodeT3::connectIn2(nodeT1* inNode) { 
    if (sizePar != inNode->sizeVal) tlsm();
    par = inNode->val;
    dPar = inNode->dVal;
}
nodeT3::~nodeT3() {
    if (par_own) {
        delete par;
        delete dPar;
    }
}
void nodeT3::transferD(vector* B, int start, bool out_in, bool delta_base, bool copy_add, bool A_W) {
    (A_W ? (delta_base ? dVal : val) : (delta_base ? dPar : par))->transferD(B, start, out_in, copy_add);
}
void nodeT3::fillH(bool A_W) {
    vector* f = A_W ? val : par;
    f->fillH();
    double sc = A_W ? 1. : scale;
    *f = (*f - .5) * (2. * scale);
}
void nodeT3::clearGrad() {
    dVal->fill();
    if (par_own) dPar->fill();
}

nodeT4::nodeT4(nodeT1* inNode, int inSize, int outSize, int countP_) : nodeT2(inNode, inSize, 0), countP(countP_) {
    parts = new nodeT2 * [countP];
    sizeVal = outSize;
}
void nodeT4::postfix() {
    delete val; val = parts[countP - 1]->val;
    delete dVal; dVal = parts[countP - 1]->dVal;
    if (sizeVal != val->size) {
        tlsm();
    }
    par_own = true;
    sizePar = 0;
    for (int i = 0; i < countP; i++) {
        sizePar += (parts[i]->par_own) ? parts[i]->sizePar : 0;
    }
}
void nodeT4::connectIn1(nodeT1* inNode) {
    parts[0]->connectIn1(inNode);
    arg = parts[0]->arg;
    dArg = parts[0]->dArg;
}
nodeT4::~nodeT4() {
    val = NULL; dVal = NULL;
    for (int i = 0; i < countP; i++) {
        delete parts[i];
    }
    delete[] parts;
}

void nodeT4::forward() {
    for (int i = 0; i < countP; i++) {
        parts[i]->forward();
    }
}
void nodeT4::clearGrad() {
    for (int i = 0; i < countP; i++) {
        parts[i]->clearGrad();
    }
}
void nodeT4::backward() {
    for (int i = countP - 1; i >= 0; i--) {
        parts[i]->backward();
    }
}

void nodeT4::fillH(bool A_W) {
    for (int i = 0; i < countP; i++) {
        parts[i]->fillH(A_W);
    }
}
void nodeT4::transferD(vector* B, int start, bool out_in, bool delta_base, bool copy_add, bool A_W) {
    if (A_W) {
        (delta_base ? dVal : val)->transferD(B, start, out_in, copy_add);
    }
    else {
        for (int i = 0, s = 0; i < countP; i++) {
            parts[i]->transferD(B, start + s, out_in, delta_base, copy_add, false);
            s += parts[i]->sizePar;
        }
    }
}

