#include "TileMap.h"
#include <iostream>
#include <SFML/Graphics.hpp>

/*TileMap::TileMap() : m_vertices(sf::Quads) {}

bool TileMap::cargar(const std::string& texturePath, sf::Vector2u tileSize,
                const std::vector<int>& tiles, unsigned int ancho, unsigned int alto) {

        // Cargar la textura del tileset
        if (!m_tileset.loadFromFile(texturePath)) {
            return false;
        }

        m_tiles = tiles;
        m_mapSize = sf::Vector2u(ancho, alto);
        m_tileSize = tileSize;

        // Redimensionar el array de vértices
        m_vertices.resize(ancho * alto * 4);

        // Poblar el array de vértices con los quads de cada tile
        for (unsigned int i = 0; i < ancho; i++) {
            for (unsigned int j = 0; j < alto; j++) {
                int tileNumber = tiles[i + j * ancho];

                // Si el tile es -1, es transparente (no se dibuja)
                if (tileNumber == -1) {
                    continue;
                }

                // Calcular la posición en el tileset
                int tu = tileNumber % (m_tileset.getSize().x / tileSize.x);
                int tv = tileNumber / (m_tileset.getSize().x / tileSize.x);

                // Obtener un puntero al quad actual
                sf::Vertex* quad = &m_vertices[(i + j * ancho) * 4];

                // Definir las 4 esquinas del quad
                quad[0].position = sf::Vector2f(i * tileSize.x, j * tileSize.y);
                quad[1].position = sf::Vector2f((i + 1) * tileSize.x, j * tileSize.y);
                quad[2].position = sf::Vector2f((i + 1) * tileSize.x, (j + 1) * tileSize.y);
                quad[3].position = sf::Vector2f(i * tileSize.x, (j + 1) * tileSize.y);

                // Definir las coordenadas de textura
                quad[0].texCoords = sf::Vector2f(tu * tileSize.x, tv * tileSize.y);
                quad[1].texCoords = sf::Vector2f((tu + 1) * tileSize.x, tv * tileSize.y);
                quad[2].texCoords = sf::Vector2f((tu + 1) * tileSize.x, (tv + 1) * tileSize.y);
                quad[3].texCoords = sf::Vector2f(tu * tileSize.x, (tv + 1) * tileSize.y);
            }
        }

        return true;
    }

    void TileMap::dibujar(sf::RenderWindow& window) {
        window.draw(m_vertices, &m_tileset);
    }*/
