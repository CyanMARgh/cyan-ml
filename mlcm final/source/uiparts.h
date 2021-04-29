#pragma once
#include "lasf.h"
#include "lacm.h"
#include <functional>

class uiPart {
protected:
	sf::RenderWindow* rw;
	sf::FloatRect zone;
	int Lb = 3;
public:
	uiPart(sf::FloatRect zone_, sf::RenderWindow* rw_, sf::FloatRect baseZone);
	virtual ~uiPart() { }

	virtual void click() { }
	virtual void dragg() { }
	virtual void draw() { }
	virtual void update();
	virtual void buttonUpdate(sf::Vector2f P, bool down_up);
};
class uiSprite : public uiPart {
private:
	sf::Texture tx;
	sf::Sprite sp;
public:
	uiSprite(std::string path, sf::FloatRect zone_, sf::RenderWindow* rw_, sf::FloatRect baseZone);

	void draw() override;
};
class uiButton : public uiPart {
private:
	sf::Texture txp, txf;
	sf::Sprite sp;
public:
	std::function<void()> action;

	uiButton(std::string pathP, std::string pathF, sf::FloatRect zone_, sf::RenderWindow* rw_, sf::FloatRect baseZone, std::function<void()> act = [&]() {printf("\a"); });

	void setPressedFree(bool p_f);
	void click() override;
	void draw() override;
	void buttonUpdate(sf::Vector2f P, bool down_up) override;
};
class uiCanvas : public uiPart {
private:
	int H, W, b;
	double p;
	sf::Texture tx;
	sf::Sprite sp;
public:
	vector map;

	uiCanvas(sf::FloatRect zone_, sf::RenderWindow* rw_, sf::FloatRect baseZone, int H_ = 20, int W_ = 0, int b_ = 0, double p_ = 0.03);
	void refresh();
	void clear();
	void dragg() override;
	void draw() override;
};
class uiText : public uiPart {
private:
	sf::Text tt;
	sf::Font f;
public:
	uiText(std::string pathT, sf::FloatRect zone_, sf::RenderWindow* rw_, sf::FloatRect baseZone, std::string t_ = "", sf::Vector2f offset = sf::Vector2f(0., 0.));

	void setText(sf::String t_);
	void draw() override;
};

class uiGroup : public uiPart {
protected:
	uint32_t countP = 0;
	uiPart** parts = NULL;
public:
	uiGroup(uint32_t countP_, sf::FloatRect zone_, sf::RenderWindow* rw_, sf::FloatRect baseZone);
	virtual ~uiGroup();

	virtual void draw() override;
	virtual void update() override;
	virtual void buttonUpdate(sf::Vector2f P, bool down_up) override;
};
class uiButtonChar : public uiGroup {
public:
	std::function<void()>* action;

	uiButtonChar(std::string pathP, std::string pathF, std::string pathT, sf::FloatRect zone_, sf::RenderWindow* rw_, sf::FloatRect baseZone, char C = ' ');
};
class uiSwitcher : public uiGroup {
private:
	uint32_t max;
	std::string S;
public:
	uint32_t current = 0;

	uiSwitcher(std::string pathP, std::string pathF, std::string pathT, sf::FloatRect zone_, sf::RenderWindow* rw_, sf::FloatRect baseZone, std::string S_, uint16_t max_);

	void refreshText();
	virtual void click() override;
};

