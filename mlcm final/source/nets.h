#pragma once
#include "nodes.h"
#include "data.h"
#include <functional>

class forwardModel {
public:
	uint32_t inSize, outSize;
	nodeT1* input, * R;
	nodeT2* calculator;
	lossNode* loss;

	forwardModel(uint32_t inSize_, uint32_t outSize_);
	void save(std::string path);
	void load(std::string path);
	void setCalculator(nodeT2* calc_);
	void setLoss(lossNode* loss_);
	~forwardModel();

	void forward(bool calcLoss);
	void backward();

	void calculate(const vector* in, vector* out);
	void learn(const pairData* pairs, std::function<void(const vector*, vector*)> method, double* err = NULL);
	void fullLearn(const pairData* pairs, uint32_t iterations, double minErr, std::function<void(const vector*, vector*)> method, uint32_t printErrFreq = 0);
};
class GANmodel {
public:
	uint32_t hashSize, resSize;
	nodeT1* inputH, *R;
	nodeT2* generator, * discriminator;
	lossNode *loss;

	GANmodel(uint32_t hashSize_, uint32_t resSize_);
	void save(std::string pathG, std::string pathD);
	void load(std::string pathG, std::string pathD);
	void setGenerator(nodeT2* gen_);
	void setDiscriminator(nodeT2* dis_);
	void setLoss(lossNode* loss_);
	~GANmodel();

	void forward(bool g_dl);
	void backward(bool ld_g);

	void generate(vector* out);
	void learn(const data* examples, uint32_t S, std::function<void(vector*)> methodD, std::function<void(vector*)> methodG, double* errD = NULL, double* errG = NULL);
	void fullLearn(const data* examples, uint32_t S, uint32_t iterations, std::function<void(vector*)> methodD, std::function<void(vector*)> methodG, uint32_t printErrFreq = 0);
};
