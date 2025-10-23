#ifndef TILEMAP_H
#define TILEMAP_H

#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>

class TileMap : public sf::Drawable {
public:
    bool load(const std::string& tilesetPath, sf::Vector2u tileSize,
          const std::vector<int>& tiles, unsigned int width, unsigned int height) {

    if (!m_tileset.loadFromFile(tilesetPath)) {
        std::cerr << "Error cargando tileset: " << tilesetPath << std::endl;
        return false;
    }

    // 🔹 DEBUG: Información del tileset
    std::cout << "Tileset cargado: " << tilesetPath << std::endl;
    std::cout << "Tamaño del tileset: " << m_tileset.getSize().x << "x" << m_tileset.getSize().y << std::endl;
    std::cout << "Tamaño de tile: " << tileSize.x << "x" << tileSize.y << std::endl;

    m_tileSize = tileSize;
    m_width = width;
    m_height = height;

    // Usar unsigned int consistentemente
    unsigned int tilesPerRow = m_tileset.getSize().x / tileSize.x;
    unsigned int tilesPerColumn = m_tileset.getSize().y / tileSize.y;

    // 🔹 DEBUG: Información de tiles
    std::cout << "Tiles por fila: " << tilesPerRow << std::endl;
    std::cout << "Tiles por columna: " << tilesPerColumn << std::endl;
    std::cout << "Total de tiles en el mapa: " << width * height << std::endl;

    m_sprites.clear();
    m_sprites.reserve(width * height);

    for (unsigned int y = 0; y < height; ++y) {
        for (unsigned int x = 0; x < width; ++x) {
            int tileNumber = tiles[x + y * width];

            // 🔹 Validar tileNumber
            if (tileNumber < 0) {
                // Tile vacío, saltar
                continue;
            }

            // 🔹 DEBUG: Primeros tiles
            if (x < 3 && y < 3) {
                std::cout << "Tile en (" << x << "," << y << "): numero=" << tileNumber;
            }

            // Conversión segura
            unsigned int tu = static_cast<unsigned int>(tileNumber) % tilesPerRow;
            unsigned int tv = static_cast<unsigned int>(tileNumber) / tilesPerRow;

            // 🔹 Validar coordenadas de textura
            if (tu >= tilesPerRow || tv >= tilesPerColumn) {
                std::cerr << "Error: TileNumber " << tileNumber << " fuera de rango" << std::endl;
                continue;
            }

            // 🔹 DEBUG: Coordenadas de textura
            if (x < 3 && y < 3) {
                std::cout << " -> UV=(" << tu << "," << tv << ")" << std::endl;
            }

            sf::Sprite sprite(m_tileset);

            // 🔹 CORRECCIÓN PARA SFML 3.0: Usar sf::Vector2i para el rectángulo
            sprite.setTextureRect(sf::IntRect(
                sf::Vector2i(static_cast<int>(tu * tileSize.x), static_cast<int>(tv * tileSize.y)),
                sf::Vector2i(static_cast<int>(tileSize.x), static_cast<int>(tileSize.y))
            ));

            // 🔹 CORRECCIÓN PARA SFML 3.0: Usar sf::Vector2f para la posición
            sprite.setPosition(sf::Vector2f(
                static_cast<float>(x * tileSize.x),
                static_cast<float>(y * tileSize.y)
            ));

            m_sprites.push_back(sprite);
        }
    }

    std::cout << "Sprites creados: " << m_sprites.size() << std::endl;
    return true;
}

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        for (const auto& sprite : m_sprites)
            target.draw(sprite, states);
    }

    sf::Texture m_tileset;
    sf::Vector2u m_tileSize;
    unsigned int m_width{}, m_height{};
    std::vector<sf::Sprite> m_sprites;
};

#endif // TILEMAP_H
