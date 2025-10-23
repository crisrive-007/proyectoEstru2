#include <SFML/Graphics.hpp>
#include <vector>
#include "Personaje.h"
#include "Casa.h"
#include "MapaPrincipal.h"
#include <iostream>
#include <optional>

MapaPrincipal::MapaPrincipal()
{
    // 1. Cargar Texturas
    if(!exteriorTileset.loadFromFile("assets/Tilesets/Tileset 2.png")) {
        std::cerr << "Error al cargar Tileset 2.png" << std::endl;
    }
    if(!edificioTileset.loadFromFile("assets/Tilesets/Tileset 1.png")) {
        std::cerr << "Error al cargar Tileset 1.png" << std::endl;
    }

    // 2. Definir Tiles del Terreno (Tileset 2)
    // ID 0: Hierba Clara (Base)
    definicionTile[0].rect = sf::IntRect(sf::Vector2i(0 * TILE_SIZE, 0 * TILE_SIZE), sf::Vector2i(TILE_SIZE, TILE_SIZE));
    // ID 1: Hierba Oscura (Parche)
    definicionTile[1].rect = sf::IntRect(sf::Vector2i(1 * TILE_SIZE, 0 * TILE_SIZE), sf::Vector2i(TILE_SIZE, TILE_SIZE));
    // ID 2: Camino/Tierra
    definicionTile[2].rect = sf::IntRect(sf::Vector2i(1 * TILE_SIZE, 2 * TILE_SIZE), sf::Vector2i(TILE_SIZE, TILE_SIZE));
    // ID 3: Agua (Centro)
    definicionTile[3].rect = sf::IntRect(sf::Vector2i(10 * TILE_SIZE, 0 * TILE_SIZE), sf::Vector2i(TILE_SIZE, TILE_SIZE));
    // ID 4: Borde de Agua (Top) - Usaremos el tile de la esquina superior izquierda del borde
    definicionTile[4].rect = sf::IntRect(sf::Vector2i(8 * TILE_SIZE, 0 * TILE_SIZE), sf::Vector2i(TILE_SIZE, TILE_SIZE));
    // ID 5: Árbol (Copa) - Usaremos el tile superior-izquierdo del árbol grande
    definicionTile[5].rect = sf::IntRect(sf::Vector2i(6 * TILE_SIZE, 1 * TILE_SIZE), sf::Vector2i(TILE_SIZE, TILE_SIZE));
    // ID 6: Roca (Piedra)
    definicionTile[6].rect = sf::IntRect(sf::Vector2i(4 * TILE_SIZE, 18 * TILE_SIZE), sf::Vector2i(TILE_SIZE, TILE_SIZE));
    // ID 7: Pasto Cultivado
    definicionTile[7].rect = sf::IntRect(sf::Vector2i(0 * TILE_SIZE, 3 * TILE_SIZE), sf::Vector2i(TILE_SIZE, TILE_SIZE));

    // 3. Inicializar la matriz del mapa
    inicializarMapa();
}

void MapaPrincipal::inicializarMapa()
{
    mapa.resize(MAP_HEIGHT, std::vector<int>(MAP_WIDTH));

    // Rellenar con Hierba Clara (ID 0)
    for(int y = 0; y < MAP_HEIGHT; ++y) {
        for(int x = 0; x < MAP_WIDTH; ++x) {
            mapa[y][x] = 0;
        }
    }

    // --- Diseño del Pueblo "Hojaverde" ---

    // Camino principal (ID 2) - Vertical por el centro
    for(int y = 0; y < MAP_HEIGHT; ++y) {
        for(int x = 12; x <= 17; ++x) {
            mapa[y][x] = 2;
        }
    }

    // Camino de salida inferior (ID 2)
    for(int x = 12; x <= 17; ++x) {
        mapa[MAP_HEIGHT - 1][x] = 2;
    }

    // Camino de salida superior (ID 2)
    for(int x = 12; x <= 17; ++x) {
        mapa[0][x] = 2;
    }

    // Árboles (ID 5) - Borde izquierdo (usa la copa para simplificar)
    for(int y = 0; y < MAP_HEIGHT; ++y) {
        mapa[y][0] = 5;
        mapa[y][1] = 5;
    }
    // Árboles (ID 5) - Borde derecho
    for(int y = 0; y < MAP_HEIGHT; ++y) {
        mapa[y][MAP_WIDTH - 1] = 5;
        mapa[y][MAP_WIDTH - 2] = 5;
    }

    // Parches de Hierba Alta (ID 1)
    for(int y = 5; y <= 8; ++y) {
        for(int x = 20; x <= 25; ++x) {
            mapa[y][x] = 1;
        }
    }

    // Zona de Cultivo (ID 7)
    for(int y = 13; y <= 16; ++y) {
        for(int x = 4; x <= 9; ++x) {
            mapa[y][x] = 7;
        }
    }

    // --- Posición de los Edificios (Solo la esquina superior izquierda del área) ---
    // (En un sistema real, necesitarías una clase o función separada para dibujar el edificio completo)

    // Casa del Jugador (Pequeña Roja, ref. 100)
    // Usaremos el ID 2 (Camino) como "base" del edificio para que se dibuje correctamente,
    // y la lógica de dibujo luego superpondrá la imagen del edificio.
    mapa[5][8] = 2; // Posición de la casa (Base)

    // Centro Pokémon (ref. 101)
    mapa[5][15] = 2; // Centro Pokémon (Base)

    // Tienda (Pokémart, ref. 102)
    mapa[5][19] = 2; // Tienda (Base)

    // Casa del Profesor (Grande, ref. otra casa del tileset 1, por ejemplo: (1, 3))
    mapa[10][8] = 2; // Casa del Profesor (Base)
}

