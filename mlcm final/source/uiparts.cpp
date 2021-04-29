#pragma once
#include "uiparts.h"

uiPart::uiPart(sf::FloatRect zone_, sf::RenderWindow* rw_, sf::FloatRect baseZone) : rw(rw_) {
	zone = zone_ >> baseZone;
}
void uiPart::update() {
	if (Lb == 0) {
		Lb++;
	}
	else if (Lb == 1) {
		dragg();
	}
	else if (Lb == 2) {
		click();
		Lb++;
	}
}
void uiPart::buttonUpdate(sf::Vector2f P, bool down_up) {
	if (down_up) {
		if (zone.contains(P)) {
			Lb = 0;
		}
	}
	else {
		if (Lb != 3) {
			Lb = zone.contains(P) ? 2 : 3;
		}
	}
}

uiSprite::uiSprite(std::string path, sf::FloatRect zone_, sf::RenderWindow* rw_, sf::FloatRect baseZone) : uiPart(zone_, rw_, baseZone) {
	tx.loadFromFile(path);
	sp.setTexture(tx);
	sp.setPosition(rectPos(zone));
	sp.setScale(rectSize(zone) / (sf::Vector2f)(tx.getSize()));
}
void uiSprite::draw() {
	rw->draw(sp);
}

void uiButton::setPressedFree(bool p_f) {
	fitTexture(&sp, p_f ? &txp : &txf, zone);
}
uiButton::uiButton(std::string pathP, std::string pathF, sf::FloatRect zone_, sf::RenderWindow* rw_, sf::FloatRect baseZone, std::function<void()> act) : uiPart(zone_, rw_, baseZone) {
	action = act;
	txp.loadFromFile(pathP);
	txf.loadFromFile(pathF);
	setPressedFree(false);
}
void uiButton::click() {
	action();
}
void uiButton::draw() {
	rw->draw(sp);
}
void uiButton::buttonUpdate(sf::Vector2f P, bool down_up) {
	if (down_up) {
		if (zone.contains(P)) {
			Lb = 0;
			setPressedFree(true);
		}
	}
	else {
		if (Lb != 3) {
			Lb = zone.contains(P) ? 2 : 3;
			setPressedFree(false);
		}
	}
}

uiCanvas::uiCanvas(sf::FloatRect zone_, sf::RenderWindow* rw_, sf::FloatRect baseZone, int H_, int W_, int b_, double p_) :uiPart(zone_, rw_, baseZone) {
	H = H_; p = p_;
	W = (W_ <= 0) ? H : W_;
	map = vector(H * W);
	tx.create(W, H);
	b = b_; p = p_;
	refresh();
	fitTexture(&sp, &tx, zone);
}
void uiCanvas::dragg() {
	sf::Vector2f P = (sf::Vector2f)sf::Mouse::getPosition(*rw);
	sf::Vector2i P_ = (sf::Vector2i)((sf::Vector2f)sf::Vector2i(H, W) * (P / zone));
	int x_ = P_.x, y_ = P_.y;
	double m;
	for (int y = y_ - b, dy = -b; y <= y_ + b; y++, dy++) {
		for (int x = x_ - b, dx = -b; x <= x_ + b; x++, dx++) {
			if (x < W && x >= 0 && y < H && y >= 0) {
				m = map(x + W * y);
				m += p / (1. + (double)dx * dx + (double)dy * dy);
				map(x + W * y) = m > 1. ? 1. : m;
			}
		}
	}
	refresh();
}
void uiCanvas::refresh() {
	unsigned char* mapC = new unsigned char[W * H * 4];
	convertDoubleUchar4(map.V, mapC, W * H);
	tx.update(mapC);
	delete[] mapC;
}
void uiCanvas::clear() {
	map.fill();
	refresh();
}
void uiCanvas::draw() {
	rw->draw(sp);
}

