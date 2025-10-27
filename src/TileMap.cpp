#include "TileMap.h"
#include <iostream>
#include <SFML/Graphics.hpp>

// 🟢 SOLUCIÓN CLAVE: Implementación correcta de la función draw.
void TileMap::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    // 1. Aplicar la transformación (posición, escala, etc.) del objeto TileMap.
    states.transform *= getTransform();

    // 2. Aplicar la textura (el tileset) que se cargó.
    states.texture = &m_tileset;

    // 3. Dibujar la geometría (los quads) usando la textura.
    target.draw(m_vertices, states);
}

bool TileMap::load(const std::string& texturePath, sf::Vector2u tileSize,
                    const std::vector<int>& tiles, unsigned int width, unsigned int height) {

    // 🛑 VERIFICACIÓN CRÍTICA: Carga del archivo del tileset (Asegúrate que la RUTA es correcta)
    if (!m_tileset.loadFromFile(texturePath)) {
        std::cerr << "🛑 ERROR CRITICO: No se pudo cargar el archivo del tileset en: " << texturePath << std::endl;
        return false;
    }

    m_tileSize = tileSize;

    m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
    m_vertices.resize(width * height * 6);

    int tilesPerRow = m_tileset.getSize().x / tileSize.x;

    for (unsigned int j = 0; j < height; ++j) {
    for (unsigned int i = 0; i < width; ++i) {

        int tileNumber = tiles[i + j * width];
        if (tileNumber < 0) continue;

        int tu = tileNumber % tilesPerRow;
        int tv = tileNumber / tilesPerRow;

        // índice base del tile actual
        sf::Vertex* tri = &m_vertices[(i + j * width) * 6];

        float x = static_cast<float>(i * tileSize.x);
        float y = static_cast<float>(j * tileSize.y);
        float u = static_cast<float>(tu * tileSize.x);
        float v = static_cast<float>(tv * tileSize.y);

        // Triángulo 1
        tri[0].position = {x, y};
        tri[1].position = {x + tileSize.x, y};
        tri[2].position = {x + tileSize.x, y + tileSize.y};

        tri[0].texCoords = {u, v};
        tri[1].texCoords = {u + tileSize.x, v};
        tri[2].texCoords = {u + tileSize.x, v + tileSize.y};

        // Triángulo 2
        tri[3].position = {x, y};
        tri[4].position = {x + tileSize.x, y + tileSize.y};
        tri[5].position = {x, y + tileSize.y};

        tri[3].texCoords = {u, v};
        tri[4].texCoords = {u + tileSize.x, v + tileSize.y};
        tri[5].texCoords = {u, v + tileSize.y};
    }
}

    return true;
}
