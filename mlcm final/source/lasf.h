#pragma once
#include "SFML/Graphics.hpp"

void printRectF(sf::FloatRect R);
void printVec2F(sf::Vector2f V);
sf::Vector2f operator*(sf::Vector2f A, sf::Vector2f B);
sf::Vector2f operator/(sf::Vector2f p, sf::FloatRect R);
sf::Vector2f operator/(sf::Vector2f A, sf::Vector2f B);
sf::Vector2f rectPos(sf::FloatRect R);
sf::Vector2f rectSize(sf::FloatRect R);
sf::FloatRect operator>>(sf::FloatRect r, sf::FloatRect R);
void fitTexture(sf::Sprite* sp, sf::Texture* tx, sf::FloatRect zone);