uiText::uiText(std::string pathT, sf::FloatRect zone_, sf::RenderWindow* rw_, sf::FloatRect baseZone, std::string t_, sf::Vector2f offset) : uiPart(zone_, rw_, baseZone) {
	f.loadFromFile(pathT);
	tt.setString((sf::String)t_);
	tt.setCharacterSize((unsigned int)(zone.height));
	tt.setFont(f);
	tt.setPosition(rectPos(zone) + rectSize(zone) * offset);
	tt.setFillColor(sf::Color::Black);
}
void uiText::draw() {
	rw->draw(tt);
}
void uiText::setText(sf::String t_) {
	tt.setString(t_);
}

uiGroup::uiGroup(uint32_t countP_, sf::FloatRect zone_, sf::RenderWindow* rw_, sf::FloatRect baseZone) : uiPart(zone_, rw_, baseZone), countP(countP_) {
	parts = new uiPart * [countP];
}
uiGroup::~uiGroup() {
	for (uint32_t i = 0; i < countP; i++) {
		delete parts[i];
	}
	delete[] parts;
}

void uiGroup::draw() {
	for (uint32_t i = 0; i < countP; i++) {
		parts[i]->draw();
	}
}
void uiGroup::update() {
	uiPart::update();
	for (uint32_t i = 0; i < countP; i++) {
		parts[i]->update();
	}
}
void uiGroup::buttonUpdate(sf::Vector2f P, bool up_down) {
	uiPart::buttonUpdate(P, up_down);
	for (uint32_t i = 0; i < countP; i++) {
		parts[i]->buttonUpdate(P, up_down);
	}
}

uiButtonChar::uiButtonChar(std::string pathP, std::string pathF, std::string pathT, sf::FloatRect zone_, sf::RenderWindow* rw_, sf::FloatRect baseZone, char C) : uiGroup(2, zone_, rw_, baseZone) {
	uiButton* btn = new uiButton(pathP, pathF, sf::FloatRect(0.f, 0.f, 1.f, 1.f), rw_, zone);
	parts[0] = btn;
	parts[1] = new uiText(pathT, sf::FloatRect(0.f, 0.f, 1.f, 1.f), rw, zone, (sf::String)C, sf::Vector2f(0.23f, -0.17f));
	action = &(btn->action);
}

void uiSwitcher::refreshText() {
	((uiText*)(parts[1]))->setText((sf::String)(S[current]));
}
uiSwitcher::uiSwitcher(std::string pathP, std::string pathF, std::string pathT, sf::FloatRect zone_, sf::RenderWindow* rw_, sf::FloatRect baseZone, std::string S_, uint16_t max_) : uiGroup(2, zone_, rw_, baseZone) {
	max = max_; S = S_;
	uiButton* btn = new uiButton(pathP, pathF, sf::FloatRect(0.f, 0.f, 1.f, 1.f), rw, zone);
	parts[0] = btn;
	parts[1] = new uiText(pathT, sf::FloatRect(0.f, 0.f, 1.f, 1.f), rw, zone, "#", sf::Vector2f(0.23f, -0.17f));
	refreshText();
	btn->action = [&]() {click(); };
}
void uiSwitcher::click() {
	(++current) %= max;
	refreshText();
}

