#include <SFML/Graphics.hpp>
#include "MapaPrincipal.h"
#include "Personaje.h"
#include "GestorEstados.h"
#include "Menu.h"
#include <unordered_set>
#include <memory>
#include <iostream>

int main() {
    sf::RenderWindow window(sf::VideoMode({1920u,1080u}), "ProyectoEstru2 - Estados");
    window.setVerticalSyncEnabled(true);

    if (!window.isOpen()) { std::cerr << "No se pudo crear ventana\n"; return -1; }

    std::unordered_set<int> tilesValidos = {0};
    Personaje jugador(3.0f, tilesValidos);

    GestorEstados gestor(window, jugador);        // pila de estados
    auto estadoMenu = std::make_unique<Menu>(&gestor, window, jugador);
    gestor.empujarEstado(std::move(estadoMenu));  // ← el menú es el estado inicial

    // bucle delegado al estado tope (como ya haces)
    while (window.isOpen()) {
        gestor.manejarEventos();
        gestor.actualizar();
        window.clear();
        gestor.dibujar();
        window.display();
    }

    return 0;
}
