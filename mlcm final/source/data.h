#pragma once
#include "lacm.h"

class data {
private:
    std::string path, pathI;
    vector* values;
    vector* Dv;
    uint32_t count;
public:
    vector* M;
    uint32_t sizeEl, fill;
    double D;

    data(uint32_t sizeEl_, std::string path_ = "", std::string pathI_ = "");
    ~data();
    void calcDM();

    void set(uint32_t id, vector* value);
    void get(uint32_t id, vector* value) const;
    void add(vector* value);
    void expand(uint32_t count_);

    void print();
    void printASCIIART(int H, int W, int D = 1);
};

class pairData {
public:
    uint32_t fill;
    data* inData, *outData;
    pairData(uint32_t sizeIn, uint32_t sizeOut, std::string pi = "", std::string pi_ = "", std::string po = "", std::string po_ = "");
    ~pairData();
    void calcDM();

    void set(uint32_t id, vector* valueI, vector* valueO);
    void get(uint32_t id, vector* valueI, vector* valueO) const;
    void add(vector* valueI, vector* valueO);
};