uiWindow::uiWindow(uint32_t countP_, uint32_t countA_, uint32_t W, uint32_t H) : countP(countP_), countA(countA_) {
	actions = new std::function<void()>*[countA];
	window = new sf::RenderWindow(sf::VideoMode(W, H), "mlcmv4");
	parts = new uiPart * [countP];
	w_zone = sf::FloatRect(sf::Vector2f(0.f, 0.f), (sf::Vector2f)(window->getSize()));
}
uiWindow::~uiWindow() {
	for (uint32_t i = 0; i < countP; i++) {
		delete parts[i];
	}
	delete[] parts;
	delete[] actions;
	delete window;
}
void uiWindow::cycle() {
	while (window->isOpen()) {
		sf::Event e;
		while (window->pollEvent(e)) {
			bool cl = e.type == sf::Event::Closed;
			bool pr = e.type == sf::Event::MouseButtonPressed;
			bool rl = e.type == sf::Event::MouseButtonReleased;
			if (cl) {
				window->close();
			}
			else if (pr || rl) {
				sf::Vector2f P = (sf::Vector2f)(sf::Mouse::getPosition(*window));
				for (uint32_t i = 0; i < countP; i++) {
					parts[i]->buttonUpdate(P, pr);
				}
			}
		}
		for (uint32_t i = 0; i < countP; i++) {
			parts[i]->update();
		}
		window->clear(sf::Color(255, 255, 255));
		for (uint32_t i = 0; i < countP; i++) {
			parts[i]->draw();
		}
		window->display();
	}
}

uiWindowV0::uiWindowV0(int W, int H, int Wc, int Hc, int typesCount, std::string types) : uiWindow(7, 4, W, H) {
	std::string bp = "images/button_pressed.png";
	std::string bf = "images/button_free.png";
	std::string F = "fonts/F.ttf";
	parts[0] = new uiButtonChar(bp, bf, F, sf::FloatRect(0.8f, 0.0f, 0.2f, 0.2f), window, w_zone, 'S');
	parts[1] = new uiButtonChar(bp, bf, F, sf::FloatRect(0.8f, 0.2f, 0.2f, 0.2f), window, w_zone, 'C');
	parts[2] = new uiButtonChar(bp, bf, F, sf::FloatRect(0.8f, 0.4f, 0.2f, 0.2f), window, w_zone, '?');
	parts[3] = new uiButtonChar(bp, bf, F, sf::FloatRect(0.8f, 0.8f, 0.2f, 0.2f), window, w_zone, ' ');
	parts[4] = new uiSwitcher(bp, bf, F, sf::FloatRect(0.8f, 0.6f, 0.2f, 0.2f), window, w_zone, types, typesCount);
	parts[5] = new uiCanvas(sf::FloatRect(0.0f, 0.0f, 0.8f, 0.8f), window, w_zone, Hc, Wc, 2, 0.2);
	parts[6] = new uiText(F, sf::FloatRect(0.f, 0.8f, 0.8f, 0.1f), window, w_zone, ">", sf::Vector2f(0.02f, -0.17f));
	for (uint32_t i = 0; i < countA; i++) {
		actions[i] = ((uiButtonChar*)(parts[i]))->action;
	}
}
void uiWindowV0::updateText(char mode, int p0) {
	std::string s;
	s = (mode == 'S') ? "fill: " + std::to_string(p0) :
		(mode == '?') ? ">>" + (std::string)(sf::String)(char)p0 : "";
	((uiText*)(parts[6]))->setText(s);
}
uiWindowV1::uiWindowV1(int W, int H, int Wc, int Hc) : uiWindow(4, 3, W, H) {
	std::string bp = "images/button_pressed.png";
	std::string bf = "images/button_free.png";
	std::string F = "fonts/F.ttf";
	parts[0] = new uiButtonChar(bp, bf, F, sf::FloatRect(0.8f, 0.0f, 0.2f, 0.2f), window, w_zone, 'S');
	parts[1] = new uiButtonChar(bp, bf, F, sf::FloatRect(0.8f, 0.2f, 0.2f, 0.2f), window, w_zone, 'C');
	parts[2] = new uiButtonChar(bp, bf, F, sf::FloatRect(0.8f, 0.4f, 0.2f, 0.2f), window, w_zone, 'G');
	parts[3] = new uiCanvas(sf::FloatRect(0.0f, 0.0f, 0.8f, 0.8f), window, w_zone, Hc, Wc, 2, 0.1);
	for (uint32_t i = 0; i < countA; i++) {
		actions[i] = ((uiButtonChar*)(parts[i]))->action;
	}
}
