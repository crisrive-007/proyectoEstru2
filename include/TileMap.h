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

    int tilesPerRow = m_tileset.getSize().x / static_cast<int>(tileSize.x);
    int tilesPerColumn = m_tileset.getSize().y / static_cast<int>(tileSize.y);

    // 🔹 DEBUG: Información de tiles
    std::cout << "Tiles por fila: " << tilesPerRow << std::endl;
    std::cout << "Tiles por columna: " << tilesPerColumn << std::endl;
    std::cout << "Total de tiles en el mapa: " << width * height << std::endl;

    m_sprites.reserve(width * height);
    for (unsigned int y = 0; y < height; ++y) {
        for (unsigned int x = 0; x < width; ++x) {
            int tileNumber = tiles[x + y * width];

            // 🔹 DEBUG: Primeros tiles
            if (x < 3 && y < 3) {
                std::cout << "Tile en (" << x << "," << y << "): numero=" << tileNumber;
            }

            int tu = tileNumber % tilesPerRow;
            int tv = tileNumber / tilesPerRow;

            // 🔹 DEBUG: Coordenadas de textura
            if (x < 3 && y < 3) {
                std::cout << " -> UV=(" << tu << "," << tv << ")" << std::endl;
            }

            sf::Sprite sprite(m_tileset);
            sprite.setTextureRect(sf::IntRect(
                {tu * static_cast<int>(tileSize.x), tv * static_cast<int>(tileSize.y)},
                {static_cast<int>(tileSize.x), static_cast<int>(tileSize.y)}
            ));

            sprite.setPosition({static_cast<float>(x * tileSize.x), static_cast<float>(y * tileSize.y)});
            m_sprites.push_back(sprite);
        }
    }
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
