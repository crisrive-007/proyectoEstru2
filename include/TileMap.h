#ifndef TILEMAP_H
#define TILEMAP_H

#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>

class TileMap : public sf::Drawable, sf::Transformable {
public:
    bool load(const std::string& tilesetPath, sf::Vector2u tileSize,
          const std::vector<int>& tiles, unsigned int width, unsigned int height);

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    sf::VertexArray m_vertices;
    sf::Texture m_tileset;
    sf::Vector2u m_tileSize;
    unsigned int m_width{}, m_height{};
    std::vector<sf::Sprite> m_sprites;
};

#endif // TILEMAP_H
