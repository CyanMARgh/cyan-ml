#pragma once
#include "nodes.h"

percL1::percL1(nodeT1* inNode, int sizeI, int sizeO) : nodeT4(inNode, sizeI, sizeO, 3) {
    parts[0] = new matMultNode(inNode, sizeI, sizeO);
    parts[1] = new addNode(parts[0], sizeO);
    parts[2] = new reluNode(parts[1], sizeO);
    postfix();
}
percL2::percL2(nodeT1* inNode, int sizeI, int sizeH, int sizeO) : nodeT4(inNode, sizeI, sizeO, 2) {
    parts[0] = new percL1(inNode, sizeI, sizeH);
    parts[1] = new percL1(parts[0], sizeH, sizeO);
    postfix();
}
percL3::percL3(nodeT1* inNode, int sizeI, int sizeH0, int sizeH1, int sizeO) : nodeT4(inNode, sizeI, sizeO, 3) {
    parts[0] = new percL1(inNode, sizeI, sizeH0);
    parts[1] = new percL1(parts[0], sizeH0, sizeH1);
    parts[2] = new percL1(parts[1], sizeH1, sizeO);
    postfix();
}
percSigmaL1::percSigmaL1(nodeT1* inNode, int sizeI, int sizeO) : nodeT4(inNode, sizeI, sizeO, 3) {
    parts[0] = new matMultNode(inNode, sizeI, sizeO);
    parts[1] = new addNode(parts[0], sizeO);
    parts[2] = new sigmaNode(parts[1], sizeO);
    postfix();
}
percSigmaL2::percSigmaL2(nodeT1* inNode, int sizeI, int sizeH, int sizeO) : nodeT4(inNode, sizeI, sizeO, 2) {
    parts[0] = new percL1(inNode, sizeI, sizeH);
    parts[1] = new percSigmaL1(parts[0], sizeH, sizeO);
    postfix();
}
percSigmaL3::percSigmaL3(nodeT1* inNode, int sizeI, int sizeH0, int sizeH1, int sizeO) : nodeT4(inNode, sizeI, sizeO, 3) {
    parts[0] = new percL1(inNode, sizeI, sizeH0);
    parts[1] = new percL1(parts[0], sizeH0, sizeH1);
    parts[2] = new percSigmaL1(parts[1], sizeH1, sizeO);
    postfix();
}
convBiasPool::convBiasPool(nodeT1* inNode, int s, int z, int so, int w, int soo) : nodeT4(inNode, s* s* z, soo* soo* w, 3) {
    parts[0] = new kernelSquare(inNode, s, s - so + 1, so, z, w, 0, 1);
    parts[1] = new tensBiasSquare(parts[0], so, w);
    parts[2] = new poolSquare(parts[1], so, soo, so / soo, w);
    postfix();
}
deconvBiasRelu::deconvBiasRelu(nodeT1* inNode, int s, int z, int so, int w) : nodeT4(inNode, s* s* z, so* so* w, 3) {
    parts[0] = new deconvSquare(inNode, s, z, so, w);
    parts[1] = new tensBiasSquare(parts[0], so, w);
    parts[2] = new reluNode(parts[1], so * so * w);
    postfix();
}
deconvBiasSigma::deconvBiasSigma(nodeT1* inNode, int s, int z, int so, int w) : nodeT4(inNode, s* s* z, so* so* w, 3) {
    parts[0] = new deconvSquare(inNode, s, z, so, w);
    parts[1] = new tensBiasSquare(parts[0], so, w);
    parts[2] = new sigmaNode(parts[1], so * so * w);
    postfix();
}
stretchDeconvDeconv::stretchDeconvDeconv(nodeT1* inNode, int h0, int s0, int s0_, int h1, int s1, int h2, int s2) : nodeT4(inNode, s0* s0* h0, s2* s2* h2, 3) {
    parts[0] = new stretchNodeSquare(inNode, s0, h0, s0_);
    parts[1] = new deconvBiasRelu(parts[0], s0_, h0, s1, h1);
    parts[2] = new deconvBiasRelu(parts[1], s1, h1, s2, h2);
    postfix();
}
stretchDeconvDeconvSigma::stretchDeconvDeconvSigma(nodeT1* inNode, int h0, int s0, int s0_, int h1, int s1, int h2, int s2) : nodeT4(inNode, s0* s0* h0, s2* s2* h2, 3) {
    parts[0] = new stretchNodeSquare(inNode, s0, h0, s0_);
    parts[1] = new deconvBiasRelu(parts[0], s0_, h0, s1, h1);
    parts[2] = new deconvBiasSigma(parts[1], s1, h1, s2, h2);
    postfix();
}
CNNv0::CNNv0(nodeT1* inNode, int h0, int s0, int h1, int s1, int s1_, int h2, int s2, int s2_, int h3, int s3, int h4) : nodeT4(inNode, s0* s0* h0, h4, 4) {
    parts[0] = new convBiasPool(inNode, s0, h0, s1, h1, s1_);
    parts[1] = new convBiasPool(parts[0], s1_, h1, s2, h2, s2_);
    parts[2] = new convBiasPool(parts[1], s2_, h2, s3, h3, 1);
    parts[3] = new percSigmaL1(parts[2], h3, h4);
    postfix();
}
CNNv1::CNNv1(nodeT1* inNode, int h0, int s0, int h1, int s1, int s1_, int h2, int s2, int s2_, int h3, int s3, int h4) : nodeT4(inNode, s0* s0* h0, h4, 4) {
    parts[0] = new convBiasPool(inNode, s0, h0, s1, h1, s1_);
    parts[1] = new convBiasPool(parts[0], s1_, h1, s2, h2, s2_);
    parts[2] = new convBiasPool(parts[1], s2_, h2, s3, h3, 1);
    parts[3] = new percL1(parts[2], h3, h4);
    postfix();
}
CNNv2::CNNv2(nodeT1* inNode, int h0, int s0, int h1, int s1, int s1_, int h2, int s2, int h3) : nodeT4(inNode, s0* s0* h0, h3, 3) {
    parts[0] = new convBiasPool(inNode, s0, h0, s1, h1, s1_);
    parts[1] = new convBiasPool(parts[0], s1_, h1, s2, h2, 1);
    parts[2] = new percL1(parts[1], h2, h3);
    postfix();
}
dCNNv0::dCNNv0(nodeT1* inNode, int h1, int s1, int h2, int s2, int h3, int s3, int h4, int s4, int h5, int s5) : nodeT4(inNode, h1* s1* s1, s5* s5* h5, 3) {
    parts[0] = new matMultNode(inNode, h1, h1 * s1 * s1);
    parts[1] = new stretchDeconvDeconv(parts[0], h1, s1, s1 * 2, h2, s2, h3, s3);
    parts[2] = new stretchDeconvDeconv(parts[1], h3, s3, 2 * s3, h4, s4, h5, s5);
    postfix();
}
