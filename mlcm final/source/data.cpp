#pragma once
#include "data.h"

data::data(uint32_t sizeEl_, std::string path_, std::string pathI_) {
    sizeEl = sizeEl_; path = path_; pathI = pathI_; fill = 0;
    M = new vector(sizeEl); Dv = new vector(sizeEl); D = 0;
    if (pathI != "") {
        loadArr(pathI, (char*)&fill, sizeof(uint32_t));
        (values = new vector(fill * sizeEl))->transferDF(path, false);
        expand(count = fill);
        values->transferDF(path, false);
        if (fill) calcDM();
    } else {
        values = new vector();
    }
}
data::~data() {
    if (pathI != "") {
        values->transferDF(path, true);
        saveArr(pathI, (char*)&fill, sizeof(uint32_t));
    }
    delete values;
    delete M; delete Dv;
}
void data::calcDM() {
    vector d(sizeEl);
    for (uint32_t i = 0; i < fill; i++) {
        get(i, &d);
        *M += d;
    }
    *M /= fill;
    for (uint32_t i = 0; i < fill; i++) {
        get(i, &d);
        d -= *M;
        *Dv += hadam(d, d);
    }
    *Dv /= fill;
    D = Dv->getTotal();
}
void data::set(uint32_t id, vector* value) {
    if (id >= count || value->size != sizeEl) {
        tlsm();
    }
    value->transferD(values, id * sizeEl, true, true);
}
void data::get(uint32_t id, vector* value) const {
    if (id >= count || value->size != sizeEl) {
        tlsm();
    }
    value->transferD(values, id * sizeEl, false, true);
}
void data::add(vector* value) {
    if (fill >= count) {
        expand(20);
    }
    set(fill, value);
    fill++;
}
void data::expand(uint32_t count_) {
    count += count_;
    *values >>= count * sizeEl;
}
void data::print() {
    vector v(sizeEl);
    for (uint32_t i = 0; i < fill; i++) {
        get(i, &v);
        v.print();
    }
}
void data::printASCIIART(int H, int W, int D) {
    vector v(sizeEl);
    for (uint32_t i = 0; i < fill; i++) {
        get(i, &v);
        v.printASCIIART(H, W, D);
        for (int j = 0; j < 2; j++) {
            for (uint16_t x = 0; x < W; x++) {
                printf("|");
            }
            printf("\n");
        }
    }
}

pairData::pairData(uint32_t sizeIn, uint32_t sizeOut, std::string pi, std::string pi_, std::string po, std::string po_) {
    inData = new data(sizeIn, pi, pi_);
    outData = new data(sizeOut, po, po_);
    fill = inData->fill;
    if (fill != outData->fill) tlsm();
}
pairData::~pairData() {
    delete inData;
    delete outData;
}
void pairData::calcDM() {
    inData->calcDM();
    outData->calcDM();
}
void pairData::set(uint32_t id, vector* valueI, vector* valueO) {
    inData->set(id, valueI);
    outData->set(id, valueO);
}
void pairData::get(uint32_t id, vector* valueI, vector* valueO) const {
    if(valueI) inData->get(id, valueI);
    if(valueO) outData->get(id, valueO);
}
void pairData::add(vector* valueI, vector* valueO) {
    fill++;
    inData->add(valueI);
    outData->add(valueO);
}