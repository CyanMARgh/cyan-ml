#pragma once
#include "nets.h"

forwardModel::forwardModel(uint32_t inSize_, uint32_t outSize_) {
	inSize = inSize_; outSize = outSize_;
	input = new nodeT1(inSize);
	R = new nodeT1(outSize);
	calculator = NULL;
	loss = NULL;// new lossL2(NULL, R, outSize);
}
void forwardModel::save(std::string path) {
	if (calculator) {
		calculator->transferDF(path, true, false);
	}
}
void forwardModel::load(std::string path) {
	if (calculator) {
		calculator->transferDF(path, false, false);
	}
}
void forwardModel::setCalculator(nodeT2* calc_) {
	delete calculator; calculator = calc_;
	calculator->connectIn1(input); calculator->fillH(false);
	if(loss) loss->connectIn1(calculator);
}
void forwardModel::setLoss(lossNode* loss_) {
	delete loss; loss = loss_;
	loss->connectIn1(calculator);
	loss->connectIn2(R);
}

forwardModel::~forwardModel() {
	delete loss;
	delete R;
	delete calculator;
	delete input;
}

void forwardModel::forward(bool calcLoss) {
	calculator->forward();
	if (calcLoss) {
		loss->forward();
	}
}
void forwardModel::backward() {
	R->clearGrad();
	calculator->clearGrad();
	loss->backward();
	input->clearGrad();
	calculator->backward();
}

void forwardModel::calculate(const vector* in, vector* out) {
	*(input->val) = *in;
	forward(false);
	*out = *(calculator->val);
}
void forwardModel::learn(const pairData* pairs, std::function<void(const vector*, vector*)> method, double* err) {
	if (pairs->inData->sizeEl != inSize || pairs->outData->sizeEl != outSize) tlsm();
	uint32_t C = pairs->fill;
	double error = 0.;
	vector dWcalc(calculator->sizePar), dWcalc_ = dWcalc;
	loss->setGrad(1.);
	for (uint32_t i = 0; i < C; i++) {
		pairs->get(i, input->val, R->val);
		forward(true);
		error += loss->getErr();
		backward();
		calculator->getParGrad(&dWcalc);
	}
	dWcalc /= -1. * C;
	method(&dWcalc, &dWcalc_);
	if (err) *err = error / (C * pairs->outData->D);
	calculator->applyParGrad(&dWcalc_);
}
void forwardModel::fullLearn(const pairData* pairs, uint32_t iterations, double minErr, std::function<void(const vector*, vector*)> method, uint32_t printErrFreq) {
	double err = 1.;
	for (uint32_t i = 1; i <= iterations && err>minErr; i++) {
		learn(pairs, method, &err);
		if (printErrFreq && !(i % printErrFreq)) printf("(i:%4d) error: %3.3f\n", i, err);
	}
}



