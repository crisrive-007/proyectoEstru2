#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H

#include <SFML/Graphics.hpp>
#include "Personaje.h"

class Biblioteca {
private:
    sf::RenderWindow ventanaBiblioteca;
    sf::Texture texturaFondo;
    sf::Sprite fondo;
    Personaje& jugador;
    bool activa;

public:
    Biblioteca(Personaje& personaje);
    bool cargarRecursos();
    void ejecutar();
    bool estaActiva() const { return activa; }
};

#endif