void MapaPrincipal::dibujar(sf::RenderWindow& window) {
    sf::Sprite sprite(exteriorTileset);
    sf::Sprite edificioSprite(edificioTileset);

    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            int tileId = mapa[y][x];

            // 1. Dibujar el Tile de Terreno (Tileset 2)
            if (definicionTile.count(tileId)) {
                sprite.setTextureRect(definicionTile[tileId].rect);
                sprite.setPosition(sf::Vector2f(static_cast<float>(x * TILE_SIZE),
                                                static_cast<float>(y * TILE_SIZE)));
                window.draw(sprite);
            }

            // 2. Dibujar Edificios (Tileset 1) - Lógica simplificada
            // En un juego real, esto se manejaría con una capa separada o una clase de objeto.
            // Para fines de demostración, simulamos la superposición del edificio.

            sf::IntRect edificioRect;
            bool isBuilding = false;

            // Aquí, en lugar de usar un TileID, simplemente colocaremos los edificios en las coordenadas
            // que definimos en inicializarMapa(), asumiendo que el tile base (ID 2) ya se dibujó.

            // Posición (5, 8): Casa Pequeña Roja (4x3 tiles) - ref. (0, 0) en Tileset 1
            if (x == 8 && y == 5) {
                edificioRect = sf::IntRect(sf::Vector2i(0 * TILE_SIZE, 0 * TILE_SIZE),
                                          sf::Vector2i(4 * TILE_SIZE, 3 * TILE_SIZE));
                isBuilding = true;
            }
            // Posición (5, 15): Centro Pokémon (5x4 tiles) - ref. (0, 1) en Tileset 1
            else if (x == 15 && y == 5) {
                edificioRect = sf::IntRect(sf::Vector2i(0 * TILE_SIZE, 1 * TILE_SIZE),
                                          sf::Vector2i(5 * TILE_SIZE, 4 * TILE_SIZE));
                isBuilding = true;
            }
            // Posición (5, 19): Tienda (4x3 tiles) - ref. (1, 0) en Tileset 1
            else if (x == 19 && y == 5) {
                edificioRect = sf::IntRect(sf::Vector2i(1 * TILE_SIZE, 0 * TILE_SIZE),
                                          sf::Vector2i(4 * TILE_SIZE, 3 * TILE_SIZE));
                isBuilding = true;
            }
            // Posición (10, 8): Casa del Profesor (4x4 tiles) - ref. (1, 3) en Tileset 1
            else if (x == 8 && y == 10) {
                edificioRect = sf::IntRect(sf::Vector2i(1 * TILE_SIZE, 3 * TILE_SIZE),
                                          sf::Vector2i(4 * TILE_SIZE, 4 * TILE_SIZE));
                isBuilding = true;
            }

            if (isBuilding) {
                edificioSprite.setTextureRect(edificioRect);
                // La posición del edificio se dibuja a partir de la esquina superior izquierda
                edificioSprite.setPosition(sf::Vector2f(static_cast<float>(x * TILE_SIZE),
                                           static_cast<float>(y * TILE_SIZE - (edificioRect.size.y - TILE_SIZE))));
                window.draw(edificioSprite);
            }
        }
    }
}

void MapaPrincipal::ejecutar() {
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(SCREEN_WIDTH, SCREEN_HEIGHT)),
                             "Pueblo Hojaverde - GBA Style");
    window.setFramerateLimit(60); // Opcional

    while (window.isOpen()) {
        while (std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        window.clear(sf::Color::Black);
        dibujar(window);
        window.display();
    }
}

MapaPrincipal::~MapaPrincipal()
{
    //dtor
}
