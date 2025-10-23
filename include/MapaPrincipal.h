#ifndef MAPAPRINCIPAL_H
#define MAPAPRINCIPAL_H

#include "Personaje.h"
#include "Casa.h"

#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <cstdlib>

constexpr int TILE_SIZE = 16;
constexpr int MAP_WIDTH = 30;
constexpr int MAP_HEIGHT = 20;
constexpr int SCREEN_WIDTH = MAP_WIDTH * TILE_SIZE * 2;
constexpr int SCREEN_HEIGHT = MAP_HEIGHT * TILE_SIZE * 2;

struct TileDef {
    sf::IntRect rect;
    // Podrías añadir un puntero a la textura para edificios
    // sf::Texture* texture;
};

class MapaPrincipal {
public:
    MapaPrincipal();
    void dibujar(sf::RenderWindow& window);
    void ejecutar();
    ~MapaPrincipal();

private:
    sf::Texture exteriorTileset;
    sf::Texture edificioTileset;
    std::map<int, TileDef> definicionTile;
    std::vector<std::vector<int>> mapa;

    void inicializarMapa();
};

#endif // MAPAPRINCIPAL_H
