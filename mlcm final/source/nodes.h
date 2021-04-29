#pragma once
#include "LAcm.h"

class nodeT1 {
public:
    int sizeVal = 0;
    vector* val = NULL, * dVal = NULL;

    nodeT1(int size);
    virtual ~nodeT1();

    void print();
    virtual void transferD(vector* B, int start, bool out_in, bool delta_base, bool copy_add, bool A_W);
    virtual void transferDF(const std::string& path, bool out_in, bool A_W);
    virtual void clearGrad();
    virtual void fillH(bool A_W);
};
class nodeT2 : public nodeT1 {
public:
    int sizeArg = 0;
    int sizePar = 0; bool par_own = false;
    vector* arg = NULL, * dArg = NULL;

    nodeT2(nodeT1* inNode, int inSize, int outSize);
    virtual void connectIn1(nodeT1* inNode);
    virtual void connectIn2(nodeT1* inNode) { }
    virtual ~nodeT2() { }

    void transferDF(const std::string& path, bool out_in, bool A_W) override;
    void getParGrad(vector* B);
    void applyParGrad(vector* B);

    virtual void forward() = 0;
    virtual void backward() = 0;
};
class nodeT3 : public nodeT2 {
public:
    vector* par, * dPar;
    double scale = 1.;

    nodeT3(nodeT1* inNode, int inSize, int parSize, int outSize);
    nodeT3(nodeT1* inNode1, nodeT1* inNode2, int inSize, int parSize, int outSize);
    void connectIn2(nodeT1* inNode);
    ~nodeT3();

    void transferD(vector* B, int start, bool out_in, bool delta_base, bool copy_add, bool A_W) override;
    void fillH(bool A_W) override;
    void clearGrad() override;
};
class nodeT4 : public nodeT2 {
public:
    int countP = 0;
    nodeT2** parts = NULL;

    nodeT4(nodeT1* nZ, int inSize, int sizeA_, int countW_);
    void connectIn1(nodeT1* inNode);
    void postfix();
    ~nodeT4();

    void forward() override;
    void clearGrad() override;
    void backward() override;

    void fillH(bool A_W) override;
    void transferD(vector* B, int start, bool out_in, bool delta_base, bool copy_add, bool A_W) override;
};

class reluNode : public nodeT2 {
public:
    reluNode(nodeT1* inNode, int outSize);
    void forward() override;
    void backward() override;
};
class sigmaNode : public nodeT2 {
public:
    sigmaNode(nodeT1* inNode, int size);
    void forward() override;
    void backward() override;
};
class tanhNode : public nodeT2 {
public:
    tanhNode(nodeT1* inNode, int size);
    void forward() override;
    void backward() override;
};
class addNode : public nodeT3 {
public:
    addNode(nodeT1* inNode1, nodeT1* inNode2, int outSize);
    addNode(nodeT1* inNode, int outSize);
    void forward() override;
    void backward() override;
};
class matMultNode : public nodeT3 {
public:
    matMultNode(nodeT1* inNode, int inSize, int outSize);
    void forward() override;
    void backward() override;
};

class tensBiasSquare : public nodeT3 {
public:
    int S, z;
    tensBiasSquare(nodeT1* inNode, int S_, int z_);
    void forward() override;
    void backward() override;
};
class kernelSquare : public nodeT3 {
public:
    int s, z, sk, st, b, so, w;
    kernelSquare(nodeT1* inNode, int s_, int sk_, int so_, int z_ = 1, int w_ = 1, int b_ = 0, int st_ = 1);
    void forward() override;
    void backward() override;
};
class deconvSquare : public kernelSquare {
public:
    deconvSquare(nodeT1* inNode, int s, int z, int so, int w);
};
class poolSquare : public nodeT2 {
public:
    int si, so, sc, sz;
    poolSquare(nodeT1* inNode, int si_, int so_, int sc_, int sz_);
    void forward() override;
    void backward() override;
};
class stretchNodeSquare : public nodeT2 {
public:
    int s, z, S;
    stretchNodeSquare(nodeT1* inNode, int s_, int z_, int S_);
    void forward() override;
    void backward() override;
};

class lossNode : public nodeT3 {
public:
    lossNode(nodeT1* inNode1, nodeT1* inNode2, int size);
    double getErr();
    void setGrad(double grad);
};
class lossL2 : public lossNode {
public:
    lossL2(nodeT1* inNode1, nodeT1* inNode2, int size);
    void forward() override;
    void backward() override;
};
class logAbsLoss : public lossNode {
public:
    logAbsLoss(nodeT1* inNode1, nodeT1* inNode2);
    void forward() override;
    void backward() override;
};
class idLoss : public lossNode {
public:
    idLoss(nodeT1* inNode1, nodeT1* inNode2, int size);
    void forward() override;
    void backward() override;
};

class percL1 : public nodeT4 {
public:
    percL1(nodeT1* inNode, int sizeI, int sizeO);
};
class percL2 : public nodeT4 {
public:
    percL2(nodeT1* inNode, int sizeI, int sizeH, int sizeO);
};
class percL3 : public nodeT4 {
public:
    percL3(nodeT1* inNode, int sizeI, int sizeH0, int sizeH1, int sizeO);
};
class percSigmaL1 : public nodeT4 {
public:
    percSigmaL1(nodeT1* inNode, int sizeI, int sizeO);
};
class percSigmaL2 : public nodeT4 {
public:
    percSigmaL2(nodeT1* inNode, int sizeI, int sizeH, int sizeO);
};
class percSigmaL3 : public nodeT4 {
public:
    percSigmaL3(nodeT1* inNode, int sizeI, int sizeH0, int sizeH1, int sizeO);
};
class convBiasPool : public nodeT4 {
public:
    convBiasPool(nodeT1* inNode, int s, int z, int so, int w, int soo);
};
class deconvBiasRelu : public nodeT4 {
public:
    deconvBiasRelu(nodeT1* inNode, int s, int z, int so, int w);
};
class deconvBiasSigma : public nodeT4 {
public:
    deconvBiasSigma(nodeT1* inNode, int s, int z, int so, int w);
};
class stretchDeconvDeconv : public nodeT4 {
public:
    stretchDeconvDeconv(nodeT1* inNode, int h0, int s0, int s0_, int h1, int s1, int h2, int s2);
};
class stretchDeconvDeconvSigma : public nodeT4 {
public:
    stretchDeconvDeconvSigma(nodeT1* inNode, int h0, int s0, int s0_, int h1, int s1, int h2, int s2);
};

class CNNv0 : public nodeT4 {
public:
    CNNv0(nodeT1* inNode, int h0, int s0, int h1, int s1, int s1_, int h2, int s2, int s2_, int h3, int s3, int h4);
};
class CNNv1 : public nodeT4 {
public:
    CNNv1(nodeT1* inNode, int h0, int s0, int h1, int s1, int s1_, int h2, int s2, int s2_, int h3, int s3, int h4);
};
class CNNv2 : public nodeT4 {
public:
    CNNv2(nodeT1* inNode, int h0, int s0, int h1, int s1, int s1_, int h2, int s2, int h3);
};
class dCNNv0 : public nodeT4 {
public:
    dCNNv0(nodeT1* inNode, int h1, int s1, int h2, int s2, int h3, int s3, int h4, int s4, int h5, int s5);
};
