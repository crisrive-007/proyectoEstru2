#include <SFML/Graphics.hpp>
#include "MapaPrincipal.h"
#include "TileMap.h"
#include "Personaje.h"
#include "Biblioteca.h"
#include "GestorEstados.h"
#include "Menu.h"
#include "SaveGameManager.h"
#include <iostream>
#include <memory>
#include <unordered_set>

int main() {
    // Ventana
    sf::RenderWindow window(sf::VideoMode({1920u, 1080u}), "ProyectoEstru2 - Menu");
    window.setFramerateLimit(60);
    if (!window.isOpen()) {
        std::cerr << "❌ ERROR: No se pudo crear la ventana\n";
        return -1;
    }

    // Personaje + Gestor
    std::unordered_set<int> tilesValidos = {0}; // ajusta según tus tilemaps transitables
    Personaje jugador(3.0f, tilesValidos);
    GestorEstados gestor(window, jugador);

    // Fuente y Menu
    sf::Font font;
    if (!font.openFromFile("assets/Pokemon_GB.ttf")) {
        std::cerr << "⚠️ No se pudo cargar assets/Pokemon_GB.ttf (seguimos con font por defecto del objeto)\n";
    }
    Menu menu(window, &font);
    menu.setTitle("Menu Principal");

    // Estado del menú
    bool menuActivo = true;

    // Callbacks
    menu.onNuevaPartida = [&](const std::string& nombre) {
        jugador.setNombre(nombre);
        if (SaveGame::guardarJuego(jugador, ProgresoJuego::get()))
            std::cout << u8"[SAVE] Nueva partida guardada.\n";
        else
            std::cout << u8"[SAVE] ERROR al guardar.\n";
        menuActivo = false;
    };

    menu.onCargarPartida = [&]() {
        if (SaveGame::cargarJuego(jugador, ProgresoJuego::get()))
            std::cout << u8"[LOAD] Partida cargada.\n";
        else
            std::cout << u8"[LOAD] No se encontró la partida.\n";
        menuActivo = false;
    };

    // Loop de app
    while (window.isOpen()) {
        // Eventos
        while (auto ev = window.pollEvent()) {
            const sf::Event& event = *ev; // SFML 3: pollEvent() devuelve optional
            if (menuActivo) {
                menu.handleEvent(event);
            } else if (event.is<sf::Event::Closed>()) {
                window.close();
            }
        }

        // Mostrar menú hasta que se elija opción
        if (menuActivo) {
            menu.update();
            window.clear(sf::Color(10, 10, 20));
            menu.draw();
            window.display();
            continue; // aún no iniciamos el juego
        }

        // ===== Cuando salimos del menú, arrancamos el juego =====
        {
            std::unique_ptr<MapaPrincipal> estadoMapa = std::make_unique<MapaPrincipal>(&gestor, window, jugador);
            estadoMapa->ejecutarMapa(); // prepara tus tilemaps, etc.
            gestor.empujarEstado(std::move(estadoMapa));
        }

        // Loop del juego (delegado al GestorEstados)
        while (window.isOpen()) {
            while (auto ev = window.pollEvent()) {
                const sf::Event& event = *ev;
                if (event.is<sf::Event::Closed>()) {
                    window.close();
                } else {
                    gestor.manejarEventos();
                }
            }

            gestor.actualizar();
            window.clear();
            gestor.dibujar();
            window.display();
        }
    }

    return 0;
}
