#include "Casa.h"

Casa::Casa(const sf::Texture& texturaReferencia, sf::Vector2f posicion, sf::Vector2f escala) : sprite(texturaReferencia)
{
    sprite.setTexture(texturaReferencia, true);

    sprite.setPosition(posicion);
    sprite.setScale(escala);
}

Casa::~Casa()
{
    //dtor
}