//class uiCell : public uiGroup {
//private:
//	std::string* paths;
//	uint32_t visible;
//public:
//	uiCell(std::string* paths_, int countP_, sf::FloatRect zone_, sf::RenderWindow* rw_, sf::FloatRect baseZone) :uiGroup(countP_, zone_, rw_, baseZone) {
//		paths = paths_; visible = 0;
//		for (int i = 0; i < countP_; i++) {
//			parts[i] = new uiSprite(paths[i], zone_, rw_, baseZone);
//		}
//	}
//	void refresh(uint32_t v_) {
//		visible = v_;
//	}
//	void draw() override {
//		uint32_t v = visible;
//		for (uint32_t i = 0; i < countP; i++) {
//			if (v & 1) parts[i]->draw();
//			v >>= 1;
//		}
//	}
//};
//class uiField : public uiGroup {
//private:
//	uint32_t cellSize;
//	std::string* paths;
//public:
//	int sY, sX; //ivec2 -> lacm
//	uiField(std::string* paths_, uint32_t cellSize_, sf::FloatRect zone_, sf::RenderWindow* rw_, sf::FloatRect baseZone, int sX_, int sY_) : uiGroup(sX_* sY_, zone_, rw_, baseZone) {
//		paths = paths_; cellSize = cellSize_;
//		sY = sY_; sX = sX_;
//		sf::Vector2f bit(1.f / sX, 1.f / sX);
//		for (int y = 0; y < sY; y++) {
//			for (int x = 0; x < sX; x++) {
//				parts[x + y * sX] = new uiCell(paths, cellSize, sf::FloatRect(bit * sf::Vector2f(x, sY - 1 - y), bit), rw_, zone);
//			}
//		}
//	}
//	~uiField() {
//		delete[] paths;
//	}
//
//	virtual uint32_t convert(int t, int x, int y) = 0;
//	void refreshField(int* info) {
//		for (int y = 0; y < sY; y++) {
//			for (int x = 0; x < sX; x++) {
//				((uiCell*)parts[x + y * sX])->refresh(convert(info[x + y * sX], x, y));
//			}
//		}
//	}
//};
//class uiCheckers : public uiField {
//protected:
//	checkers* ch;
//public:
//	uiCheckers(std::string* paths_, sf::FloatRect zone_, sf::RenderWindow* rw_, sf::FloatRect baseZone) :uiField(paths_, 5, zone_, rw_, baseZone, 8, 8) { 
//		ch = new checkers();
//		refreshField(ch->field);
//	}
//	~uiCheckers() {
//		delete ch;
//	}
//	uint32_t convert(int t, int x, int y) override {
//		t *= (ch->player) ? -1 : 1;
//		uint32_t T = 0;
//		T |= (x + y) % 2 << 0;
//		T |= (x + y + 1) % 2 << 1;
//		T |= (t > 0) << 2;
//		T |= (t < 0) << 3;
//		T |= (abs(t) > 1) << 4;
//		return T;
//	}
//};
//class uiCheckersPvP : public uiCheckers {
//public:
//	uiCheckersPvP(std::string* paths_, sf::FloatRect zone_, sf::RenderWindow* rw_, sf::FloatRect baseZone) : uiCheckers(paths_, zone_, rw_, baseZone) { }
//	
//	void click() override {
//		static ivec2 p$(-1, -1);
//		sf::Vector2f P = (sf::Vector2f)sf::Mouse::getPosition(*rw);
//		sf::Vector2i P_ = (sf::Vector2i)((sf::Vector2f)sf::Vector2i(8, 8) * (P / zone));
//		ivec2 p_(P_.x, 7 - P_.y);
//		if (inZone(p_, { 8,8 }) && inZone(p$, { 8,8 })) {
//			bool kill;
//			ch->fixedStep(p$, p_, kill, p_ == p$);
//			if (!checkNextStates()) {
//				printf("player #%d won!", (ch->player + 1) % 2);
//			}
//			refreshField(ch->field);
//			p_ = { -1,-1 };
//		}
//		p$ = p_;
//	};
//	bool checkNextStates() {
//		std::list<checkers> next = {};
//		ch->getNextStates(next);
//		return next.size();
//	}
//};
//class uiCheckersPvE : public uiCheckers {
//public:
//	uiCheckersPvE(std::string* paths_, sf::FloatRect zone_, sf::RenderWindow* rw_, sf::FloatRect baseZone) : uiCheckers(paths_, zone_, rw_, baseZone) { }
//
//	void click() override {
//		static ivec2 p$(-1, -1);
//		sf::Vector2f P = (sf::Vector2f)sf::Mouse::getPosition(*rw);
//		sf::Vector2i P_ = (sf::Vector2i)((sf::Vector2f)sf::Vector2i(8, 8) * (P / zone));
//		ivec2 p_(P_.x, 7 - P_.y);
//		if (inZone(p_, { 8,8 }) && inZone(p$, { 8,8 })) {
//			bool kill;
//			if (ch->fixedStep(p$, p_, kill, p_ == p$)) {
//				if (ch->begin_continue) {
//					botStep();
//				}
//			}
//			p_ = { -1,-1 };
//			refreshField(ch->field);
//		}
//		p$ = p_;
//	};
//	void botStep() {
//		std::list<checkers> next = {};
//		ch->getNextStates(next);
//		int s = next.size();
//		if (!s) {
//			printf("player won!\n");
//		} else {
//			int i = rand() % s;
//			auto n = next.begin();
//			std::advance(n, i);
//			*ch = *n;
//			next = {};
//			ch->getNextStates(next);
//			if (!next.size()) {
//				printf("bot won!\n");
//			}
//		}
//	}
//};

class uiWindow {
private:
	uint32_t countP = 0;
protected:
	sf::RenderWindow* window;
	sf::FloatRect w_zone;
	uint32_t countA = 0;
public:
	uiPart** parts;
	std::function<void()>** actions;

	uiWindow(uint32_t countP_, uint32_t countA_, uint32_t W, uint32_t H);
	virtual ~uiWindow();

	void cycle();
};
class uiWindowV0 : public uiWindow {
public:
	uiWindowV0(int W, int H, int Wc, int Hc, int typesCount, std::string types);
	void updateText(char mode, int p0 = 0);
};
class uiWindowV1 : public uiWindow {
public:
	uiWindowV1(int W, int H, int Wc, int Hc);
};
//class uiWindowV2 : public uiWindow {
//public:
//	uiWindowV2(int H) : uiWindow(1, 0, (H * 5) / 4, H) {
//		std::string fb = "images/field_black.png";
//		std::string fw = "images/field_white.png";
//		std::string cb = "images/blackbig.png";
//		std::string cw = "images/bluebig.png";
//		std::string cr = "images/redsmall.png";
//		parts[0] = new uiCheckersPvE(new std::string[5]{ fw, fb, cw, cb, cr }, sf::FloatRect(0., 0., .8, 1.), window, w_zone);
//	}
//};
