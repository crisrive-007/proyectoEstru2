#ifndef CASA_H
#define CASA_H

#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

class Casa
{
    public:
        Casa(const sf::Texture& texturaReferencia, sf::Vector2f posicion, sf::Vector2f escala);

        void dibujar(sf::RenderWindow& ventana) const {
            ventana.draw(sprite);
        }

        sf::FloatRect obtenerLimites() const {
            return sprite.getGlobalBounds();
        }

        void setEscala(sf::Vector2f escala) {
            sprite.setScale(escala);
        }

        sf::FloatRect obtenerHitbox() const {
            sf::FloatRect bounds = sprite.getGlobalBounds();
            float reduccion = 0.2f;
            bounds.position.x += bounds.size.x * reduccion / 2;
            bounds.position.y += bounds.size.y * reduccion / 2;
            bounds.size.x *= (1.0f - reduccion);
            bounds.size.y *= (0.9f - reduccion);
            return bounds;
        }

        virtual ~Casa();

    protected:

    private:
        sf::Sprite sprite;
};

#endif // CASA_H
