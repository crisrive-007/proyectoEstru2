#ifndef BARRAVIDA_H
#define BARRAVIDA_H

#pragma once
#include <SFML/Graphics.hpp>
#include <array>

class BarraVida {
public:
    BarraVida() = default;

    bool cargar(const std::array<std::string,5>& rutas, const sf::Vector2f& pos) {
        for (size_t i=0;i<5;i++){
            if(!textures[i].loadFromFile(rutas[i])) return false;
        }
        sprite.setTexture(textures[0]);
        sprite.setPosition(pos);
        vidaIdx = 0;
        return true;
    }

    void reiniciar() { vidaIdx = 0; sprite.setTexture(textures[vidaIdx]); }
    void herir()     { if (vidaIdx<4) { vidaIdx++; sprite.setTexture(textures[vidaIdx]); } }
    void curar()     { if (vidaIdx>0) { vidaIdx--; sprite.setTexture(textures[vidaIdx]); } }

    bool estaVivo() const { return vidaIdx < 4; }
    bool estaMuerto() const { return vidaIdx == 4; }
    void vaciar() { vidaIdx = 4; sprite.setTexture(textures[vidaIdx]); }

    void draw(sf::RenderTarget& t) const { t.draw(sprite); }

private:
    std::array<sf::Texture,5> textures;
    sf::Sprite sprite;
    int vidaIdx{0};
};

#endif // BARRAVIDA_H
