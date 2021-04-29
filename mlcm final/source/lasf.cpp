#pragma once
#include "lasf.h"

void printRectF(sf::FloatRect R) {
	printf("{(%f, %f), (%f, %f)}\n", R.left, R.top, R.width, R.height);
}
void printVec2F(sf::Vector2f V) {
	printf("(%f, %f)\n", V.x, V.y);
}
sf::Vector2f operator*(sf::Vector2f A, sf::Vector2f B) {
	return sf::Vector2f(A.x * B.x, A.y * B.y);
}
sf::Vector2f operator/(sf::Vector2f p, sf::FloatRect R) {
	return sf::Vector2f((p.x - R.left) / R.width, (p.y - R.top) / R.height);
}
sf::Vector2f operator/(sf::Vector2f A, sf::Vector2f B) {
	return sf::Vector2f(A.x / B.x, A.y / B.y);
}
sf::Vector2f rectPos(sf::FloatRect R) {
	return sf::Vector2f(R.left, R.top);
}
sf::Vector2f rectSize(sf::FloatRect R) {
	return sf::Vector2f(R.width, R.height);
}
sf::FloatRect operator>>(sf::FloatRect r, sf::FloatRect R) {
	sf::Vector2f p, s, P, S;
	p = rectPos(r); s = rectSize(r);
	P = rectPos(R); S = rectSize(R);
	return sf::FloatRect(P + S * p, S * s);
}
void fitTexture(sf::Sprite* sp, sf::Texture* tx, sf::FloatRect zone) {
	sp->setPosition(rectPos(zone));
	sp->setScale(rectSize(zone) / (sf::Vector2f)tx->getSize());
	sp->setTexture(*tx);
}