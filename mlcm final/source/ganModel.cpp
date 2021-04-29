#pragma once
#include "nets.h"
#define PRINTSTART 0
#define LEARNRATE 10
//#define DEFERROR 0.25
#define DEFERROR log(2)

GANmodel::GANmodel(uint32_t hashSize_, uint32_t resSize_) {
	hashSize = hashSize_; resSize = resSize_;
	inputH = new nodeT1(hashSize);
	generator = NULL;
	discriminator = NULL;
	R = new nodeT1(1);
	loss = NULL;// new logLoss(NULL, R);
}
void GANmodel::save(std::string pathG, std::string pathD) {
	if (generator) {
		generator->transferDF(pathG, true, false);
	}
	if (discriminator) {
		discriminator->transferDF(pathD, true, false);
	}
}
void GANmodel::load(std::string pathG, std::string pathD) {
	if (generator) {
		generator->transferDF(pathG, false, false);
	}
	if (discriminator) {
		discriminator->transferDF(pathD, false, false);
	}
}
void GANmodel::setGenerator(nodeT2* gen_) {
	delete generator; generator = gen_;
	generator->connectIn1(inputH); generator->fillH(false);
	if (discriminator) discriminator->connectIn1(generator);
}
void GANmodel::setDiscriminator(nodeT2* dis_) {
	delete discriminator; discriminator = dis_;
	if(generator) discriminator->connectIn1(generator);
	discriminator->fillH(false);
	if(loss) loss->connectIn1(discriminator);
}
void GANmodel::setLoss(lossNode* loss_) {
	delete loss;
	loss = loss_;
	loss->connectIn1(discriminator);
	loss->connectIn2(R);
}
GANmodel::~GANmodel() {
	delete loss;
	delete R;
	delete discriminator;
	delete generator;
	delete inputH;
}

void GANmodel::forward(bool g_dl) {
	if (g_dl) {
		inputH->fillH(true);
		generator->forward();
	} else {
		discriminator->forward();
		loss->forward();
	}
}
void GANmodel::backward(bool ld_g) {
	if (ld_g) {
		R->clearGrad();
		discriminator->clearGrad();
		loss->backward();
		generator->clearGrad();
		discriminator->backward();
	} else {
		inputH->clearGrad();
		generator->backward();
	}
}

void GANmodel::learn(const data* examples, uint32_t S, std::function<void(vector*)> methodD, std::function<void(vector*)> methodG, double* errD, double* errG) {
	static int h = 0; h++;
	if (examples->sizeEl != resSize) tlsm();
	uint32_t C = examples->fill;
	double errorD = 0., errorG = 0.;
	vector dWgen(generator->sizePar);
	vector dWdis(discriminator->sizePar);
	//discriminator
	loss->setGrad(-1.);
	(*(R->val))() = 0.;
	for (uint32_t i = 0, j; i < S; i++) {
		j = i; if(S!=C) j = hash(i, h) % C;
		examples->get(j, discriminator->arg);
		forward(false);
		errorD += loss->getErr();
		backward(true);
		discriminator->getParGrad(&dWdis);
	}
	(*(R->val))() = 1.;
	for (uint32_t i = 0; i < S; i++) {
		forward(true); forward(false);
		errorD += loss->getErr();
		backward(true);
		discriminator->getParGrad(&dWdis);
	}
	//generator
	(*(R->val))() = 0.;
	for (uint32_t i = 0; i < S; i++) {
		forward(true); forward(false);
		errorG += loss->getErr();
		backward(true); backward(false);
		generator->getParGrad(&dWgen);
	}
	errorD /= 2. * S * DEFERROR;
	errorG /= S * DEFERROR;
	methodD(&(dWdis /= 2. * S));
	methodG(&(dWgen /= S));
	if (errD) *errD = errorD;
	if (errG) *errG = errorG;

	discriminator->applyParGrad(&dWdis);
#ifdef LEARNRATE
	static int T = 0; (++T) %= LEARNRATE;
	if (!T) 
#endif
		generator->applyParGrad(&dWgen);
}
void GANmodel::fullLearn(const data* examples, uint32_t S, uint32_t iterations, std::function<void(vector*)> methodD, std::function<void(vector*)> methodG, uint32_t printErrFreq) {
	double errD = 1., errG = 1.;
	for (uint32_t i = 1; i <= iterations; i++) {
		learn(examples, S, methodD, methodG, &errD, &errG);
		if (printErrFreq && !(i % printErrFreq)) {
			printf("(i:%4d) error: %3.3f(D), %3.3f(G)\n", i, errD, errG);
			if (i / printErrFreq % 5 == 0) {
				vector exm(resSize);
				generate(&exm);
#ifdef  PRINTSTART
				exm.transferDIF("images/output/" + std::to_string(i + PRINTSTART) + ".png", 0, true, sf::Vector2u(32, 32));
				if (i / printErrFreq % 50 == 0) {
					save("data/models/G" + std::to_string(i + PRINTSTART), "data/models/D" + std::to_string(i + PRINTSTART));
				}
#else 
				printf("example: "); exm.print();
#endif
			}
		}
	}
}

void GANmodel::generate(vector* out) {
	forward(true);
	*out = *(generator->val);
}
