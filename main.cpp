#include <SFML/Graphics.hpp>
#include "MapaPrincipal.h"
#include "TileMap.h"
#include "Personaje.h"
#include "Biblioteca.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

int main() {
    std::cout << "🎮 Iniciando aplicación..." << std::endl;

    sf::RenderWindow window(sf::VideoMode({1920, 1080}), "Mapa con Tilemaps");

    if (!window.isOpen()) {
        std::cerr << "❌ ERROR: No se pudo crear la ventana" << std::endl;
        return -1;
    }

    std::cout << "✅ Ventana creada correctamente" << std::endl;

    // CREAR EL PERSONAJE primero
    std::cout << "🔹 Creando personaje..." << std::endl;
    std::unordered_set<int> tilesValidos = {0};
    Personaje jugador(3.0f, tilesValidos); // Velocidad de 200.0f como en tu código

    std::cout << "✅ Personaje creado correctamente en posición: "
              << jugador.obtenerPosicion().x << ", "
              << jugador.obtenerPosicion().y << std::endl;

    // Crear el mapa principal pasando la ventana Y EL PERSONAJE
    std::cout << "🔹 Creando MapaPrincipal con personaje..." << std::endl;
    MapaPrincipal mapa(window, jugador); // Ahora pasa el personaje
    std::cout << "✅ MapaPrincipal creado correctamente" << std::endl;

    // Ejecutar la función que configura todo el mapa
    std::cout << "🔹 Ejecutando mapa..." << std::endl;
    mapa.ejecutarMapa();
    std::cout << "✅ Mapa ejecutado correctamente" << std::endl;

    std::cout << "🎮 Entrando al loop principal..." << std::endl;

    // Reloj para el control de FPS
    sf::Clock clock;

    // Loop principal
    while (window.isOpen()) {
        // Calcular delta time
        float deltaTime = clock.restart().asSeconds();

        // Procesar eventos
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        // ACTUALIZAR: Lógica del juego (incluye movimiento del personaje con colisiones)
        mapa.actualizar();

        // DIBUJAR
        window.clear();
        mapa.dibujar(); // Esto dibuja los tilemaps Y el personaje
        window.display();
    }

    std::cout << "🏁 Aplicación terminada correctamente" << std::endl;
    return 0;
}
