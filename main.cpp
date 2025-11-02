#include <SFML/Graphics.hpp>
#include "MapaPrincipal.h"
#include "TileMap.h"
#include "Personaje.h"
#include "Biblioteca.h"
#include "GestorEstados.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <memory> // Necesario para std::unique_ptr

int main() {
    sf::RenderWindow window(sf::VideoMode({1920, 1080}), "Mapa con Tilemaps");
    window.setFramerateLimit(60);

    if (!window.isOpen()) {
        std::cerr << "❌ ERROR: No se pudo crear la ventana" << std::endl;
        return -1;
    }

    std::unordered_set<int> tilesValidos = {0}; // Asumiendo que 0 es el tile transitable
    Personaje jugador(3.0f, tilesValidos);

    // **1. Inicializar el Gestor de Estados (State Manager)**
    GestorEstados gestor(window, jugador);

    std::unique_ptr<MapaPrincipal> estadoMapa = std::make_unique<MapaPrincipal>(&gestor, window, jugador);
    //std::unique_ptr<Biblioteca> estadoMapa = std::make_unique<Biblioteca>(&gestor, window, jugador);

    // Ejecutar la función que configura los TileMaps, etc.
    estadoMapa->ejecutarMapa();

    // Empujar el estado al gestor. El gestor ahora controla este estado.
    gestor.empujarEstado(std::move(estadoMapa));

    sf::Clock clock;

    // Loop principal
    while (window.isOpen()) {
        // float deltaTime = clock.restart().asSeconds(); // Opcional si usas setFramerateLimit(60)

        // **3. DELEGACIÓN COMPLETA AL GESTOR DE ESTADOS**
        // El Gestor llama a los métodos del estado que esté activo (MapaPrincipal o Biblioteca)

        // Manejar Eventos: El estado activo se encarga de los eventos (incluido el cierre de la ventana y el movimiento del personaje)
        gestor.manejarEventos();

        // ACTUALIZAR: Lógica del juego y colisiones (incluye la detección de entrada a la biblioteca)
        gestor.actualizar();

        // DIBUJAR
        window.clear();
        gestor.dibujar(); // Dibuja el estado activo
        window.display();
    }

    return 0;
}
